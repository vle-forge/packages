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

#ifndef GVLE_DISCRETE_TIME_PANEL_H
#define GVLE_DISCRETE_TIME_PANEL_H

#include <QWidget>
#include <QObject>
#include <QDebug>
#include <QGroupBox>
#include <vle/gvle/vlevpz.hpp>
#include <vle/gvle/plugin_mainpanel.h>
#include <vle/gvle/gvle_widgets.h>
#include <vle/gvle/gvle_file.h>

#include "DiscreteTimeLeftWidget.h"
#include "DiscreteTimeRightWidget.h"
#include "vlesmdt.h"

namespace vv = vle::value;

namespace vle {
namespace gvle {

class VleTextEdit;

class DiscreteTimePanel : public PluginMainPanel
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "fr.inra.vle.gvle.PluginMainPanel")
    Q_INTERFACES(vle::gvle::PluginMainPanel)

public:
    DiscreteTimePanel();
    virtual ~DiscreteTimePanel();

    QString  getname() override;
    QWidget* leftWidget() override;
    QWidget* rightWidget() override;
    void undo() override;
    void redo() override;
    void init(const gvle_file& file, utils::Package* pkg, Logger* ,
            gvle_plugins*, const utils::ContextPtr&) override;
    QString canBeClosed() override;
    void save() override;
    void discard() override;
    PluginMainPanel* newInstance() override;

    VleTextEdit* getComputeWidget();
    VleTextEdit* getConstructorWidget();
    VleTextEdit* getUserSectionWidget();
    VleTextEdit* getIncludesWidget();
    VleDoubleEdit* getTimeStepWidget();
    QGroupBox* getTimeStepBox();
    void reload();
    void insertRowVar(int row, const QString& val);
    VleTextEdit* getTextEdit(int row, int col);

public slots:
    //for both left and right
    void onUndoAvailable(bool);
    //for left widget
    void onComputeChanged(const QString& id, const QString& old,
			  const QString& newVal);
    void onConstructorChanged(const QString& id, const QString& old,
			      const QString& newVal);
    void onUserSectionChanged(const QString& id, const QString& old,
			      const QString& newVal);
    void onIncludesChanged(const QString& id, const QString& old,
			   const QString& newVal);
    void onParamTimeStep(bool checked);
    void onTimeStepUpdated(const QString& /* id */, double val);
    void onTableVarsMenu(const QPoint&);
    void onModified();
    void onSetCompute();
    void onSetConstructor();
    void onSetUserSection();
    void onSetIncludes();
    //for right widget
    void onTypeUpdated(const QString& id, const QString& val);
    void onTextUpdated(const QString& id, const QString& oldname,
		       const QString& newname);
    void onSelected(const QString&);
    void onObs(const QString& id, bool val);
    void onValUpdated(const vle::value::Value& newVal);
    void onHistoryUpdated(const QString& id, int val);
    void onDimUpdated(const QString& id, int val);
    void onSyncUpdated(const QString& id, int val);

private:
    void updateConfigVar();

public:
    DiscreteTimeLeftWidget*  left;
    DiscreteTimeRightWidget* right;
    vleSmDT*                 cppMetadata;
    QString                  mCurrVar;
};

}} //namespaces


#endif
