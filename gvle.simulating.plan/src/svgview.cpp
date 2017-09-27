/*
 * This file is part of VLE, a framework for multi-modeling, simulation
 * and analysis of complex dynamical systems.
 * http://www.vle-project.org
 *
 * Copyright (c) 2017-2017 INRA http://www.inra.fr
 *
 * See the AUTHORS or Authors.txt file for copyright owners and
 * contributors
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or (at
 * your option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */
#include <QFile>
#include <QGraphicsSvgItem>
#include <QScrollBar>
#include <QToolTip>
#include <QtXml>
#include <QtXmlPatterns>
#include <QXmlStreamReader>
#include <QtDebug>
#include "svgview.h"
#include <iostream>

SvgView::SvgView(QWidget *parent)
    : QGraphicsView(parent),
    mGraphicItem(0)
{
    setScene(new QGraphicsScene(this));
    setTransformationAnchor(AnchorUnderMouse);
    setDragMode(ScrollHandDrag);
    setViewportUpdateMode(FullViewportUpdate);
    setAlignment(Qt::AlignLeft | Qt::AlignTop);

    mSvgRenderer = new QSvgRenderer();

    mPlan = NULL;
    mGroupHeight = 50;
    mPixelPerDay = 1;
    mZoomFactor  = 1.15;
    mZoomLevel   = 1;
    mMaxWidth    = 1500;
    mConfig.clear();
}

void SvgView::convert(const QString &xsltFile)
{
    (void)xsltFile;
    QXmlQuery query(QXmlQuery::XSLT20);
    QString   qOut;

    // ToDo : This function is a skeleton that can be used for XSLT processing

    query.setFocus(QUrl(mFilename));
    query.setQuery(QUrl(xsltFile));
    query.evaluateTo(&qOut);

}

QString SvgView::getTplHeader(void)
{
    QString str;
    QTextStream stream(&str);
    mTplHeader.save(stream, QDomNode::EncodingFromTextStream);
    return str;
}

QString SvgView::getTplTask(void)
{
    QString str;
    QTextStream stream(&str);
    mTplTask.save(stream, QDomNode::EncodingFromTextStream);
    return str;
}

QString SvgView::getTplTime(void)
{
    QString str;
    QTextStream stream(&str);
    mTplTime.save(stream, QDomNode::EncodingFromTextStream);
    return str;
}

