/*
 * This file is part of VLE, a framework for multi-modeling, simulation
 * and analysis of complex dynamical systems.
 * http://www.vle-project.org
 *
 * Copyright (c) 2014-2015 INRA http://www.inra.fr
 *
 * See the AUTHORS or Authors.txt file for copyright owners and
 * contributors
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or (at
 * your option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef GVLE_FORRESTER_RIGHT_WIDGET_H
#define GVLE_FORRESTER_RIGHT_WIDGET_H

#include <QWidget>
#include <QObject>
#include <QDebug>
#include <vle/gvle/vlevpz.hpp>

namespace Ui {
    class rightWidget;
}


namespace gvle {
namespace forrester {

class ForresterPanel;

class ForresterRightWidget : public QWidget
{
    Q_OBJECT
public:
    ForresterRightWidget(ForresterPanel*  f);
    virtual ~ForresterRightWidget();
    void reload();

    Ui::rightWidget* ui;
    ForresterPanel*  forrester;
    QString currMatFlow;
    QString currComp;
    QString currParam;

public slots:
    void onMatFlowSelected(QString name);
    void onCompSelected(QString name);
    void onParamSelected(QString name);

    //modification intial values
    void onInitValue(double);

    //button for operator
    void onButAbs();
    void onButCos();
    void onButDivide();
    void onButExp();
    void onButMinus();
    void onButParentIn();
    void onButParentOut();
    void onButPlus();
    void onButSin();
    void onButSqrt();
    void onButTan();
    void onButTimes();

    //in edition widget of MatFlow
    void onMatFlowCompSelected(QListWidgetItem* itemComp);
    void onMatFlowParamSelected(QListWidgetItem* itemComp);
    void onMatFlowAdvancedEdit();

    void onSmModified(int modifType);
    void onDerivative(const QString&, const QString&, const QString&);

};

}} //namespaces


#endif
