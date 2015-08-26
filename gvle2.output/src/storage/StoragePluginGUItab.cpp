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
#include "StoragePluginGUItab.h"
#include "ui_StoragePluginGvle.h"

/**
 * @brief StoragePluginGUItab::StoragePluginGUItab
 *        Default constructor
 */
StoragePluginGUItab::StoragePluginGUItab(QWidget *parent) :
    QWidget(parent), ui(new Ui::StoragePluginGvle)
{
//
    ui->setupUi(this);
    QObject::connect(ui->spinBoxRows, SIGNAL(valueChanged(int)),
                     this,         SLOT(rowsChanged(int)) );
    QObject::connect(ui->spinBoxColumns, SIGNAL(valueChanged(int)),
                     this,         SLOT(columnsChanged(int)) );
    QObject::connect(ui->spinBoxUpdateRows, SIGNAL(valueChanged(int)),
                     this,         SLOT(incRowsChanged(int)) );
    QObject::connect(ui->spinBoxUpdateColumns, SIGNAL(valueChanged(int)),
                     this,         SLOT(incColumnsChanged(int)) );
}

StoragePluginGUItab::~StoragePluginGUItab()
{
    delete ui;
    delete outputNodeConfig;
}


void
StoragePluginGUItab::init(vleVpz* vpz, const QString& viewName)
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
    ui->spinBoxRows->setValue(outputNodeConfig->getInt("rows"));
    ui->spinBoxColumns->setValue(outputNodeConfig->getInt("columns"));
    ui->spinBoxUpdateRows->setValue(outputNodeConfig->getInt("inc_rows"));
    ui->spinBoxUpdateColumns->setValue(outputNodeConfig->getInt("inc_columns"));
}



void
StoragePluginGUItab::rowsChanged(int v)
{
    QDomNode mapConfig = mvleVpz->obtainChild(outputNode, "map", true);
    int& toUp = outputNodeConfig->getInt("rows");
    toUp = v;
    mvleVpz->fillWithValue(mapConfig, *outputNodeConfig);
}

void
StoragePluginGUItab::columnsChanged(int v)
{
    QDomNode mapConfig = mvleVpz->obtainChild(outputNode, "map", true);
    int& toUp = outputNodeConfig->getInt("columns");
    toUp = v;
    mvleVpz->fillWithValue(mapConfig, *outputNodeConfig);
}

void
StoragePluginGUItab::incRowsChanged(int v)
{
    QDomNode mapConfig = mvleVpz->obtainChild(outputNode, "map", true);
    int& toUp = outputNodeConfig->getInt("inc_rows");
    toUp = v;
    mvleVpz->fillWithValue(mapConfig, *outputNodeConfig);
}

void
StoragePluginGUItab::incColumnsChanged(int v)
{
    QDomNode mapConfig = mvleVpz->obtainChild(outputNode, "map", true);
    int& toUp = outputNodeConfig->getInt("inc_columns");
    toUp = v;
    mvleVpz->fillWithValue(mapConfig, *outputNodeConfig);
}

bool StoragePluginGUItab::wellFormed()
{
    if (not outputNodeConfig) {
        return false;
    }
    if (outputNodeConfig->size() != 4) {
        return false;
    }
    vle::value::Map::const_iterator itb = outputNodeConfig->begin();
    vle::value::Map::const_iterator ite = outputNodeConfig->end();
    for (; itb != ite; itb++) {
        const std::string& key = itb->first;
        const vle::value::Value* v = itb->second;
        if (key == "rows") {
            if (not v->isInteger()) {
                return false;
            }
        } else if (key == "columns") {
            if (not v->isInteger()) {
                return false;
            }
        } else if (key == "inc_rows") {
            if (not v->isInteger()) {
                return false;
            }
        }else if (key == "inc_columns") {
            if (not v->isInteger()) {
                return false;
            }
        } else {
            return false;
        }
    }
    return true;
}

void StoragePluginGUItab::buildDefaultConfig()
{
    outputNodeConfig->clear();
    outputNodeConfig->addInt("rows", 15);
    outputNodeConfig->addInt("columns", 15);
    outputNodeConfig->addInt("inc_rows", 10);
    outputNodeConfig->addInt("inc_columns", 10);
}
