/*
 * This file is part of VLE, a framework for multi-modeling, simulation
 * and analysis of complex dynamical systems.
 * http://www.vle-project.org
 *
 * Copyright (c) 2014-2015 INRA http://www.inra.fr
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

#include <iostream>
#include <cmath>

#include <QGraphicsSceneMouseEvent>
#include <QDrag>
#include <QMimeData>

#include "ForresterPanel.h"
#include "ForresterLeftWidget.h"
#include "ui_leftWidget.h"

namespace gvle {
namespace forrester {

using namespace vle::gvle;

/********************** Not in API *******************************/

enum {
    Compartement     = QGraphicsItem::UserType + 1,
    Parameter        = QGraphicsItem::UserType + 2,
    MaterialFlow     = QGraphicsItem::UserType + 3,
    SoftLink         = QGraphicsItem::UserType + 4,
    SoftLinkPath     = QGraphicsItem::UserType + 5,
    HardLinkPath     = QGraphicsItem::UserType + 6
};

QVector<QPointF>
eqParam(QPointF p1, QPointF p2, QPointF p3)
{
    //we use parametric equations to solve the curved line that gets from
    //p1 to p3 going through p2
    //Let t in [0,1]:
    // x(t) = a t^2 + b t + c
    // y(t) = c t^2 + e t + f
    // t=0 then we are in p1:
    //    x(0) = c = x1
    //    y(0) = f = y1
    // t=alpha (parameter) then we are in p2:
    //    x(alpha) = a alpha^2 + b alpha + c = x2
    //    y(alpha) = d alpha^2 + e alpha + f = y2
    // t=1 then we are in p3:
    //    x(1) = a + b + c = x3
    //    y(1) = d + e + f = y3
    // then the pb to solve is:
    //  c = x1
    //  f = y1
    //  a alpha^2 + b alpha + c = x2
    //  d alpha^2 + e alpha + f = y2
    //  a + b + c = x3
    //  d + e + f = y3
    // Solution is:
    //  c = x1
    //  f = y1
    //  b = (x2 - x3 alpha^2 + x1 alpha^2 - x1)/(alpha - alpha^2)
    //  e = (y2 - y3 alpha^2 + y1 alpha^2 - y1)/(alpha - alpha^2)
    //  a = x3 - b - x1
    //  d = y3 - e - y1

    float alpha = 0.5;
    float c = p1.x();
    float f = p1.y();
    float alpha2 = alpha*alpha;
    float b = (p2.x() - p3.x()*alpha2 + p1.x()*alpha2 - p1.x())
            /(alpha - alpha2);
    float e = (p2.y() - p3.y()*alpha2 + p1.y()*alpha2 - p1.y())
            /(alpha - alpha2);
    float a = p3.x() - b - p1.x();
    float d = p3.y() - e - p1.y();

    int DISCR = 10;
    QVector<QPointF> points;
    float dt = (1 - 0)/(float)DISCR;
    for (int i =0; i<DISCR; i++) {
        float ti = dt*(float)(i+1);
        points.append(QPointF(
                a * ti*ti+ b*ti+ c,
                d * ti*ti+ e*ti+ f
                ));
    }
    return points;
}


void intersection(float s1, float i1, float s2, float i2,
        QPointF& inter)
{
    inter.setX((i1-i2)/(s2-s1));
    inter.setY(s1*inter.x() + i1);
}

//intersection point of a line between orig and the middle of a compartment
//and the compartment itself (one oits 4 lines)

QPointF
intersection(QGraphicsItem* item, QPointF orig)
{
    QRectF itemRect(item->pos(),
            QPointF(item->pos().x() + item->boundingRect().bottomRight().x(),
                    item->pos().y() + item->boundingRect().bottomRight().y()));
    //= comp->boundingRect();
    QLineF origToCenter(itemRect.center(), orig);
    QPointF inter(0,0);

    QLineF try_line (itemRect.topLeft(), itemRect.topRight());
    if (origToCenter.intersect(try_line, &inter) ==
            QLineF::BoundedIntersection) {
        return inter;
    }
    try_line = QLineF(itemRect.topRight(), itemRect.bottomRight());
    if (origToCenter.intersect(try_line, &inter) ==
            QLineF::BoundedIntersection) {
        return inter;
    }
    try_line = QLineF(itemRect.bottomRight(), itemRect.bottomLeft());
    if (origToCenter.intersect(try_line, &inter) ==
            QLineF::BoundedIntersection) {
        return inter;
    }
    try_line = QLineF(itemRect.bottomLeft(), itemRect.topLeft());
    if (origToCenter.intersect(try_line, &inter) ==
            QLineF::BoundedIntersection) {
        return inter;
    }
    return orig;
}


QColor
colorObj(bool selected)
{
    if (selected) {
        return QColor(255, 0, 0);
    } else {
        return QColor(0, 0, 255);
    }

}

QPen penContourObj(bool selected)
{

    return QPen(colorObj(selected), 3, Qt::SolidLine);

}

QPen penParameter(bool selected)
{
    return QPen(colorObj(selected), 2, Qt::DashDotLine);
}

QPen penSoftLinkPath()
{
    return QPen(QColor(0, 0, 0), 1.5, Qt::DashLine);
}

QPen penHardLinkPath()
{
    return QPen(QColor(0, 0, 0), 4, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin);
}

QGraphicsLineItem* newSoftLinkPath(QPointF orig, QPointF dest)
{
    QGraphicsLineItem* line = new QGraphicsLineItem(
            QLineF(orig, dest));
    line->setData(0, QVariant((int) SoftLinkPath));
    line->setPen(penSoftLinkPath());
    return line;
}

QGraphicsLineItem* newHardLinkPath(QPointF orig, QPointF dest)
{
    QGraphicsLineItem* line = new QGraphicsLineItem(
            QLineF(orig, dest));
    line->setData(0, QVariant((int) HardLinkPath));
    line->setPen(penHardLinkPath());
    return line;
}

QPointF
centerQGraphicsItem(QGraphicsItem* item)
{
    float x = item->pos().x()+item->boundingRect().x()
            + item->boundingRect().width()/2.0;

    float y = item->pos().y()+item->boundingRect().y()
            + item->boundingRect().height()/2.0;
    return QPointF(x,y);
}


