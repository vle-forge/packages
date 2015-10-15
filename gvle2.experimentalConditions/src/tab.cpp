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

#include <QDebug>
#include <vle/value/String.hpp>
#include "tab.h"
#include "ui_tab.h"

/**
 * @brief MainTab::MainTab
 *        Default constructor
 */
MainTab::MainTab(QWidget *parent) :
    QWidget(parent), ui(new Ui::MainTab), mExpCond(), mVpz(0)
{

    ui->setupUi(this);
    QObject::connect(ui->calendar, SIGNAL(clicked(QDate)),
                     this,         SLOT(dateSelected(QDate)) );
}

/**
 * @brief MainTab::~MainTab
 *        Default destructor
 */
MainTab::~MainTab()
{
    delete ui;
}

/**
 * @brief MainTab::setExpCond
 *        Set the Experimental Condition to use
 */
void MainTab::setExpCond(const QString&  cond)
{
    // Save it
    mExpCond = cond;

    QDomNode datePort;

    // Search the "date" port of the experimental condition
    QDomNode condXml = mVpz->condFromConds(mVpz->condsFromDoc(), cond);
    if (not mVpz->existPortFromCond(condXml, "day")) {
        mVpz->addCondPortToDoc(cond, "day");
        mVpz->addValuePortCondToDoc(cond, "day", vle::value::String("9"));
    }
    if (not mVpz->existPortFromCond(condXml, "month")) {
        mVpz->addCondPortToDoc(cond, "month");
        mVpz->addValuePortCondToDoc(cond, "month", vle::value::String("4"));
    }
    if (not mVpz->existPortFromCond(condXml, "year")) {
        mVpz->addCondPortToDoc(cond, "year");
        mVpz->addValuePortCondToDoc(cond, "year", vle::value::String("2015"));
    }
    vle::value::Value* dayV = mVpz->buildValueFromDoc(cond, "day",0);
    vle::value::Value* monthV = mVpz->buildValueFromDoc(cond, "month",0);
    vle::value::Value* yearV = mVpz->buildValueFromDoc(cond, "year",0);

    int dayI = QVariant(dayV->toString().value().c_str()).toInt();
    int monthI = QVariant(monthV->toString().value().c_str()).toInt();
    int yearI = QVariant(yearV->toString().value().c_str()).toInt();

    delete dayV;
    delete monthV;
    delete yearV;


    QDate selDate;
    selDate.setDate(dayI, monthI, yearI);

    ui->calendar->setSelectedDate(selDate);

}

void
MainTab::setVpz(vle::gvle2::vleVpz* vpz)
{
    mVpz = vpz;
}

/**
 * @brief MainTab::dateSelected (slot)
 *        Called when a new date is selected from calendar widget
 */
void MainTab::dateSelected(QDate date)
{
    mVpz->fillWithValue(mExpCond, "day", 0, vle::value::String(
            QVariant(date.day()).toString().toStdString()));
    mVpz->fillWithValue(mExpCond, "month", 0, vle::value::String(
            QVariant(date.month()).toString().toStdString()));
    mVpz->fillWithValue(mExpCond, "year", 0, vle::value::String(
            QVariant(date.year()).toString().toStdString()));
    emit valueChanged(mExpCond);
}
