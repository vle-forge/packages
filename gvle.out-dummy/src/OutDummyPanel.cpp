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

#include "OutDummyPanel.h"
//#include "ui_leftWidget.h"
//#include "ui_rightWidget.h"

namespace vle {
namespace gvle {

OutDummyPanel::OutDummyPanel():
    PluginMainPanel(),  m_edit(0), m_file(""), outMetadata(0)
{
    m_edit = new QTextEdit();
    QObject::connect(m_edit, SIGNAL(undoAvailable(bool)),
                     this, SLOT(onUndoAvailable(bool)));
}

OutDummyPanel::~OutDummyPanel()
{
    //delete right;
    //delete left;
}

QString
OutDummyPanel::getname()
{
    return "out-dummy";
}

QWidget*
OutDummyPanel::leftWidget()
{
    return m_edit;;
}

QWidget*
OutDummyPanel::rightWidget()
{
    return 0;
}

void
OutDummyPanel::undo()
{
    outMetadata->undo();
    //reload();
}
void
OutDummyPanel::redo()
{
    outMetadata->redo();
    //reload();
}

void
OutDummyPanel::init(QString& relPath, utils::Package* pkg, Logger* ,
        gvle_plugins*)
{
    QString basepath = pkg->getDir(vle::utils::PKG_SOURCE).c_str();
    QString outpath = basepath + "/" + relPath;
    QString ompath = basepath + "/metadata/" + relPath;
    ompath.replace(".dat",".om");

    outMetadata = new vleOmOD(outpath, ompath, getname());
    outMetadata->save();

    m_file = basepath+"/"+relPath;
    QFile outFile (m_file);

    if (!outFile.open(QFile::ReadOnly | QFile::Text)) {
        qDebug() << " Error DefaultOutPanel::init ";
    }

    QTextStream in(&outFile);
    m_edit->setText(in.readAll());

}

QString
OutDummyPanel::canBeClosed()
{
    return "";
}

void
OutDummyPanel::save()
{
    outMetadata->save();
}

PluginMainPanel*
OutDummyPanel::newInstance()
{
    return new OutDummyPanel;
}

void
OutDummyPanel::onUndoAvailable(bool b)
{
    emit undoAvailable(b);
}

}} //namespaces
