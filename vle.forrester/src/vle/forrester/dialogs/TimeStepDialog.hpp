/*
 * @file vle/forrester/dialogs/TimeStepDialog.hpp
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


#ifndef VLE_GVLE_MODELING_FSA_TIME_STEP_DIALOG_HPP
#define VLE_GVLE_MODELING_FSA_TIME_STEP_DIALOG_HPP

#include <gtkmm/dialog.h>
#include <gtkmm/entry.h>
#include <gtkmm/builder.h>
#include <gtkmm/spinbutton.h>
#include <boost/lexical_cast.hpp>

namespace vle {
namespace gvle {
namespace modeling {
namespace forrester {

class TimeStepDialog
{
public:
    TimeStepDialog(const Glib::RefPtr < Gtk::Builder >& xml);

    virtual ~TimeStepDialog()
    {
        textModifyHandler.disconnect();
    }

     double timeStep() const
    {
        return boost::lexical_cast<double>(mTimeStep->get_text());
    }

    int run(double timeStep);

private:
    void onTextChange();

    Gtk::Dialog* mDialog;
    Gtk::Entry* mTimeStep;
    Gtk::Button* mOkButton;
    sigc::connection textModifyHandler;
};

}
}
}
}    // namespace vle gvle modeling forrester

#endif
