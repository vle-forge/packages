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

#include "vledmdd.h"
#include "DecisionPanel.h"

namespace vle {
namespace gvle {

namespace vv = vle::value;
using namespace vle::gvle;

DecisionPanel::DecisionPanel():
    PluginMainPanel(),  left(0), m_file(""), dataMetadata(0)
{
    left = new DecisionLeftWidget(this);
    right = new DecisionRightWidget(this);

    QObject::connect(left,
                     SIGNAL(actSelected(QString)),
                     right, SLOT(onActSelected(QString)));

    QObject::connect(left,
                     SIGNAL(precedenceSelected(QString, QString)),
                     right, SLOT(onPrecedenceSelected(QString, QString)));

    QObject::connect(left,
                     SIGNAL(nothingSelected()),
                     right, SLOT(onNothingSelected()));
}

DecisionPanel::~DecisionPanel()
{
    delete right;
    delete left;
}

QString
DecisionPanel::getname()
{
    return "Plan Activities";
}

QWidget*
DecisionPanel::leftWidget()
{
    return left;
}

QWidget*
DecisionPanel::rightWidget()
{
    return right;
}

void
DecisionPanel::undo()
{
    dataMetadata->undo();
    reload();
}
void
DecisionPanel::redo()
{
    dataMetadata->redo();
    reload();
}

void
DecisionPanel::init(const gvle_file& gf, vle::utils::Package* pkg, Logger*,
                     gvle_plugins*, const vle::utils::ContextPtr&)
{
    QString basepath = pkg->getDir(vle::utils::PKG_SOURCE).c_str();
    m_file = gf.source_file;

    dataMetadata = new vleDmDD(gf.source_file,
                               gf.metadata_file, getname());
    dataMetadata->setPackageToDoc("vle.discrete-time.decision", false);
    dataMetadata->setClassNameToDoc("agentDTG", false);
    dataMetadata->setPluginNameToDoc(getname(), false);
    dataMetadata->setDataNameToDoc(gf.baseName(), pkg->name().c_str(), false);
    dataMetadata->setDataPackageToDoc(pkg->name().c_str(), false);
    dataMetadata->save();

    QObject::connect(dataMetadata, SIGNAL(undoAvailable(bool)),
                     this, SLOT(onUndoAvailable(bool)));

    QObject::connect(dataMetadata, SIGNAL(modified (int)),
            left, SLOT(onDmModified(int)));

    QObject::connect(dataMetadata, SIGNAL(modified (int)),
            right, SLOT(onDmModified(int)));

    QFile dataFile (m_file);

    if (!dataFile.open(QFile::ReadOnly | QFile::Text)) {
        qDebug() << " warning DecisionPanel::init datafile does not exist";
    }

    QTextStream in(&dataFile);
    reload();
}

QString
DecisionPanel::canBeClosed()
{
    return "";
}

void
DecisionPanel::save()
{
    dataMetadata->save();
}

PluginMainPanel*
DecisionPanel::newInstance()
{
    return new DecisionPanel;
}

void
DecisionPanel::reload()
{
    left->reload();
    right->reload();
}

void
DecisionPanel::onUndoAvailable(bool b)
{
    emit undoAvailable(b);
}

}} //namespaces
