/*
 * This file is part of VLE, a framework for multi-modeling, simulation
 * and analysis of complex dynamical systems.
 * http://www.vle-project.org
 *
 * Copyright (c) 2014 INRA
 *
 */
#ifndef SIM_LOG_H
#define SIM_LOG_H

#include <QObject>
#include <QList>
#include <QSettings>
#include <QTreeWidgetItem>
#include <QWidget>
#include <QThread>
#include <vle/gvle2/plugin_sim.h>
#include <vle/gvle2/logger.h>
#include "thread.h"
#include "simtab.h"
#include "widtoolbar.h"

class SimLog : public QObject, public PluginSimulator
{
    Q_OBJECT
    Q_INTERFACES(PluginSimulator)

public:
    SimLog();
    ~SimLog();
    QString getname();
    QWidget *getWidget();
    void     delWidget();
    QWidget *getWidgetToolbar();
    void     delWidgetToolbar();
    void setSettings(QSettings *s);
    void setLogger(Logger *logger);
    void setVpz(vleVpz *vpz);
    void *getVpz();
    void setPackage(vle::utils::Package *pkg);

public slots:
    void startStop();
    void addPlotSig(QString view, QString port);
    void delPlotSig(QString port);
    void simulationGetStep();
    void simulationFinished();

private:
    void log(QString message);
    void simulationStart();

private:
    QString name;
    QSettings             *  mSettings;
    Logger                *  mLogger;
    SimTabLog             *  mWidgetTab;
    widToolbar            *  mWidgetToolbar;
    vleVpz                *  mVpz;
    vle::utils::Package   *  mCurrPackage;
    QList<QString>           mObsPorts;
    QThread               *  mThread;
    simLogThread         *  mSimThread;
    vle::value::Map       *  mOutputs;
    QMap<QString, int>       mViewLastRow;
    bool                     mSimDataValid;
    int                      mStepCount;
    double                   mDuration;
};

#endif // SIM_PLOT_H
