/*
 * This file is part of VLE, a framework for multi-modeling, simulation
 * and analysis of complex dynamical systems.
 * http://www.vle-project.org
 *
 * Copyright (c) 2014 INRA
 *
 */
#include <QDir>
#include <QObject>
#include <QtPlugin>
#include <QDebug>
#include "sim_spacial.h"
#include "simtab.h"

namespace vle {
namespace gvle {

/**
 * @brief SimSpacial::SimSpacial
 *        Default constructor of the plugin main object
 */
SimSpacial::SimSpacial() {
    mLogger    = 0;
    mSettings  = 0;
    mWidgetTab = 0;
    mWidgetToolbar = 0;
    mCurrPackage   = 0;
}

/**
 * @brief SimSpacial::~SimSpacial
 *        Default destructor
 */
SimSpacial::~SimSpacial()
{
    // Delete the main widget if it has been used
    delWidget();
    // Delete the toolbar widget if it has been used
    delWidgetToolbar();

    // Close the current shape file
    mShapeFile.close();
}

/**
 * @brief SimSpacial::getname
 *        Return the plugin name
 */
QString SimSpacial::getname() {
    QString name = "Shape";
    return name;
}

/**
 * @brief SimSpacial::setSettings
 *        Save a pointer to the configuration file wrapper
 */
void SimSpacial::setSettings(QSettings *s)
{
    mSettings = s;

    QVariant vShapeKeyName = mSettings->value("Plugin-Spacial/shapekey");
    if (vShapeKeyName.isValid())
        mShapeKeyName =vShapeKeyName.toString();
    else
    {
        mShapeKeyName = "GisFileName";
        mSettings->setValue("Plugin-Spacial/shapekey", mShapeKeyName);
    }
}

/**
 * @brief SimSpacial::setLogger
 *        Save a pointer to the logger instance of main app
 */
void SimSpacial::setLogger(Logger *logger)
{
    mLogger = logger;
}

/**
 * @brief SimSpacial::setPackage
 *        Set the vle-package used for current simulation context
 */
void SimSpacial::setPackage(vle::utils::Package *pkg)
{
    mCurrPackage = pkg;

    if (mWidgetTab)
        mWidgetTab->setPackage(mCurrPackage);
}

/**
 * @brief SimSpacial::getWidget
 *        Create the plugin GUI (widget inserted into main app tab)
 */
QWidget *SimSpacial::getWidget()
{
    // If the widget has already been allocated
    if (mWidgetTab)
        // return saved pointer
        return mWidgetTab;

    // Allocate (and return) a new Simulation Tab widget
    mWidgetTab = new SimTab();

    mWidgetTab->setSettings(mSettings);

    if (mWidgetToolbar)
        mWidgetTab->setToolbar(mWidgetToolbar);

    return mWidgetTab;
}

/**
 * @brief SimSpacial::delWidget
 *        Delete the main widget (when tab is closed without plugin unload)
 */
void SimSpacial::delWidget()
{
    // If widget is not allocated, nothing to do
    if (mWidgetTab == 0)
        return;

    // Delete widget and clear pointer
    delete mWidgetTab;
    mWidgetTab = 0;
}

/**
 * @brief SimSpacial::getWidgetToolbar
 *        Create the plugin GUI toolbox (widget inserted into right column)
 */
QWidget *SimSpacial::getWidgetToolbar()
{
    if (mWidgetToolbar)
        return mWidgetToolbar;

    mWidgetToolbar = new widToolbar();
    if (mWidgetTab)
        mWidgetTab->setToolbar(mWidgetToolbar);
    return mWidgetToolbar;
}

/**
 * @brief SimSpacial::delWidgetToolbar
 *        Delete the toolbar widget (when tab is closed without plugin unload)
 */
void SimSpacial::delWidgetToolbar()
{
    // If widget is not allocated, nothing to do
    if (mWidgetToolbar == 0)
        return;

    // Delete widget and clear pointer
    delete mWidgetToolbar;
    mWidgetToolbar = 0;
}

/**
 * @brief SimSpacial::init
 */
void SimSpacial::init(vleVpm *vpm)
{
    mVpm = vpm;

    QString fileName =  vpm->getFilename();

    QString currentDir = QDir::currentPath() += "/";

    fileName.replace(currentDir, "");

    QString pkgName = fileName.split("/").at(0);

    QString shapeName = getShapeName(vpm);

    mShapeFilename = QString("%1/%2/data/%3")
                       .arg(QDir::currentPath())
                       .arg(pkgName)
                       .arg(shapeName);

    try {
        mShapeFile.open(mShapeFilename);

        if ( ! getWidget())
            throw -3;

        mWidgetTab->setVpm(vpm);
        mWidgetTab->setFile(&mShapeFile);
    } catch(...) {
        qDebug() << "Shape init failed !" << fileName << " " << pkgName << "  " << shapeName << " " << mShapeFilename << " " ;
//<< *mCurrPackage->getDataDir().c_str();
    }
}

void *SimSpacial::getVpm()
{
    return (void *)mVpm;
}

QString SimSpacial::getShapeName(vleVpm *vpm)
{
    std::vector<vle::value::Value*> values;
    QDomNodeList condList = vpm->condsListFromConds(vpm->condsFromDoc());
    vle::value::Value* val = 0;

    if (mShapeKeyName == "")
        throw -1;

    // Search into all Experimental Conditions
    for (unsigned int i = 0; i < condList.length(); i++) {
        QDomNode cond = condList.item(i);
        QString name = mVpm->vdo()->attributeValue(cond, "name");
        QDomNodeList portList = vpm->portsListFromDoc(name);

        // Get the list of ports for this condition
        for (unsigned int j = 0; j < portList.length(); j++) {
            QDomNode port = portList.at(j);
            // If the port has the name specified by config ...
            if (mVpm->vdo()->attributeValue(port, "name") == mShapeKeyName)
            {
                vpm->fillWithMultipleValue(port, values);
                break;
            }
        }
        if (not values.empty())
            break;
    }
    if (values.empty())
        throw -2;

    // Search a string into the port values
    for (unsigned int i = 0; i < values.size(); i++) {
        vle::value::Value* vall = values[i];

        if (not vall->getType() == vle::value::Value::STRING)
            continue;
        val = vall;
        break;
    }
    if (val == 0) {
        throw -3;
    }

    return val->toString().value().c_str();
}

}} //namespaces

Q_EXPORT_PLUGIN2(sim_spacial, vle::gvle::SimSpacial)
