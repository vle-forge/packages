/*
 * @file vle/examples/decision/OnlyAgentPrecedenceConstraint.cpp
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

/*
 * @@tagdynamic@@
 * @@tagdepends: vle.extension.decision @@endtagdepends
 */

#include <vle/extension/Decision.hpp>
#include <sstream>

namespace vd = vle::devs;
namespace vv = vle::value;
namespace vmd = vle::extension::decision;

namespace vle { namespace examples { namespace decision {

using namespace std::placeholders;

class OnlyAgentPrecedenceConstraint : public vmd::Agent
{
public:
    OnlyAgentPrecedenceConstraint(const vd::DynamicsInit& mdl,
                                  const vd::InitEventList& evts)
        : vmd::Agent(mdl, evts), mStart(false)
    {
        vmd::Activity& a = addActivity("A", 0, devs::infinity);
        a.addOutputFunction(
            std::bind(&OnlyAgentPrecedenceConstraint::aout, this,
                      std::placeholders::_1, std::placeholders::_2,
                      std::placeholders::_3));

        vmd::Activity& b = addActivity("B", devs::negativeInfinity,
                                       devs::infinity);
        b.addOutputFunction(
            std::bind(&OnlyAgentPrecedenceConstraint::aout, this,
                      std::placeholders::_1, std::placeholders::_2,
                      std::placeholders::_3));

        vmd::Activity& c = addActivity("C", devs::negativeInfinity,
                                       devs::infinity);
        c.addOutputFunction(
            std::bind(&OnlyAgentPrecedenceConstraint::aout, this,
                      std::placeholders::_1, std::placeholders::_2,
                      std::placeholders::_3));

        vmd::Activity& d = addActivity("D", devs::negativeInfinity,
                                       devs::infinity);
        d.addOutputFunction(
            std::bind(&OnlyAgentPrecedenceConstraint::aout, this,
                      std::placeholders::_1, std::placeholders::_2,
                      std::placeholders::_3));

        addStartToStartConstraint("A", "B", 1.0, devs::infinity);
        addStartToStartConstraint("B", "C", 1.0, devs::infinity);
        addStartToStartConstraint("C", "D", 8.0, devs::infinity);
    }

    virtual ~OnlyAgentPrecedenceConstraint()
    {
    }

    virtual std::unique_ptr<vv::Value> observation(
    		const vd::ObservationEvent& evt) const override
    {
        if (evt.onPort("text")) {
            std::ostringstream out;
            out << *this;

            return value::String::create(out.str());
        }
        return 0;
    }

    void aout(const std::string& /*name*/, const vmd::Activity& activity,
              vd::ExternalEventList& out)
    {
        if (activity.isInStartedState()) {
        	out.emplace_back("out");
        }
    }

private:
    bool mStart;
};

}}} // namespace vle examples decision

DECLARE_DYNAMICS(vle::examples::decision::OnlyAgentPrecedenceConstraint)
