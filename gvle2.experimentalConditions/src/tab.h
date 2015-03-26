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

#ifndef TAB_H
#define TAB_H

#include <QObject>
#include <QWidget>
#include <vle/gvle2/vpzexpcond.h>

namespace Ui {
class MainTab;
}

class MainTab : public QWidget
{
    Q_OBJECT

public:
    explicit MainTab(QWidget *parent = 0);
    ~MainTab();
    void setExpCond(vpzExpCond *cond);

signals:
    void valueChanged(vpzExpCond *cond);

private slots:
    void dateSelected(QDate date);

private:
    Ui::MainTab *ui;
    vpzExpCond  *mExpCond;
private:
    vpzExpCondValue *mEcValueDay;
    vpzExpCondValue *mEcValueMonth;
    vpzExpCondValue *mEcValueYear;
};

#endif // TAB_H
