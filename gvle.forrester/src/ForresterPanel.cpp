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

#include "vlesmForrester.h"
#include "ForresterPanel.h"



namespace gvle {
namespace forrester {

namespace vv = vle::value;
using namespace vle::gvle;

ForresterPanel::ForresterPanel():
        PluginMainPanel(), left(0),
        right(0), cppMetadata(0)
{
    left = new ForresterLeftWidget(this);
    right = new ForresterRightWidget(this);

    QObject::connect(left,
                SIGNAL(matFlowSelected(QString)),
                right, SLOT(onMatFlowSelected(QString)));
    QObject::connect(left,
                SIGNAL(compSelected(QString)),
                right, SLOT(onCompSelected(QString)));
    QObject::connect(left,
                SIGNAL(paramSelected(QString)),
                right, SLOT(onParamSelected(QString)));

}

ForresterPanel::~ForresterPanel()
{
    delete left;
    delete right;
}

QString
ForresterPanel::getname()
{
    return "Forrester";
}
QWidget*
ForresterPanel::leftWidget()
{
    return left;
}

QWidget*
ForresterPanel::rightWidget()
{
    return right;
}
void
ForresterPanel::undo()
{
    std::cout << " ForresterPanel::undo() " << "\n";
    cppMetadata->undo();
}

void
ForresterPanel::redo()
{
    cppMetadata->redo();
    reload();
}

void
ForresterPanel::init(const gvle_file& gf, vle::utils::Package* pkg, Logger* ,
        gvle_plugins*, const vle::utils::ContextPtr&)
{

    cppMetadata = new vleSmForrester(gf.source_file,
            gf.metadata_file, getname());
    cppMetadata->setPackageToDoc(pkg->name().c_str(), false);
    cppMetadata->setClassNameToDoc(gf.baseName(), false);
    cppMetadata->save();

    QObject::connect(cppMetadata, SIGNAL(undoAvailable(bool)),
                     this, SLOT(onUndoAvailable(bool)));

    QObject::connect(cppMetadata, SIGNAL(modified (int)),
            right, SLOT(onSmModified(int)));
    QObject::connect(cppMetadata, SIGNAL(modified (int)),
            left, SLOT(onSmModified(int)));

    reload();
}

QString
ForresterPanel::canBeClosed()
{
    return "";
}

void
ForresterPanel::save()
{
    cppMetadata->save();
}

void
ForresterPanel::discard()
{
    //TODO what about discard
//    vleSmDT* cppDiscarded = new vleSmDT( cppMetadata->getSrcPath(),
//                                         cppMetadata->getSmPath(),
//                                         getname());
//
//    cppDiscarded->provideCpp();
}

PluginMainPanel*
ForresterPanel::newInstance()
{
    return new ForresterPanel;
}

VleTextEdit*
ForresterPanel::getComputeWidget()
{
    return 0;
}

VleTextEdit*
ForresterPanel::getConstructorWidget()
{
    return 0;
}

VleTextEdit*
ForresterPanel::getUserSectionWidget()
{
    return 0;
}

VleTextEdit*
ForresterPanel::getIncludesWidget()
{
    return 0;
}

void
ForresterPanel::reload()
{
    left->reload();
    right->reload();
}

void
ForresterPanel::insertRowVar(int /*row*/, const QString& /*name*/)
{
}

VleTextEdit*
ForresterPanel::getTextEdit(int /*row*/, int /*col*/)
{
    return 0;
}



void
ForresterPanel::onUndoAvailable(bool b)
{
    emit undoAvailable(b);
}

void
ForresterPanel::onUserSectionChanged(const QString& /* id */,
                                        const QString& old,
                                        const QString& newVal)
{
    if (old != newVal) {
        //cppMetadata->setUserSectionToDoc(newVal);
    }
}

void
ForresterPanel::onConstructorChanged(const QString& /* id */,
                                        const QString& old,
                                        const QString& newVal)
{
    if (old != newVal) {
        //cppMetadata->setConstructorToDoc(newVal);
    }
}

void
ForresterPanel::onIncludesChanged(const QString& /* id */,
                                     const QString& old,
                                     const QString& newVal)
{
    if (old != newVal) {
        //cppMetadata->setIncludesToDoc(newVal);
    }
}

void
ForresterPanel::onTableVarsMenu(const QPoint& /*pos*/)
{
}


void
ForresterPanel::onSetConstructor()
{
    QString Body = getConstructorWidget()->toPlainText();
    //cppMetadata->setConstructorToDoc(Body);
}

void
ForresterPanel::onSetUserSection()
{
    QString Body = getUserSectionWidget()->toPlainText();
    //cppMetadata->setUserSectionToDoc(Body);
}

void
ForresterPanel::onSetIncludes()
{
    QString Body = getIncludesWidget()->toPlainText();
    //cppMetadata->setIncludesToDoc(Body);
}



void
ForresterPanel::onTypeUpdated(const QString& /*id*/, const QString& /*val*/)
{
}

void
ForresterPanel::onTextUpdated(const QString& /*id*/,
                                 const QString& oldname,
                                 const QString& newname)
{
    if (oldname != newname) {
        //cppMetadata->renameVariableToDoc(oldname, newname);
    }
}

void
ForresterPanel::onSelected(const QString& /*id*/)
{

}

void
ForresterPanel::onObs(const QString& /*id*/, bool /*val*/)
{
}

void
ForresterPanel::onValUpdated(const vv::Value& /*newVal*/)
{
}

void
ForresterPanel::updateConfigVar()
{
}

}} //namespaces
