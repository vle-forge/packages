/*
 * @file vle/forrester/dialogs/ExogenousVarDialog.hpp
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

#ifndef VLE_GVLE_MODELING_FORRESTER_EXOGENOUS_VAR_DIALOG_HPP
#define VLE_GVLE_MODELING_FORRESTER_EXOGENOUS_VAR_DIALOG_HPP

#include <vle/forrester/Forrester.hpp>
#include <vle/forrester/graphicalItems/ExogenousVariable.hpp>

#include <gtkmm/dialog.h>
#include <gtkmm/entry.h>
#include <gtkmm/builder.h>
#include <gtkmm/image.h>
#include <gtkmm/stock.h>

namespace vle {
namespace gvle {
namespace modeling {
namespace forrester {

/**
 * @class ExogenousVarDialog
 * @brief Launch a new window to modify an exogenous variable of the Forrester diagram
 */
class ExogenousVarDialog
{
public:
    ExogenousVarDialog(const Glib::RefPtr < Gtk::Builder >& xml,
        const Forrester& forrester_);

    ExogenousVarDialog(const Glib::RefPtr < Gtk::Builder >& xml,
        const ExogenousVariable& , const Forrester& forrester_);

    virtual ~ExogenousVarDialog()
    {
        textModifyHandler.disconnect();
    }

    std::string getName() const
    { return mNameEntry->get_text(); }

    int run ();

private:
    void onTextChange ();
    void init(const Glib::RefPtr < Gtk::Builder >&);

    Gtk::Dialog* mDialog;
    Gtk::Entry* mNameEntry;
    Gtk::Image* mOkImage;
    Gtk::Button* mOkButton;
    const Forrester& mForrester;
    std::string mOldName;
    sigc::connection textModifyHandler;
};

}
}
}
}
#endif