QString
type_str(int t)
{
    switch (t) {
    case Compartement:
        return "Compartement";
    case MaterialFlow:
        return "MaterialFlow";
    case SoftLink:
        return "SoftLink";
    case 2:
        return "QGraphicsPathItem";
    case 3:
        return "QGraphicsRectItem";
    case 4:
        return "QGraphicsEllipseItem";
    case 6:
        return "QGraphicsLineItem";
    case 9:
        return "QGraphicsSimpleTextItem";
    default:
        return "unknown";
    }
}

/********************** ForresterCompartment *******************************/

ForresterCompartment::ForresterCompartment(QDomNode n,
        vleSmForrester* smForrester) : sm(smForrester), node(n)
{
    QGraphicsTextItem* nameItem = new QGraphicsTextItem(
            DomFunctions::attributeValue(node, "name"), this);
    nameItem->setFlag(QGraphicsItem::ItemIsSelectable, false);
    QGraphicsItem::setFlag(QGraphicsItem::ItemIsMovable, false);
    QGraphicsItem::setFlag(QGraphicsItem::ItemIsSelectable, true);
    update();
}

ForresterCompartment::~ForresterCompartment()
{
}

int
ForresterCompartment::type() const
{
    return Compartement;
}


QRectF
ForresterCompartment::boundingRect() const
{
    return childrenBoundingRect();
}
void
ForresterCompartment::paint(QPainter */*painter*/,
        const QStyleOptionGraphicsItem */*option*/,
        QWidget */*widget*/)
{
}
void
ForresterCompartment::update(const QRectF & /*rect*/)
{
    QList<QGraphicsItem *>  children = childItems();
    for (int i =0; i<children.length(); i++) {
        if (children.at(i)->type() == 3) {//3 for QGraphicsRectItem
            delete children.at(i);

        }
    }
    QGraphicsTextItem* nameItem = getNameItem();
    new QGraphicsRectItem(
            QRectF(0,0,nameItem->boundingRect().width(),
                    nameItem->boundingRect().height()), this);
    QGraphicsRectItem* compRect = getRectangle();
    compRect->setPen(penContourObj(isSelected()));
    getNameItem()->setDefaultTextColor(colorObj(isSelected()));
}

QString
ForresterCompartment::name() const
{
    return DomFunctions::attributeValue(node, "name");
}

QGraphicsRectItem*
ForresterCompartment::getRectangle() const
{
    QList<QGraphicsItem *>  children = childItems();
    for (int i =0; i<children.length(); i++) {
        if (children.at(i)->type() == 3) {//3 for QGraphicsRectItem
            return static_cast<QGraphicsRectItem*>(children.at(i));
        }
    }
    return 0;
}




QGraphicsTextItem*
ForresterCompartment::getNameItem() const
{
    QList<QGraphicsItem *>  children = childItems();
    for (int i =0; i<children.length(); i++) {
        if (children.at(i)->type() == 8) {//8 for QGraphicsTextItem
            return static_cast<QGraphicsTextItem*>(children.at(i));
        }
    }
    return 0;
}

void
ForresterCompartment::setNameEdition(bool val)
{
    QGraphicsTextItem* textItem = getNameItem();
    if (val and textItem->textInteractionFlags() == Qt::NoTextInteraction) {
        textItem->setTextInteractionFlags(Qt::TextEditorInteraction);
        textItem->setFocus(Qt::ActiveWindowFocusReason); // this gives the item keyboard focus
        textItem->setSelected(true); // this ensures that itemChange() gets called when we click out of the item
    } else {
        QTextCursor cursor(textItem->textCursor());
        cursor.clearSelection();
        textItem->setTextCursor(cursor);
        textItem->setTextInteractionFlags(Qt::NoTextInteraction);
        textItem->setSelected(false);
        textItem->clearFocus();
        if (name() != getNameItem()->toPlainText()) {
            sm->renameCompartmentToDoc(name(), getNameItem()->toPlainText());
            update();
        }

    }
}


/********************** ForresterParameter *******************************/

ForresterParameter::ForresterParameter(QDomNode n,
        vleSmForrester* smForrester) : sm(smForrester), node(n)
{
    QGraphicsTextItem* nameItem = new QGraphicsTextItem(
            DomFunctions::attributeValue(node, "name"), this);
    nameItem->setFlag(QGraphicsItem::ItemIsSelectable, false);

    QGraphicsItem::setFlag(QGraphicsItem::ItemIsMovable, false);
    QGraphicsItem::setFlag(QGraphicsItem::ItemIsSelectable, true);
    update();
}

ForresterParameter::~ForresterParameter()
{
}

int
ForresterParameter::type() const
{
    return Parameter;
}


QRectF
ForresterParameter::boundingRect() const
{
    return childrenBoundingRect();
}
void
ForresterParameter::paint(QPainter */*painter*/,
        const QStyleOptionGraphicsItem */*option*/,
        QWidget */*widget*/)
{
}
void
ForresterParameter::update(const QRectF & /*rect*/)
{
    QList<QGraphicsItem *>  children = childItems();
    for (int i =0; i<children.length(); i++) {
        if (children.at(i)->type() == 3) {//3 for QGraphicsRectItem
            delete children.at(i);

        }
    }
    QGraphicsTextItem* nameItem = getNameItem();
    new QGraphicsRectItem(
            QRectF(0,0,nameItem->boundingRect().width(),
                    nameItem->boundingRect().height()), this);
    QGraphicsRectItem* compRect = getRectangle();
    compRect->setPen(penParameter(isSelected()));
    getNameItem()->setDefaultTextColor(colorObj(isSelected()));
}

QString
ForresterParameter::name() const
{
    return DomFunctions::attributeValue(node, "name");
}

QGraphicsRectItem*
ForresterParameter::getRectangle() const
{
    QList<QGraphicsItem *>  children = childItems();
    for (int i =0; i<children.length(); i++) {
        if (children.at(i)->type() == 3) {//3 for QGraphicsRectItem
            return static_cast<QGraphicsRectItem*>(children.at(i));
        }
    }
    return 0;
}

QGraphicsTextItem*
ForresterParameter::getNameItem() const
{
    QList<QGraphicsItem *>  children = childItems();
    for (int i =0; i<children.length(); i++) {
        if (children.at(i)->type() == 8) {//8 for QGraphicsTextItem
            return static_cast<QGraphicsTextItem*>(children.at(i));
        }
    }
    return 0;
}

