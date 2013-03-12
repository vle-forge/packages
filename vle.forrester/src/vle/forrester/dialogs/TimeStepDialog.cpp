/*
 * @file vle/forrester/dialogs/TimeStepDialog.cpp
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


#include <vle/forrester/dialogs/TimeStepDialog.hpp>

namespace vle {
namespace gvle {
namespace modeling {
namespace forrester {

TimeStepDialog::TimeStepDialog(
    const Glib::RefPtr < Gtk::Builder >& xml)
{
    xml->get_widget("TimeStepDialog", mDialog);
    xml->get_widget("timestep", mTimeStep);
    xml->get_widget("TimeStepDialogOk",mOkButton);
    textModifyHandler = mTimeStep->signal_changed().connect(
            sigc::mem_fun(*this, &TimeStepDialog::onTextChange));
}

int TimeStepDialog::run(double timeStep)
{
    mTimeStep->set_text(boost::lexical_cast<std::string>(timeStep));
    int response = mDialog->run();

    mDialog->hide();
    return response;
}

void TimeStepDialog::onTextChange()
{
    try {
        boost::lexical_cast<double>(mTimeStep->get_text());
        mOkButton->set_sensitive(true);
    }catch(boost::bad_lexical_cast &){
        mOkButton->set_sensitive(false);
    }
}


}
}
}
}    // namespace vle gvle modeling forrester
