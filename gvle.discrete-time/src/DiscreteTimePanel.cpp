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

#include "DiscreteTimePanel.h"
#include "ui_leftWidget.h"
#include "ui_rightWidget.h"



namespace vle {
namespace gvle {


DiscreteTimePanel::DiscreteTimePanel():
        PluginMainPanel(), left(new DiscreteTimeLeftWidget),
        right(new DiscreteTimeRightWidget), cppMetadata(0), mCurrVar("")
{
    //set Compute widget
    left->ui->editCompute->addWidget(
            new VleTextEdit(left->ui->editCompute, "","", true));
    left->ui->editCompute->setCurrentIndex(0);

    QObject::connect(right->ui->tableVars,
            SIGNAL(customContextMenuRequested(const QPoint&)),
            this, SLOT(onTableVarsMenu(const QPoint&)));

    QObject::connect(right->ui->allowInitialValue,
            SIGNAL(stateChanged(int)),
            this, SLOT(onAllowInitialValue(int)));


    QObject::connect(right->ui->initialValue,
            SIGNAL(valueChanged(double)),
            this, SLOT(onInitialValue(double)));
}

DiscreteTimePanel::~DiscreteTimePanel()
{
    delete right;
    delete left;
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
DiscreteTimePanel::init(QString& relPath, utils::Package* pkg, Logger* ,
        gvle_plugins*)
{

    QString className = relPath;
    className.replace("src/","");
    className.replace(".cpp","");
    QString basepath = pkg->getDir(vle::utils::PKG_SOURCE).c_str();
    QString scrpath = basepath + "/" +relPath;
    QString smpath = basepath + "/metadata/" +relPath;
    smpath.replace(".cpp",".sm");

    cppMetadata = new vleSmDT(scrpath, smpath, getname());
    cppMetadata->setNamespaceToDoc(pkg->name().c_str());
    cppMetadata->setClassNameToDoc(className);
    cppMetadata->save();

    getComputeWidget()->setText(cppMetadata->getComputeBody());

    QObject::connect(getComputeWidget(),
            SIGNAL(textUpdated(const QString&, const QString&, const QString&)),
            this,
            SLOT(onComputeChanged(const QString&, const QString&,
                                  const QString&)));

    QObject::connect(cppMetadata, SIGNAL(undoAvailable(bool)),
                     this, SLOT(onUndoAvailable(bool)));

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

PluginMainPanel*
DiscreteTimePanel::newInstance()
{
    return new DiscreteTimePanel;
}

VleTextEdit*
DiscreteTimePanel::getComputeWidget()
{
    return (VleTextEdit*)left->ui->editCompute->widget(0);
}

void
DiscreteTimePanel::reload()
{
    //left widget
    getComputeWidget()->setText(cppMetadata->getComputeBody());

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
    }

    //reload intial value config
    right->ui->stackConfig->setCurrentIndex(0);

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

VleTextEdit*
DiscreteTimePanel::getTextEdit(int row, int col)
{
    return qobject_cast<VleTextEdit*>(
            right->ui->tableVars->cellWidget(row,col));
}


void
DiscreteTimePanel::onUndoAvailable(bool b)
{
    emit undoAvailable(b);
}


void
DiscreteTimePanel::onComputeChanged(const QString& id, const QString& old,
        const QString& newVal)
{
    if (old != newVal) {
        cppMetadata->setComputeToDoc(newVal);
    }
}

void
DiscreteTimePanel::onTableVarsMenu(const QPoint& pos)
{
    QPoint globalPos = right->ui->tableVars->viewport()->mapToGlobal(pos);
    QModelIndex index = right->ui->tableVars->indexAt(pos);
    QWidget* item = right->ui->tableVars->cellWidget(index.row(), index.column());

    QAction* action;
    QMenu menu;
    action = menu.addAction("Add variable");
    action->setData(1);
    action = menu.addAction("Add vector");
    action->setData(2);
    action = menu.addAction("Remove");
    action->setData(3);
    action->setEnabled(item != 0);

    QAction* selAction = menu.exec(globalPos);
    if (selAction) {
        int actCode =  selAction->data().toInt();
        switch(actCode){
        case 1: {//Add variable
            cppMetadata->addVariableToDoc(cppMetadata->newVarNameToDoc());
            reload();
            emit undoAvailable(true);
            break;
        } case 2: {//Add vector

            break;
        } case 3: {//Remove

            break;
        }}
    }
}



void DiscreteTimePanel::onSetCompute()
{
    QString computeBody = getComputeWidget()->toPlainText();
    cppMetadata->setComputeToDoc(computeBody);
}


void
DiscreteTimePanel::onTextUpdated(const QString& id, const QString& oldVal,
        const QString& newVal)
{
    cppMetadata->renameVariableToDoc(oldVal, newVal);
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
DiscreteTimePanel::onAllowInitialValue(int state)
{

    if (state == Qt::Checked) {//enabled
        //TODO should be the same value as in the extension
        qDebug() << " dbg onAllowInitialValue check " << state;
        cppMetadata->setInitialValue(mCurrVar, vle::value::Double(0.0));
    } else {//disabled
        qDebug() << " dbg onAllowInitialValue uncheck " << state;
        cppMetadata->rmInitialValue(mCurrVar);
    }
    updateConfigVar();
}

void
DiscreteTimePanel::onInitialValue(double val)
{
    if (mCurrVar == "") {
        return;
    }
    cppMetadata->setInitialValue(mCurrVar,vle::value::Double(val));
}

void
DiscreteTimePanel::updateConfigVar()
{
    int stackSize = right->ui->stackConfig->count();
    if (stackSize < 1 or stackSize > 2) {
        qDebug() << " Internal error DiscreteTimePanel::setConfigVar ";
        return ;
    }
    if (mCurrVar == "") {
        right->ui->stackConfig->setCurrentIndex(0);
        return;
    }
    right->ui->stackConfig->setCurrentIndex(1);

    //initalize panel with the var initial conditions
    QString lab = mCurrVar + ": imposed initial conditions";
    right->ui->varLabel->setText(lab);
    vle::value::Value* val = cppMetadata->getInitialValue(mCurrVar);
    bool oldBlock = right->ui->allowInitialValue->blockSignals(true);
    if (val) {
        qDebug() << " dbg updateConfigVar checked " ;
        right->ui->allowInitialValue->setCheckState(Qt::Checked);
    } else {
        qDebug() << " dbg updateConfigVar unchecked ";
        right->ui->allowInitialValue->setCheckState(Qt::Unchecked);
    }
    right->ui->allowInitialValue->blockSignals(oldBlock);

    oldBlock = right->ui->initialValue->blockSignals(true);
    if (val) {
        right->ui->initialValue->setEnabled(true);
        right->ui->initialValue->setValue(val->toDouble().value());
    } else {
        right->ui->initialValue->setEnabled(false);
        right->ui->initialValue->setValue(0.0);
    }
    right->ui->initialValue->blockSignals(oldBlock);
    delete val;

}




}} //namespaces