void
ForresterParameter::setNameEdition(bool val)
{
    QGraphicsTextItem* textItem = getNameItem();
    if (val and textItem->textInteractionFlags() == Qt::NoTextInteraction) {
        textItem->setTextInteractionFlags(Qt::TextEditorInteraction);
        textItem->setFocus(Qt::ActiveWindowFocusReason); // this gives the item keyboard focus
        textItem->setSelected(true); // this ensures that itemChange() gets called when we click out of the item
    } else {
        QTextCursor cursor(textItem->textCursor());
        cursor.clearSelection();
        textItem->setTextCursor(cursor);
        textItem->setTextInteractionFlags(Qt::NoTextInteraction);
        textItem->setSelected(false);
        textItem->clearFocus();
        if (name() != getNameItem()->toPlainText()) {
            sm->renameParameterToDoc(name(), getNameItem()->toPlainText());
            update();
        }

    }
}


/***************************************************/


ForresterMaterialFlow::ForresterMaterialFlow(QDomNode n,
        vleSmForrester* smForrester) : sm(smForrester), node(n)
{
    new QGraphicsTextItem(DomFunctions::attributeValue(node, "name"), this);
    QGraphicsItem::setFlag(QGraphicsItem::ItemIsMovable, false);
    QGraphicsItem::setFlag(QGraphicsItem::ItemIsSelectable, true);
    update();
}

ForresterMaterialFlow::~ForresterMaterialFlow()
{
}


int
ForresterMaterialFlow::type() const
{
    return MaterialFlow;
}


QRectF
ForresterMaterialFlow::boundingRect() const
{
    return childrenBoundingRect();
}
void
ForresterMaterialFlow::paint(QPainter */*painter*/,
        const QStyleOptionGraphicsItem */*option*/,
        QWidget */*widget*/)
{
}
void
ForresterMaterialFlow::update(const QRectF & /*rect*/)
{
    //remove lines
    QList<QGraphicsItem *>  children = childItems();
    for (int i =0; i<children.length(); i++) {
        if (children.at(i)->type() == 6) {//6 for QGraphicsLineItem
            delete children.at(i);
        }
    };

    //add liens to form the flow

    QGraphicsTextItem* nameItem = getNameItem();
    nameItem->setDefaultTextColor(colorObj(isSelected()));

    double SPACE = 0;
    double HAT_SPACE = 30;

    QPen linePen = penContourObj(isSelected());

    //   __   // line 6
    //   \/   // line 5 and 4
    //   /\   // line 4 and 5
    //  |  |  // line 2 and 3
    //  ----  // line 1


    //line 1
    QGraphicsLineItem* line = new QGraphicsLineItem(
            nameItem->pos().x()+SPACE,
            nameItem->pos().y()+nameItem->boundingRect().height()+SPACE,
            nameItem->pos().x()+nameItem->boundingRect().width()+SPACE,
            nameItem->pos().y()+nameItem->boundingRect().height()+SPACE,
            this);
    line->setPen(linePen);
    //line 2
    line = new QGraphicsLineItem(
            nameItem->pos().x()+SPACE,
            nameItem->pos().y()+nameItem->boundingRect().height()+SPACE,
            nameItem->pos().x()+SPACE,
            nameItem->pos().y()+SPACE,
            this);
    line->setPen(linePen);
    //line 3
    line = new QGraphicsLineItem(
            nameItem->pos().x()+nameItem->boundingRect().width()+SPACE,
            nameItem->pos().y()+nameItem->boundingRect().height()+SPACE,
            nameItem->pos().x()+nameItem->boundingRect().width()+SPACE,
            nameItem->pos().y()+SPACE,
            this);
    line->setPen(linePen);
    //line 4
    line = new QGraphicsLineItem(
            nameItem->pos().x()+SPACE,
            nameItem->pos().y()+SPACE,
            nameItem->pos().x()+nameItem->boundingRect().width()+SPACE,
            nameItem->pos().y()-HAT_SPACE,
            this);
    line->setPen(linePen);
    //line 5
    line = new QGraphicsLineItem(
            nameItem->pos().x()+SPACE,
            nameItem->pos().y()-HAT_SPACE,
            nameItem->pos().x()+nameItem->boundingRect().width()+SPACE,
            nameItem->pos().y()+SPACE,
            this);
    line->setPen(linePen);
    //line 6
    line = new QGraphicsLineItem(
            nameItem->pos().x()+SPACE,
            nameItem->pos().y()-HAT_SPACE,
            nameItem->pos().x()+nameItem->boundingRect().width()+SPACE,
            nameItem->pos().y()-HAT_SPACE,
            this);
    line->setPen(linePen);



}

QGraphicsTextItem*
ForresterMaterialFlow::getNameItem() const
{
    QList<QGraphicsItem *>  children = childItems();
    for (int i =0; i<children.length(); i++) {
        if (children.at(i)->type() == 8) {//8 for QGraphicsTextItem
            return static_cast<QGraphicsTextItem*>(children.at(i));
        }
    }
    return 0;
}

void
ForresterMaterialFlow::setNameEdition(bool val)
{
    QGraphicsTextItem* textItem = getNameItem();
    if (val and textItem->textInteractionFlags() == Qt::NoTextInteraction) {
        textItem->setTextInteractionFlags(Qt::TextEditorInteraction);
        textItem->setFocus(Qt::ActiveWindowFocusReason); // this gives the item keyboard focus
        textItem->setSelected(true); // this ensures that itemChange() gets called when we click out of the item
    } else {
        QTextCursor cursor(textItem->textCursor());
        cursor.clearSelection();
        textItem->setTextCursor(cursor);
        textItem->setTextInteractionFlags(Qt::NoTextInteraction);
        textItem->setSelected(false);
        textItem->clearFocus();
        if (name() != getNameItem()->toPlainText()) {
            sm->renameMaterialFlowToDoc(name(), getNameItem()->toPlainText());
            update();
        }

    }
}

QString
ForresterMaterialFlow::name() const
{
    return DomFunctions::attributeValue(node, "name");
}

/***************************************************
 *
 *  ForresterSoftLink
 *
 * **************************************************/


ForresterSoftLink::ForresterSoftLink(QDomNode /*dep*/,
        ForresterCompartment* _comp, ForresterMaterialFlow* _flow):
                comp(_comp), param(0), flow(_flow)
{
    reload();
    update();
}

ForresterSoftLink::ForresterSoftLink(QDomNode /*dep*/,
        ForresterParameter* _param, ForresterMaterialFlow* _flow):
        comp(0), param(_param), flow(_flow)
{
    reload();
    update();
}

ForresterSoftLink::~ForresterSoftLink()
{
    clear();
}

void ForresterSoftLink::clear()
{
}

