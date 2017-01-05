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

#ifndef GVLE_FORRESTER_H
#define GVLE_FORRESTER_H

#include <QWidget>
#include <QObject>
#include <QDebug>
#include <QGroupBox>
#include <vle/gvle/vlevpz.hpp>
#include <vle/gvle/plugin_mainpanel.h>
#include <vle/gvle/gvle_widgets.h>
#include <vle/gvle/gvle_file.h>

#include "vlesmForrester.h"
#include "ForresterLeftWidget.h"
#include "ForresterRightWidget.h"

namespace vv = vle::value;

namespace gvle {
namespace forrester {

class ForresterPanel : public vle::gvle::PluginMainPanel
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "fr.inra.vle.gvle.PluginMainPanel")
    Q_INTERFACES(vle::gvle::PluginMainPanel)

public:
    ForresterPanel();
    virtual ~ForresterPanel();

    QString  getname() override;
    QWidget* leftWidget() override;
    QWidget* rightWidget() override;
    void undo() override;
    void redo() override;
    void init(const vle::gvle::gvle_file& file,
            vle::utils::Package* pkg, vle::gvle::Logger* ,
            vle::gvle::gvle_plugins*, const vle::utils::ContextPtr&) override;
    QString canBeClosed() override;
    void save() override;
    void discard() override;
    PluginMainPanel* newInstance() override;

    vle::gvle::VleTextEdit* getComputeWidget();
    vle::gvle::VleTextEdit* getConstructorWidget();
    vle::gvle::VleTextEdit* getUserSectionWidget();
    vle::gvle::VleTextEdit* getIncludesWidget();
    void reload();
    void insertRowVar(int row, const QString& val);
    vle::gvle::VleTextEdit* getTextEdit(int row, int col);

public slots:
    //for both left and right
    void onUndoAvailable(bool);
    //for left widget
    void onConstructorChanged(const QString& id, const QString& old,
            const QString& newVal);
    void onUserSectionChanged(const QString& id, const QString& old,
            const QString& newVal);
    void onIncludesChanged(const QString& id, const QString& old,
            const QString& newVal);
    void onTableVarsMenu(const QPoint&);
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
private:
    void updateConfigVar();

public:
    ForresterLeftWidget*   left;
    ForresterRightWidget*  right;
    vleSmForrester*        cppMetadata;
};

}} //namespaces


#endif
