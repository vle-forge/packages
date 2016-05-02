/*
 * @file vle/extension/fsa/FSA.cpp
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


#include <vle/extension/fsa/FSA.hpp>
#include <vle/utils/Exception.hpp>
#include <vle/value/Value.hpp>

namespace vle { namespace extension { namespace fsa {

void Base::initialState(int state)
{
    if (not existState(state)) {
        throw utils::InternalError((boost::format(
                "FSA::Base model, unknow state %1%") % state).str());
    }

    mInitialState = state;
    mInit = true;
}


void Base::copyExternalEventAttrs(
    const vle::devs::ExternalEvent& event,
    vle::devs::ExternalEvent& tofill) const
{
    if (event.attributes()) {
        vle::value::Map::const_iterator it = event.getMap().begin();
        if (tofill.attributes() == nullptr) {
            tofill.addMap();
        }

        while (it != event.getMap().end()) {
            tofill.attributes()->toMap().add(it->first, it->second->clone());
            ++it;
        }
    }
}

}}} // namespace vle extension fsa
