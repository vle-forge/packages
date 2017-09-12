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

#include <iostream>
#include <QMenu>
#include <vle/gvle/gvle_widgets.h>
#include <vle/value/Double.hpp>
#include <vle/value/Integer.hpp>
#include <vle/value/Tuple.hpp>

#include "DecisionPanel.h"
#include "DecisionRightWidget.h"
#include "ui_rightWidget.h"

#include <vle/gvle/vle_dom.hpp>
#include <vle/value/Double.hpp>


namespace vle {
namespace gvle {

using namespace vle::gvle;


enum {
    EDIT_ACTIVITY = 0,
    EDIT_PRECEDENCE = 1,
    NO_EDIT = 2
};

enum {
    FS = 0,
    SS = 1,
    FF = 2
};

/***********************************************/

DecisionRightWidget::DecisionRightWidget(DecisionPanel*  d):
        QWidget(), ui(new Ui::rightWidget), decision(d),
        currAct("")
{
    ui->setupUi(this);

    mMinStartDate = ui->minStart;
    mMaxFinishDate = ui->maxFinish;
    mMinStartCheckBox = ui->minStartCheckBox;
    mMaxFinishCheckBox = ui->maxFinishCheckBox;
    mRelativeYearMinStart = ui->relativeYearMinStart;
    mRelativeYearMaxFinish = ui->relativeYearMaxFinish;
    mRelativeMonthDayMinStart = ui->relativeMonthDayMinStart;
    mRelativeMonthDayMaxFinish = ui->relativeMonthDayMaxFinish;

    mMaxIter = ui->maxIter;
    mTimeLag = ui->timeLag;

    ui->stackedWidget->setCurrentIndex(NO_EDIT);

    QObject::connect(mMinStartDate, SIGNAL(editingFinished()),
                     this, SLOT(onMinStartDate()));

    QObject::connect(mMaxFinishDate, SIGNAL(editingFinished()),
                     this, SLOT(onMaxFinishDate()));

    QObject::connect(mRelativeYearMinStart, SIGNAL(editingFinished()),
                     this, SLOT(onMinStartDate()));

    QObject::connect(mRelativeYearMaxFinish, SIGNAL(editingFinished()),
                     this, SLOT(onMaxFinishDate()));

    QObject::connect(mRelativeMonthDayMinStart, SIGNAL(editingFinished()),
                     this, SLOT(onMinStartDate()));

    QObject::connect(mRelativeMonthDayMaxFinish, SIGNAL(editingFinished()),
                     this, SLOT(onMaxFinishDate()));

    QObject::connect(ui->tableOutputParams,
                     SIGNAL(customContextMenuRequested(const QPoint&)),
                     this, SLOT(onOutParActMenu(const QPoint&)));

    QObject::connect(ui->minStartCheckBox,
                     SIGNAL(stateChanged(int)),
                     this, SLOT(onMinStart(int)));

    QObject::connect(ui->maxFinishCheckBox,
                     SIGNAL(stateChanged(int)),
                     this, SLOT(onMaxFinish(int)));

    QObject::connect(ui->relativeDateCheckBox,
                     SIGNAL(stateChanged(int)),
                     this, SLOT(onRelativeDate(int)));

    QObject::connect(ui->comboBoxType,
                     SIGNAL(currentIndexChanged(int)),
                     this, SLOT(onCurrentIndexChangedPrecedenceType(int)));

    QObject::connect(ui->spinBoxMinLag,
                     SIGNAL(valueChanged(int)),
                     this, SLOT(onPrecedenceMinLag(int)));

    QObject::connect(ui->spinBoxMaxLag,
                     SIGNAL(valueChanged(int)),
                     this, SLOT(onPrecedenceMaxLag(int)));

    QObject::connect(ui->groupBoxMinLag,
                     SIGNAL(clicked(bool)),
                     this, SLOT(onPrecedenceMinLag(bool)));

    QObject::connect(ui->groupBoxMaxLag,
                     SIGNAL(clicked(bool)),
                     this, SLOT(onPrecedenceMaxLag(bool)));

    QObject::connect(mMaxIter, SIGNAL(editingFinished()),
                     this, SLOT(onMaxIter()));

    QObject::connect(mMaxIter, static_cast<void (QSpinBox::*)(int)>(&QSpinBox::valueChanged),
                     this, &DecisionRightWidget::onMaxIterChange);

    QObject::connect(mTimeLag, SIGNAL(editingFinished()),
                     this, SLOT(onTimeLag()));
}

void
DecisionRightWidget::onPrecedenceMinLag(bool checked)
{
    if (checked) {
        decision->dataMetadata->setPrecedenceMinLag(currPrecedenceFirst,
                                                    currPrecedenceSecond,
                                                    ui->spinBoxMinLag->value());
    } else {
        decision->dataMetadata->setPrecedenceMinLag(currPrecedenceFirst,
                                                    currPrecedenceSecond,
                                                    0);
    }
}

void
DecisionRightWidget::onPrecedenceMaxLag(bool checked)
{
    if (checked) {
        decision->dataMetadata->setPrecedenceMaxLag(currPrecedenceFirst,
                                                    currPrecedenceSecond,
                                                    ui->spinBoxMaxLag->value());
    } else {
        decision->dataMetadata->setPrecedenceMaxLag(currPrecedenceFirst,
                                                    currPrecedenceSecond,
                                                    0);
    }
}


void
DecisionRightWidget::onPrecedenceMinLag(int value)
{
    decision->dataMetadata->setPrecedenceMinLag(currPrecedenceFirst,
                                                currPrecedenceSecond,
                                                value);
}

void
DecisionRightWidget::onPrecedenceMaxLag(int value)
{
    decision->dataMetadata->setPrecedenceMaxLag(currPrecedenceFirst,
                                                currPrecedenceSecond,
                                                value);
}

void
DecisionRightWidget::onCurrentIndexChangedPrecedenceType(int index)
{
    decision->dataMetadata->setPrecedenceType(currPrecedenceFirst,
                                              currPrecedenceSecond,
                                              index);
}

void
DecisionRightWidget::onMinStart(int state)
{
     if (state == 2) {
         if (ui->relativeDateCheckBox->isChecked()) {
             int rY = ui->relativeYearMinStart->value();
             QString rDate = "+" + QString::number(rY) + "-" + ui->relativeMonthDayMinStart->text();
             decision->dataMetadata->setMinStart(currAct, rDate);
         } else {
             decision->dataMetadata->setMinStart(currAct, mMinStartDate->text());
         }
     } else {
         if (ui->relativeDateCheckBox->isChecked()) {
             decision->dataMetadata->setMinStart(currAct, "+");
         } else {
             decision->dataMetadata->setMinStart(currAct, "");
         }
     }
}

void
DecisionRightWidget::disableMinStart()
{
    bool oldBlock = ui->minStartCheckBox->blockSignals(true);
    ui->minStartCheckBox->setChecked(false);
    mMinStartDate->setEnabled(false);
    mRelativeYearMinStart->setEnabled(false);
    mRelativeMonthDayMinStart->setEnabled(false);
    ui->minStartCheckBox->blockSignals(oldBlock);
}

void
DecisionRightWidget::disableMaxFinish()
{
    bool oldBlock = ui->maxFinishCheckBox->blockSignals(true);
    ui->maxFinishCheckBox->setChecked(false);
    mMaxFinishDate->setEnabled(false);
    mRelativeYearMaxFinish->setEnabled(false);
    mRelativeMonthDayMaxFinish->setEnabled(false);
    ui->maxFinishCheckBox->blockSignals(oldBlock);
}

void
DecisionRightWidget::enableMinStart()
{
    bool oldBlock = ui->minStartCheckBox->blockSignals(true);
    ui->minStartCheckBox->setChecked(true);
    if (ui->relativeDateCheckBox->isChecked()) {
        mMinStartDate->setEnabled(false);
        mRelativeYearMinStart->setEnabled(true);
        mRelativeMonthDayMinStart->setEnabled(true);
    } else {
        mMinStartDate->setEnabled(true);
        mRelativeYearMinStart->setEnabled(false);
        mRelativeMonthDayMinStart->setEnabled(false);
    }
    ui->minStartCheckBox->blockSignals(oldBlock);
}

void
DecisionRightWidget::enableMaxFinish()
{
    bool oldBlock = ui->maxFinishCheckBox->blockSignals(true);
    ui->maxFinishCheckBox->setChecked(true);
    if (ui->relativeDateCheckBox->isChecked()) {
        mMaxFinishDate->setEnabled(false);
        mRelativeYearMaxFinish->setEnabled(true);
        mRelativeMonthDayMaxFinish->setEnabled(true);
    } else {
        mMaxFinishDate->setEnabled(true);
        mRelativeYearMaxFinish->setEnabled(false);
        mRelativeMonthDayMaxFinish->setEnabled(false);
    }
    ui->maxFinishCheckBox->blockSignals(oldBlock);
}

void
DecisionRightWidget::onMaxFinish(int state)
{
    if (state == 2) {
        if (ui->relativeDateCheckBox->isChecked()) {
            int rY = ui->relativeYearMaxFinish->value();
            QString rDate = "+" + QString::number(rY) + "-" + ui->relativeMonthDayMaxFinish->text();
            decision->dataMetadata->setMaxFinish(currAct, rDate);
        } else {
            decision->dataMetadata->setMaxFinish(currAct, mMaxFinishDate->text());
        }
    } else {
        if (ui->relativeDateCheckBox->isChecked()) {
            decision->dataMetadata->setMaxFinish(currAct, "+");
        } else {
            decision->dataMetadata->setMaxFinish(currAct, "");
        }
    }
}

void
DecisionRightWidget::onRelativeDate(int state)
{
    if (state == 2) {
        if (ui->minStartCheckBox->isChecked()) {
            mMinStartDate->setEnabled(false);
            mRelativeYearMinStart->setEnabled(true);
            mRelativeMonthDayMinStart->setEnabled(true);
            int rY = ui->relativeYearMinStart->value();
            QString rDate = "+" + QString::number(rY) + "-" + ui->relativeMonthDayMinStart->text();
            decision->dataMetadata->setMinStart(currAct, rDate);
        } else {
             decision->dataMetadata->setMinStart(currAct,"+");
        }

        if (ui->maxFinishCheckBox->isChecked()) {
            mMaxFinishDate->setEnabled(false);
            mRelativeYearMaxFinish->setEnabled(true);
            mRelativeMonthDayMaxFinish->setEnabled(true);
            int rY = ui->relativeYearMaxFinish->value();
            QString rDate = "+" + QString::number(rY) + "-" + ui->relativeMonthDayMaxFinish->text();
            decision->dataMetadata->setMaxFinish(currAct, rDate);
        } else {
            decision->dataMetadata->setMaxFinish(currAct,"+");
        }

    } else {
         if (ui->minStartCheckBox->isChecked()) {
             mMinStartDate->setEnabled(true);
             mRelativeYearMinStart->setEnabled(false);
             mRelativeMonthDayMinStart->setEnabled(false);
             decision->dataMetadata->setMinStart(currAct, mMinStartDate->text());
         } else {
             decision->dataMetadata->setMinStart(currAct,"");
         }
         if (ui->maxFinishCheckBox->isChecked()) {
             mMaxFinishDate->setEnabled(true);
             mRelativeYearMaxFinish->setEnabled(false);
             mRelativeMonthDayMaxFinish->setEnabled(false);
             decision->dataMetadata->setMaxFinish(currAct, mMaxFinishDate->text());
         } else {
             decision->dataMetadata->setMaxFinish(currAct,"");
         }
    }
}

void
DecisionRightWidget::onOutParActMenu(const QPoint& pos)
{
    QPoint globalPos = ui->tableOutputParams->viewport()->mapToGlobal(pos);
    QModelIndex index = ui->tableOutputParams->indexAt(pos);


    QString parName = "";
    if (index.row() >= 0 ){
        VleLineEdit* item = (VleLineEdit*)ui->tableOutputParams->cellWidget(
            index.row(), 0);
        parName = item->text();
    }

    QAction* action;
    QMenu menu;
    action = menu.addAction("Add Output");
    action->setData(1);
    action = menu.addAction("Remove");
    action->setData(2);
    action->setEnabled(parName != "");

    QAction* selAction = menu.exec(globalPos);
    if (selAction) {
        int actCode =  selAction->data().toInt();
        switch(actCode) {
        case 1: //Add variable
            decision->dataMetadata->addOutParActToDoc(currAct,
                                                      decision->dataMetadata->newOutParToDoc());
            mCurrPar = parName;
            emit undoAvailable(true);
            break;
        case 2: //Remove
            decision->dataMetadata->rmOutputParActToDoc(currAct, parName);
            mCurrPar = parName;
            emit undoAvailable(true);
            break;
        }
    }
}

DecisionRightWidget::~DecisionRightWidget()
{
    delete ui;
}

void
DecisionRightWidget::onTextUpdated(const QString& /*id*/,
                                 const QString& oldname,
                                 const QString& newname)
{
    if (oldname != newname) {
        decision->dataMetadata->renameOutParActToDoc(currAct, oldname, newname);
    }
    mCurrPar =  "";
    reload(); //to manage the case when new name already used
}

VleLineEdit*
DecisionRightWidget::getTextEdit(int row, int col)
{
    return qobject_cast<VleLineEdit*>(
            ui->tableOutputParams->cellWidget(row,col));
}

void
DecisionRightWidget::onSelected(const QString& id)
{
    QStringList split = id.split(",");
    int row = split.at(0).toInt();
    mCurrPar =  getTextEdit(row, 0)->text();
}

void
DecisionRightWidget::onValueUpdated(const QString& id, double value)
{
    QStringList split = id.split(",");
    int row = split.at(0).toInt();
    mCurrPar =  getTextEdit(row, 0)->text();
    if (mCurrPar == "") {
        return;
    }

    decision->dataMetadata->setValueOutParAct(currAct, mCurrPar, value);

    reload();
}

void
DecisionRightWidget::insertRowOut(int row, const QString& outParName)
{
    QDomNode par = decision->dataMetadata->nodeOutParAct(currAct, outParName);
    double value = par.attributes().namedItem("value").nodeValue().toDouble();

    int col = 0;

    //insert Name
    {
        col = 0;
        QString id = QString("%1,%2").arg(row).arg(col);
        VleLineEdit* w = new VleLineEdit(ui->tableOutputParams, outParName, id, false);
        ui->tableOutputParams->setCellWidget(row, col, w);
        ui->tableOutputParams->setItem(row, col, new QTableWidgetItem);
        QObject::connect(w, SIGNAL(
                textUpdated(const QString&, const QString&, const QString&)),
                this, SLOT(
                onTextUpdated(const QString&, const QString&, const QString&)));
        QObject::connect(w, SIGNAL(selected(const QString&)),
                this, SLOT(onSelected(const QString&)));
    }
    //insert value
    {
        col = 1;
        QString id = QString("%1,%2").arg(row).arg(col);
        VleDoubleEdit* w = new VleDoubleEdit(ui->tableOutputParams, value, id, true);
        QObject::connect(w, SIGNAL(selected(const QString&)),
                            this, SLOT(onSelected(const QString&)));
        QObject::connect(w, SIGNAL(valUpdated(const QString&, double)),
                         this, SLOT(onValueUpdated(const QString&, double)));
        ui->tableOutputParams->setCellWidget(row, col, w);
        ui->tableOutputParams->setItem(row, col, new QTableWidgetItem);

    }
}

void
DecisionRightWidget::reloadRules()
{
    if (not currAct.isEmpty()) {
        bool oldBlock = ui->listRules->blockSignals(true);
        ui->listRules->clear();
        QSet<QString> rules = decision->dataMetadata->rulesNames();
        QSet<QString>::iterator itb = rules.begin();
        QSet<QString>::iterator ite = rules.end();
        for (; itb != ite; itb++) {
            QListWidgetItem* wi = new QListWidgetItem(ui->listRules);
            ui->listRules->addItem(wi);
            QCheckBox* cb = new QCheckBox(this);
            cb->setText(*itb);
            if (decision->dataMetadata->existRuleActToDoc(currAct, *itb)) {
                cb->setCheckState(Qt::Checked);
            } else {
                cb->setCheckState(Qt::Unchecked);
            }

            QObject::connect(
                cb, SIGNAL(toggled(bool)), this, SLOT(onCheckboxRuleToggle(bool)));
            ui->listRules->setItemWidget(wi, cb);
        }
        ui->listRules->blockSignals(oldBlock);
    }
}

void
DecisionRightWidget::reload()
{
    if (not currAct.isEmpty() and
        not decision->dataMetadata->existActToDoc(currAct)) {
        currAct = "";
    }

    if (not currPrecedenceFirst.isEmpty() and
        not currPrecedenceSecond.isEmpty() and
        not decision->dataMetadata->existPrecedenceToDoc(currPrecedenceFirst,
                                                         currPrecedenceSecond)) {
          currPrecedenceFirst= "";
          currPrecedenceSecond= "";
    }

    if (not currAct.isEmpty()) {

        ui->actName->setText(currAct);

        if (decision->dataMetadata->isRelativeDate(currAct)) {

            ui->relativeDateCheckBox->blockSignals(true);
            ui->relativeDateCheckBox->setChecked(true);
            ui->relativeDateCheckBox->blockSignals(false);
            {
                if (decision->dataMetadata->getMinStart(currAct) == "+"||
                    decision->dataMetadata->getMinStart(currAct) == "") {
                    disableMinStart();
                } else {
                    enableMinStart();
                    bool oldBlock1 = ui->relativeYearMinStart->blockSignals(true);
                    bool oldBlock2 = ui->relativeMonthDayMinStart->blockSignals(true);

                    QString rDString = decision->dataMetadata->getMinStart(currAct);
                    QStringList rDSlist = rDString.split(QRegExp("(\\+|\\-)"), QString::SkipEmptyParts);
                    int rY = rDSlist[0].toInt();
                    QString rMD = rDSlist[1]+ "-" + rDSlist[2];

                    ui->relativeYearMinStart->setValue(rY);
                    ui->relativeMonthDayMinStart->setDate(QDate::fromString(rMD, "M-d"));

                    ui->relativeYearMinStart->blockSignals(oldBlock1);
                    ui->relativeMonthDayMinStart->blockSignals(oldBlock2);
                }
            }
            {
                if (decision->dataMetadata->getMaxFinish(currAct) == "+"||
                    decision->dataMetadata->getMaxFinish(currAct) == "") {
                    disableMaxFinish();
                } else {
                    enableMaxFinish();
                    bool oldBlock1 = ui->relativeYearMaxFinish->blockSignals(true);
                    bool oldBlock2 = ui->relativeMonthDayMaxFinish->blockSignals(true);

                    QString rDString = decision->dataMetadata->getMaxFinish(currAct);
                    QStringList rDSlist = rDString.split(QRegExp("(\\+|\\-)"), QString::SkipEmptyParts);
                    int rY =  rDSlist[0].toInt();
                    QString rMD = rDSlist[1]+ "-" + rDSlist[2];
                    ui->relativeYearMaxFinish->setValue(rY);
                    ui->relativeMonthDayMaxFinish->setDate(QDate::fromString(rMD, "M-d"));

                    ui->relativeYearMaxFinish->blockSignals(oldBlock1);
                    ui->relativeMonthDayMaxFinish->blockSignals(oldBlock2);
                }
            }
        } else {
            ui->relativeDateCheckBox->blockSignals(true);
            ui->relativeDateCheckBox->setChecked(false);
            ui->relativeDateCheckBox->blockSignals(false);
            bool oldBlock = ui->minStart->blockSignals(true);
            QString restoreLocale = QLocale::system().name();
            QLocale::setDefault(QLocale::C);

            if (decision->dataMetadata->getMinStart(currAct) == "") {
                disableMinStart();
            } else {
                enableMinStart();
                ui->minStart->setDate(QDate::fromString(decision->dataMetadata->getMinStart(currAct),
                                                        "yyyy-M-d"));
            }
            ui->minStart->blockSignals(oldBlock);

            oldBlock = ui->maxFinish->blockSignals(true);
            if (decision->dataMetadata->getMaxFinish(currAct) == "") {
                disableMaxFinish();
            } else {
                enableMaxFinish();
                ui->maxFinish->setDateTime(QLocale().toDateTime(decision->dataMetadata->getMaxFinish(currAct),
                                                                "yyyy-M-d"));
            }
            ui->maxFinish->blockSignals(oldBlock);

        }

        //reload iterate
        {
            bool oldBlock = ui->maxIter->blockSignals(true);
            ui->maxIter->setValue(decision->dataMetadata->getMaxIter(currAct));
            ui->maxIter->blockSignals(oldBlock);
        }
        {
            bool oldBlock = ui->timeLag->blockSignals(true);
            ui->timeLag->setValue(decision->dataMetadata->getTimeLag(currAct));
            mTimeLag->setEnabled(not (decision->dataMetadata->getMaxIter(currAct) == 1));
            ui->timeLag->blockSignals(oldBlock);
        }

        //reload table of variables

        // Clear the current list content
        ui->tableOutputParams->clearContents();
        ui->tableOutputParams->setRowCount(0);

        QDomNodeList params = decision->dataMetadata->outParActFromDoc(currAct);
        for (int i = 0; i < params.length(); i++) {
            QDomNode variable = params.item(i);
            QString name = variable.attributes().namedItem("name").nodeValue();
            ui->tableOutputParams->insertRow(i);
            insertRowOut(i, name);
        }

        reloadRules();

        ui->stackedWidget->setCurrentIndex(EDIT_ACTIVITY);

    } else if (not currPrecedenceFirst.isEmpty() and
               not currPrecedenceSecond.isEmpty()) {
        ui->First->setText(currPrecedenceFirst);
        ui->Second->setText(currPrecedenceSecond);

        QString typeString = decision->dataMetadata->getPrecedenceType(currPrecedenceFirst,
                                                                       currPrecedenceSecond);
        int idCB;
        if (typeString == "FS") {
            idCB = FS;
        } else if (typeString == "SS") {
            idCB = SS;
        } else {
            idCB = FF;
        }
        bool oldBlock = ui->comboBoxType->blockSignals(true);
        ui->comboBoxType->setCurrentIndex(idCB);
        ui->comboBoxType->blockSignals(oldBlock);

        {
            QString minLagString = decision->dataMetadata->getPrecedenceMinLag(currPrecedenceFirst,
                                                                               currPrecedenceSecond);
            bool oldBlock = ui->groupBoxMinLag->blockSignals(true);
            if (minLagString == "") {
                ui->groupBoxMinLag->setChecked(false);
            } else {
                ui->groupBoxMinLag->setChecked(true);
                ui->spinBoxMinLag->setValue(minLagString.toInt());

            }
            ui->groupBoxMinLag->blockSignals(oldBlock);
        }
        {
            QString maxLagString = decision->dataMetadata->getPrecedenceMaxLag(currPrecedenceFirst,
                                                                               currPrecedenceSecond);
            bool oldBlock = ui->groupBoxMaxLag->blockSignals(true);
            if (maxLagString == "") {
                ui->groupBoxMaxLag->setChecked(false);
            } else {
                ui->groupBoxMaxLag->setChecked(true);
                ui->spinBoxMaxLag->setValue(maxLagString.toInt());

            }
            ui->groupBoxMaxLag->blockSignals(oldBlock);
        }
        ui->stackedWidget->setCurrentIndex(EDIT_PRECEDENCE);
    } else {
        ui->stackedWidget->setCurrentIndex(NO_EDIT);
    }

}
void
DecisionRightWidget::onCheckboxRuleToggle(bool checked)
{
    // Get the QCheckbox that emit signal
    QObject* sender = QObject::sender();
    QCheckBox* cb = qobject_cast<QCheckBox*>(sender);

    if (checked) {
        decision->dataMetadata->addRuleActToDoc(currAct, cb->text());
    } else {
        decision->dataMetadata->rmRuleActToDoc(currAct, cb->text());
    }
}

void
DecisionRightWidget::onActSelected(QString name)
{
    currAct = name;
    currPrecedenceFirst = "";
    currPrecedenceSecond = "";
    reload();
}

void
DecisionRightWidget::onPrecedenceSelected(QString first, QString second)
{
    currAct = "";
    currPrecedenceFirst = first;
    currPrecedenceSecond = second;
    reload();
}

void
DecisionRightWidget::onNothingSelected()
{
    currAct = "";
    currPrecedenceFirst = "";
    currPrecedenceSecond = "";
    ui->stackedWidget->setCurrentIndex(NO_EDIT);
}

void
DecisionRightWidget::onMinStartDate()
{
    if (ui->relativeDateCheckBox->isChecked()) {
        int rY = ui->relativeYearMinStart->value();
        QString rDate = "+" + QString::number(rY) + "-" + ui->relativeMonthDayMinStart->text();
        decision->dataMetadata->setMinStart(currAct, rDate);
    } else {
        decision->dataMetadata->setMinStart(currAct, mMinStartDate->text());
    }
}

void
DecisionRightWidget::onMaxFinishDate()
{
    if (ui->relativeDateCheckBox->isChecked()) {
        int rY = ui->relativeYearMaxFinish->value();
        QString rDate = "+" + QString::number(rY) + "-" + ui->relativeMonthDayMaxFinish->text();
        decision->dataMetadata->setMaxFinish(currAct, rDate);
    } else {
        decision->dataMetadata->setMaxFinish(currAct, mMaxFinishDate->text());
    }
}

void
DecisionRightWidget::onMaxIter()
{
    int maxIter = ui->maxIter->value();

    mTimeLag->setEnabled(not (maxIter == 1));

    decision->dataMetadata->setMaxIter(currAct, maxIter);

}

void
DecisionRightWidget::onMaxIterChange(int i)
{
    mTimeLag->setEnabled(not (i == 1));
}

void
DecisionRightWidget::onTimeLag()
{
    int timeLag = ui->timeLag->value();

    decision->dataMetadata->setTimeLag(currAct, timeLag);

}

void
DecisionRightWidget::onDmModified(int modifType)
{
    switch((MODIF_TYPE) modifType) {
    case RENAMERULE:
        reloadRules();
        break;
    case RENAME:
//        currMatFlow.clear();
//        currAct.clear();
//        currParam.clear();
        reload();
        break;
    default:
        reload();
        break;
    }
}

}} //namespaces