void SvgView::loadPlan(vlePlan *plan)
{
    if ( mTplHeader.isNull() )
    {
        // ToDo : improve error handling
        return;
    }

    // Compute the height of a group
    if (mTplHeader.hasAttribute("height"))
        mGroupHeight = mTplHeader.attribute("height").toDouble();
    else
        mGroupHeight = 100;

    // Compute size of the whole plan
    int planHeight = mGroupHeight * (3 + plan->countGroups());
    int planWidth  = (mMaxWidth * mZoomLevel);

    // Create SVG document
    QDomDocument planSVG("xml");
    // Create root element
    QDomElement e = planSVG.createElement("svg");
    e.setAttribute("width",   QString(planWidth));
    e.setAttribute("height",  QString(planHeight));
    e.setAttribute("viewBox", QString("0 0 %1 %2").arg(planWidth).arg(planHeight));
    e.setAttribute("version", "1.1");

    QDate dateStart = plan->dateStart();
    QDate dateEnd   = plan->dateEnd();
    int nbDays = dateStart.daysTo(dateEnd);

    // In the plan duration is more than 1500 days
    if (nbDays > mMaxWidth)
    {
        // Update "pixel-per-day" to avoid very large picture
        qreal widgetSize = mMaxWidth;
        mPixelPerDay = (widgetSize / nbDays);
    }

    if (plan != mPlan)
    {
    // qWarning() << "Plan period is from" << dateStart.toString("dd/MM/yyyy")
    //         << "to" << dateEnd.toString("dd/MM/yyyy")
    //         << "(" << nbDays<< "days)"
    //         << "[" << mPixelPerDay << "pixel per day]";
    }

    // First insert the time rule
    QDomElement timeGrp = mTplHeader.cloneNode().toElement();
    updateField(timeGrp, "{{name}}", "");
    updatePos  (timeGrp, 0, 0);
    updateAttr (timeGrp, "header_background", "width", QString::number(planWidth));
    float yLen = (mPixelPerDay * 365 * mZoomLevel);
    // Show Weeks
    if (yLen > 2000)
    {
        QDate r;
            r.setDate(dateStart.year(), dateStart.month(), dateStart.day());
        while (r < dateEnd)
        {
            QDomElement newTimeStep = mTplTime.cloneNode().toElement();
            if (yLen < 5000)
                updateField(newTimeStep, "{{name}}", r.toString("dd/MM") );
            else
                updateField(newTimeStep, "{{name}}", r.toString("dd/MM/yy") );

            updateAttr (newTimeStep, "step_block", "width", QString::number(4));

            int offset = dateStart.daysTo(r);
            int aPos = (offset * mPixelPerDay * mZoomLevel);
            updatePos(newTimeStep, aPos, 0);
            timeGrp.appendChild(newTimeStep);
            // add tics
            for(int i = 0; i < 6; i++) {
                r = r.addDays(1);
                QDomElement newTic = mTplTime.cloneNode().toElement();
                updateAttr (newTic, "step_block", "width", QString::number(1));
                int offset = dateStart.daysTo(r);
                int aPos = (offset * mPixelPerDay * mZoomLevel);
                updatePos(newTic, aPos, 0);
                QString cfgColor("#ffffff");
                QString fillStyle = QString(";fill: %1; z-index: 1;").arg(cfgColor);

                updateAttr(newTic, "step_block", "style", fillStyle, false);
                updateField(newTic, "{{name}}", "" );
                updateAttr (newTic, "step_block", "height", QString::number(30 + 50 * plan->countGroups()));
                timeGrp.appendChild(newTic);
            }

            r = r.addDays(1);
        }
    }
    // Show month
    else if (yLen > 500)
    {
        QDate r;
        if (dateStart.daysInMonth() == 1)
            r.setDate(dateStart.year(), dateStart.month(), dateStart.day());
        else
            r.setDate(dateStart.year(), dateStart.month() + 1, 1);
        while (r < dateEnd)

        {
            QDomElement newTimeStep = mTplTime.cloneNode().toElement();
            if (yLen < 1000)
                updateField(newTimeStep, "{{name}}", r.toString("MMM") );
            else
                updateField(newTimeStep, "{{name}}", r.toString("MMM yy") );
            updateAttr (newTimeStep, "step_block", "width", QString::number(4));

            int offset = dateStart.daysTo(r);
            int aPos = (offset * mPixelPerDay * mZoomLevel);
            updatePos(newTimeStep, aPos, 0);
            timeGrp.appendChild(newTimeStep);
            r = r.addMonths(1);
        }
    }
    // Show Year
    else
    {
        QDate r;
        if (dateStart.dayOfYear() == 1)
            r.setDate(dateStart.year(), dateStart.month(), dateStart.day());
        else
            r.setDate(dateStart.year() + 1, 1, 1);
        while (r < dateEnd)
        {
            QDomElement newTimeStep = mTplTime.cloneNode().toElement();
            updateField(newTimeStep, "{{name}}", QString::number(r.year()) );
            updateAttr (newTimeStep, "step_block", "width", QString::number(4));

            int offset = dateStart.daysTo(r);
            int aPos = (offset * mPixelPerDay * mZoomLevel);
            updatePos(newTimeStep, aPos, 0);
            timeGrp.appendChild(newTimeStep);
            r = r.addYears(1);
        }
    }
    e.appendChild(timeGrp);

    // Insert all the known groups
    for (int i=0; i < plan->countGroups(); i++)
    {
        vlePlanGroup *planGroup = plan->getGroup(i);
        vlePlanActivity *prevActivity = 0;
        int prevLen = 0;
        int prevOffset = 0;

        // Create a new Group
        QDomElement newGrp = mTplHeader.cloneNode().toElement();
        updateField(newGrp, "{{name}}", planGroup->getName());
        updatePos  (newGrp, 0, ((i + 1) * mGroupHeight));
        updateAttr (newGrp, "header_background", "width", QString::number(planWidth));

        for (int j = 0; j < planGroup->count(); j++)
        {
            vlePlanActivity *planActivity = planGroup->getActivity(j);

            QDate actStart = planActivity->dateStart();
            QDate actEnd   = planActivity->dateEnd();

            qreal actLength = (mPixelPerDay * actStart.daysTo(actEnd) * mZoomLevel);
            if (actLength < 1)
                actLength = 1;

            QDomElement newAct = mTplTask.cloneNode().toElement();
            updateField(newAct, "{{name}}", planActivity->getName());

            updateAttr (newAct, "activity_block", "width", QString::number(actLength));

            QString cfgColor("#00edda");
            QString activityClass = planActivity->getClass();
            if ( ! activityClass.isEmpty() )
            {
                QString cfg = getConfig("color", activityClass);
                if ( ! cfg.isEmpty() )
                    cfgColor = cfg;
            }
            QString fillStyle = QString(";fill:%1").arg(cfgColor);
            updateAttr (newAct, "activity_block", "style", fillStyle, false);

            int date = dateStart.daysTo(planActivity->dateStart());
            int aPos = (date * mPixelPerDay * mZoomLevel);

            if (prevActivity)
            {
                if (prevLen > aPos)
                {
                    if (prevOffset < 40)
                        prevOffset += 15;
                    updateAttr(newAct, "activity_name", "y", QString::number(prevOffset));
                }
                else
                    prevOffset = 15;
            }

            updatePos(newAct, aPos, 0);
            newGrp.appendChild(newAct);

            prevActivity = planActivity;
            prevLen = aPos + (planActivity->getName().size() * 8);
        }

        e.appendChild(newGrp);
    }
    planSVG.appendChild( e );

    QByteArray data;
    QTextStream stream(&data);
    planSVG.save(stream, QDomNode::EncodingFromTextStream);

#ifdef PLAN_OUT
    QFile File("planOut.svg");
    File.open( QIODevice::WriteOnly );
    QTextStream TextStream(&File);
    planSVG.save(TextStream, 0);
    File.close();
    mFilename = "planOut.svg";
#else
    mFilename.clear();
#endif

    mPlan = plan;

    QXmlStreamReader xData(data);
    mSvgRenderer->load(&xData);
    refresh();
}

