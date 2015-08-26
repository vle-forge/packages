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
#include <QtCore/qnamespace.h>
#include "FilePluginGUItab.h"
#include "ui_FilePluginGvle.h"

/**
 * @brief FilePluginGUItab::FilePluginGUItab
 *        Default constructor
 */
FilePluginGUItab::FilePluginGUItab(QWidget *parent) :
    QWidget(parent), ui(new Ui::FilePluginGvle), mvleVpz(0), outputNode(),
    outputNodeConfig(0)
{
    ui->setupUi(this);

    QObject::connect(ui->flushByBag, SIGNAL(stateChanged(int)),
                     this,           SLOT(flushByBagChanged(int)));
    QObject::connect(ui->julianDay, SIGNAL(stateChanged(int)),
                     this,          SLOT(julianDayChanged(int)));
    QObject::connect(ui->locale, SIGNAL(currentIndexChanged(const QString&)),
                         this,   SLOT(localeChanged(const QString&)) );
    QObject::connect(ui->destination, SIGNAL(currentIndexChanged(const QString&)),
                         this,        SLOT(destinationChanged(const QString&)));
    QObject::connect(ui->fileType, SIGNAL(currentIndexChanged(const QString&)),
                     this,         SLOT(fileTypeChanged(const QString&)) );
}

FilePluginGUItab::~FilePluginGUItab()
{
    delete outputNodeConfig;
    delete ui;
}


void
FilePluginGUItab::init(vleVpz* vpz, const QString& viewName)
{
    mvleVpz = vpz;
    outputNode = mvleVpz->outputFromOutputs(
            mvleVpz->outputsFromViews(
                    mvleVpz->viewsFromDoc()), viewName);

    QDomNode mapConfig = mvleVpz->obtainChild(outputNode, "map", true);
    outputNodeConfig = dynamic_cast<vle::value::Map*>(
            mvleVpz->buildValue(mapConfig, true));
    if (not wellFormed()) {
        buildDefaultConfig();
        bool res = mvleVpz->fillWithValue(mapConfig, *outputNodeConfig);
        if (! res){
            qDebug() << " Internal Error FilePluginGUItab::init " << mvleVpz->toQString(mapConfig);
        }
    }
    ui->flushByBag->setCheckState(Qt::CheckState(
            getCheckState(outputNodeConfig->getBoolean("flush-by-bag"))));
    ui->julianDay->setCheckState(Qt::CheckState(
            getCheckState(outputNodeConfig->getBoolean("julian-day"))));
    ui->locale->setCurrentIndex(ui->locale->findText(
            QString(outputNodeConfig->getString("locale").c_str())));
    ui->destination->setCurrentIndex(ui->destination->findText(QString("File")));
    ui->destination->setEnabled(false);
    ui->fileType->setCurrentIndex(ui->fileType->findText(
            QString(outputNodeConfig->getString("type").c_str())));
}

void
FilePluginGUItab::flushByBagChanged(int val)
{
    bool& b = outputNodeConfig->getBoolean("flush-by-bag");
    if (b != (bool) val) {
        b = (bool) val;
        QDomNode mapConfig = mvleVpz->obtainChild(outputNode, "map", true);
        bool res = mvleVpz->fillWithValue(mapConfig, *outputNodeConfig);
        if (! res){
            qDebug() << " Internal Error FilePluginGUItab::flushByBagChanged ";
        }
    }
}

void
FilePluginGUItab::julianDayChanged(int val)
{
    bool& b = outputNodeConfig->getBoolean("julian-day");
    if (b != (bool) val) {
        b = (bool) val;
        QDomNode mapConfig = mvleVpz->obtainChild(outputNode, "map", true);
        bool res = mvleVpz->fillWithValue(mapConfig, *outputNodeConfig);
        if (! res){
            qDebug() << " Internal Error FilePluginGUItab::julianDayChanged ";
        }
    }
}

void
FilePluginGUItab::localeChanged(const QString& val)
{
    std::string& type = outputNodeConfig->getString("locale");
    if (type != val.toStdString()) {
        type.assign(val.toStdString());
        QDomNode mapConfig = mvleVpz->obtainChild(outputNode, "map", true);
        bool res = mvleVpz->fillWithValue(mapConfig, *outputNodeConfig);
        if (! res){
            qDebug() << " Internal Error FilePluginGUItab::localeChanged ";
        }
    }
}

void
FilePluginGUItab::destinationChanged(const QString& val)
{
    std::string& type = outputNodeConfig->getString("output");
    if (type != val.toStdString()) {
        type.assign(val.toStdString());
        QDomNode mapConfig = mvleVpz->obtainChild(outputNode, "map", true);
        bool res = mvleVpz->fillWithValue(mapConfig, *outputNodeConfig);
        if (! res){
            qDebug() << " Internal Error FilePluginGUItab::destinationChanged ";
        }
    }
}

void
FilePluginGUItab::fileTypeChanged(const QString& val)
{
    std::string& type = outputNodeConfig->getString("type");
    if (type != val.toStdString()) {
        type.assign(val.toStdString());
        QDomNode mapConfig = mvleVpz->obtainChild(outputNode, "map", true);
        bool res = mvleVpz->fillWithValue(mapConfig, *outputNodeConfig);
        if (! res){
            qDebug() << " Internal Error FilePluginGUItab::fileTypeChanged ";
        }
    }
}

int
FilePluginGUItab::getCheckState(const vle::value::Boolean& v)
{
    if (v.value()) {
        return 2;/*Qt::CheckState::Checked*/;
    } else {
        return 0;/*Qt::CheckState::Unchecked;*/
    }
    return 0;
}

bool
FilePluginGUItab::wellFormed()
{
    if (not outputNodeConfig) {
        return false;
    }
    if (outputNodeConfig->size() != 5) {
        return false;
    }
    vle::value::Map::const_iterator itb = outputNodeConfig->begin();
    vle::value::Map::const_iterator ite = outputNodeConfig->end();
    for (; itb != ite; itb++) {
        const std::string& key = itb->first;
        const vle::value::Value* v = itb->second;
        if (key == "flush-by-bag") {
            if (not v->isBoolean()) {
                return false;
            }
        } else if (key == "julian-day") {
            if (not v->isBoolean()) {
                return false;
            }
        } else if (key == "locale") {
            if (not v->isString()) {
                return false;
            }
            if ((v->toString().value() != "C") and
                    (v->toString().value() != "user")) {
                return false;
            }
        }else if (key == "output") {
            if (not v->isString()) {
                return false;
            }
        } else if (key == "type") {
            if (not v->isString()) {
                return false;
            }
            if ((v->toString().value() != "csv") and
                    (v->toString().value() != "rdata") and
                    (v->toString().value() != "text")) {
                return false;
            }
        }  else {
            return false;
        }
    }
    return true;
}

void
FilePluginGUItab::buildDefaultConfig()
{
    if (not outputNodeConfig) {
        qDebug() << " Internal Error FilePluginGUItab::buildDefaultConfig() ";
        return ;
    }
    outputNodeConfig->clear();
    outputNodeConfig->addBoolean("flush-by-bag",false);
    outputNodeConfig->addBoolean("julian-day",false);
    outputNodeConfig->addString("locale","C");
    outputNodeConfig->addString("output","file");
    outputNodeConfig->addString("type","text");
}
