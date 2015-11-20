/*
 * This file is part of VLE, a framework for multi-modeling, simulation
 * and analysis of complex dynamical systems.
 * http://www.vle-project.org
 *
 * Copyright (c) 2014 INRA
 *
 */
#include <QDebug>
#include <QLinearGradient>
#include <QPainter>
#include "shaderwidget.h"

/**
 * @brief shaderWidget::shaderWidget
 *        Default constructor
 */
shaderWidget::shaderWidget(QWidget *parent) :
    QWidget(parent)
{
    mCurrentPalette = 0;
    mPalette.clear();

#ifdef DEFAULT_PALETTE
    // Add a basic greyscale palette

    addPalette("Greyscale/0.0=0:0:0|1.0=255:255:255");
#endif
}

/**
 * @brief shaderWidget::~shaderWidget
 *        Default destructor
 */
shaderWidget::~shaderWidget()
{
    clearPalette();
}

/**
 * @brief shaderWidget::paintEvent
 *        Overload of QWidget paint event - Used to redraw widget
 */
void shaderWidget::paintEvent(QPaintEvent *)
{
    // If no palette selected, widget is "blank"
    if (mCurrentPalette == 0)
    {
        QPainter p(this);
        p.setPen(QColor(146, 146, 146));
        p.drawRect(0, 0, width() - 1, height() - 1);
        return;
    }

    if (mShadeImg.isNull() || mShadeImg.size() != size())
    {
        mShadeImg = QImage(size(), QImage::Format_RGB32);
        QLinearGradient shade(0, height(), 0, 0);
        for (int i=0; i < mCurrentPalette->mSections.length(); i++)
        {
            double pos = mCurrentPalette->mSections.at(i)->start;
            QColor col = mCurrentPalette->mSections.at(i)->color;
            shade.setColorAt(pos, col);
        }
        QPainter p(&mShadeImg);
        p.fillRect(rect(), shade);
    }

    QPainter p(this);
    p.drawImage(0, 0, mShadeImg);

    p.setPen(QColor(146, 146, 146));
    p.drawRect(0, 0, width() - 1, height() - 1);
}

/**
 * @brief shaderWidget::addPalette
 *        Register a new palette (using (source is a formated string)
 */
void shaderWidget::addPalette(QString cfg)
{
    QString paletteName;
    QString cfgPalette;

    // String format is like :
    // name/position1=red:green:blue|positionN=red:green:blue

    QStringList cfgName = cfg.split("/");
    if (cfgName.length() > 1)
    {
        paletteName = cfgName.at(0);
        cfgPalette = cfgName.at(1);
    }
    else
    {
        paletteName = tr("No Name");
        cfgPalette = cfg;
    }

    // Get a list of each color points
    QStringList cfgEntries = cfgPalette.split("|");

    if (cfgEntries.length() < 1)
    {
        qWarning() << "SimSpacial: " << tr("Try to add an empty or malformed palette");
        return;
    }

    shaderPalette *newPalette = new shaderPalette();
    newPalette->setName(paletteName);

    for (int i=0; i < cfgEntries.length() ; i++)
    {
        bool convOk;

        // Separate point position and point color value
        QStringList cfgPoint = cfgEntries.at(i).split("=");
        double pos = cfgPoint.at(0).toDouble(&convOk);
        if ( (! convOk) || (pos < 0) || (pos > 1) )
        {
            qWarning() << "SimSpacial: " << tr("Try to add a malformed palette, invalid pos ") << cfgPoint.at(0);
            continue;
        }
        if (cfgPoint.length() < 2)
        {
            qWarning() << "SimSpacial: " << tr("Try to add a malformed palette, no color at pos ") << cfgPoint.at(0);
            continue;
        }

        // Separate each color value
        QStringList cfgColor = cfgPoint.at(1).split(":");
        if (cfgColor.length() < 3)
        {
            qWarning() << "SimSpacial: " << tr("Try to add a malformed palette, need 3 colors at pos ") << cfgPoint.at(0);
            continue;
        }

        // Get the red color value
        int red = cfgColor.at(0).toInt(&convOk);
        if ( ( ! convOk) || (red < 0) || (red > 255) )
        {
            qWarning() << "SimSpacial: " << tr("Try to add a malformed palette, RED value at pos ") << cfgPoint.at(0);
            continue;
        }

        // Get the green color value
        int grn = cfgColor.at(1).toInt(&convOk);
        if ( ( ! convOk) || (grn < 0) || (grn > 255) )
        {
            qWarning() << "SimSpacial: " << tr("Try to add a malformed palette, GREEN value at pos ") << cfgPoint.at(0);
            continue;
        }

        // Get the blue color value
        int blu = cfgColor.at(2).toInt(&convOk);
        if ( ( ! convOk) || (blu < 0) || (blu > 255) )
        {
            qWarning() << "SimSpacial: " << tr("Try to add a malformed palette, BLUE value at pos ") << cfgPoint.at(0);
            continue;
        }

        newPalette->addPoint(pos, QColor(red, grn, blu) );
    }

    if (newPalette->mSections.length() < 2)
    {
        qWarning() << "SimSpacial: " << tr("Try to add an empty palette") << paletteName;
        delete newPalette;
        return;
    }

    // Insert into the list of known palettes
    mPalette.append(newPalette);

    if (mCurrentPalette == 0)
        mCurrentPalette = newPalette;
}

