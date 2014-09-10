/*
 * @file vle/forrester/dialogs/CompartementDialog.cpp
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

#include <vle/forrester/dialogs/CompartmentDialog.hpp>
#include <boost/lexical_cast.hpp>

namespace vle {
namespace gvle {
namespace modeling {
namespace forrester {

CompartmentDialog::CompartmentDialog(const Glib::RefPtr<Gtk::Builder>& xml,
        const Forrester& forrester_):
    mTimeStepEntry(0),
    mForrester(forrester_),
    oldName("")
{
    Compartment emptyCompartment(forrester_);
    init(xml,emptyCompartment);
    onTextChange();
}

CompartmentDialog::CompartmentDialog(const Glib::RefPtr<Gtk::Builder>& xml,
    const Compartment& compartment, const Forrester& _forrester):
    mTimeStepEntry(0),
    mForrester(_forrester),
    oldName(compartment.getName())
{
    init(xml,compartment);
    onTextChange();
}

int CompartmentDialog::run()
{
    int response = mDialog->run();

    mDialog->hide();
    return response;
}

void CompartmentDialog::onTextChange()
{
    std::string entryName = getName();

    if (entryName.empty()) {
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
        boost::lexical_cast<double>(mInitialValueEntry->get_text());
    } catch(boost::bad_lexical_cast &) {
        mOkButton->set_sensitive(false);
    }

    if (mForrester.integrationScheme() == Forrester::QSS2) {
        double timeStep = vu::convert < double >(mTimeStepEntry->get_text(), true);
        if (timeStep > 0.0) {
        } else {
            mOkButton->set_sensitive(false);
        }

    }
}

void CompartmentDialog::init(const Glib::RefPtr < Gtk::Builder >& xml,
            const Compartment& compartment)
{
    if (mForrester.integrationScheme() != Forrester::QSS2) {
        xml->get_widget("CompartmentDialog", mDialog);
        xml->get_widget("CompartmentNameEntry", mNameEntry);
        xml->get_widget("CompartmentImage",mOkImage);
        xml->get_widget("CompartmentOkButton",mOkButton);
        xml->get_widget("CompartmentInitialValue",mInitialValueEntry);
    } else {
        xml->get_widget("QSS2CompartmentDialog", mDialog);
        xml->get_widget("QSS2CompartmentTSEntry", mTimeStepEntry);
        xml->get_widget("QSS2CompartmentNameEntry", mNameEntry);
        xml->get_widget("QSS2CompartmentImage",mOkImage);
        xml->get_widget("QSS2CompartmentOkButton",mOkButton);
        xml->get_widget("QSS2CompartmentInitialValue",mInitialValueEntry);
        mTimeStepEntry->set_text(
            utils::toScientificString(compartment.getTimeStep(), true));
        textModifyHandler.push_back(mTimeStepEntry->signal_changed().connect(
            sigc::mem_fun(*this, &CompartmentDialog::onTextChange)));
    }
    mInitialValueEntry->set_text(
        boost::lexical_cast<std::string>(compartment.getInitialValue()));
    mNameEntry->set_text(oldName);
    mOkImage->set(Gtk::StockID(Gtk::Stock::YES),
                        Gtk::IconSize(1));
    textModifyHandler.push_back(mNameEntry->signal_changed().connect(
            sigc::mem_fun(*this, &CompartmentDialog::onTextChange)));
    textModifyHandler.push_back(mInitialValueEntry->signal_changed().connect(
            sigc::mem_fun(*this, &CompartmentDialog::onTextChange)));

}

}
}
}
}
