/*
 * @file vle/extension/decision/Agent.cpp
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


#include <vle/discrete-time/decision/AgentDT.hpp>
#include <vle/value/String.hpp>
#include <cassert>
#include <iomanip>

namespace vle {
namespace discrete_time {
namespace decision {

AgentDT::AgentDT(const devs::DynamicsInit& mdl,
      const devs::InitEventList& evts)
    : DiscreteTimeDyn(mdl, evts)//, mCurrentTime(0)
{

}

AgentDT::~AgentDT()
{
}

const devs::Time&
AgentDT::currentTime() const
{
    return TemporalValuesProvider::getCurrentTime();
}

void
AgentDT::compute(const vle::devs::Time& t)
{
    KnowledgeBase::processChanges(t);
}

void
AgentDT::outputVar(const vle::devs::Time& time,
        vle::devs::ExternalEventList& output) const
{
    {
        const ActivityList& lst = latestStartedActivities();
        ActivityList::const_iterator it = lst.begin();
        for (; it != lst.end(); ++it) {
            (*it)->second.output((*it)->first, output);
        }
    }
    {
        const ActivityList& lst = latestFailedActivities();
        ActivityList::const_iterator it = lst.begin();
        for (; it != lst.end(); ++it) {
            (*it)->second.output((*it)->first, output);
        }
    }
    {
        const ActivityList& lst = latestDoneActivities();
        ActivityList::const_iterator it = lst.begin();
        for (; it != lst.end(); ++it) {
            (*it)->second.output((*it)->first, output);
        }
    }
    {
        const ActivityList& lst = latestEndedActivities();
        ActivityList::const_iterator it = lst.begin();
        for (; it != lst.end(); ++it) {
            (*it)->second.output((*it)->first, output);
        }
    }
    DiscreteTimeDyn::outputVar(time, output);
}

void
AgentDT::handleExtEvt(const vle::devs::Time& t,
            const vle::devs::ExternalEventList& ext)
{

    for (devs::ExternalEventList::const_iterator it = ext.begin();
         it != ext.end(); ++it) {
        const std::string& port((*it)->getPortName());
        const value::Map& atts = (*it)->getAttributes();

        if (port == "ack") {

            const std::string& activity(atts.getString("name"));
            const std::string& order(atts.getString("value"));

            if (order == "done") {
                KnowledgeBase::setActivityDone(activity, t);
            } else if (order == "fail") {
                KnowledgeBase::setActivityFailed(activity, t);
            } else {
                throw utils::ModellingError(
                    fmt(_("Decision: unknown order `%1%'")) % order);
            }
        } else {
            DiscreteTimeDyn::handleExtEvt(t, port, atts);
        }
    }
    for (devs::ExternalEventList::const_iterator it = ext.begin();
             it != ext.end(); ++it) {
        const std::string& port((*it)->getPortName());
        const value::Map& atts = (*it)->getAttributes();
        if (KnowledgeBase::facts().exist(port)) {
            applyFact(port, *atts.get("value"));
        }
    }
    //KnowledgeBase::processChanges(t);
}

value::Value*
AgentDT::observation(const devs::ObservationEvent& event) const
{
    const std::string port = event.getPortName();
    if (port == "KnowledgeBase") {
        std::stringstream out;
        out << *this;
        return new value::String(out.str());
    } else if (port == "Activities") {
        std::stringstream out;
        out << activities();
        return new value::String(out.str());
    } else if ((port.compare(0, 9, "Activity_") == 0) and port.size() > 9) {
        std::string activity(port, 9, std::string::npos);
        const vdec::Activity& act(activities().get(activity)->second);
        std::stringstream out;
        out << act.state();
        return new value::String(out.str());
    } else if ((port.compare(0, 6, "Rules_") == 0) and port.size() > 6) {
        std::string rule(port, 6, std::string::npos);
        const vdec::Rule& ru(rules().get(rule));
        return new value::Boolean(ru.isAvailable());
    }
    return DiscreteTimeDyn::observation(event);
}

}}} // namespace vle ext decision

