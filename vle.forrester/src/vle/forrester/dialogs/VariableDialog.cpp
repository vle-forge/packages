/*
 * @file vle/forrester/dialogs/VariableDialog.cpp
 *
 * This file is part of VLE, a framework for multi-modeling, simulation
 * and analysis of complex dynamical systems
 * http://www.vle-project.org
 *
 * Copyright (c) 2003-2007 Gauthier Quesnel <quesnel@users.sourceforge.net>
 * Copyright (c) 2003-2011 ULCO http://www.univ-littoral.fr
 * Copyright (c) 2007-2011 INRA http://www.inra.fr
 *
 * See the AUTHORS or Authors.txt file for copyright owners and contributors
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <vle/forrester/dialogs/VariableDialog.hpp>
#include <boost/lexical_cast.hpp>
#include <iostream>

namespace vle {
namespace gvle {
namespace modeling {
namespace forrester {

VariableDialog::VariableDialog(const Glib::RefPtr<Gtk::Builder>& xml,
        const Forrester& forrester_):
    mForrester(forrester_),
    oldName("")
{
    Variable emptyVariable(forrester_);
    init(xml,emptyVariable);
    onTextChange();
}

VariableDialog::VariableDialog(const Glib::RefPtr<Gtk::Builder>& xml,
    const Variable& variable, const Forrester& _forrester):
    mForrester(_forrester),
    oldName(variable.getName())
{
    init(xml,variable);
    onTextChange();
}

int VariableDialog::run()
{
    int response = mDialog->run();

    mDialog->hide();
    return response;
}

void VariableDialog::onTextChange()
{
    std::string entryName = getName();

    if (entryName.empty() || getValue().empty()) {
        mOkImage->set(Gtk::StockID(Gtk::Stock::NO),
                         Gtk::IconSize(1));
        mOkButton->set_sensitive(false);
    } else {
        if ((not(mForrester.exist(entryName)) &&
        mForrester.isValidName(entryName)) ||
        (entryName == oldName && entryName != "")) {
            mOkImage->set(Gtk::StockID(Gtk::Stock::YES),
                             Gtk::IconSize(1));
            mOkButton->set_sensitive(true);
        } else {
            mOkImage->set(Gtk::StockID(Gtk::Stock::NO),
                             Gtk::IconSize(1));
            mOkButton->set_sensitive(false);
        }
    }

}

void VariableDialog::init(const Glib::RefPtr < Gtk::Builder >& xml,
            const Variable& variable)
{
    xml->get_widget("VariableDialog", mDialog);
    xml->get_widget("VariableNameEntry", mNameEntry);
    xml->get_widget("VariableImage",mOkImage);
    xml->get_widget("VariableOk",mOkButton);
    xml->get_widget("VariableTextEntry",mValueEntry);
    mValueEntry->set_text(variable.getValue());
    mNameEntry->set_text(oldName);
    mOkImage->set(Gtk::StockID(Gtk::Stock::YES),
                        Gtk::IconSize(1));
    textModifyHandler.push_back(mNameEntry->signal_changed().connect(
            sigc::mem_fun(*this, &VariableDialog::onTextChange)));
    textModifyHandler.push_back(mValueEntry->signal_changed().connect(
            sigc::mem_fun(*this, &VariableDialog::onTextChange)));

}

}
}
}
}
