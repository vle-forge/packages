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

#ifndef GVLE_DECISION_RIGHT_WIDGET_H
#define GVLE_DECISION_RIGHT_WIDGET_H

#include <QWidget>
#include <QObject>
#include <QDebug>
#include <QDateTime>
#include <vle/gvle/gvle_widgets.h>
#include <vle/gvle/vlevpz.hpp>

namespace Ui {
    class rightWidget;
}


namespace vle {
namespace gvle {

class DecisionPanel;

class DecisionRightWidget : public QWidget
{
    Q_OBJECT
public:
    DecisionRightWidget(DecisionPanel*  d);
    virtual ~DecisionRightWidget();
    void reload();
    void reloadRules();

    Ui::rightWidget* ui;
    DecisionPanel*  decision;
    QString currAct;
    QString currPrecedenceFirst;
    QString currPrecedenceSecond;

public slots:
    void onActSelected(QString name);
    void onPrecedenceSelected(QString first, QString second);
    void onNothingSelected();

    void onMinStartDate();
    void onMaxFinishDate();

    void onDmModified(int modifType);
    void onDerivative(const QString&, const QString&, const QString&);
    void onOutParActMenu(const QPoint& pos);
    void insertRowOut(int row, const QString& outParName);
    void onValueUpdated(const QString& id, double value);
    VleLineEdit* getTextEdit(int row, int col);
    void onSelected(const QString& id);
    void onTextUpdated(const QString& /*id*/,
		       const QString& oldname,
		       const QString& newname);

    void onMinStart(int state);
    void disableMinStart();
    void disableMaxFinish();
    void enableMinStart();
    void enableMaxFinish();
    void onMaxFinish(int state);
    void onRelativeDate(int state);

    void onCurrentIndexChangedPrecedenceType(int index);
    void onPrecedenceMinLag(int value);
    void onPrecedenceMaxLag(int value);
    void onPrecedenceMinLag(bool checked);
    void onPrecedenceMaxLag(bool checked);

    void onCheckboxRuleToggle(bool checked);

    void onMaxIter();
    void onMaxIterChange(int i);
    void onTimeLag();

signals:

    void undoAvailable(bool);

private:

    QDateEdit*          mMinStartDate;
    QDateEdit*          mMaxFinishDate;
    QCheckBox*          mRelativeDateCheckBox;
    QCheckBox*          mMinStartCheckBox;
    QCheckBox*          mMaxFinishCheckBox;
    QSpinBox*           mRelativeYearMinStart;
    QSpinBox*           mRelativeYearMaxFinish;
    QDateEdit*          mRelativeMonthDayMinStart;
    QDateEdit*          mRelativeMonthDayMaxFinish;
    QSpinBox*           mMaxIter;
    QSpinBox*           mTimeLag;

    QString             mCurrPar;
};

}} //namespaces


#endif