void
ForresterSoftLink::reload()
{
    clear();
    new QGraphicsEllipseItem(0,0,10,10, this);
    QGraphicsItem::setFlag(QGraphicsItem::ItemIsMovable, false);
    QGraphicsItem::setFlag(QGraphicsItem::ItemIsSelectable, true);
}

QGraphicsEllipseItem*
ForresterSoftLink::getEllipse() const
{
    QList<QGraphicsItem *>  children = childItems();
    for (int i =0; i<children.length(); i++) {
        if (children.at(i)->type() == 4) {//QGraphicsEllipseItem
            return static_cast<QGraphicsEllipseItem*>(children.at(i));
        }
    }
    return 0;
}

QGraphicsItem*
ForresterSoftLink::varItem() const
{
    if (comp) {
        return comp;
    } else {
        return param;
    }

}

QGraphicsItem*
ForresterSoftLink::matFlowItem() const
{
    return flow;
}

QString
ForresterSoftLink::varName() const
{
    if (comp) {
        return comp->name();
    } else {
        return param->name();
    }
}

QString
ForresterSoftLink::matFlowName() const
{
    return flow->name();
}

int
ForresterSoftLink::type() const
{
    return SoftLink;
}




QRectF
ForresterSoftLink::boundingRect() const
{
    return childrenBoundingRect();
}
void
ForresterSoftLink::paint(QPainter */*painter*/,
        const QStyleOptionGraphicsItem */*option*/,
        QWidget */*widget*/)
{
}
void
ForresterSoftLink::update(const QRectF & /*rect*/)
{
    QGraphicsEllipseItem* ellipse = getEllipse();
    ellipse->setPen(penContourObj(isSelected()));
}

/***************************************************/

ForresterHardLink::ForresterHardLink(ForresterMaterialFlow* _flow,
        ForresterCompartment* _comp): QGraphicsLineItem(), flow(_flow),
                comp(_comp), flow_to_comp(true)
{
}

ForresterHardLink::ForresterHardLink(ForresterCompartment* _comp,
        ForresterMaterialFlow* _flow): QGraphicsLineItem(), flow(_flow),
                comp(_comp), flow_to_comp(false)
{
}

ForresterHardLink::~ForresterHardLink()
{
}

int
ForresterHardLink::type() const
{
    return HardLinkPath;
}

QRectF
ForresterHardLink::boundingRect() const
{
    qreal extra = (pen().width() + 20) / 2.0;

    return QRectF(line().p1(), QSizeF(line().p2().x() - line().p1().x(),
            line().p2().y() - line().p1().y()))
            .normalized()
            .adjusted(-extra, -extra, extra, extra);
}

QPainterPath
ForresterHardLink::shape() const
{
    QPainterPath path = QGraphicsLineItem::shape();
    path.addPolygon(arrowHead);
    return path;
}

void
ForresterHardLink::paint(QPainter* painter,
        const QStyleOptionGraphicsItem */*option*/,
        QWidget */*widget*/)
{

    qreal arrowSize = 10;
    painter->setPen(penHardLinkPath());
    if (flow_to_comp) {
        setLine(QLineF(intersection(comp, centerQGraphicsItem(flow)),
            intersection(flow, centerQGraphicsItem(comp))));
    } else {
        setLine(QLineF(intersection(flow, centerQGraphicsItem(comp)),
                intersection(comp, centerQGraphicsItem(flow))));
    }

    double angle = ::acos(line().dx() / line().length());
    double Pi = std::acos(-1);

    if (line().dy() >= 0) {
        angle = (Pi * 2) - angle;
    }

    QPointF arrowP1 = line().p1() + QPointF(std::sin(angle + Pi / 3) * arrowSize,
                                    cos(angle + Pi / 3) * arrowSize);
    QPointF arrowP2 = line().p1() + QPointF(sin(angle + Pi - Pi / 3) * arrowSize,
                                    cos(angle + Pi - Pi / 3) * arrowSize);

    arrowHead.clear();
    arrowHead << line().p1() << arrowP1 << arrowP2;

    // Fill polygon
    QPainterPath path;
    path.addPolygon(arrowHead);

    // Draw polygon
    painter->drawPolygon(arrowHead);
    painter->fillPath(path, QBrush(QColor("black")));
    painter->drawLine(line());
}

void
ForresterHardLink::update(const QRectF & /*rect*/)
{
}

/***************************************************/

ForresterScene::ForresterScene(ForresterPanel* p): forrester(p)
{
}

void
ForresterScene::reload()
{
//    forrester->left->ui->graphicsView->installEventFilter(
//            new GenericQEventFilterVle(forrester, this));

    clear();

    //reload compartments
    QDomNodeList compsXml = forrester->cppMetadata->compartmentsFromDoc();
    for (int i = 0; i < compsXml.length(); i++) {

        QDomNode comp = compsXml.item(i);
        QString name = comp.attributes().namedItem("name").nodeValue();
        ForresterCompartment* compItem = new ForresterCompartment(comp,
                forrester->cppMetadata);
        addItem(compItem);
        QPointF pos(comp.attributes().namedItem("x").nodeValue().toFloat(),
                comp.attributes().namedItem("y").nodeValue().toFloat());
        compItem->setPos(pos);

    }

    //reload parameters
    QDomNodeList paramsXml = forrester->cppMetadata->parametersFromDoc();
    for (int i = 0; i < paramsXml.length(); i++) {

        QDomNode param = paramsXml.item(i);
        QString name = param.attributes().namedItem("name").nodeValue();
        ForresterParameter* paramItem = new ForresterParameter(param,
                forrester->cppMetadata);
        addItem(paramItem);
        QPointF pos(param.attributes().namedItem("x").nodeValue().toFloat(),
                param.attributes().namedItem("y").nodeValue().toFloat());
        paramItem->setPos(pos);

    }

    //reload flows
    QDomNodeList matsXml = forrester->cppMetadata->materialFlowsFromDoc();
    for (int i = 0; i < matsXml.length(); i++) {

        QDomNode mat = matsXml.item(i);
        QString name = mat.attributes().namedItem("name").nodeValue();
        ForresterMaterialFlow* matItem = new ForresterMaterialFlow(mat,
                forrester->cppMetadata);
        addItem(matItem);
        QPointF pos(mat.attributes().namedItem("x").nodeValue().toFloat(),
                mat.attributes().namedItem("y").nodeValue().toFloat());
        matItem->setPos(pos);
    }

    reloadSoftLinks();
}