/**
 * @brief shaderWidget::setPalette
 *       Define the currently used palette
 */
void shaderWidget::setPalette(int id)
{
    if (id >= mPalette.length())
        return;

    // Set the current palette pointer according to selection
    mCurrentPalette = mPalette.at(id);
    // Clear the gradient image cache
    mShadeImg = QImage();
    // Call repaint() now to update widget
    repaint();
}

/**
 * @brief shaderWidget::clearPalette
 *       Remove all palettes and clean memory
 */
void shaderWidget::clearPalette()
{
    while(mPalette.length())
    {
        shaderPalette *p = mPalette.takeFirst();
        delete p;
    }
}

/**
 * @brief shaderWidget::getColor
 *       Get color at a specified position into current palette
 */
QColor shaderWidget::getColor(double at)
{
    // If no palette has been selected, return black
    if (mCurrentPalette == 0)
        return QColor(0, 0, 0);
    // If current palette is empty, return black
    if (mCurrentPalette->mSections.length() == 0)
        return QColor(0, 0, 0);

    // If current palette contain only one color, return it
    if (mCurrentPalette->mSections.length() == 1)
        return mCurrentPalette->mSections.at(0)->color;

    // Ok, here we know that palette contains at least two colors, we can interpolate requested value

    double dStart = mCurrentPalette->mSections.at(0)->start;
    QColor cStart = mCurrentPalette->mSections.at(0)->color;
    double dEnd   = mCurrentPalette->mSections.at(1)->start;
    QColor cEnd   = mCurrentPalette->mSections.at(1)->color;

    for (int i = 0; i < mCurrentPalette->mSections.length(); i++)
    {
        if (at > mCurrentPalette->mSections.at(i)->start)
        {
            dStart = mCurrentPalette->mSections.at(i)->start;
            cStart = mCurrentPalette->mSections.at(i)->color;
            if (mCurrentPalette->mSections.length() > (i+1))
            {
                dEnd = mCurrentPalette->mSections.at(i+1)->start;
                cEnd = mCurrentPalette->mSections.at(i+1)->color;
            }
            else
            {
                dEnd = mCurrentPalette->mSections.at(i)->start;
                cEnd = mCurrentPalette->mSections.at(i)->color;
            }
        }
        else
            break;
    }
    // Length of the interval
    double len = dEnd - dStart;
    // Compute the ratio into the interval
    double ratio = (at - dStart) / len;

    int red   = (int)(cStart.red()  * (1-ratio) + cEnd.red()  * ratio);
    int green = (int)(cStart.green()* (1-ratio) + cEnd.green()* ratio);
    int blue  = (int)(cStart.blue() * (1-ratio) + cEnd.blue() * ratio);

    return QColor(red, green, blue);
}

/* ------------------------------ Palette ------------------------------ */

/**
 * @brief shaderPalette::shaderPalette
 *       Default constructor
 */
shaderPalette::shaderPalette()
{
    mSections.clear();
}

/**
 * @brief shaderPalette::~shaderPalette
 *       Default destructor
 */
shaderPalette::~shaderPalette()
{
    while(mSections.length())
    {
        shaderSection *p = mSections.takeFirst();
        delete p;
    }
}

/**
 * @brief shaderPalette::getName
 *       Return the name of the palette
 */
QString shaderPalette::getName()
{
    return mName;
}

/**
 * @brief shaderPalette::setName
 *       Set a name for this palette
 */
void shaderPalette::setName(QString name)
{
    mName = name;
}

/**
 * @brief shaderPalette::addPoint
 *       Insert a new point with a specified color
 */
void shaderPalette::addPoint(double pos, QColor color)
{
    shaderSection *newSection = new shaderSection();
    newSection->start = pos;
    newSection->color = color;

    mSections.append(newSection);
}
