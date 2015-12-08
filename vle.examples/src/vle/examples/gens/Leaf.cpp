/*
 * @file vle/examples/gens/Leaf.cpp
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

class Leaf : public devs::Executive
{
public:
    Leaf(const devs::ExecutiveInit& mdl, const devs::InitEventList& events) :
        devs::Executive(mdl, events)
    {}

    virtual ~Leaf()
    {}

    virtual devs::Time init(const devs::Time& /* time */)
    {
        return 10.0;
    }

    virtual devs::Time timeAdvance() const
    {
        return 10.0;
    }

    virtual void output(const devs::Time& /*time*/,
                        devs::ExternalEventList& output) const
    {
        vle::devs::ExternalEvent *evt = new vle::devs::ExternalEvent("out");
        evt->putAttribute("out", new vle::value::String("leaf"));

        output.push_back(evt);
    }
};

}}} // namespace vle examples gens

DECLARE_EXECUTIVE_DBG(vle::examples::gens::Leaf)
