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
    delete left;
    delete right;
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

    cppMetadata->setPackageToDoc(pkg->name().c_str(), false);
    cppMetadata->setClassNameToDoc(gf.baseName(), false);
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
        insertRowVar(i, name);
    }

    updateConfigVar();
}

void
DiscreteTimePanel::insertRowVar(int row, const QString& name)
{

    bool isVect = cppMetadata->isVect(name);
    int dim = cppMetadata->getDim(name);
    QString type = cppMetadata->getType(name);
    int sync = cppMetadata->getSync(name);
    int historySize = cppMetadata->getHistorySize(name);
    bool obs = cppMetadata->isObs(name);
    int col = 0;

    //insert Name
    {
        col = 0;
        QString id = QString("%1,%2").arg(row).arg(col);
        VleTextEdit* w = new VleTextEdit(right->ui->tableVars, name, id, true);
        right->ui->tableVars->setCellWidget(row, col, w);
        right->ui->tableVars->setItem(row, col, new QTableWidgetItem);
        QObject::connect(w, SIGNAL(
                textUpdated(const QString&, const QString&, const QString&)),
                this, SLOT(
                onTextUpdated(const QString&, const QString&, const QString&)));
        QObject::connect(w, SIGNAL(selected(const QString&)),
                this, SLOT(onSelected(const QString&)));
    }
    //insert dim
    {
        col = 1;
        QString id = QString("%1,%2").arg(row).arg(col);
        VleSpinBox* w = new VleSpinBox(right->ui->tableVars, dim, id, 1);
        QObject::connect(w, SIGNAL(selected(const QString&)),
                            this, SLOT(onSelected(const QString&)));
        if (isVect) {
            QObject::connect(w, SIGNAL(valUpdated(const QString&, int)),
                    this, SLOT(onDimUpdated(const QString&, int)));
        } else {
            w->setReadOnly(true);
        }
        right->ui->tableVars->setCellWidget(row, col, w);
        right->ui->tableVars->setItem(row, col, new QTableWidgetItem);

    }
    //insert type (In or Out or In/Out)
    {
        col = 2;
        QString id = QString("%1,%2").arg(row).arg(col);
        VleCombo* w = new VleCombo(right->ui->tableVars, id);
        QList <QString> l;
        l.append("In");
        l.append("Out");
        l.append("In/Out");
        w->addItems(l);
        w->setCurrentIndex(w->findText(type));
        QObject::connect(w, SIGNAL(valUpdated(const QString&, const QString&)),
                this, SLOT(onTypeUpdated(const QString&, const QString&)));
        QObject::connect(w, SIGNAL(selected(const QString&)),
                this, SLOT(onSelected(const QString&)));
        right->ui->tableVars->setCellWidget(row, col, w);
        right->ui->tableVars->setItem(row, col, new QTableWidgetItem);
    }
    //insert Sync
    {
        col = 3;
        QString id = QString("%1,%2").arg(row).arg(col);
        VleSpinBox* w = new VleSpinBox(right->ui->tableVars, sync,
                                       id, 0);
        QObject::connect(w, SIGNAL(selected(const QString&)),
                                 this, SLOT(onSelected(const QString&)));
        if (type == "Out") {
            w->setReadOnly(true);
        } else {
            QObject::connect(w, SIGNAL(valUpdated(const QString&, int)),
                             this, SLOT(onSyncUpdated(const QString&, int)));
        }
        right->ui->tableVars->setCellWidget(row, col, w);
        right->ui->tableVars->setItem(row, col, new QTableWidgetItem);
    }
    //insert history size
    {
        col = 4;
        QString id = QString("%1,%2").arg(row).arg(col);
        VleSpinBox* w = new VleSpinBox(right->ui->tableVars, historySize,
                id, 1);

        right->ui->tableVars->setCellWidget(row, col, w);
        right->ui->tableVars->setItem(row, col, new QTableWidgetItem);

        QObject::connect(w, SIGNAL(valUpdated(const QString&, int)),
                this, SLOT(onHistoryUpdated(const QString&, int)));
        QObject::connect(w, SIGNAL(selected(const QString&)),
                this, SLOT(onSelected(const QString&)));
    }
    //insert obs check box
    {
        col = 5;
        QString id = QString("%1,%2").arg(row).arg(col);
        VleBooleanEdit* w = new VleBooleanEdit(right->ui->tableVars, obs, id);

        right->ui->tableVars->setCellWidget(row, col, w);
        right->ui->tableVars->setItem(row, col, new QTableWidgetItem);

        QObject::connect(w, SIGNAL(valUpdated(const QString&, bool)),
                this, SLOT(onObs(const QString&, bool)));
        QObject::connect(w, SIGNAL(selected(const QString&)),
                this, SLOT(onSelected(const QString&)));

    }
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


    QString varName = "";
    bool hasInitialValue = false;
    if (index.row() >= 0 ){
        VleTextEdit* item = (VleTextEdit*)right->ui->tableVars->cellWidget(
                index.row(), 0);
        varName = item->getCurrentText();
        hasInitialValue = cppMetadata->hasInitialValue(varName);
    }

    QAction* action;
    QMenu menu;
    action = menu.addAction("Add variable");
    action->setData(1);
    action = menu.addAction("Add vector");
    action->setData(2);
    action = menu.addAction("Remove");
    action->setData(3);
    action->setEnabled(varName != "");
    menu.addSeparator();
    action = menu.addAction("Set initial value");
    action->setData(4);
    action->setEnabled(varName != "" and not hasInitialValue);
    action = menu.addAction("Unset initial value");
    action->setData(5);
    action->setEnabled(varName != "" and hasInitialValue);

    QAction* selAction = menu.exec(globalPos);
    if (selAction) {
        int actCode =  selAction->data().toInt();
        switch(actCode) {
        case 1: //Add variable
            cppMetadata->addVariableToDoc(cppMetadata->newVarNameToDoc(), false);
            mCurrVar = varName;
            reload();
            emit undoAvailable(true);
            break;
        case 2: //Add vector
            cppMetadata->addVariableToDoc(cppMetadata->newVarNameToDoc(), true);
            mCurrVar = varName;
            reload();
            emit undoAvailable(true);
            break;
        case 3: //Remove
            cppMetadata->rmVariableToDoc(varName);
            mCurrVar = "";
            reload();
            emit undoAvailable(true);
            break;
        case 4: //Set initial value
            cppMetadata->setInitialDefaultValue(varName);
            mCurrVar = varName;
            reload();
            emit undoAvailable(true);
            break;
        case 5: //Unset initial value
            cppMetadata->unsetInitialValue(varName);
            mCurrVar = varName;
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
DiscreteTimePanel::onTypeUpdated(const QString& id, const QString& val)
{
    QStringList split = id.split(",");
    int row = split.at(0).toInt();
    mCurrVar =  getTextEdit(row, 0)->getCurrentText();
    cppMetadata->setType(mCurrVar, val);
    reload();
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
    cppMetadata->setHistorySize(mCurrVar, value);
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

    cppMetadata->setDim(mCurrVar, value);

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
    cppMetadata->setSync(mCurrVar,value);
}

void
DiscreteTimePanel::updateConfigVar()
{
    // even if we only expect one widget to be there..
    for (int i = 0; i < right->ui->vlInitialValue->count(); i++) {
        QWidget* currwid = right->ui->vlInitialValue->itemAt(i)->widget();
        right->ui->vlInitialValue->removeWidget(currwid);
        delete currwid;
    }
    if (mCurrVar == "") {
        return;
    }
    QString lab = mCurrVar + " default initial conditions";
    std::unique_ptr<vv::Value> val = std::move(
            cppMetadata->getInitialValue(mCurrVar));

    if (val) {
        VleValueWidget* valWidget = new VleValueWidget(right, true, lab);
        QObject::connect(valWidget,
                SIGNAL(valUpdated(const vle::value::Value&)),
                this, SLOT(onValUpdated(const vle::value::Value&)));

        right->ui->vlInitialValue->addWidget(valWidget);
        valWidget->setValue(std::move(val));
        valWidget->show();
    }
}

}} //namespaces