void SvgView::loadFile(QString fileName)
{
    qWarning() << "SVG load file " << fileName;

    mFilename = fileName;

    QFile file(fileName);
    file.open(QIODevice::ReadOnly);
    QXmlStreamReader xmlReader( file.readAll() );
    file.close();

    mSvgRenderer->load(&xmlReader);

    refresh();
}

bool SvgView::loadTemplate()
{
    QString tpl =
        "<?xml version=\"1.0\" encoding=\"UTF-8\" standalone=\"no\"?>\n" \
        "<!-- Created with Inkscape (http://www.inkscape.org/) -->\n"   \
        "\n"                                                            \
        "<svg\n"                                                        \
        "   xmlns:dc=\"http://purl.org/dc/elements/1.1/\"\n"            \
        "   xmlns:cc=\"http://creativecommons.org/ns#\"\n"              \
        "   xmlns:rdf=\"http://www.w3.org/1999/02/22-rdf-syntax-ns#\"\n" \
        "   xmlns:svg=\"http://www.w3.org/2000/svg\"\n"                 \
        "   xmlns=\"http://www.w3.org/2000/svg\"\n"                     \
        "   xmlns:sodipodi=\"http://sodipodi.sourceforge.net/DTD/sodipodi-0.dtd\"\n" \
        "   xmlns:inkscape=\"http://www.inkscape.org/namespaces/inkscape\"\n" \
        "   xmlns:vle=\"http://www.vle.org/\"\n"                        \
        "   width=\"1000\"\n"                                           \
        "   height=\"100\"\n"                                           \
        "   viewBox=\"0 0 1000 100\"\n"                                 \
        "   id=\"svg2\"\n"                                              \
        "   version=\"1.1\"\n"                                          \
        "   inkscape:version=\"0.91 r13725\"\n"                         \
        "   sodipodi:docname=\"draw.svg\">\n"                           \
        "  <defs\n"                                                     \
        "     id=\"defs4\" />\n"                                        \
        "  <sodipodi:namedview\n"                                       \
        "     id=\"base\"\n"                                            \
        "     pagecolor=\"#ffffff\"\n"                                  \
        "     bordercolor=\"#666666\"\n"                                \
        "     borderopacity=\"1.0\"\n"                                  \
        "     inkscape:pageopacity=\"0.0\"\n"                           \
        "     inkscape:pageshadow=\"2\"\n"                              \
        "     inkscape:zoom=\"1.4\"\n"                                  \
        "     inkscape:cx=\"259.64551\"\n"                              \
        "     inkscape:cy=\"147.92073\"\n"                              \
        "     inkscape:document-units=\"px\"\n"                         \
        "     inkscape:current-layer=\"layer1\"\n"                      \
        "     showgrid=\"false\"\n"                                     \
        "     inkscape:window-width=\"1920\"\n"                         \
        "     inkscape:window-height=\"1080\"\n"                        \
        "     inkscape:window-x=\"0\"\n"                                \
        "     inkscape:window-y=\"27\"\n"                               \
        "     inkscape:window-maximized=\"1\"\n"                        \
        "     units=\"px\" />\n"                                        \
        "  <g\n"                                                        \
        "     inkscape:label=\"Header\"\n"                              \
        "     inkscape:groupmode=\"layer\"\n"                           \
        "     id=\"g1234\"\n"                                           \
        "     width=\"1000.0\"\n"                                       \
        "     height=\"50.0\"\n"                                        \
        "     vle:template=\"header\"\n"                                \
        "     transform=\"translate(0, 0)\"\n"                          \
        "     style=\"display:inline\">\n"                              \
        "    <rect\n"                                                   \
        "       style=\"fill:#c8c8c8;fill-opacity:1;stroke:#000000;stroke-width:0.1111979;stroke-miterlimit:4;stroke-dasharray:none;stroke-opacity:1\"\n" \
        "       id=\"rect4280\"\n"                                      \
        "       width=\"100.0\"\n"                                      \
        "       height=\"50.0\"\n"                                      \
        "       x=\"0.0\"\n"                                            \
        "       y=\"0.0\" />\n"                                         \
        "    <text\n"                                                   \
        "       xml:space=\"preserve\"\n"                               \
        "       style=\"font-style:normal;font-weight:normal;font-size:15px;line-height:125%;font-family:sans-serif;letter-spacing:0px;word-spacing:0px;display:inline;fill:#000000;fill-opacity:1;stroke:none;stroke-width:1px;stroke-linecap:butt;stroke-linejoin:miter;stroke-opacity:1\"\n" \
        "       x=\"-30px\"\n"                                          \
        "       y=\"30px\"\n"                                           \
        "       id=\"text4257\"\n"                                      \
        "       sodipodi:linespacing=\"125%\">\n"                       \
        "       <tspan>{{name}}</tspan>\n"                              \
        "    </text>\n"                                                 \
        "    <rect\n"                                                   \
        "      style=\"fill:#dcdcdc;fill-opacity:.25;stroke:#000000;stroke-width:0.18060616;stroke-miterlimit:4;stroke-dasharray:none;stroke-opacity:1\"\n" \
        "       id=\"rect4215\"\n"                                      \
        "       vle:selector=\"header_background\"\n"                   \
        "       width=\"900.0\"\n"                                      \
        "       height=\"50\"\n"                                        \
        "       x=\"100.0\"\n"                                          \
        "       y=\"0.0\" />\n"                                         \
        "  </g>\n"                                                      \
        "  <!-- Layer : Task -->\n"                                     \
        "  <g\n"                                                        \
        "     inkscape:label=\"Task\"\n"                                \
        "     inkscape:groupmode=\"layer\"\n"                           \
        "     id=\"taskTemplate\"\n"                                    \
        "     \n"                                                       \
        "     vle:template=\"task\">\n"                                 \
        "    <g id=\"g4217\">\n"                                        \
        "      <rect\n"                                                 \
        "         y=\"20.0\"\n"                                         \
        "         x=\"120.0\"\n"                                        \
        "         height=\"30.0\"\n"                                    \
        "         width=\"100.0\"\n"                                    \
        "         id=\"rect4219\"\n"                                    \
        "         vle:selector=\"activity_block\"\n"                    \
        "         style=\"fill-opacity:1;stroke:#000000;stroke-width:0.187594;stroke-miterlimit:4;stroke-dasharray:none;stroke-opacity:1\" />\n" \
        "      <text\n"                                                 \
        "         xml:space=\"preserve\"\n"                             \
        "         style=\"font-style:normal;font-weight:normal;font-size:40px;line-height:125%;font-family:sans-serif;letter-spacing:0px;word-spacing:0px;fill:#000000;fill-opacity:1;stroke:none;stroke-width:1px;stroke-linecap:butt;stroke-linejoin:miter;stroke-opacity:1\"\n" \
        "         x=\"-10\"\n"                                          \
        "         y=\"15.0\"\n"                                         \
        "         vle:selector=\"activity_name\">\n"                    \
        "        <tspan\n"                                              \
        "           style=\"font-size:12px\"\n"                         \
        "           x=\"125.0\"\n"                                      \
        "           y=\"42.5\">{{name}}</tspan></text>\n"               \
        "    </g>\n"                                                    \
        "  </g>\n"                                                      \
        "  <!-- Layer : Time Rule -->\n"                                \
        "  <g\n"                                                        \
        "     inkscape:label=\"Time\"\n"                                \
        "     inkscape:groupmode=\"layer\"\n"                           \
        "     id=\"timeTemplate\"\n"                                    \
        "     vle:template=\"time\">\n"                                 \
        "    <g id=\"g2048\">\n"                                        \
        "      <rect\n"                                                 \
        "         y=\"20.0\"\n"                                         \
        "         x=\"120.0\"\n"                                        \
        "         height=\"30.0\"\n"                                    \
        "         width=\"100.0\"\n"                                    \
        "         id=\"rect4219\"\n"                                    \
        "         vle:selector=\"step_block\"\n"                        \
        "         style=\"fill-opacity:1;stroke:#000000;stroke-width:0.187594;stroke-miterlimit:4;stroke-dasharray:none;stroke-opacity:1\" />\n" \
        "      <text\n"                                                 \
        "         xml:space=\"preserve\"\n"                             \
        "         style=\"font-style:normal;font-weight:normal;font-size:40px;line-height:125%;font-family:sans-serif;letter-spacing:0px;word-spacing:0px;fill:#000000;fill-opacity:1;stroke:none;stroke-width:1px;stroke-linecap:butt;stroke-linejoin:miter;stroke-opacity:1\"\n" \
        "         x=\"-10\"\n"                                          \
        "         y=\"12.5\"\n"                                         \
        "         id=\"text4221\"\n"                                    \
        "         sodipodi:linespacing=\"125%\"><tspan\n"               \
        "           style=\"font-size:12px\"\n"                         \
        "           sodipodi:role=\"line\"\n"                           \
        "           id=\"tspan4223\"\n"                                 \
        "           x=\"125.0\"\n"                                      \
        "           y=\"42.5\">{{name}}</tspan></text>\n"               \
        "    </g>\n"                                                    \
        "  </g>\n"                                                      \
        "</svg>\n";

    mTplDocument.setContent(tpl);

    // Sanity check
    QDomElement e = mTplDocument.documentElement();
    if (e.nodeName() != "svg") {
        return false;
    }

    mTplHeader.clear();
    mTplTask.clear();
    mTplTime.clear();

    // Parse XML, search VLE templates
    for(QDomElement n = e.firstChildElement(); !n.isNull(); n = n.nextSiblingElement())
    {
        // Only "svg groups" are relevant
        if (n.nodeName() != "g")
            continue;

        if ( n.hasAttribute("vle:template") )
        {
            QString tplName = n.attribute("vle:template");

            if (tplName == "")
                continue;

            if (tplName == "header")
                mTplHeader = n;
            else if (tplName == "task")
                mTplTask = n;
            else if (tplName == "time")
                mTplTime = n;
        }
        else
            qWarning() << "SVG group with no template : " << n.attribute("id");
    }
    mTplRoot = e;

    return true;
}

