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

namespace vle {
namespace gvle {


/**
 * @brief MainTab::MainTab
 *        Default constructor
 */
MainTab::MainTab(QWidget *parent) :
    QWidget(parent), ui(new Ui::MainTab), mExpCond(), mVpm(0)
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

void MainTab::setExpCond(vleVpm* vpm, const QString&  cond)
{
    // Save it
    mVpm = vpm;
    mExpCond = cond;

    QDomNode datePort;

    // Search the "date" port of the experimental condition

    QDomNode condXml = mVpm->condFromConds(mVpm->condsFromDoc(), cond);
    if (not mVpm->existPortFromCond(condXml, "day")) {
        mVpm->addCondPortToDoc(cond, "day");
        mVpm->addValuePortCondToDoc(cond, "day", vle::value::String("9"));
    }
    if (not mVpm->existPortFromCond(condXml, "month")) {
        mVpm->addCondPortToDoc(cond, "month");
        mVpm->addValuePortCondToDoc(cond, "month", vle::value::String("4"));
    }
    if (not mVpm->existPortFromCond(condXml, "year")) {
        mVpm->addCondPortToDoc(cond, "year");
        mVpm->addValuePortCondToDoc(cond, "year", vle::value::String("2015"));
    }
    vle::value::Value* dayV = mVpm->buildValueFromDoc(cond, "day",0);
    vle::value::Value* monthV = mVpm->buildValueFromDoc(cond, "month",0);
    vle::value::Value* yearV = mVpm->buildValueFromDoc(cond, "year",0);

    int dayI = QVariant(dayV->toString().value().c_str()).toInt();
    int monthI = QVariant(monthV->toString().value().c_str()).toInt();
    int yearI = QVariant(yearV->toString().value().c_str()).toInt();

    delete dayV;
    delete monthV;
    delete yearV;


    QDate selDate;
    selDate.setDate(yearI, monthI, dayI);

    ui->calendar->setSelectedDate(selDate);
    ui->calendar->setCurrentPage(monthI, yearI);
    ui->calendar->showSelectedDate();

}

/**
 * @brief MainTab::dateSelected (slot)
 *        Called when a new date is selected from calendar widget
 */
void MainTab::dateSelected(QDate date)
{
    mVpm->fillWithValue(mExpCond, "day", 0, vle::value::String(
            QVariant(date.day()).toString().toStdString()));
    mVpm->fillWithValue(mExpCond, "month", 0, vle::value::String(
            QVariant(date.month()).toString().toStdString()));
    mVpm->fillWithValue(mExpCond, "year", 0, vle::value::String(
            QVariant(date.year()).toString().toStdString()));
    ui->calendar->setCurrentPage(date.month(), date.year());
    ui->calendar->showSelectedDate();
}


}}//namespaces
