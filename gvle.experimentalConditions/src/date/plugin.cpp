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

namespace vle{
namespace gvle{

/**
 * @brief CondDatePlugin::CondDatePlugin
 *        Default constructor
 */
CondDatePlugin::CondDatePlugin() :
  mSettings(0), mLogger(0), mWidgetTab(0), mWidgetToolbar(0),
  mExpCond(), mVpm(0)

{
}

CondDatePlugin::~CondDatePlugin()
{
    // Nothing to do ...
}

QString
CondDatePlugin::getname()
{
    QString name = "Date";
    return name;
}


void
CondDatePlugin::setSettings(QSettings *s)
{
    mSettings = s;
}

void
CondDatePlugin::setLogger(Logger *logger)
{
    mLogger = logger;
}

void
CondDatePlugin::setPackage(vle::utils::Package *pkg)
{
    (void)pkg;
}


QWidget *CondDatePlugin::getWidget()
{
    return mWidgetTab;
}

void
CondDatePlugin::delWidget()
{
    // If widget is not allocated, nothing to do
    if (mWidgetTab == 0)
        return;

    // Delete widget and clear pointer
    delete mWidgetTab;
    mWidgetTab = 0;
}

void
CondDatePlugin::onTabDeleted(QObject *obj)
{
    // If the deleted object is the main tab widget
    if (obj == mWidgetTab)
        // Update local pointer
        mWidgetTab = 0;
}

QWidget*
CondDatePlugin::getWidgetToolbar()
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
void
CondDatePlugin::delWidgetToolbar()
{
    // If widget is not allocated, nothing to do
    if (mWidgetToolbar == 0)
        return;

    // Delete widget and clear pointer
    delete mWidgetToolbar;
    mWidgetToolbar = 0;
}

void
CondDatePlugin::init(vleVpm* vpm, const QString& cond)
{
    mVpm = vpm;
    mExpCond = cond;
    // If the widget has already been allocated
    if (mWidgetTab == 0)
    {
        // Allocate a new tab widget
        mWidgetTab = new MainTab();
        // Catch the "destroyed" signal
        QObject::connect(mWidgetTab, SIGNAL(destroyed(QObject*)),
                         this,       SLOT  (onTabDeleted(QObject *)));

        // Update the current Exprimental Condition to use
        mWidgetTab->setExpCond(mVpm, mExpCond);
    }
}
}}//namespaces

