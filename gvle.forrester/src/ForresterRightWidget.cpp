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


#include "ForresterPanel.h"
#include "ForresterRightWidget.h"
#include "ui_rightWidget.h"

#include <vle/gvle/vle_dom.hpp>
#include <vle/value/Double.hpp>


namespace gvle {
namespace forrester {

using namespace vle::gvle;

vle::gvle::VleCodeEdit*
getDerivativeAssist(Ui::rightWidget* ui)
{
    return static_cast<vle::gvle::VleCodeEdit*>(
            ui->inplaceDerivativeAssist->widget(1));
}

vle::gvle::VleCodeEdit*
getDerivativeAdvanced(Ui::rightWidget* ui)
{
    return static_cast<vle::gvle::VleCodeEdit*>(
            ui->inplaceDerivativeAdvanced->widget(1));
}

//stacked widget ids
enum {
    EDIT_FLOW_ASSIST = 0,
    EDIT_FLOW_ADVANCED = 1,
    EDIT_COMP = 2,
    EDIT_PARAM = 3,
    NO_EDIT = 4
};

/***********************************************/

ForresterRightWidget::ForresterRightWidget(ForresterPanel*  f):
        QWidget(), ui(new Ui::rightWidget), forrester(f), currMatFlow(""),
        currComp(""), currParam("")
{
    ui->setupUi(this);

    ui->inplaceDerivativeAssist->addWidget(
            new vle::gvle::VleCodeEdit(this,"", "id", true));
    ui->inplaceDerivativeAssist->setCurrentIndex(1);


    ui->inplaceDerivativeAdvanced->addWidget(
            new vle::gvle::VleCodeEdit(this,"", "id", true));
    ui->inplaceDerivativeAdvanced->setCurrentIndex(1);

    ui->stackedWidget->setCurrentIndex(NO_EDIT);

    QObject::connect(ui->depComps, SIGNAL(itemPressed (QListWidgetItem *)),
            this, SLOT(onMatFlowCompSelected(QListWidgetItem *)));

    QObject::connect(ui->depParams, SIGNAL(itemPressed (QListWidgetItem *)),
            this, SLOT(onMatFlowParamSelected(QListWidgetItem *)));

    QObject::connect(ui->butAdvancedEdit, SIGNAL(pressed ()),
            this, SLOT(onMatFlowAdvancedEdit()));

    QObject::connect(getDerivativeAssist(ui),
      SIGNAL(textUpdated(const QString&, const QString&, const QString&)),
      this,
      SLOT(onDerivative(const QString&, const QString&, const QString&)));

    QObject::connect(getDerivativeAdvanced(ui),
      SIGNAL(textUpdated(const QString&, const QString&, const QString&)),
      this,
      SLOT(onDerivative(const QString&, const QString&, const QString&)));

    QObject::connect(ui->compInitValue, SIGNAL(valueChanged(double)),
                this, SLOT(onInitValue(double)));

    QObject::connect(ui->paramValue, SIGNAL(valueChanged(double)),
                this, SLOT(onInitValue(double)));

    QObject::connect(ui->butAbs, SIGNAL(clicked ()),
            this, SLOT(onButAbs()));
    QObject::connect(ui->butCos, SIGNAL(clicked ()),
                this, SLOT(onButCos()));
    QObject::connect(ui->butDivide, SIGNAL(clicked ()),
                this, SLOT(onButDivide()));
    QObject::connect(ui->butExp, SIGNAL(clicked ()),
                this, SLOT(onButExp()));
    QObject::connect(ui->butMinus, SIGNAL(clicked ()),
                this, SLOT(onButMinus()));
    QObject::connect(ui->butParentIn, SIGNAL(clicked ()),
                this, SLOT(onButParentIn()));
    QObject::connect(ui->butParentOut, SIGNAL(clicked ()),
                this, SLOT(onButParentOut()));
    QObject::connect(ui->butPlus, SIGNAL(clicked ()),
                this, SLOT(onButPlus()));
    QObject::connect(ui->butSin, SIGNAL(clicked ()),
                this, SLOT(onButSin()));
    QObject::connect(ui->butSqrt, SIGNAL(clicked ()),
                this, SLOT(onButSqrt()));
    QObject::connect(ui->butTan, SIGNAL(clicked ()),
                this, SLOT(onButTan()));
    QObject::connect(ui->butTimes, SIGNAL(clicked ()),
                this, SLOT(onButTimes()));

}
ForresterRightWidget::~ForresterRightWidget()
{
    delete ui;
}

void
ForresterRightWidget::reload()
{
    if (not currMatFlow.isEmpty() and
            not forrester->cppMetadata->existMaterialFlowToDoc(currMatFlow)) {
        currMatFlow = "";
    }

    if (not currComp.isEmpty() and
                not forrester->cppMetadata->existCompToDoc(currComp)) {
        currComp = "";
        }

    if (not currParam.isEmpty() and
            not forrester->cppMetadata->existParameterToDoc(currParam)) {
        currParam = "";
    }

    if (not currMatFlow.isEmpty()) {
        if (forrester->cppMetadata->hasAdvancedEdition(currMatFlow)) {
            QDomNode matFlow = forrester->cppMetadata->nodeMaterialFlow(
                    currMatFlow);
            ui->flowNameAdvanced->setText(currMatFlow);
            getDerivativeAdvanced(ui)->setPlainText(
                    forrester->cppMetadata->getCompute(currMatFlow));
            ui->stackedWidget->setCurrentIndex(EDIT_FLOW_ADVANCED);
        } else {
            QDomNode matFlow = forrester->cppMetadata->nodeMaterialFlow(
                    currMatFlow);
            ui->flowNameAssist->setText(currMatFlow);
            getDerivativeAssist(ui)->setText(
                    forrester->cppMetadata->getCompute(currMatFlow));
            ui->depComps->clear();
            ui->depParams->clear();
            //add compartments declared as dependencies
            QList<QDomNode> depComp =
                    DomFunctions::childNodesWithoutText(matFlow, "depends_on");
            for (auto c : depComp) {
                QString name = DomFunctions::attributeValue(c, "name");
                if (forrester->cppMetadata->existCompToDoc(name)) {
                    ui->depComps->addItem(new QListWidgetItem(name));
                } else  if (forrester->cppMetadata->existParameterToDoc(name)){
                    ui->depParams->addItem(new QListWidgetItem(name));
                }

            }
            //add compartment used as hard link to
            QString compName = forrester->cppMetadata->hardLinkTo(currMatFlow);
            if (compName != "") {
                ui->depComps->addItem(new QListWidgetItem(compName));
            }
            //add compartment used as hard link from
            compName = forrester->cppMetadata->hardLinkFrom(currMatFlow);
            if (compName != "") {
                ui->depComps->addItem(new QListWidgetItem(compName));
            }
            ui->stackedWidget->setCurrentIndex(EDIT_FLOW_ASSIST);
        }
    } else if (not currComp.isEmpty()) {
        ui->compName->setText(currComp);
        bool oldBlock = ui->compInitValue->blockSignals(true);
        ui->compInitValue->setValue(forrester->cppMetadata
                ->getInitialValue(currComp)->toDouble().value());
        ui->compInitValue->blockSignals(oldBlock);
        ui->stackedWidget->setCurrentIndex(EDIT_COMP);
    } else if (not currParam.isEmpty()) {
        ui->paramName->setText(currParam);
        bool oldBlock = ui->paramValue->blockSignals(true);
        ui->paramValue->setValue(forrester->cppMetadata
                ->getInitialValue(currParam)->toDouble().value());
        ui->paramValue->blockSignals(oldBlock);
        ui->stackedWidget->setCurrentIndex(EDIT_PARAM);
    } else {
        ui->stackedWidget->setCurrentIndex(NO_EDIT);
    }

}

void
ForresterRightWidget::onMatFlowSelected(QString name)
{

    currMatFlow = name;
    currComp = "";
    currParam = "";
    reload();
}

void
ForresterRightWidget::onCompSelected(QString name)
{
    currMatFlow = "";
    currComp = name;
    currParam = "";
    reload();
}

void
ForresterRightWidget::onParamSelected(QString name)
{
    currMatFlow = "";
    currComp = "";
    currParam = name;
    reload();
}

void
ForresterRightWidget::onInitValue(double d)
{
    if (not currComp.isEmpty()) {
        forrester->cppMetadata->setInitialValue(currComp, d);
    } else if (not currParam.isEmpty()) {
        forrester->cppMetadata->setInitialValue(currParam, d);
    }
}


void
ForresterRightWidget::onButAbs()
{
    getDerivativeAssist(ui)->textCursor().insertText("abs" );
    forrester->cppMetadata->setCompute(ui->flowNameAssist->text(),
            getDerivativeAssist(ui)->getCurrentText());
}
void
ForresterRightWidget::onButCos()
{
    getDerivativeAssist(ui)->textCursor().insertText("cos" );
    forrester->cppMetadata->setCompute(ui->flowNameAssist->text(),
            getDerivativeAssist(ui)->getCurrentText());
}
void
ForresterRightWidget::onButDivide()
{
    getDerivativeAssist(ui)->textCursor().insertText(" / " );
    forrester->cppMetadata->setCompute(ui->flowNameAssist->text(),
            getDerivativeAssist(ui)->getCurrentText());
}
void
ForresterRightWidget::onButExp()
{
    getDerivativeAssist(ui)->textCursor().insertText(" exp " );
    forrester->cppMetadata->setCompute(ui->flowNameAssist->text(),
            getDerivativeAssist(ui)->getCurrentText());
}
void
ForresterRightWidget::onButMinus()
{
    getDerivativeAssist(ui)->textCursor().insertText(" - " );
    forrester->cppMetadata->setCompute(ui->flowNameAssist->text(),
            getDerivativeAssist(ui)->getCurrentText());
}
void
ForresterRightWidget::onButParentIn()
{
    getDerivativeAssist(ui)->textCursor().insertText("(" );
    forrester->cppMetadata->setCompute(ui->flowNameAssist->text(),
            getDerivativeAssist(ui)->getCurrentText());
}
void
ForresterRightWidget::onButParentOut()
{
    getDerivativeAssist(ui)->textCursor().insertText(")" );
    forrester->cppMetadata->setCompute(ui->flowNameAssist->text(),
            getDerivativeAssist(ui)->getCurrentText());
}
void
ForresterRightWidget::onButPlus()
{
    getDerivativeAssist(ui)->textCursor().insertText(" + " );
    forrester->cppMetadata->setCompute(ui->flowNameAssist->text(),
            getDerivativeAssist(ui)->getCurrentText());
}

void
ForresterRightWidget::onButSin()
{
    getDerivativeAssist(ui)->textCursor().insertText("sin" );
    forrester->cppMetadata->setCompute(ui->flowNameAssist->text(),
            getDerivativeAssist(ui)->getCurrentText());
}
void
ForresterRightWidget::onButSqrt()
{
    getDerivativeAssist(ui)->textCursor().insertText("sqrt" );
    forrester->cppMetadata->setCompute(ui->flowNameAssist->text(),
            getDerivativeAssist(ui)->getCurrentText());
}
void
ForresterRightWidget::onButTan()
{
    getDerivativeAssist(ui)->textCursor().insertText("tan");
    forrester->cppMetadata->setCompute(ui->flowNameAssist->text(),
            getDerivativeAssist(ui)->getCurrentText());
}
void
ForresterRightWidget::onButTimes()
{
    getDerivativeAssist(ui)->textCursor().insertText(" * " );
    forrester->cppMetadata->setCompute(ui->flowNameAssist->text(),
            getDerivativeAssist(ui)->getCurrentText());
}


void
ForresterRightWidget::onMatFlowCompSelected(QListWidgetItem* itemComp)
{
    //remove "0" if present
    if (getDerivativeAssist(ui)->getCurrentText() == "0") {
        getDerivativeAssist(ui)->setText("");
    }
    getDerivativeAssist(ui)->textCursor().insertText(itemComp->text() + "()");
    forrester->cppMetadata->setCompute(ui->flowNameAssist->text(),
            getDerivativeAssist(ui)->getCurrentText());
}

void
ForresterRightWidget::onMatFlowParamSelected(QListWidgetItem* itemComp)
{
    //remove "0" if present
    if (getDerivativeAssist(ui)->getCurrentText() == "0") {
        getDerivativeAssist(ui)->setText("");
    }
    getDerivativeAssist(ui)->textCursor().insertText(itemComp->text() + "()");
    forrester->cppMetadata->setCompute(ui->flowNameAssist->text(),
            getDerivativeAssist(ui)->getCurrentText());
}

void
ForresterRightWidget::onMatFlowAdvancedEdit()
{
    forrester->cppMetadata->setAdvancedEdition(currMatFlow);
    reload();
}

void
ForresterRightWidget::onSmModified(int modifType)
{
    switch((MODIF_TYPE) modifType) {
    case RENAME:
        currMatFlow.clear();
        currComp.clear();
        currParam.clear();
        reload();
        break;
    default:
        reload();
        break;
    }
}

void
ForresterRightWidget::onDerivative(const QString&, const QString&,
        const QString& newVal)
{
    forrester->cppMetadata->setCompute(ui->flowNameAssist->text(), newVal);
}


}} //namespaces