void
ForresterScene::reloadSoftLinks()
{
    clearSoftLinks();
    QDomNodeList matsXml = forrester->cppMetadata->materialFlowsFromDoc();
    for (int i = 0; i < matsXml.length(); i++) {
        QDomNode mat = matsXml.item(i);
        QString name = mat.attributes().namedItem("name").nodeValue();
        ForresterMaterialFlow* matItem = itemMaterialFlow(name);
        //reload soft link
        for (QDomNode dep : DomFunctions::childNodesWithoutText(
                mat, "depends_on")) {
            QString otherName = DomFunctions::attributeValue(dep, "name");
            if (forrester->cppMetadata->existCompToDoc(otherName)) {
                ForresterCompartment* compItem = itemCompartment(otherName);
                QPointF soft_link_point(
                    QVariant(DomFunctions::attributeValue(dep, "x")).toFloat(),
                    QVariant(DomFunctions::attributeValue(dep, "y")).toFloat());
                ForresterSoftLink* soft_link_item = new ForresterSoftLink(dep,
                        compItem, matItem);
                addItem(soft_link_item);
                soft_link_item->setPos(soft_link_point);
            } else if (forrester->cppMetadata->existParameterToDoc(otherName)){
                ForresterParameter* paramItem = itemParameter(otherName);
                QPointF soft_link_point(
                    QVariant(DomFunctions::attributeValue(dep, "x")).toFloat(),
                    QVariant(DomFunctions::attributeValue(dep, "y")).toFloat());
                ForresterSoftLink* soft_link_item = new ForresterSoftLink(dep,
                        paramItem, matItem);
                addItem(soft_link_item);
                soft_link_item->setPos(soft_link_point);
            }
        }
    }
    reloadPath();
}

void
ForresterScene::reloadHardLinks()
{
    clearHardLinks();
    QDomNodeList matsXml = forrester->cppMetadata->materialFlowsFromDoc();
    for (int i = 0; i < matsXml.length(); i++) {
        QDomNode mat = matsXml.item(i);
        QString name = mat.attributes().namedItem("name").nodeValue();
        ForresterMaterialFlow* matItem = itemMaterialFlow(name);
        //reload hard link to
        QString compName = forrester->cppMetadata->hardLinkTo(name);
        if (compName != "") {
            ForresterCompartment* compItem = itemCompartment(compName);
            addItem(new ForresterHardLink(matItem, compItem));
        }
        //reload hard link to
        compName = forrester->cppMetadata->hardLinkFrom(name);
        if (compName != "") {
            ForresterCompartment* compItem = itemCompartment(compName);
            addItem(new ForresterHardLink(compItem, matItem));
        }
    }
}

void
ForresterScene::reloadPath()
{
    clearPath();

    QList<QGraphicsItem *> itemsIn = items();
    for (auto it : itemsIn) {
        if (it->type() == SoftLink){//compartment
            ForresterSoftLink* sl =
                    static_cast<ForresterSoftLink*>(it);

            QPointF softlink = centerQGraphicsItem(sl);
            QPointF orig = intersection(sl->varItem(), softlink);
            QPointF dest = intersection(sl->matFlowItem(), softlink);

            QVector<QPointF> points = eqParam(orig, softlink, dest);
            QGraphicsLineItem* line = newSoftLinkPath(orig, points[0]);
            addItem(line);
            for (int p=1; p<points.length(); p++) {
                line = newSoftLinkPath(points[p-1], points[p]);
                addItem(line);
            }

        }
    }
    reloadHardLinks();
}

void
ForresterScene::unselectAllItems()
{
    QList<QGraphicsItem *> itemsIn = items();
    for (auto it : itemsIn) {
        if (it->type() == Compartement){
            ForresterCompartment* comp =
                    static_cast<ForresterCompartment*>(it);
            comp->setSelected(false);
            comp->update();
        } else if (it->type() == Parameter){
            ForresterParameter* param =
                    static_cast<ForresterParameter*>(it);
            param->setSelected(false);
            param->update();
        } else if (it->type() == MaterialFlow){
            ForresterMaterialFlow* mat =
                    static_cast<ForresterMaterialFlow*>(it);
            mat->setSelected(false);
            mat->update();
        } else if (it->type() == SoftLink){
            ForresterSoftLink* sl =
                    static_cast<ForresterSoftLink*>(it);
            sl->setSelected(false);
            sl->update();
        }
    }
}

void
ForresterScene::removeNameEditionMode()
{

    QList<QGraphicsItem*>  children = items();
    for (int i =0; i<children.length(); i++) {
        if (children.at(i)) {
            if (children.at(i)->type() == MaterialFlow) {
                static_cast<ForresterMaterialFlow*>(
                        children.at(i))->setNameEdition(false);
            } else if (children.at(i)->type() == Compartement) {
                static_cast<ForresterCompartment*>(
                        children.at(i))->setNameEdition(false);
            } else if (children.at(i)->type() == Parameter) {
                static_cast<ForresterParameter*>(
                        children.at(i))->setNameEdition(false);

            }

        }
    }
}

void
ForresterScene::clearPath()
{
    QList<QGraphicsItem *> itemsIn = items();

    for (auto it : itemsIn) {
        if (it->type() == 6 //QGraphicsLineItem type
                and it->data(0).isValid()
                and (it->data(0).toInt() == SoftLinkPath)){
            removeItem(it);
            //delete it;
        }
        if (it->type() == 6 //QGraphicsLineItem type
                and it->data(0).isValid()
                and (it->data(0).toInt() == HardLinkPath)){
            removeItem(it);
            //delete it;
        }
    }
}

void
ForresterScene::clearSoftLinks()
{
    QList<QGraphicsItem *> itemsIn = items();

    for (auto it : itemsIn) {
        if (it->type() == SoftLink) {
            removeItem(it);
            //delete it;
        }
    }
}

void
ForresterScene::clearHardLinks()
{
    QList<QGraphicsItem *> itemsIn = items();

    for (auto it : itemsIn) {
        if (it->type() == HardLinkPath) {
            removeItem(it);
            //delete it;
        }
    }
}

ForresterCompartment*
ForresterScene::itemCompartment(QString compName)
{
    QList<QGraphicsItem *> itemsIn = items();
    for (auto it : itemsIn) {
        if (it->type() == Compartement){
            ForresterCompartment* comp =
                    static_cast<ForresterCompartment*>(it);
            if (comp->name() == compName) {
                return comp;
            }
        }
    }
    return 0;
}

