/*
 * This file is part of VLE, a framework for multi-modeling, simulation
 * and analysis of complex dynamical systems.
 * http://www.vle-project.org
 *
 * Copyright (c) 2014 INRA
 *
 */
#ifndef SIMTAB_H
#define SIMTAB_H

#include <QWidget>
#include <qwt/qwt_plot_curve.h>

namespace Ui {
class SimTab;
}

class SimTab : public QWidget
{
    Q_OBJECT
    
public:
    explicit SimTab(QWidget *parent = 0);
    ~SimTab();
    void setModelName(QString name);
    QwtPlot *getPlotWidget();
    void updatePlot();
    void simStarted();
    void simPaused();
    void simStoped();
    void progressBar(int value);
    void progressBar(QString format);

signals:
     void doStartStop();

private slots:
    void onButtonGo();
    void onButtonSave();

private:
    Ui::SimTab *ui;
};

#endif // SIMTAB_H
