/*
 * This file is part of VLE, a framework for multi-modeling, simulation
 * and analysis of complex dynamical systems.
 * http://www.vle-project.org
 *
 * Copyright (c) 2014 INRA
 *
 */
#ifndef SIM_PLOT_H
#define SIM_PLOT_H

#include <QObject>
#include <QList>
#include <QSettings>
#include <QTreeWidgetItem>
#include <QWidget>
#include <QThread>
#include <vle/gvle/plugin_sim.h>
#include <vle/gvle/logger.h>
#include "thread.h"
#include "simtab.h"
#include "widtoolbar.h"
#include "plotsignal.h"

namespace vle {
namespace gvle {

class SimPlot : public QObject, public PluginSimulator
{
    Q_OBJECT
    Q_INTERFACES(vle::gvle::PluginSimulator)

public:
    SimPlot();
    ~SimPlot();
    QString getname();
    QWidget *getWidget();
    void     delWidget();
    QWidget *getWidgetToolbar();
    void     delWidgetToolbar();
    void setSettings(QSettings *s);
    void setLogger(Logger *logger);
    void init(vleVpm *vpm);
    void *getVpm();
    void setPackage(vle::utils::Package *pkg);

public slots:
    void startStop();
    void addPlotSig(plotSignal *plot);
    void delPlotSig(plotSignal *plot);
    void updatePlotSig(plotSignal *plot);
    void simulationGetStep();
    void simulationFinished();

private:
    void log(QString message);
    void simulationStart();

private:
    QString name;
    QSettings             *  mSettings;
    Logger                *  mLogger;
    SimTab                *  mWidgetTab;
    widToolbar            *  mWidgetToolbar;
    vleVpm                *  mVpm;
    vle::utils::Package   *  mCurrPackage;
    QList<plotSignal *>      mPlotSignals;
    QThread               *  mThread;
    simPlotThread         *  mSimThread;
    QMap<QString, int>       mViewLastRow;
    bool                     mSimDataValid;
    int                      mStepCount;
    double                   mDuration;
};

}}

#endif // SIM_PLOT_H