bool SvgView::loadTemplate(QString fileName)
{
    QFile file(fileName);

    // Read SVG file as pure XML
    file.open(QIODevice::ReadOnly);
    mTplDocument.setContent(&file);
    file.close();

    // Sanity check
    QDomElement e = mTplDocument.documentElement();
    if (e.nodeName() != "svg")
        return false;

    mTplHeader.clear();
    mTplTask.clear();
    mTplTime.clear();

    // Parse XML, search VLE templates
    for(QDomElement n = e.firstChildElement(); !n.isNull(); n = n.nextSiblingElement())
    {
        // Only "svg groups" are relevant
        if (n.nodeName() != "g")
            continue;

        if ( n.hasAttribute("vle:template") )
        {
            QString tplName = n.attribute("vle:template");
            if (tplName == "")
                continue;

            if (tplName == "header")
                mTplHeader = n;
            else if (tplName == "task")
                mTplTask = n;
            else if (tplName == "time")
                mTplTime = n;
        }
        else
            qWarning() << "SVG group with no template : " << n.attribute("id");
    }
    mTplRoot = e;

    return true;
}

void SvgView::refresh(void)
{
    QGraphicsScene *s = scene();
    QPixmap Image(mSvgRenderer->defaultSize());
    QPainter Painter;

    Image.fill(Qt::transparent);

    Painter.begin(&Image);
    mSvgRenderer->render(&Painter);
    Painter.end();
    s->clear();
    s->addPixmap(Image);
}

