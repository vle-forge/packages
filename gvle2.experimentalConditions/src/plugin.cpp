/*
 * This file is part of VLE, a framework for multi-modeling, simulation
 * and analysis of complex dynamical systems
 * http://www.vle-project.org
 *
 * Copyright (c) 2015 INRA http://www.inra.fr
 *
 * See the AUTHORS or Authors.txt file for copyright owners and contributors
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <QObject>
#include <QtPlugin>
#include "plugin.h"
#include "tab.h"
#include <iostream>
#include "vle/vpz/AtomicModel.hpp"

/**
 * @brief ExpCondDummy::ExpCondDummy
 *        Default constructor
 */
ExpCondDummy::ExpCondDummy() {
    mExpCond   = 0;
    mLogger    = 0;
    mSettings  = 0;
    mWidgetTab = 0;
    mWidgetToolbar = 0;
}

/**
 * @brief ExpCondDummy::~ExpCondDummy
 *        Default destructor
 */
ExpCondDummy::~ExpCondDummy() {
    // Nothing to do ...
}

/**
 * @brief ExpCondDummy::getname
 *        Return the plugin name
 */
QString ExpCondDummy::getname() {
    QString name = "Date";
    return name;
}

/**
 * @brief ExpCondDummy::setExpCond
 *        Set the current Experimental Condition tu use
 */
void ExpCondDummy::setExpCond(vpzExpCond *cond)
{
    mExpCond = cond;
}

/**
 * @brief ExpCondDummy::setSettings
 *        Save a pointer to the configuration file wrapper
 */
void ExpCondDummy::setSettings(QSettings *s)
{
    mSettings = s;
}

/**
 * @brief ExpCondDummy::setLogger
 *        Save a pointer to the logger instance of main app
 */
void ExpCondDummy::setLogger(Logger *logger)
{
    mLogger = logger;
}

/**
 * @brief ExpCondDummy::setPackage
 *        Set the vle-package to use
 */
void ExpCondDummy::setPackage(vle::utils::Package *pkg)
{
    (void)pkg;
}

/**
 * @brief ExpCondDummy::getWidget
 *        Create the plugin GUI (widget inserted into main app tab)
 */
QWidget *ExpCondDummy::getWidget()
{
    // If the widget has already been allocated
    if (mWidgetTab == 0)
    {
        // Allocate a new tab widget
        mWidgetTab = new MainTab();
        // Catch the "destroyed" signal
        QObject::connect(mWidgetTab, SIGNAL(destroyed(QObject*)),
                         this,       SLOT  (onTabDeleted(QObject *)));
    }

    // Update the current Exprimental Condition to use
    mWidgetTab->setExpCond(mExpCond);

    return mWidgetTab;
}

/**
 * @brief SimSpacial::delWidget
 *        Delete the main widget (when tab is closed without plugin unload)
 */
void ExpCondDummy::delWidget()
{
    // If widget is not allocated, nothing to do
    if (mWidgetTab == 0)
        return;

    // Delete widget and clear pointer
    delete mWidgetTab;
    mWidgetTab = 0;
}

/**
 * @brief SimSpacial::onTabDeleted (slot)
 *        Called when a tab widget has been deleted
 */
void ExpCondDummy::onTabDeleted(QObject *obj)
{
    // If the deleted object is the main tab widget
    if (obj == mWidgetTab)
        // Update local pointer
        mWidgetTab = 0;
}

/**
 * @brief ExpCondDummy::getWidgetToolbar
 *        Create the plugin GUI toolbox (widget inserted into right column)
 */
QWidget *ExpCondDummy::getWidgetToolbar()
{
    if (mWidgetToolbar)
        return mWidgetToolbar;

    mWidgetToolbar = new widToolbar();
    return mWidgetToolbar;
}

/**
 * @brief SimSpacial::delWidgetToolbar
 *        Delete the toolbar widget (when tab is closed without plugin unload)
 */
void ExpCondDummy::delWidgetToolbar()
{
    // If widget is not allocated, nothing to do
    if (mWidgetToolbar == 0)
        return;

    // Delete widget and clear pointer
    delete mWidgetToolbar;
    mWidgetToolbar = 0;
}

/**
 * @brief ExpCondDummy::setVpz
 *        Set the VPZ package to use - Not used for this plugin
 */
void ExpCondDummy::setVpz(vleVpz *vpz)
{
    (void)vpz;
}

/**
 * @brief ExpCondDummy::getVpz
 *        Get the registered VPZ - Not used for this plugin
 */
void *ExpCondDummy::getVpz()
{
    return (void *)0;
}

Q_EXPORT_PLUGIN2(expcond_dummy, ExpCondDummy)