ForresterParameter*
ForresterScene::itemParameter(QString paramName)
{
    QList<QGraphicsItem *> itemsIn = items();
    for (auto it : itemsIn) {
        if (it->type() == Parameter){
            ForresterParameter* param =
                    static_cast<ForresterParameter*>(it);
            if (param->name() == paramName) {
                return param;
            }
        }
    }
    return 0;
}


ForresterMaterialFlow*
ForresterScene::itemMaterialFlow(QString matName)
{
    QList<QGraphicsItem *> itemsIn = items();
    for (auto it : itemsIn) {
        if (it->type() == MaterialFlow){
            ForresterMaterialFlow* mat =
                    static_cast<ForresterMaterialFlow*>(it);
            if (mat->name() == matName) {
                return mat;
            }
        }
    }
    return 0;
}


void
ForresterScene::contextMenuEvent(QGraphicsSceneContextMenuEvent* event)
{
    QList<QGraphicsItem*> sels = items(event->scenePos());
    ForresterCompartment* selComp = 0;
    ForresterParameter* selParam = 0;
    ForresterMaterialFlow* selMat = 0;
    for (auto it : sels) {
        if (it->type() == Compartement) {
            selComp = static_cast<ForresterCompartment*>(it);
            selParam = 0;
            selMat = 0;
        } else if (it->type() == Parameter) {
            selComp = 0;
            selParam = static_cast<ForresterParameter*>(it);
            selMat = 0;
        } else if (it->type() == MaterialFlow) {
            selComp = 0;
            selParam = 0;
            selMat = static_cast<ForresterMaterialFlow*>(it);
        }
    }

    QSet<QString> curr_comps = forrester->cppMetadata->compartmentNames();
    QSet<QString> curr_params = forrester->cppMetadata->parameterNames();
    QSet<QString> curr_mat = forrester->cppMetadata->materialFlowNames();

    QMenu menu;
    QAction* action;
    QMenu* submenu;
    QMap<QString, QVariant> action_map;

    QString obj_name = selComp ? selComp->name() :
            (selMat ? selMat->name() :
            (selParam ? selParam->name() : ""));
    action = menu.addAction("Edit name");
    action_map.insert("code", QVariant(0));
    action_map.insert("name", obj_name);
    action->setData(action_map);
    action->setEnabled(action_map["name"].toString() != "");
    action = menu.addAction("Remove");
    action_map.clear();
    action_map.insert("code", QVariant(1));
    action_map.insert("name", obj_name);
    action->setData(action_map);
    //--
    menu.addSeparator();

    action = menu.addAction("Add compartment");
    action_map.clear();
    action_map.insert("code", QVariant(2));
    action->setData(action_map);
    action = menu.addAction("Add parameter");
    action_map.clear();
    action_map.insert("code", QVariant(3));
    action->setData(action_map);
    action = menu.addAction("Add material flow");
    action_map.clear();
    action_map.insert("code", QVariant(4));
    action->setData(action_map);
    //--
    menu.addSeparator();
    submenu = menu.addMenu("Hard link to");
    submenu->setEnabled(selMat != 0 and curr_comps.size() > 0);
    if (submenu->isEnabled()) {
        for (auto comp : curr_comps) {
            action = submenu->addAction(comp);
            action->setCheckable(true);
            action->setChecked(forrester->cppMetadata->hardLinkTo(
                    selMat->name()) == comp);
            action_map.clear();
            action_map.insert("code", QVariant(5));
            action_map.insert("name", QVariant(comp));
            action->setData(action_map);

        }
    }
    submenu = menu.addMenu("Hard link from");
    submenu->setEnabled(selMat != 0 and curr_comps.size() > 0);
    if (submenu->isEnabled()) {
        for (auto comp : curr_comps) {
            action = submenu->addAction(comp);
            action->setCheckable(true);
            action->setChecked(forrester->cppMetadata->hardLinkFrom(
                    selMat->name()) == comp);
            action_map.clear();
            action_map.insert("code", QVariant(6));
            action_map.insert("name", QVariant(comp));
            action->setData(action_map);
        }
    }
    //--
    menu.addSeparator();
    submenu = menu.addMenu("Depends on compartment");
    submenu->setEnabled(selMat != 0 and curr_comps.size() > 0);
    if (submenu->isEnabled()) {
        bool enable = false;
        for (auto comp : curr_comps) {
            if (not (forrester->cppMetadata->hardLinkTo(
                    selMat->name()) == comp) and
                not (forrester->cppMetadata->hardLinkFrom(
                    selMat->name()) == comp)) {
                action = submenu->addAction(comp);
                action->setCheckable(true);
                action->setChecked(forrester->cppMetadata->usefullForFlow(
                        selMat->name(), comp));
                action_map.clear();
                action_map.insert("code", QVariant(7));
                action_map.insert("name", QVariant(comp));
                action->setData(action_map);
                enable = true;
            }
        }
        submenu->setEnabled(enable);
    }
    submenu = menu.addMenu("Depends on parameter");
    submenu->setEnabled(selMat != 0 and curr_params.size() > 0);
    if (submenu->isEnabled()) {
        for (auto param : curr_params) {
            action = submenu->addAction(param);
            action->setCheckable(true);
            action->setChecked(forrester->cppMetadata->usefullForFlow(
                    selMat->name(), param));
            action_map.clear();
            action_map.insert("code", QVariant(8));
            action_map.insert("name", QVariant(param));
            action->setData(action_map);
        }
    }


    action = menu.exec(event->screenPos());
    if (action) {

        int actCode = action->data().toMap().find("code")->toInt();
        switch (actCode) {
        case 0:{ //edit name
            if (selMat) {
                selMat->setNameEdition(true);
            }
            if (selComp) {
                selComp->setNameEdition(true);
            }
            if (selParam) {
                selParam->setNameEdition(true);
            }
            break;
        } case 1:{ //remove
            if (selMat) {
                forrester->cppMetadata->rmMateriaFlowToDoc(obj_name);
            }
            if (selComp) {
                forrester->cppMetadata->rmCompartmentToDoc(obj_name);
            }
            if (selParam) {
                forrester->cppMetadata->rmParameterToDoc(obj_name);
            }
            reload();
            emit forrester->undoAvailable(true);
            break;
        } case 2:{ //add compartment
            forrester->cppMetadata->addCompartmentToDoc(
                    forrester->cppMetadata->newCompartmentNameToDoc(),
                    event->scenePos());
            reload();
            emit forrester->undoAvailable(true);
            break;
        } case 3:{ //add parameter
            forrester->cppMetadata->addParameterToDoc(
                    forrester->cppMetadata->newParameterNameToDoc(),
                    event->scenePos());
            reload();
            emit forrester->undoAvailable(true);
            break;
        } case 4:{ //add material flow
            forrester->cppMetadata->addMaterialFlowToDoc(
                    forrester->cppMetadata->newMaterialFlowNameToDoc(),
                    event->scenePos());
            reload();
            emit forrester->undoAvailable(true);
            break;
        } case 5:{ //hard link to
            QString compName = action->data().toMap().find("name")->toString();
            forrester->cppMetadata->setHardLinkTo(selMat->name(), compName,
                    not (forrester->cppMetadata->hardLinkTo(
                            selMat->name()) == compName));
            reload();
            emit forrester->undoAvailable(true);
            break;
        } case 6:{ //hard link from
            QString compName = action->data().toMap().find("name")->toString();
            forrester->cppMetadata->setHardLinkFrom(selMat->name(), compName,
                    not (forrester->cppMetadata->hardLinkFrom(
                            selMat->name()) == compName));
            reload();
            emit forrester->undoAvailable(true);
            break;
        } case 7:{ //Depends on compartment

            QString compName = action->data().toMap().find("name")->toString();
            ForresterCompartment* compItem = itemCompartment(compName);
            bool usefull = forrester->cppMetadata->usefullForFlow(
                    selMat->name(), compName);
            forrester->cppMetadata->setUsefullForFlow(selMat->name(), compName,
                    not usefull, centerQGraphicsItem(compItem));
            reload();
            emit forrester->undoAvailable(true);
            break;
        } case 8:{ //Depends on parameter

            QString paramName = action->data().toMap().find("name")->toString();
            ForresterParameter* paramItem = itemParameter(paramName);
            bool usefull = forrester->cppMetadata->usefullForFlow(
                    selMat->name(), paramName);
            forrester->cppMetadata->setUsefullForFlow(selMat->name(), paramName,
                    not usefull, centerQGraphicsItem(paramItem));
            reload();
            emit forrester->undoAvailable(true);
            break;
        }  default:
            break;
        }
    }
}

