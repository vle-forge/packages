/*
 * This file is part of VLE, a framework for multi-modeling, simulation
 * and analysis of complex dynamical systems.
 * http://www.vle-project.org
 *
 * Copyright (c) 2014-2017 INRA http://www.inra.fr
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

#ifndef GVLE_DECISION_H
#define GVLE_DECISION_H

#include <QWidget>
#include <QObject>
#include <QDebug>

#include <vle/gvle/vlevpz.hpp>
#include <vle/gvle/plugin_mainpanel.h>
#include <vle/gvle/gvle_file.h>

#include "vledmdd.h"
#include "DecisionLeftWidget.h"
#include "DecisionRightWidget.h"


namespace vle {
namespace gvle {

class DecisionPanel : public vle::gvle::PluginMainPanel
{
    Q_OBJECT
    Q_INTERFACES(vle::gvle::PluginMainPanel)
    Q_PLUGIN_METADATA(IID "fr.inra.vle.gvle.PluginMainPanel")

public:
    DecisionPanel();
    virtual ~DecisionPanel();
    QString  getname();
    QWidget* leftWidget() override;
    QWidget* rightWidget() override;
    void undo();
    void redo();
    void init(const vle::gvle::gvle_file& file,
	      vle::utils::Package* pkg, vle::gvle::Logger*,
	      vle::gvle::gvle_plugins* plug, const vle::utils::ContextPtr& ctx);
    QString canBeClosed();
    void save();
    void discard() {};
    PluginMainPanel* newInstance();
    void reload();

public slots:
    void onUndoAvailable(bool);

public:
    DecisionLeftWidget* left;
    DecisionRightWidget* right;

    QString             m_file;
    vleDmDD*            dataMetadata;
};

}} //namespaces


#endif
