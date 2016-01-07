/*
 * This file is part of VLE, a framework for multi-modeling, simulation
 * and analysis of complex dynamical systems.
 * http://www.vle-project.org
 *
 * Copyright (c) 2014 INRA
 *
 */
#include <QObject>
#include <QtPlugin>
#include <QDebug>
#include "sim_plot.h"
#include "simtab.h"
#include <iostream>
#include "vle/vpz/AtomicModel.hpp"

namespace vle {
namespace gvle {

SimPlot::SimPlot() {
    mLogger = 0;
    mThread = 0;
    mSettings = 0;
    mWidgetTab = 0;
    mCurrPackage = 0;
    mWidgetToolbar = 0;
    mSimDataValid = false;
    mDuration = 0;
    mStepCount = 0;
}

SimPlot::~SimPlot() {
    // Nothing to do ...
}

/**
 * @brief SimPlot::getname
 *        Return the plugin name
 */
QString SimPlot::getname() {
    QString name = "Plot";
    return name;
}

/**
 * @brief SimPlot::log
 *        Send a log message to main app
 */
void SimPlot::log(QString message)
{
    if (mLogger)
        mLogger->log(message);
}

/**
 * @brief SimPlot::setSettings
 *        Save a pointer to the configuration file wrapper
 */
void SimPlot::setSettings(QSettings *s)
{
    mSettings = s;
}

/**
 * @brief SimPlot::setLogger
 *        Save a pointer to the logger instance of main app
 */
void SimPlot::setLogger(Logger *logger)
{
    mLogger = logger;
}

/**
 * @brief SimPlot::setPackage
 *        Set the vle-package used for current simulation context
 */
void SimPlot::setPackage(vle::utils::Package *pkg)
{
    mCurrPackage = pkg;
}

/**
 * @brief SimPlot::getWidget
 *        Create the plugin GUI (widget inserted into main app tab)
 */
QWidget *SimPlot::getWidget()
{
    // If the widget has already been allocated
    if (mWidgetTab)
        // return saved pointer
        return mWidgetTab;

    // Allocate (and return) a new Simulation Tab widget
    mWidgetTab = new SimTab();
    return mWidgetTab;
}
/**
 * @brief Delete the main widget (when tab is closed without plugin unload)
 */
void SimPlot::delWidget()
{
    // If widget is not allocated, nothing to do
    if (mWidgetTab == 0)
        return;

    // Delete widget and clear pointer
    delete mWidgetTab;
    mWidgetTab = 0;
}

/**
 * @brief SimPlot::getWidgetToolbar
 *        Create the plugin GUI toolbox (widget inserted into right column)
 */
QWidget *SimPlot::getWidgetToolbar()
{
    if (mWidgetToolbar)
        return mWidgetToolbar;

    mWidgetToolbar = new widToolbar();
    return mWidgetToolbar;
}

/**
 * @brief delWidgetToolbar
 *        Delete the toolbar widget (when tab is closed without plugin unload)
 */
void SimPlot::delWidgetToolbar()
{
    // If widget is not allocated, nothing to do
    if (mWidgetToolbar == 0)
        return;

    // Delete widget and clear pointer
    delete mWidgetToolbar;
    mWidgetToolbar = 0;
}

/**
 * @brief SimPlot::addPlotSig
 *        Slot called when a new signal is enabled into the tree (toolbox)
 */
void SimPlot::addPlotSig(plotSignal *plot)
{
    plot->attach( mWidgetTab->getPlotWidget() );

    mPlotSignals.append(plot);

    if ( ! mViewLastRow.contains(plot->getView()))
        mViewLastRow.insert(plot->getView(), 1);

    if (mSettings)
    {
        // Count the number of selected signals
        int index = mWidgetToolbar->sigCount();

        // Search the default color for plot index n
        QString keyName = QString("Plot/color_%1").arg(index);
        QVariant varValue = mSettings->value(keyName);
        // If a value found, convert to rgb and set it to signal
        if (varValue.isValid())
        {
            int red = 0;
            int grn = 0;
            int blu = 0;

            QStringList strColors = varValue.toStringList();
            if (strColors.size() >= 1)
                red = strColors[0].toInt();
            if (strColors.size() >= 2)
                grn = strColors[1].toInt();
            if (strColors.size() >= 3)
                blu = strColors[2].toInt();

            QColor sigColor(red, grn, blu);
            plot->setColor(sigColor);
        }
    }
}

/**
 * @brief SimPlot::delPlotSig
 *        Slot called when a signal is disabled into the tree (toolbox)
 */
void SimPlot::delPlotSig(plotSignal *plot)
{
    for (int k = 0; k < mPlotSignals.count(); k++)
    {
        plotSignal *savedPlot = mPlotSignals.at(k);
        if (savedPlot != plot)
            continue;

        // Remove it from the list of used signels
        mPlotSignals.removeAt(k);

        // The object itself is freed by widToolbar::onTreeItemChanged
    }

}

/**
 * @brief SimPlot::updatePlotSig
 *        Slot called when a color is changed, in order to keep it
 */
void SimPlot::updatePlotSig(plotSignal *plot)
{
    //Save the new color as default color for current index
    if (mSettings)
    {
        QColor c = plot->getColor();
        int idx = mPlotSignals.indexOf(plot);
        QString keyName = QString("Plot/color_%1").arg(idx + 1);
        QStringList keyValue;
        keyValue << QString("%1").arg(c.red());
        keyValue << QString("%1").arg(c.green());
        keyValue << QString("%1").arg(c.blue());
        mSettings->setValue(keyName, keyValue);
    }
}

/**
 * @brief SimLog::init
 */
void SimPlot::init(vleVpm *vpm)
{
    mVpm = vpm;

    vle::vpz::Vpz   *oldVpz;

    // NOTE - View list is loaded from disk using vle::Vpz
    // only because GVLE::vleVpz does not support views
    // direct access yet. This must be changed in future.

    QString fileName = vpm->getFilename();
    oldVpz = new vle::vpz::Vpz(fileName.toStdString());

    vle::vpz::Observables curVpzObs;
    vle::vpz::Views       curVpzViews;

    curVpzViews  = oldVpz->project().experiment().views();
    curVpzObs    = oldVpz->project().experiment().views().observables();

    if ( getWidget() )
    {
        // Update the title (Experiment Name and VPZ file name)
        QString expName = oldVpz->project().experiment().name().c_str();
        QString simTitle = QString("%1 (%2)").arg(expName).arg(oldVpz->filename().c_str());
        mWidgetTab->setModelName(simTitle);

        QObject::connect(mWidgetTab, SIGNAL(doStartStop()),
                         this,       SLOT  (startStop()));
    }

    if ( getWidgetToolbar() )
    {
        mWidgetToolbar->buildTree(oldVpz);

        QObject::connect(mWidgetToolbar, SIGNAL(addSig    (plotSignal *)),
                         this,           SLOT  (addPlotSig(plotSignal *)));
        QObject::connect(mWidgetToolbar, SIGNAL(delSig    (plotSignal *)),
                         this,           SLOT  (delPlotSig(plotSignal *)));
        QObject::connect(mWidgetToolbar, SIGNAL(updateSig    (plotSignal *)),
                         this,           SLOT  (updatePlotSig(plotSignal *)));
    }
}

void *SimPlot::getVpm()
{
    return (void *)mVpm;
}

void SimPlot::startStop()
{
    if (mThread == 0)
    {
        mWidgetTab->simStarted();
        mWidgetToolbar->simStarted();
        simulationStart();
    }
    else
    {
        if (mSimThread)
        {
            if (mSimThread->isPaused())
            {
                mSimThread->setPause(false);
                mWidgetTab->simStarted();
                mWidgetToolbar->simStarted();
            }
            else
            {
                mSimThread->setPause(true);
                mWidgetTab->simPaused();
                mWidgetToolbar->simPaused();
            }
        }
    }
}

void SimPlot::simulationStart()
{
    vle::vpz::Vpz   *oldVpz;

    // NOTE - View list is loaded from disk using vle::Vpz
    // only because GVLE::vleVpz does not support views
    // direct access yet. This must be changed in future.

    QString fileName = mVpm->getFilename();
    oldVpz = new vle::vpz::Vpz(fileName.toStdString());

    mSimThread = new simPlotThread(oldVpz);

    // Load and init the simulation into VLE
    try {
        if (mCurrPackage == 0)
            throw 0;
        mCurrPackage->addDirectory("","output",vle::utils::PKG_SOURCE);
        std::string path = mCurrPackage->getOutputDir(vle::utils::PKG_SOURCE);
        mSimThread->setOutputPath(path.c_str());
        mSimThread->load();
        mSimThread->init();
    } catch (...) {
        mWidgetTab->progressBar(tr("Fatal Error"));
        QString logMessage;
        logMessage = tr("Project error:") + mSimThread->getError();
        qDebug() << logMessage;
        log(logMessage);
        delete mSimThread;
        mSimThread = 0;
        return;
    }
    mSimDataValid = true;
    mStepCount   = 0;
    QMap<QString, int>::iterator i;
    for (i = mViewLastRow.begin(); i != mViewLastRow.end(); ++i)
        i.value() = 1;

    for (int k = 0; k < mPlotSignals.count(); k++)
        mPlotSignals.at(k)->clear();

    //ui->buttonGo->setIcon(*(new QIcon(":/icon/resources/control_pause_blue.png")));

    QString expName = oldVpz->project().experiment().name().c_str();
    mDuration = oldVpz->project().experiment().duration();
    QString startMessage;
    startMessage  = tr("Simulation started. ");
    startMessage += tr("Experiment name: ") + expName + " ";
    startMessage += QString(tr("Duration=%1")).arg(mDuration, 0, 'f', 4);
    log(startMessage);

    // Start the simulation thread
    mThread = new QThread();
    mSimThread->moveToThread(mThread);
    connect(mThread,    SIGNAL(started()), mSimThread, SLOT(run()));
    connect(mSimThread, SIGNAL(step()),    this, SLOT(simulationGetStep()));
    connect(mSimThread, SIGNAL(end()),     this, SLOT(simulationFinished()));
    mThread->start();
}

/**
 * @brief SimPlot::simulationGetStep
 *        Called when thread emit a "step" signal (each run loop)
 *
 */
void SimPlot::simulationGetStep()
{
    vle::vpz::Vpz   *oldVpz;

    // NOTE - View list is loaded from disk using vle::Vpz
    // only because GVLE::vleVpz does not support views
    // direct access yet. This must be changed in future.

    QString fileName = mVpm->getFilename();
    oldVpz = new vle::vpz::Vpz(fileName.toStdString());

    // Update the progress bar
    double debugTime = mSimThread->getTimeOfOutputs();

    double debugElapsed = debugTime - oldVpz->project().experiment().begin();
    double percent = (debugElapsed / mDuration) * 100.00;
    if (percent > 100) {
        percent = 100;
    }

    mWidgetTab->progressBar(percent);

    mSimThread->mValueMutex.lock();
    if (mSimThread->getOutputs())
    {
        try {
        uint i;
        vle::value::Map::iterator itb = mSimThread->getOutputs()->begin();
        vle::value::Map::iterator ite = mSimThread->getOutputs()->end();
        for (; itb != ite; itb++)
        {
            QString matViewName = QString::fromStdString(itb->first);
            // If the selected view isn't used by any plot, ignore it
            if ( ! mViewLastRow.contains(matViewName))
                continue;

            vle::value::Matrix* mat;
            mat = dynamic_cast<vle::value::Matrix*>(itb->second);
            if (mat == 0)
            {
                QString logMessage;
                logMessage  = tr("Project error: could not read Matrix on step ");
                logMessage += QString("%1").arg(mStepCount);
                log(logMessage);
                continue;
            }
            // If no result available, nothing more to do
            if (mat->rows() == 0)
            {
                continue;
            }

            // Here, we know that :
            //   - selected view is valid and used by one or more sig,
            //   - datas are available

            vle::value::Value *headTest = mat->get(0, 0);
            int headType = headTest->getType();
            if (headType != 3)
            {
                if (mSimDataValid)
                {
                    QString logMessage;
                    logMessage = tr("Simulation plotting error (no header) ");
                    logMessage += QString("view: %1").arg(matViewName);
                    log(logMessage);
                    mSimDataValid = false;
                }
                continue;
            }

            QList<QString> portNames;
            for (i = 0; i < mat->columns(); i++)
            {
                QString portName;
                vle::value::Value *vHead = mat->get(i, 0);
                QString colHeader = QString(vHead->toString().value().c_str());
                QStringList list1 = colHeader.split(":");
                if (list1.size() > 1)
                    portName = list1[1];
                else
                    portName = colHeader;
                portNames.append(portName);
            }

            for (i = mViewLastRow.value(matViewName); i < mat->rows(); i++)
            {
                double t;
                try {
                    // Read the time position of the result sample
                    t = mat->getDouble(0, i);
                } catch (std::exception& e) {
                    QString logMessage= tr("Simulation: could not get current time of the sample : ");
                    logMessage += QString(e.what());
                    log(logMessage);
                    continue;
                }

                for (uint j = 1; j < mat->columns(); j++)
                {
                    vle::value::Value *vCell;
                    QString portName = portNames.at(j);

                    try {
                        vCell = mat->get(j, i);
                    } catch (std::exception& e) {
                        QString logMessage= tr("Simulation: result read error : ");
                        logMessage += QString(e.what());
                        log(logMessage);
                        continue;
                    }

                    // Convert the value to a new plotting point according to data type
                    QPointF newPoint;
                    try {
                        if (vCell->isInteger())
                        {
                            int vInt = vCell->toInteger().value();
                            newPoint = QPointF(t, vInt);
                        }
                        else if (vCell->isDouble())
                        {
                            double vDouble = vCell->toDouble().value();
                            newPoint = QPointF(t, vDouble);
                        }
                        else if (vCell->isBoolean())
                        {
                            bool vBool = vCell->toBoolean().value();
                            newPoint = QPointF(t, vBool);
                        }
                    } catch(const std::exception& e) {
                        QString logMessage= tr("Simulation: result process error : ");
                        logMessage += QString(e.what());
                        log(logMessage);
                        continue;
                    }

                    for (int k = 0; k < mPlotSignals.count(); k++)
                    {
                        plotSignal *sig = mPlotSignals.at(k);
                        if ( ! sig->isView(matViewName))
                            continue;
                        if ( ! sig->isPort(portName))
                            continue;
                        sig->addPoint(newPoint);
                        sig->update();
                    }
                }
            }
            mViewLastRow.operator[] (matViewName) = i;
        } // for { } mOutputs iterator

        mWidgetTab->updatePlot();

        } catch(...) {
            //
        }

    }
    mSimThread->mValueMutex.unlock();

    mStepCount++;
}

/**
 * @brief SimPlot::simulationFinished
 *        Called when thread emit a "finish" signal (at the end of sim)
 *
 */
void SimPlot::simulationFinished()
{
    try {
        // Call finish to close vle::devs job
        mSimThread->finish();
        // Terminate simulation thread
        mThread->quit();
        mThread->wait();
        delete mThread;
        mThread = 0;
        // Clean memory
        delete mSimThread;
        mSimThread = 0;
    } catch (...) {
        QString logMessage;
        logMessage = tr("Project error (finish):") + mSimThread->getError();
        log(logMessage);
    }
    log(tr("Simulation finished"));
    mWidgetTab->simStoped();
    mWidgetToolbar->simStoped();
}

}} //namespaces

Q_EXPORT_PLUGIN2(sim_plot, vle::gvle::SimPlot)
