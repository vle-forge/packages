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

#include <QMenu>
#include <vle/gvle/gvle_widgets.h>
#include <vle/value/Double.hpp>
#include <vle/value/Integer.hpp>
#include <vle/value/Tuple.hpp>

#include "DiscreteTimePanel.h"
#include "ui_leftWidget.h"
#include "ui_rightWidget.h"

namespace vv = vle::value;

namespace vle {
namespace gvle {

DiscreteTimePanel::DiscreteTimePanel():
        PluginMainPanel(), left(new DiscreteTimeLeftWidget),
        right(new DiscreteTimeRightWidget), cppMetadata(0), mCurrVar("")
{
    left->ui->computeContent->addWidget(
        new VleTextEdit(left, "","", true));
    left->ui->constructorContent->addWidget(
        new VleTextEdit(left, "","", true));
    left->ui->userSectionContent->addWidget(
        new VleTextEdit(left, "","", true));
    left->ui->includesContent->addWidget(
        new VleTextEdit(left, "","", true));

    left->ui->boxLayout->addWidget(
        new VleDoubleEdit(left->ui->boxTimeStep, 1., "dummy"));

    QObject::connect(right->ui->tableVars,
            SIGNAL(customContextMenuRequested(const QPoint&)),
            this, SLOT(onTableVarsMenu(const QPoint&)));
}

DiscreteTimePanel::~DiscreteTimePanel()
{
    delete left->ui;
    delete right->ui;
}

QString
DiscreteTimePanel::getname()
{
    return "discrete-time";
}
QWidget*
DiscreteTimePanel::leftWidget()
{
    return left;
}
QWidget*
DiscreteTimePanel::rightWidget()
{
    return right;
}
void
DiscreteTimePanel::undo()
{
    cppMetadata->undo();
    reload();
}
void
DiscreteTimePanel::redo()
{
    cppMetadata->redo();
    reload();
}

void
DiscreteTimePanel::init(const gvle_file& gf, utils::Package* pkg, Logger* ,
        gvle_plugins*, const utils::ContextPtr&)
{

    cppMetadata = new vleSmDT(gf.source_file, gf.metadata_file, getname());
    cppMetadata->setPackageToDoc(pkg->name().c_str());
    cppMetadata->setClassNameToDoc(gf.baseName());
    cppMetadata->save();

    getComputeWidget()->setText(cppMetadata->getComputeBody());
    getConstructorWidget()->setText(cppMetadata->getConstructorBody());
    getUserSectionWidget()->setText(cppMetadata->getUserSectionBody());
    getIncludesWidget()->setText(cppMetadata->getIncludesBody());

    getTimeStepBox()->setChecked(cppMetadata->hasTimeStep());
    if (cppMetadata->hasTimeStep()) {
        getTimeStepWidget()->setValue(cppMetadata->getTimeStep());
    }

    QObject::connect(getComputeWidget(),
            SIGNAL(textUpdated(const QString&, const QString&, const QString&)),
            this,
            SLOT(onComputeChanged(const QString&, const QString&,
                                  const QString&)));
    QObject::connect(getConstructorWidget(),
            SIGNAL(textUpdated(const QString&, const QString&, const QString&)),
            this,
            SLOT(onConstructorChanged(const QString&, const QString&,
                                  const QString&)));
    QObject::connect(getUserSectionWidget(),
            SIGNAL(textUpdated(const QString&, const QString&, const QString&)),
            this,
            SLOT(onUserSectionChanged(const QString&, const QString&,
                                  const QString&)));
    QObject::connect(getIncludesWidget(),
            SIGNAL(textUpdated(const QString&, const QString&, const QString&)),
            this,
            SLOT(onIncludesChanged(const QString&, const QString&,
                                  const QString&)));
    QObject::connect(getTimeStepBox(),
                     SIGNAL(clicked(bool)),
                     this,
                     SLOT(onParamTimeStep(bool)));

    QObject::connect(getTimeStepWidget(),
                     SIGNAL(valUpdated(const QString&, double)),
                     this,
                     SLOT(onTimeStepUpdated(const QString&, double)));

    QObject::connect(cppMetadata, SIGNAL(undoAvailable(bool)),
                     this, SLOT(onUndoAvailable(bool)));

    QObject::connect(cppMetadata, SIGNAL(modified()),
                     this, SLOT(onModified()));

    reload();

}

QString
DiscreteTimePanel::canBeClosed()
{
    return "";
}

void
DiscreteTimePanel::save()
{
    cppMetadata->save();
}

void
DiscreteTimePanel::discard()
{
    vleSmDT* cppDiscarded = new vleSmDT( cppMetadata->getSrcPath(),
                                         cppMetadata->getSmPath(),
                                         getname());

    cppDiscarded->provideCpp();
}

PluginMainPanel*
DiscreteTimePanel::newInstance()
{
    return new DiscreteTimePanel;
}

VleTextEdit*
DiscreteTimePanel::getComputeWidget()
{
    return (VleTextEdit*)left->ui->computeContent->itemAt(0)->widget();
}

VleTextEdit*
DiscreteTimePanel::getConstructorWidget()
{
    return (VleTextEdit*)left->ui->constructorContent->itemAt(0)->widget();
}

VleTextEdit*
DiscreteTimePanel::getUserSectionWidget()
{
    return (VleTextEdit*)left->ui->userSectionContent->itemAt(0)->widget();
}

VleTextEdit*
DiscreteTimePanel::getIncludesWidget()
{
    return (VleTextEdit*)left->ui->includesContent->itemAt(0)->widget();
}

VleDoubleEdit*
DiscreteTimePanel::getTimeStepWidget()
{
    return (VleDoubleEdit*)left->ui->boxLayout->itemAt(0)->widget();
}

QGroupBox*
DiscreteTimePanel::getTimeStepBox()
{
    return left->ui->boxTimeStep;
}

void
DiscreteTimePanel::reload()
{
    //left widget
    getComputeWidget()->setText(cppMetadata->getComputeBody());
    getConstructorWidget()->setText(cppMetadata->getConstructorBody());
    getUserSectionWidget()->setText(cppMetadata->getUserSectionBody());
    getIncludesWidget()->setText(cppMetadata->getIncludesBody());
    getTimeStepBox()->setChecked(cppMetadata->hasTimeStep());
    if (cppMetadata->hasTimeStep()) {
        getTimeStepWidget()->setValue(cppMetadata->getTimeStep());
    }

    //reload table of variables

    // Clear the current list content
    right->ui->tableVars->clearContents();
    right->ui->tableVars->setRowCount(0);

    QDomNodeList variablesXml = cppMetadata->variablesFromDoc();
    for (int i = 0; i < variablesXml.length(); i++) {
        QDomNode variable = variablesXml.item(i);
        QString name = variable.attributes().namedItem("name").nodeValue();
        right->ui->tableVars->insertRow(i);
        insertTextEdit(i, 0, name);
        insertSpinBoxDim(i, 1, (cppMetadata->getDim(name))->toInteger().value());
        insertSpinBoxHistory(i, 2, (cppMetadata->getHistorySize(name))->toInteger().value());
        insertSpinBoxSync(i, 3, (cppMetadata->getSync(name))->toInteger().value());
        insertBooleanIn(i, 4, cppMetadata->hasInFromDoc(name));
        insertBooleanOut(i, 5, cppMetadata->hasOutFromDoc(name));
        insertBooleanObs(i, 6, cppMetadata->hasObsFromDoc(name));
        insertBooleanParam(i, 7, cppMetadata->isParametrable(name));
    }

    updateConfigVar();
}

void
DiscreteTimePanel::insertTextEdit(int row, int col, const QString& val)
{
    QString id = QString("%1,%2").arg(row).arg(col);
    VleTextEdit* w = new VleTextEdit(right->ui->tableVars, val, id, true);
    right->ui->tableVars->setCellWidget(row, col, w);
    right->ui->tableVars->setItem(row, col, new QTableWidgetItem);//used to find it
    QObject::connect(w, SIGNAL(
            textUpdated(const QString&, const QString&, const QString&)),
            this, SLOT(
            onTextUpdated(const QString&, const QString&, const QString&)));
    QObject::connect(w, SIGNAL(selected(const QString&)),
            this, SLOT(onSelected(const QString&)));
}

void
DiscreteTimePanel::insertBooleanIn(int row, int col, bool val)
{

    QString id = QString("%1,%2").arg(row).arg(col);
    VleBooleanEdit* w = new VleBooleanEdit(right->ui->tableVars, val, id);

    right->ui->tableVars->setCellWidget(row, col, w);
    right->ui->tableVars->setItem(row, col, new QTableWidgetItem);

    QObject::connect(w, SIGNAL(valUpdated(const QString&, bool)),
            this, SLOT(onIn(const QString&, bool)));
    QObject::connect(w, SIGNAL(selected(const QString&)),
            this, SLOT(onSelected(const QString&)));
}

void
DiscreteTimePanel::insertBooleanOut(int row, int col, bool val)
{
    QString id = QString("%1,%2").arg(row).arg(col);
    VleBooleanEdit* w = new VleBooleanEdit(right->ui->tableVars, val, id);

    right->ui->tableVars->setCellWidget(row, col, w);
    right->ui->tableVars->setItem(row, col, new QTableWidgetItem);

    QObject::connect(w, SIGNAL(valUpdated(const QString&, bool)),
            this, SLOT(onOut(const QString&, bool)));
    QObject::connect(w, SIGNAL(selected(const QString&)),
            this, SLOT(onSelected(const QString&)));
}

void
DiscreteTimePanel::insertBooleanObs(int row, int col, bool val)
{
    QString id = QString("%1,%2").arg(row).arg(col);
    VleBooleanEdit* w = new VleBooleanEdit(right->ui->tableVars, val, id);

    right->ui->tableVars->setCellWidget(row, col, w);
    right->ui->tableVars->setItem(row, col, new QTableWidgetItem);

    QObject::connect(w, SIGNAL(valUpdated(const QString&, bool)),
            this, SLOT(onObs(const QString&, bool)));
    QObject::connect(w, SIGNAL(selected(const QString&)),
            this, SLOT(onSelected(const QString&)));
}

void
DiscreteTimePanel::insertBooleanParam(int row, int col, bool val)
{
    QString id = QString("%1,%2").arg(row).arg(col);
    VleBooleanEdit* w = new VleBooleanEdit(right->ui->tableVars, val, id);

    right->ui->tableVars->setCellWidget(row, col, w);
    right->ui->tableVars->setItem(row, col, new QTableWidgetItem);

    QObject::connect(w, SIGNAL(valUpdated(const QString&, bool)),
            this, SLOT(onParam(const QString&, bool)));
    QObject::connect(w, SIGNAL(selected(const QString&)),
            this, SLOT(onSelected(const QString&)));
}

void
DiscreteTimePanel::insertSpinBoxHistory(int row, int col, int val)
{
    QString id = QString("%1,%2").arg(row).arg(col);
    VleSpinBox* w = new VleSpinBox(right->ui->tableVars, val,
                                   id, 0);

    right->ui->tableVars->setCellWidget(row, col, w);
    right->ui->tableVars->setItem(row, col, new QTableWidgetItem);

    QObject::connect(w, SIGNAL(valUpdated(const QString&, int)),
                     this, SLOT(onHistoryUpdated(const QString&, int)));
    QObject::connect(w, SIGNAL(selected(const QString&)),
                     this, SLOT(onSelected(const QString&)));
}

void
DiscreteTimePanel::insertSpinBoxDim(int row, int col, int val)
{
    QString id = QString("%1,%2").arg(row).arg(col);
    VleSpinBox* w = new VleSpinBox(right->ui->tableVars, val,
                                   id, 1);

    right->ui->tableVars->setCellWidget(row, col, w);
    right->ui->tableVars->setItem(row, col, new QTableWidgetItem);

    QObject::connect(w, SIGNAL(valUpdated(const QString&, int)),
                     this, SLOT(onDimUpdated(const QString&, int)));
    QObject::connect(w, SIGNAL(selected(const QString&)),
                     this, SLOT(onSelected(const QString&)));
}
void
DiscreteTimePanel::insertSpinBoxSync(int row, int col, int val)
{
    QString id = QString("%1,%2").arg(row).arg(col);
    VleSpinBox* w = new VleSpinBox(right->ui->tableVars, val,
                                   id, 0);

    right->ui->tableVars->setCellWidget(row, col, w);
    right->ui->tableVars->setItem(row, col, new QTableWidgetItem);

    QObject::connect(w, SIGNAL(valUpdated(const QString&, int)),
                     this, SLOT(onSyncUpdated(const QString&, int)));
    QObject::connect(w, SIGNAL(selected(const QString&)),
                     this, SLOT(onSelected(const QString&)));
}

VleTextEdit*
DiscreteTimePanel::getTextEdit(int row, int col)
{
    return qobject_cast<VleTextEdit*>(
            right->ui->tableVars->cellWidget(row,col));
}

void
DiscreteTimePanel::onModified()
{
    cppMetadata->provideCpp();
}


void
DiscreteTimePanel::onUndoAvailable(bool b)
{
    emit undoAvailable(b);
}

void
DiscreteTimePanel::onParamTimeStep(bool checked)
{
    if (checked) {
        vv::Double* value = new vv::Double(1.);
        cppMetadata->setTimeStep(*value);
        getTimeStepWidget()->setValue(1.);
    } else {
        cppMetadata->UnSetTimeStep();
    }
}

void
DiscreteTimePanel::onTimeStepUpdated(const QString& /* id */, double val)
{
    vv::Double* value = new vv::Double(val);
    cppMetadata->setTimeStep(*value);
}

void
DiscreteTimePanel::onComputeChanged(const QString& /* id */,
                                    const QString& old,
                                    const QString& newVal)
{
    if (old != newVal) {
        cppMetadata->setComputeToDoc(newVal);
    }
}

void
DiscreteTimePanel::onUserSectionChanged(const QString& /* id */,
                                        const QString& old,
                                        const QString& newVal)
{
    if (old != newVal) {
        cppMetadata->setUserSectionToDoc(newVal);
    }
}

void
DiscreteTimePanel::onConstructorChanged(const QString& /* id */,
                                        const QString& old,
                                        const QString& newVal)
{
    if (old != newVal) {
        cppMetadata->setConstructorToDoc(newVal);
    }
}

void
DiscreteTimePanel::onIncludesChanged(const QString& /* id */,
                                     const QString& old,
                                     const QString& newVal)
{
    if (old != newVal) {
        cppMetadata->setIncludesToDoc(newVal);
    }
}

void
DiscreteTimePanel::onTableVarsMenu(const QPoint& pos)
{
    QPoint globalPos = right->ui->tableVars->viewport()->mapToGlobal(pos);
    QModelIndex index = right->ui->tableVars->indexAt(pos);
    VleTextEdit* item = (VleTextEdit*)right->ui->tableVars->cellWidget(index.row(), index.column());

    QAction* action;
    QMenu menu;
    action = menu.addAction("Add variable");
    action->setData(1);
    action = menu.addAction("Remove");
    action->setData(2);
    action->setEnabled(item != 0);

    QAction* selAction = menu.exec(globalPos);
    if (selAction) {
        int actCode =  selAction->data().toInt();
        switch(actCode) {
        case 1: //Add variable
            cppMetadata->addVariableToDoc(cppMetadata->newVarNameToDoc());
            reload();
            emit undoAvailable(true);
            break;
        case 2: //Remove
            cppMetadata->rmVariableToDoc(item->getSavedText());
            mCurrVar = "";
            reload();
            emit undoAvailable(true);
            break;

        }
    }
}

void
DiscreteTimePanel::onSetCompute()
{
    QString Body = getComputeWidget()->toPlainText();
    cppMetadata->setComputeToDoc(Body);
}

void
DiscreteTimePanel::onSetConstructor()
{
    QString Body = getConstructorWidget()->toPlainText();
    cppMetadata->setConstructorToDoc(Body);
}

void
DiscreteTimePanel::onSetUserSection()
{
    QString Body = getUserSectionWidget()->toPlainText();
    cppMetadata->setUserSectionToDoc(Body);
}

void
DiscreteTimePanel::onSetIncludes()
{
    QString Body = getIncludesWidget()->toPlainText();
    cppMetadata->setIncludesToDoc(Body);
}

void
DiscreteTimePanel::onSelected(const QString& id)
{
    QStringList split = id.split(",");
    int row = split.at(0).toInt();
    mCurrVar =  getTextEdit(row, 0)->getCurrentText();
    updateConfigVar();

}

void
DiscreteTimePanel::onInitialValue(const QString& /*id*/, double val)
{
    if (mCurrVar == "") {
        return;
    }
    cppMetadata->setInitialValue(mCurrVar,vv::Double(val));
}


void
DiscreteTimePanel::onTextUpdated(const QString& /*id*/,
                                 const QString& oldname,
                                 const QString& newname)
{
    if (oldname != newname) {
        cppMetadata->renameVariableToDoc(oldname, newname);
    }
    mCurrVar =  "";
    reload(); //to manage the case when new name already used
}


void
DiscreteTimePanel::onIn(const QString& /*id*/, bool val)
{
    if (mCurrVar == "") {
        return;
    }
    if (val) {
        cppMetadata->addInToDoc(mCurrVar);
    } else {
        cppMetadata->rmInToDoc(mCurrVar);
    }
}

void
DiscreteTimePanel::onOut(const QString& /*id*/, bool val)
{
    if (mCurrVar == "") {
        return;
    }
    if (val) {
        cppMetadata->addOutToDoc(mCurrVar);
    } else {
        cppMetadata->rmOutToDoc(mCurrVar);
    }
}

void
DiscreteTimePanel::onParam(const QString& /*id*/, bool val)
{
    if (mCurrVar == "") {
        return;
    }
    cppMetadata->Parametrable(mCurrVar, val, true);
    reload(); //only to "disable" the checkbox when not appropriate
}

void
DiscreteTimePanel::onObs(const QString& /*id*/, bool val)
{
    if (mCurrVar == "") {
        return;
    }
    if (val) {
        cppMetadata->addObsToDoc(mCurrVar);
    } else {
        cppMetadata->rmObsToDoc(mCurrVar);
    }
}

void
DiscreteTimePanel::onValUpdated(const vv::Value& newVal)
{
    if (mCurrVar == "") {
        return;
    }

    cppMetadata->setInitialValue(mCurrVar,newVal);
}

void
DiscreteTimePanel::onHistoryUpdated(const QString& id, int value)
{
    QStringList split = id.split(",");
    int row = split.at(0).toInt();
    mCurrVar =  getTextEdit(row, 0)->getCurrentText();
    if (mCurrVar == "") {
        return;
    }

    cppMetadata->setHistorySizeAndValue(mCurrVar, vv::Integer(value));

    reload();
}

void
DiscreteTimePanel::onDimUpdated(const QString& id, int value)
{
    QStringList split = id.split(",");
    int row = split.at(0).toInt();
    mCurrVar =  getTextEdit(row, 0)->getCurrentText();
    if (mCurrVar == "") {
        return;
    }

    cppMetadata->setDimAndValue(mCurrVar, vv::Integer(value));

    reload();
}

void
DiscreteTimePanel::onSyncUpdated(const QString& id, int value)
{
    QStringList split = id.split(",");
    int row = split.at(0).toInt();
    mCurrVar =  getTextEdit(row, 0)->getCurrentText();
    if (mCurrVar == "") {
        return;
    }
    cppMetadata->setSync(mCurrVar,vv::Integer(value));
}

void
DiscreteTimePanel::updateConfigVar()
{
    if (mCurrVar == "") {
        right->ui->varLabel->setText("");
        return;
    }

    QString lab;
    if (not cppMetadata->isParametrable(mCurrVar)) {
        lab = mCurrVar + " fixed initial conditions";
    } else {
        lab = mCurrVar + " default initial conditions";
    }
    right->ui->varLabel->setText(lab);

    std::unique_ptr<vv::Value> val = std::move(
            cppMetadata->getInitialValue(mCurrVar));

    // even if we only expect one widget to be there..
    for (int i = 0; i < right->ui->vlInitialValue->count(); i++) {
        QWidget* currwid = right->ui->vlInitialValue->itemAt(i)->widget();
        right->ui->vlInitialValue->removeWidget(currwid);
        delete currwid;
    }

    if (val) {
        if (val->isDouble()) {
            VleDoubleEdit* vde = new VleDoubleEdit(right, 0., "dummy");
            QObject::connect(vde, SIGNAL(valUpdated(const QString&, double)),
                             this, SLOT(onInitialValue(const QString&, double)));
            right->ui->vlInitialValue->addWidget(vde);

            vde->show();

            if (val) {
                vde->setValue(val->toDouble().value());
            } else {
                vde->setValue(0.0);
            }
        } else {
            VleValueWidget* valWidget = new VleValueWidget(right, true);
            QObject::connect(valWidget,
                             SIGNAL(valUpdated(const vle::value::Value&)),
                             this, SLOT(onValUpdated(const vle::value::Value&)));

            right->ui->vlInitialValue->addWidget(valWidget);
            valWidget->setValue(std::move(val));
            valWidget->show();
        }
    }

}

}} //namespaces
