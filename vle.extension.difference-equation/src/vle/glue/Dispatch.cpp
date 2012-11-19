/*
 * @file vle/glue/Dispatch.cpp
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

class Dispatch : public vle::extension::DifferenceEquation::Multiple
{
public:
    Dispatch(const vle::devs::DynamicsInit& init,
             const vle::devs::InitEventList& events) :
	vle::extension::DifferenceEquation::Multiple(init, events)
    {
        mMode = PORT;

        {
            const vle::graph::ConnectionList& list =
                getModel().getOutputPortList();

            for (vle::graph::ConnectionList::const_iterator it =
                     list.begin(); it != list.end(); ++it) {
                mWeights[it->first] =
                    vle::value::toDouble(events.get(it->first));
                mVars[it->first] = createVar(it->first);
            }
        }

        {
            const vle::graph::ConnectionList& list =
                getModel().getInputPortList();

            if (list.size() != 1) {
                throw vle::utils::ModellingError(
                    vle::fmt(_("[%1%] glue::dispatch "                \
                               "- wrong input port number")) %
                    getModelName());
            }

            in = createSync(list.begin()->first);
        }
    }

    virtual ~Dispatch() { }

    virtual void compute(const vle::devs::Time& /* time */)
    {
        for (std::map < std::string, Var >::iterator it = mVars.begin();
             it != mVars.end(); ++it) {
            it->second = in() * mWeights[it->first];
        }
    }

    virtual void initValue(const vle::devs::Time& time)
    { computeInit(time); }

private:
    std::map < std::string, double > mWeights;
    std::map < std::string, Var > mVars;
    Sync in;
};

} // namespace glue

DECLARE_DYNAMICS(glue::Dispatch)
