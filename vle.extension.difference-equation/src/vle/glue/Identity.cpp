/*
 * @file vle/glue/Identity.cpp
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

#include <vle/extension/DifferenceEquation.hpp>
#include <vle/extension/DifferenceEquationDbg.hpp>
#include <vle/devs/DynamicsDbg.hpp>

namespace glue {

class Identity : public vle::extension::DifferenceEquation::Multiple
{
public:
    Identity(const vle::devs::DynamicsInit& init,
             const vle::devs::InitEventList& events) :
	vle::extension::DifferenceEquation::Multiple(init, events)
    {
        mMode = PORT;

        const vle::graph::ConnectionList& list =
            getModel().getInputPortList();

        for (vle::graph::ConnectionList::const_iterator it =
                 list.begin(); it != list.end(); ++it) {
            mVars[it->first] = createVar(it->first);
            mSync[it->first] = createSync(it->first);

        }
    }

    virtual ~Identity() { }

    virtual void compute(const vle::devs::Time& /* time */)
    {
        for (std::map < std::string, Var >::iterator it = mVars.begin();
             it != mVars.end(); ++it) {
            it->second = mSync[it->first]();
        }
    }

private:
    std::map < std::string, Var > mVars;
    std::map < std::string, Sync > mSync;
};

} // namespace glue

DECLARE_DYNAMICS(glue::Identity)
