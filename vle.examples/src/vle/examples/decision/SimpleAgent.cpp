/*
 * @file vle/examples/decision/SimpleAgent.cpp
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

#include <vle/extension/decision/Agent.hpp>
#include <vle/extension/decision/Activity.hpp>
#include <vle/devs/DynamicsDbg.hpp>
#include <sstream>

namespace vd = vle::devs;
namespace vv = vle::value;
namespace vmd = vle::extension::decision;

namespace vle { namespace examples { namespace decision {

class SimpleAgent: public vmd::Agent
{
public:
    SimpleAgent(const vd::DynamicsInit& mdl, const vd::InitEventList& evts)
        : vmd::Agent(mdl, evts), mStart(false)
    {
        addFact("start", boost::bind(&SimpleAgent::start, this, _1));

        vmd::Rule& r = addRule("rule");
        r.add(boost::bind(&SimpleAgent::isStarted, this));

        vmd::Rule& r2 = addRule("rule2");
        r2.add(boost::bind(&SimpleAgent::alwaysFalse, this));

        vmd::Activity& b = addActivity("B", 0.0, 10.0);
        b.addRule("rule2", r2);

        vmd::Activity& a = addActivity("A", 0.0, 10.0);
        a.addRule("rule", r);

        addStartToStartConstraint("A", "B", 3.0);
    }

    virtual ~SimpleAgent()
    {
    }

    void start(const vle::value::Value& val)
    {
        if (not mStart) {
            mStart = val.toBoolean().value();
        }
    }

    bool isStarted() const
    {
        return mStart;
    }

    bool alwaysFalse() const
    {
        return false;
    }

    virtual vv::Value* observation(const vd::ObservationEvent& evt) const
    {
        if (evt.onPort("text")) {
            std::ostringstream out;
            out << *this;

            return new vv::String(out.str());
        }

        return 0;
    }

private:
    bool mStart;
};

}}} // namespace vle examples decision

DECLARE_DYNAMICS_DBG(vle::examples::decision::SimpleAgent)
