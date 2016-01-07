/*
 * This file is part of VLE, a framework for multi-modeling, simulation
 * and analysis of complex dynamical systems.
 * http://www.vle-project.org
 *
 * Copyright (c) 2014 INRA
 *
 */
#ifndef SIM_SPACIAL_H
#define SIM_SPACIAL_H

#include <QObject>
#include <QSettings>
#include <vle/gvle/plugin_sim.h>
#include <vle/gvle/logger.h>
#include <vle/gvle/vlevpm.h>
#include <vle/gvle/simulation.h>
#include "simtab.h"
#include "widtoolbar.h"
#include "shapefile.h"

#ifndef Q_MOC_RUN
#include <vle/utils/Package.hpp>
#include <vle/utils/ModuleManager.hpp>
#endif

namespace vle {
namespace gvle {

class SimSpacial : public QObject, public PluginSimulator
{
    Q_OBJECT
    Q_INTERFACES(vle::gvle::PluginSimulator)

public:
    SimSpacial();
    ~SimSpacial();
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
private:
    QString getShapeName(vleVpm *vpm);

private:
    QString name;
    QSettings             *  mSettings;
    Logger                *  mLogger;
    SimTab                *  mWidgetTab;
    widToolbar            *  mWidgetToolbar;

private:
    vleVpm     *mVpm;
    QString     mShapeKeyName;
    ShapeFile   mShapeFile;
    QString     mShapeFilename;

    simulationThread *mSimThread;
    vle::utils::Package   *mCurrPackage;
};

}}

#endif // SIM_SPACIAL_H
