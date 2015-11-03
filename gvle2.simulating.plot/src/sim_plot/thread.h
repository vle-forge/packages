/*
 * This file is part of VLE, a framework for multi-modeling, simulation
 * and analysis of complex dynamical systems.
 * http://www.vle-project.org
 *
 * Copyright (c) 2014 INRA
 *
 */
#ifndef THREAD_H
#define THREAD_H

#include <QObject>
#include <QMutex>
#include <QTimer>
#include <vle/vpz/Vpz.hpp>
#include <vle/devs/RootCoordinator.hpp>
#include <vle/utils/Package.hpp>
#include <vle/utils/ModuleManager.hpp>

class simPlotThread : public QObject
{
    Q_OBJECT

public:
    simPlotThread(vle::vpz::Vpz *vpz = 0);
    ~simPlotThread();
    QString getError()
    {
        return mErrorMessage;
    }
    bool   isPaused();
    void   setPause(bool pause);
    void   forceQuit();
    void   setOutputPath(QString path);
    double getTimeOfOutputs();
    vle::value::Map    *getOutputs();
    void   updateOutputs();
    void   load();
    void   init();
    void   finish();

public slots:
    void run();
    void restart();
signals:
     void step();
     void end();
public:
    QMutex                      mValueMutex;

private:
    enum State { Wait, Init, Play, Error, Finish, Close, Pause };
private:
    State                       mCurrentState;
    QString                     mErrorMessage;
    QString                     mOutputPath;
    vle::vpz::Vpz              *mVpz;
    vle::devs::RootCoordinator *mRoot;
    vle::value::Map            *mOutputs;
    vle::utils::ModuleManager   mLoadedPlugin;
    double                      mTimeOfOutputs;
};

#endif // THREAD_H