void
ForresterScene::mousePressEvent(QGraphicsSceneMouseEvent* mouseEvent)
{
    if (mouseEvent->button() == Qt::LeftButton) {
        removeNameEditionMode();

        QList<QGraphicsItem*> items = this->items(mouseEvent->scenePos());
        QList<QGraphicsItem*> itemsAll = this->items();
//        qDebug() << " ** dbg list items mousePressEvent nbiTems:" << itemsAll.length() << " nbItemsSel=" << items.length();
//        for (int i =0; i< itemsAll.length(); i++) {
//            qDebug() << " Dbg all " << "typeint=" << itemsAll.at(i)->type()
//                    << type_str(itemsAll.at(i)->type()) << " topLevelItem "<< itemsAll.at(i)->topLevelItem()->type();
//        }
//        for (int i =0; i< items.length(); i++) {
//            qDebug() << " Dbg sel " << type_str(items.at(i)->type()) << " topLevelItem "<< items.at(i)->topLevelItem()->type();
//        }

        if(items.length() == 0) {
            unselectAllItems();
            return ;
        }

        //Look for a drag
        for (int i =0; i< items.length(); i++) {
            QGraphicsItem* it = items.at(i);

            if (it->type() == Compartement){//compartment
                ForresterCompartment* comp =
                        static_cast<ForresterCompartment*>(it);

                if (not comp->isSelected()) {
                    unselectAllItems();
                    comp->setSelected(true);
                    comp->update();
                    emit forrester->left->compSelected(comp->name());
                }
                QDrag* drag = new QDrag(mouseEvent->widget());
                drag->setMimeData(new QMimeData);
                QObject::connect(drag, SIGNAL(destroyed(QObject *)),
                        this, SLOT  (onDragDestroyed(QObject *)));
                drag->start();
                return;
            } else if (it->type() == Parameter) {
                ForresterParameter* param =
                        static_cast<ForresterParameter*>(it);
                if (not param->isSelected()) {
                    unselectAllItems();
                    param->setSelected(true);
                    param->update();
                    emit forrester->left->paramSelected(param->name());
                }
                QDrag* drag = new QDrag(mouseEvent->widget());
                drag->setMimeData(new QMimeData);
                QObject::connect(drag, SIGNAL(destroyed(QObject *)),
                        this, SLOT  (onDragDestroyed(QObject *)));
                drag->start();
                return;
            } else if (it->type() == MaterialFlow) {
                ForresterMaterialFlow* mat =
                        static_cast<ForresterMaterialFlow*>(it);
                if (not mat->isSelected()) {
                    unselectAllItems();
                    mat->setSelected(true);
                    mat->update();
                    emit forrester->left->matFlowSelected(mat->name());
                }
                QDrag* drag = new QDrag(mouseEvent->widget());
                drag->setMimeData(new QMimeData);
                QObject::connect(drag, SIGNAL(destroyed(QObject *)),
                        this, SLOT  (onDragDestroyed(QObject *)));
                drag->start();
                return;
            } else if (it->type() == SoftLink) {
                ForresterSoftLink* sl =
                        static_cast<ForresterSoftLink*>(it);
                if (not sl->isSelected()) {
                    unselectAllItems();
                    sl->setSelected(true);
                    sl->update();
                }
                QDrag* drag = new QDrag(mouseEvent->widget());
                drag->setMimeData(new QMimeData);
                QObject::connect(drag, SIGNAL(destroyed(QObject *)),
                        this, SLOT  (onDragDestroyed(QObject *)));
                drag->start();
                return;
            }
        }
    }
}

