/*
 * This file is part of VLE, a framework for multi-modeling, simulation
 * and analysis of complex dynamical systems.
 * http://www.vle-project.org
 *
 * Copyright (c) 2014 INRA
 *
 */
#ifndef PLOTSIGNAL_H
#define PLOTSIGNAL_H

#include <qwt/qwt_plot_curve.h>

class plotSignal
{
public:
    explicit plotSignal();
    ~plotSignal();
    void    attach( QwtPlot *plot);
    void    clear();
    QColor  getColor();
    void    setColor(QColor color);
    void    setSignal(QString view, QString port);
    bool    isView(QString view);
    QString getView();
    bool    isPort(QString port);
    void    addPoint(QPointF point);
    void    update();

private:
    QString       mViewName;
    QString       mPortName;
    QwtPlotCurve *mCurve;
    QPolygonF     mPoints;
    QwtPlot      *mAssocPlot;

};

#endif // PLOTSIGNAL_H
