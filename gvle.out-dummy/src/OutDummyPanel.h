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

#ifndef GVLE_OUT_DUMMY_PANEL_H
#define GVLE_OUT_DUMMY_PANEL_H

#include <QWidget>
#include <QObject>
#include <QDebug>
#include <vle/gvle/vlevpm.h>
#include <vle/gvle/plugin_mainpanel.h>

//#include "OutDummyLeftWidget.h"
//#include "OutDummyRightWidget.h"
#include "vleomod.h"


namespace vle {
namespace gvle {

class VleTextEdit;

class OutDummyPanel : public PluginMainPanel
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "fr.inra.vle.gvle.PluginMainPanel")
    Q_INTERFACES(vle::gvle::PluginMainPanel)

public:
    OutDummyPanel();
    virtual ~OutDummyPanel();
    QString  getname();
    QWidget* leftWidget();
    QWidget* rightWidget();
    void undo();
    void redo();
    void init(QString& file, utils::Package* pkg, Logger* ,gvle_plugins*);
    QString canBeClosed();
    void save();
    PluginMainPanel* newInstance();

public slots:
    //for both left and right
    void onUndoAvailable(bool);
    void onUndoRedoSm(QDomNode oldValSm, QDomNode newValSm);

private:

public:
    QTextEdit*  m_edit;
    QString     m_file;
    vleOmOD*    outMetadata;
};

}} //namespaces


#endif