void
ForresterScene::mouseReleaseEvent(QGraphicsSceneMouseEvent * /*mouseEvent*/)
{
}
void
ForresterScene::dragEnterEvent(QGraphicsSceneDragDropEvent * /*event*/)
{
}
void
ForresterScene::dragLeaveEvent(QGraphicsSceneDragDropEvent * /*event*/)
{
}
void
ForresterScene::dragMoveEvent(QGraphicsSceneDragDropEvent * event)
{
    QList<QGraphicsItem*> sel = selectedItems();
    if (sel.size() != 1) {
        return;
    }
    if (sel.at(0)->type() == Compartement) {
        ForresterCompartment* comp =
                static_cast<ForresterCompartment*>(sel.at(0));
        QPointF p = event->scenePos();
        forrester->cppMetadata->setPositionToCompartment(
                comp->name(), p);
        comp->setPos(p);
        reloadPath();
    } else if(sel.at(0)->type() == Parameter) {
        ForresterParameter* param =
                static_cast<ForresterParameter*>(sel.at(0));
        QPointF p = event->scenePos();
        forrester->cppMetadata->setPositionToParam(
                param->name(), p);
        param->setPos(p);
        reloadPath();
    } else if(sel.at(0)->type() == MaterialFlow) {
        ForresterMaterialFlow* mat =
                static_cast<ForresterMaterialFlow*>(sel.at(0));
        QPointF p = event->scenePos();
        forrester->cppMetadata->setPositionToMaterialFlow(
                mat->name(), p);
        mat->setPos(p);
        reloadPath();
    } else if(sel.at(0)->type() == SoftLink) {
        ForresterSoftLink* sl =
                static_cast<ForresterSoftLink*>(sel.at(0));
        QPointF p = event->scenePos();
        forrester->cppMetadata->setPositionToSoftLink(sl->matFlowName(),
                sl->varName(), p);
        sl->setPos(p);
        reloadPath();
    }
}
void
ForresterScene::dropEvent(QGraphicsSceneDragDropEvent * /*event*/)
{
}

void
ForresterScene::onDragDestroyed(QObject* /*obj*/)
{
}

/*---------------------------------------------------*/

ForresterLeftWidget::ForresterLeftWidget(ForresterPanel*  f):
        QWidget(), ui(new Ui::leftWidget), mScene(new ForresterScene(f)),
        forrester(f)
{
    ui->setupUi(this);
    ui->graphicsView->setScene(mScene);
    QObject::connect(ui->content,   SIGNAL(currentChanged(int)),
                     this, SLOT  (onTabChanged(int)));

    QObject::connect(ui->comboMethod, SIGNAL(currentIndexChanged(int)),
            this, SLOT(onNumMethodUpdate(int)));
    QObject::connect(ui->timeStep, SIGNAL(valueChanged(double)),
            this, SLOT(tsChanged(double)));
    QObject::connect(ui->comboCompartment,
            SIGNAL(currentIndexChanged(const QString&)),
            this, SLOT(onCompUpdate(const QString&)));
    QObject::connect(ui->quantum, SIGNAL(valueChanged(double)),
            this, SLOT(qtChanged(double)));

}
ForresterLeftWidget::~ForresterLeftWidget()
{
    delete mScene;
    delete ui;
}

void
ForresterLeftWidget::reload()
{
    reloadGeneral();
    mScene->reload();
}

void
ForresterLeftWidget::reloadGeneral()
{
    QString meth = forrester->cppMetadata->numericalIntegration();
    bool oldBlock1 = ui->comboMethod->blockSignals(true);
    bool oldBlock2 = ui->timeStep->blockSignals(true);
    bool oldBlock3 = ui->comboCompartment->blockSignals(true);
    bool oldBlock4 = ui->quantum->blockSignals(true);

    if (meth == "euler") {
        ui->comboMethod->setCurrentIndex(0);
        ui->timeStep->setReadOnly(false);
        ui->timeStep->setValue(forrester->cppMetadata->numericalTimeStep());
        ui->comboCompartment->setEnabled(false);
        ui->quantum->setReadOnly(true);
    } else if (meth == "rk4") {
        ui->comboMethod->setCurrentIndex(1);
        ui->timeStep->setReadOnly(false);
        ui->timeStep->setValue(forrester->cppMetadata->numericalTimeStep());
        ui->comboCompartment->setEnabled(false);
        ui->quantum->setReadOnly(false);
        ui->quantum->setValue(0.0);
        ui->quantum->setReadOnly(true);
    } else if (meth == "qss2") {
        ui->comboMethod->setCurrentIndex(2);
        ui->timeStep->setReadOnly(false);
        ui->timeStep->setValue(0.0);
        ui->timeStep->setReadOnly(true);
        ui->comboCompartment->setEnabled(true);
        QString currentComp = ui->comboCompartment->currentText();
        ui->comboCompartment->clear();
        for (auto comp : forrester->cppMetadata->compartmentNames()) {
            ui->comboCompartment->addItem(comp);
        }
        if (currentComp != "") {
            ui->comboCompartment->setCurrentText(currentComp);
        }
        if (ui->comboCompartment->currentIndex() == -1) {
            ui->quantum->setReadOnly(false);
            ui->quantum->setValue(0.0);
            ui->quantum->setReadOnly(true);
        } else {
            ui->quantum->setReadOnly(false);
            ui->quantum->setValue(forrester->cppMetadata->numericalQuantum(
                    ui->comboCompartment->currentText()));

        }
    }
    ui->comboMethod->blockSignals(oldBlock1);
    ui->timeStep->blockSignals(oldBlock2);
    ui->comboCompartment->blockSignals(oldBlock3);
    ui->quantum->blockSignals(oldBlock4);
}

void
ForresterLeftWidget::onSmModified(int modifType)
{
    switch((MODIF_TYPE) modifType) {
    case RENAME:
        reloadGeneral();
        break;
    case MOVE_OBJ:
        break;
    case NUMERICAL_INT:
        reloadGeneral();
        break;
    default:
        reload();
        break;
    }
}

void
ForresterLeftWidget::onTabChanged(int)
{
    QString tab = ui->content->tabText(ui->content->currentIndex());
    forrester->cppMetadata->setCurrentTab(tab);
}

void
ForresterLeftWidget::onNumMethodUpdate(int index)
{
    if (index == 0) {//Euler
        forrester->cppMetadata->setNumericalIntegration("euler");
    } else if (index == 1) {//RK4
        forrester->cppMetadata->setNumericalIntegration("rk4");
    } else if (index == 2) {//QSS2
        forrester->cppMetadata->setNumericalIntegration("qss2");
    }

}

void
ForresterLeftWidget::tsChanged(double v)
{
    forrester->cppMetadata->setNumericalTimeStep(v);
}

void
ForresterLeftWidget::onCompUpdate(const QString& /*comp*/)
{
    bool oldBlock = ui->quantum->blockSignals(true);
    ui->quantum->setValue(forrester->cppMetadata->numericalQuantum(
                        ui->comboCompartment->currentText()));
    ui->quantum->blockSignals(oldBlock);

}

void
ForresterLeftWidget::qtChanged(double v)
{
    forrester->cppMetadata->setNumericalQuantum(
            ui->comboCompartment->currentText(), v);
}

}} //namespaces

