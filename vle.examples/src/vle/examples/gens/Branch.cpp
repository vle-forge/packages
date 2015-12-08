/*
 * @file vle/examples/gens/Branch.cpp
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

#include <vle/devs/Executive.hpp>
#include <vle/devs/ExecutiveDbg.hpp>
#include <vle/value/String.hpp>

namespace vle { namespace examples { namespace gens {

class Branch : public devs::Executive
{
public:
    Branch(const devs::ExecutiveInit& mdl, const devs::InitEventList& events) :
        devs::Executive(mdl, events)
    {}

    virtual ~Branch()
    {}

    virtual devs::Time init(const devs::Time& /* time */)
    {
        return 10.0;
    }

    virtual devs::Time timeAdvance() const
    {
        return 10.0;
    }

    virtual void internalTransition(const devs::Time& /*time*/)
    {
        std::vector<std::string> modelname;

        {
            vpz::ModelList::const_iterator it =
                coupledmodel().getModelList().begin();
            vpz::ModelList::const_iterator et =
                coupledmodel().getModelList().end();

            for (; it != et; ++it)
                if (it->first != getModelName())
                    modelname.push_back(it->first);
        }

        {
            for (std::size_t i = 0, ei = modelname.size(); i != ei; ++i)
                delModel(modelname[i]);
        }
    }

    virtual void output(const devs::Time& /*time*/,
                        devs::ExternalEventList& output) const
    {
        vle::devs::ExternalEvent *evt = new vle::devs::ExternalEvent("out");
        evt->putAttribute("out", new vle::value::String("branch"));

        output.push_back(evt);
    }
};

}}} // namespace vle examples gens

DECLARE_EXECUTIVE_DBG(vle::examples::gens::Branch)
