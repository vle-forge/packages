/*
 * @file vle/forrester/dialogs/ExogenousVarDialog.cpp
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

#include <vle/forrester/dialogs/ExogenousVarDialog.hpp>

namespace vle {
namespace gvle {
namespace modeling {
namespace forrester {

ExogenousVarDialog::ExogenousVarDialog(const Glib::RefPtr < Gtk::Builder >& xml,
        const Forrester& forrester_):
    mForrester(forrester_),
    mOldName("")
{
    ExogenousVariable emptyVar(0,0,"",forrester_);
    init(xml);
    onTextChange();
}

ExogenousVarDialog::ExogenousVarDialog(const Glib::RefPtr < Gtk::Builder >& xml,
        const ExogenousVariable& var, const Forrester& _forrester):
    mForrester(_forrester),
    mOldName(var.getName())
{
    init(xml);
    onTextChange();
}

int ExogenousVarDialog::run()
{
    int response = mDialog->run();

    mDialog->hide();
    return response;
}

void ExogenousVarDialog::onTextChange()
{
    std::string entryName = getName();

    if (entryName.empty())
    {
        mOkImage->set(Gtk::StockID(Gtk::Stock::NO),
                         Gtk::IconSize(1));
        mOkButton->set_sensitive(false);
    }
    else
    {
        if ((not(mForrester.exist(entryName)) &&
            mForrester.isValidName(entryName)) ||
            (entryName == mOldName && entryName != ""))
        {
            mOkImage->set(Gtk::StockID(Gtk::Stock::YES),
                             Gtk::IconSize(1));
            mOkButton->set_sensitive(true);
        }
        else
        {
            mOkImage->set(Gtk::StockID(Gtk::Stock::NO),
                             Gtk::IconSize(1));
            mOkButton->set_sensitive(false);
        }
    }
}

void ExogenousVarDialog::init(const Glib::RefPtr < Gtk::Builder >& xml)
{
    xml->get_widget("TextEntryBox", mDialog);
    xml->get_widget("TextEntryBoxEntry", mNameEntry);
    xml->get_widget("TextEntryBoxChecker",mOkImage);
    xml->get_widget("TextEntryBoxOk",mOkButton);
    mNameEntry->set_text(mOldName);
    mOkImage->set(Gtk::StockID(Gtk::Stock::YES),
                        Gtk::IconSize(1));
    textModifyHandler = mNameEntry->signal_changed().connect(
            sigc::mem_fun(*this, &ExogenousVarDialog::onTextChange));
}

}
}
}
}
