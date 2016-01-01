/*
 * This file is part of VLE, a framework for multi-modeling, simulation
 * and analysis of complex dynamical systems.
 * http://www.vle-project.org
 *
 * Copyright (c) 2014 INRA
 *
 */
#include "plotsignal.h"

plotSignal::plotSignal()
{
    mAssocPlot = 0;

    QPen pen;
    pen.setColor(Qt::darkGray);
    pen.setWidth(1);

    mCurve = new QwtPlotCurve();
    mCurve->setPen(pen);
    mCurve->setRenderHint( QwtPlotItem::RenderAntialiased, true );
}

plotSignal::~plotSignal()
{
    if (mAssocPlot)
        mCurve->detach();
    delete mCurve;
}

void plotSignal::attach(QwtPlot *plot)
{
    mAssocPlot = plot;
    mCurve->attach(plot);
}

void plotSignal::clear()
{
    mPoints.clear();
}

QColor plotSignal::getColor()
{
    QPen   pen = mCurve->pen();
    QColor color = pen.color();
    return color;
}

void plotSignal::setColor(QColor color)
{
    QPen pen;
    pen.setColor(color);
    pen.setWidth(1);

    mCurve->setPen(pen);
}

void plotSignal::addPoint(QPointF point)
{
    mPoints << point;
}

void plotSignal::update()
{
    // Copy points before sending them to Qwt
    QPolygonF points(mPoints);
    mCurve->setSamples( points );
}

void plotSignal::setSignal(QString view, QString port)
{
    mViewName = view;
    mPortName = port;

    mCurve->setTitle(mPortName);
}

QString plotSignal::getView()
{
    return mViewName;
}

bool plotSignal::isView(QString view)
{
    return (mViewName == view);
}

bool plotSignal::isPort(QString port)
{
    return (mPortName == port);
}
