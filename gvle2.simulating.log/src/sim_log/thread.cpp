/*
 * This file is part of VLE, a framework for multi-modeling, simulation
 * and analysis of complex dynamical systems.
 * http://www.vle-project.org
 *
 * Copyright (c) 2014 INRA
 *
 */
#include "thread.h"
#include <QElapsedTimer>

simLogThread::simLogThread(vle::vpz::Vpz *vpz)
{
    mOutputs = 0;
    mCurrentState = Wait;
    if (vpz)
    {
        // Make a copy of current VPZ before use it for sim
        mVpz = new vle::vpz::Vpz(*vpz);

        // Instanciate the root coordinator    ToDo : Plugins
        mRoot = new vle::devs::RootCoordinator(mLoadedPlugin);
    }
    else
    {
        mVpz  = 0;
        mRoot = 0;
    }
}

/**
 * @brief simLogThread default destructor
 *
 */
simLogThread::~simLogThread()
{
    if (mRoot)
        delete mRoot;

    if (mVpz)
        delete mVpz;
}

/**
 * @brief simLogThread::getOutputs
 *        Get the map of ouputs datas
 *
 * This method take place into -simulationView- thread!
 */
vle::value::Map *simLogThread::getOutputs()
{
    return mOutputs;
}

/**
 * @brief simPlotThread::updateOutputs
 *        Copy the current outputs form RootCoordinator
 *
 * This method take place into -simulationView- thread!
 */
void simLogThread::updateOutputs()
{
    mValueMutex.lock();
    delete mOutputs;
    if (mRoot->outputs()) {
        mOutputs = new vle::value::Map(*mRoot->outputs());
    }
    mTimeOfOutputs = mRoot->getCurrentTime();
    mValueMutex.unlock();
}

/**
 * @brief simLogThread::getOutputs
 *        Get the map of ouputs datas
 *
 * This method can be executed in both threads
 */
double simLogThread::getTimeOfOutputs()
{
    return mTimeOfOutputs;
}

/**
 * @brief simLogThread::setOutputPath
 *        Configure output plugins for a specific location
 * @param QString Path name
 *
 * This method take place into -simulationView- thread!
 */
void simLogThread::setOutputPath(QString path)
{
    vle::vpz::Outputs::iterator itb;
    vle::vpz::Outputs::iterator ite;

    mOutputPath = path;

    if (mVpz == 0)
        return;

    itb = mVpz->project().experiment().views().outputs().begin();
    ite = mVpz->project().experiment().views().outputs().end();
    for (; itb!=ite; itb++)
    {
        vle::vpz::Output& output = itb->second;
        if (output.location().empty())
            output.setLocalStreamLocation(mOutputPath.toStdString());
    }
}

/**
 * @brief simLogThread::isPaused
 *        Test if the simulation thread is in Pause state
 */
bool simLogThread::isPaused()
{
    if (mCurrentState == Pause)
        return true;
    else
        return false;
}
/**
 * @brief simLogThread::setPause
 *        Request the simulation thread to wait some time
 */
void simLogThread::setPause(bool pause)
{
    if (pause)
        mCurrentState = Pause;
    else
        mCurrentState = Play;
}

/**
 * @brief simLogThread::forceQuit
 *        Force the current state to Finish (this will end the run loop)
 *
 */
void simLogThread::forceQuit()
{
    mCurrentState = Finish;
}

/**
 * @brief simLogThread::run
 *        Do the sim !
 *
 * This method take place into -dedicated- thread!
 */
void simLogThread::run()
{
    mCurrentState = Play;
    QElapsedTimer timer;
    timer.start();


    while((mCurrentState == Play) || (mCurrentState == Pause))
    {
        // If the simulation is paused ... just wait
        while (mCurrentState == Pause)
        {
            // QThread::msleep(100);
            QTimer::singleShot(800, this, SLOT(restart()));
            return;
        }

        try {
            if (mRoot->run() == false) {
                updateOutputs();
                emit step();
                mCurrentState = Finish;
            }
            // Notify the view
            if (timer.hasExpired(100)) { //TODO should be a parameter
                updateOutputs();
                emit step();
                timer.restart();
            }
        } catch (const std::exception& e) {
            mErrorMessage = QString(e.what());
        }
    }
    emit end();
}

/**
 * @brief simLogThread::restart
 *        Used by thread when in PAUSE state ; used to restart sim
 */
void simLogThread::restart()
{
    if (mCurrentState == Pause)
        QTimer::singleShot(800, this, SLOT(restart()));
    else
        run();
}

/**
 * @brief simLogThread::load
 *        Load the VPZ into devs coordinator
 *
 * This method take place into the -simulationView- thread!
 */
void simLogThread::load()
{
    if ((mVpz == 0) || (mRoot == 0))
    {
        mErrorMessage = tr("Model or RootCoordinator missing");
        throw 10;
        return;
    }

    try {
        mRoot->load(*mVpz);
    } catch (const std::exception &e) {
        mErrorMessage = QString(e.what());
        throw 1;
    }
}

/**
 * @brief simLogThread::init
 *        Initialize the devs coordinator
 *
 * This method take place into the -simulationView- thread!
 */
void simLogThread::init()
{
    try {
        mRoot->init();
    } catch (const std::exception &e) {
        mErrorMessage = QString(e.what());
        throw 2;
    }
}

/**
 * @brief simLogThread::finish
 *        Terminate devs coordinator jobs
 *
 * This method take place into the -simulationView- thread!
 */
void simLogThread::finish()
{
    try {
        mRoot->finish();
    } catch (const std::exception& e) {
        mErrorMessage = QString(e.what());
        throw 3;
    }
}