void SvgView::reload(void)
{
    if (mPlan)
        loadPlan(mPlan);
}

QString SvgView::getConfig(QString c, QString key)
{
    SvgViewConfig *entry = NULL;

    for (int i = 0; i < mConfig.count(); i++)
    {
        entry = mConfig.at(i);
        if (entry->getName() == c)
            break;
    }
    if (entry == NULL)
        return QString();
    return entry->getKey(key);
}

void SvgView::setConfig(QString c, QString key, QString value)
{
    SvgViewConfig *entry = NULL;
    for (int i = 0; i < mConfig.count(); i++)
    {
        SvgViewConfig *e;
        e = mConfig.at(i);
        if (e->getName() == c)
        {
            entry = e;
            break;
        }
    }

    if (entry == NULL)
    {
        entry = new SvgViewConfig();
        entry->setName(c);
        mConfig.push_back(entry);
    }
    if ( ! value.isEmpty())
        entry->setKey(key, value);
    else
        entry->removeKey(key);
}

void SvgView::setZommFactor(qreal factor)
{
    mZoomFactor = factor;
}

void SvgView::mouseMoveEvent(QMouseEvent *event)
{
    if (mPlan == NULL)
        return;

    // Search the group at the current mouse Y
    QPoint pos = event->pos();
    int mouseGroup = pos.y() / mGroupHeight;
    // If mouse is outside the plan, nothing to do
    if ( (mouseGroup == 0) ||
         (mouseGroup > mPlan->countGroups()) )
    {
        if (QToolTip::isVisible())
            QToolTip::hideText();
        return;
    }

    vlePlanGroup *planGroup = mPlan->getGroup(mouseGroup - 1);
    QDate dateStart = mPlan->dateStart();

    // Get mouse X position
    int currentX = horizontalScrollBar()->value() + pos.x();
    int mouseTimePos = (currentX - 120);

    // Search if the mouse is over one acivity of the current group
    for (int j = 0; j < planGroup->count(); j++)
    {
        vlePlanActivity *planActivity = planGroup->getActivity(j);
        // Convert activity date to "days from the begining of the plan"
        int dataOffsetStart = dateStart.daysTo(planActivity->dateStart());
        int dateOffsetEnd   = dateStart.daysTo(planActivity->dateEnd());
        // Convert this number of days to pixels/coordinates
        int startPos = (dataOffsetStart * mPixelPerDay * mZoomLevel);
        int endPos   = (dateOffsetEnd   * mPixelPerDay * mZoomLevel);
        // Compare activity start/end to the current mouse position
        if ( (mouseTimePos >= startPos) &&
             (mouseTimePos <= endPos) )
        {
            if ( ( planActivity->attributeCount() ) &&
                 ( ! QToolTip::isVisible()) )
            {
                QString newMsg(planActivity->getName());

                for (int k = 0; k < planActivity->attributeCount(); k++)
                    newMsg += "\n" + planActivity->getAttribute(k);
                QRect tipPos(pos.x() - 10, pos.y() - 10, 20, 20);
                QToolTip::showText(event->globalPos(), newMsg, this, tipPos);
            }
        }
    }
}

