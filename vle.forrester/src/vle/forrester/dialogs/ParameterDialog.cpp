/*
 * @file vle/forrester/dialogs/ParameterDialog.cpp
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

#include <vle/forrester/dialogs/ParameterDialog.hpp>
#include <boost/lexical_cast.hpp>
#include <iostream>

namespace vle {
namespace gvle {
namespace modeling {
namespace forrester {

ParameterDialog::ParameterDialog(const Glib::RefPtr < Gtk::Builder >& xml,
        const Forrester& forrester_):
    mForrester(forrester_),
    oldName("")
{
    Parameter emptyParameter(forrester_);
    init(xml,emptyParameter);
    onTextChange();
}

ParameterDialog::ParameterDialog(const Glib::RefPtr < Gtk::Builder >& xml,
    const Parameter& parameter, const Forrester& _forrester):
    mForrester(_forrester),
    oldName(parameter.getName())
{
    init(xml,parameter);
    onTextChange();
}

int ParameterDialog::run()
{
    int response = mDialog->run();

    mDialog->hide();
    return response;
}

void ParameterDialog::onTextChange()
{
    std::string entryName = getName();

    if (entryName.empty() || entryName == "variables") {
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

    try {
        boost::lexical_cast<double>(mValue->get_text());
    } catch(boost::bad_lexical_cast &) {
        mOkButton->set_sensitive(false);
    }
}

void ParameterDialog::init(const Glib::RefPtr < Gtk::Builder >& xml,
            const Parameter& parameter)
{
    xml->get_widget("ParameterDialog", mDialog);
    xml->get_widget("ParameterName", mNameEntry);
    xml->get_widget("ParameterImage",mOkImage);
    xml->get_widget("ButtonOkParameter",mOkButton);
    xml->get_widget("ParameterValue",mValue);

    mValue->set_text(
        boost::lexical_cast<std::string>(parameter.getValue()));
    mNameEntry->set_text(oldName);
    mOkImage->set(Gtk::StockID(Gtk::Stock::YES),
                        Gtk::IconSize(1));
    textModifyHandler.push_back(mNameEntry->signal_changed().connect(
            sigc::mem_fun(*this, &ParameterDialog::onTextChange)));
    textModifyHandler.push_back(mValue->signal_changed().connect(
            sigc::mem_fun(*this, &ParameterDialog::onTextChange)));
}

}
}
}
}