void SvgView::wheelEvent(QWheelEvent* event)
{
    if(event->delta() > 0)
        mZoomLevel = (mZoomLevel * mZoomFactor);
    else
    {
        if (mZoomLevel > 0.4)
            mZoomLevel = (mZoomLevel / mZoomFactor);
    }

    loadPlan(mPlan);
}

void SvgView::updateAttr(QDomNode &node, QString selector, QString tag, QString value, bool replace)
{
    if ( ! node.isElement())
        return;

    QDomElement e = node.toElement();

    if (e.hasAttribute("vle:selector") && (e.attribute("vle:selector") == selector))
    {
        if (replace)
            e.setAttribute(tag, value);
        else
        {
            QString newVal = e.attribute(tag);
            newVal.append(value);
            e.setAttribute(tag, newVal);
        }
    }

    // Continue search across child nodes
    for(QDomNode n = e.firstChild(); !n.isNull(); n = n.nextSibling())
        updateAttr(n, selector, tag, value, replace);
}

void SvgView::updateField(QDomNode &e, QString tag, QString value)
{
    if ( e.nodeType() == QDomNode::TextNode)
    {
        if (e.nodeValue() == tag)
            e.setNodeValue(value);
    }
    else
    {
        for(QDomNode n = e.firstChild(); !n.isNull(); n = n.nextSibling())
            updateField(n, tag, value);
    }
}

void SvgView::updatePos(QDomElement &e, int x, int y)
{
    QString v = QString("translate(%1,%2)").arg(x).arg(y);
    e.setAttribute("transform", v);
}
