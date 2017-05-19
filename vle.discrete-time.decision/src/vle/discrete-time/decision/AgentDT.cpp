/*
 * This file is part of VLE, a framework for multi-modeling, simulation
 * and analysis of complex dynamical systems
 * http://www.vle-project.org
 *
 * Copyright (c) 2014-2014 INRA http://www.inra.fr
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
#include <vle/utils/Tools.hpp>
#include <vle/utils/DateTime.hpp>
#include <cassert>
#include <sstream>
#include <iomanip>

namespace vle {
namespace discrete_time {
namespace decision {

AgentDT::AgentDT(const devs::DynamicsInit& mdl,
      const devs::InitEventList& events)
    : DiscreteTimeDyn(mdl, events), KnowledgeBase(context()),
      mdefaultValues(), begin_date(), current_date()
{
    if (!(events.exist("begin_date") &&
          events.get("begin_date")->isString())) {
        throw vle::utils::FileError(
            vle::utils::format("[%s] begin_date condition missing",
                               getModelName().c_str()));
        }
    begin_date = vle::utils::DateTime::toJulianDayNumber(
        events.getString("begin_date"));
    current_date = begin_date;

    if (! events.exist("output_nil")) {
        global_output_nils(true);
    }

    vle::devs::InitEventList::const_iterator itb = events.begin();
    vle::devs::InitEventList::const_iterator ite = events.end();
    std::string prefix;
    std::string var_name;
    //first init
    for (; itb != ite; itb++) {
        const std::string& event_name = itb->first;
        if (event_name == "default_values") {
            vle::value::Map::const_iterator idb = itb->second->toMap().begin();
            vle::value::Map::const_iterator ide = itb->second->toMap().end();
            for (; idb != ide; idb++) {
                mdefaultValues.add(idb->first, idb->second->clone());
            }
        }
    }
    //2nd init (prior)
    itb = events.begin();
    for (; itb != ite; itb++) {
        const std::string& event_name = itb->first;
        if (!prefix.assign("default_value_").empty() and
                !event_name.compare(0, prefix.size(), prefix)) {
            var_name.assign(event_name.substr(prefix.size(),
                    event_name.size()));
            mdefaultValues.set(var_name, itb->second->clone());
        }
    }
}

AgentDT::~AgentDT()
{
}

void
AgentDT::compute(const vle::devs::Time& t)
{
    current_date = begin_date + t;

    KnowledgeBase::processChanges(current_date);
    Variables&  vars = getVariables();
    Variables::const_iterator itb = vars.begin();
    Variables::const_iterator ite = vars.end();

    for (; itb !=ite; itb++) {
        if ((itb->second->lastUpdateTime() < current_date)
                && (mdefaultValues.exist(itb->first))) {
            switch (itb->second->getType()) {
            case MONO: {
                VarMono* vmono = static_cast < VarMono* >(itb->second);
                vmono->update(t, *mdefaultValues.get(itb->first));
                break;
            } case MULTI: {
                VarMulti* vmulti = static_cast < VarMulti* >(itb->second);
                vmulti->update(t, *mdefaultValues.get(itb->first));
                break;
            } case VALUE_VLE: {
                VarValue* vvalue = static_cast < VarValue* >(itb->second);
                vvalue->update(t, *mdefaultValues.get(itb->first));
                break;
            }}

        }
    }
}

void
AgentDT::outputVar(const vle::vpz::AtomicModel& model,
                   const vle::devs::Time& time,
                   vle::devs::ExternalEventList& output)
{
    current_date = begin_date + time;
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
    DiscreteTimeDyn::outputVar(model, time, output);
}

void
AgentDT::handleExtEvt(const vle::devs::Time& t,
            const vle::devs::ExternalEventList& ext)
{
    current_date = begin_date + t;

    for (devs::ExternalEventList::const_iterator it = ext.begin();
         it != ext.end(); ++it) {
        const std::string& port(it->getPortName());
        const value::Map& atts = it->attributes()->toMap();

        if (port == "ack") {

            const std::string& activity(atts.getString("name"));
            const std::string& order(atts.getString("value"));

            if (order == "done") {
                KnowledgeBase::setActivityDone(activity, current_date);
            } else if (order == "fail") {
                KnowledgeBase::setActivityFailed(activity, current_date);
            } else {
                throw utils::ModellingError(
                    vle::utils::format("Decision: unknown order `%s'",
                     order.c_str()));
            }
        } else {
            DiscreteTimeDyn::handleExtVar(t, port, atts);
        }
    }
    for (devs::ExternalEventList::const_iterator it = ext.begin();
             it != ext.end(); ++it) {
        const std::string& port(it->getPortName());
        const value::Map& atts = it->attributes()->toMap();
        if (KnowledgeBase::facts().exist(port)) {
            applyFact(port, *atts.get("value"));
        }
    }
    //KnowledgeBase::processChanges(t);
}

std::unique_ptr<vle::value::Value>
AgentDT::observation(const devs::ObservationEvent& event) const
{
    using vle::extension::decision::operator<<;

    const std::string port = event.getPortName();
    if (port == "KnowledgeBase") {
        std::stringstream out;
        out << *this;
        return std::unique_ptr<vle::value::Value>(
                new value::String(out.str()));
    } else if (port == "Activities") {
        std::stringstream out;
        out << activities();
        return std::unique_ptr<vle::value::Value>(
                new value::String(out.str()));
    } else if ((port.compare(0, 9, "Activity_") == 0) and port.size() > 9) {
        std::string activity(port, 9, std::string::npos);
        const vdec::Activity& act(activities().get(activity)->second);
        std::stringstream out;
        out << act.state();
        return std::unique_ptr<vle::value::Value>(new value::String(out.str()));
    } else if ((port.compare(0, 16, "Activity(state)_") == 0) and port.size() > 16) {
        std::string activity(port, 16, std::string::npos);
        if (activities().exist(activity)) {
            const  vdec::Activity& act(activities().get(activity)->second);
            std::stringstream out;
            out << act.state();
            return std::unique_ptr<vle::value::Value>(
                new value::String(out.str()));
        }
    } else if ((port.compare(0, 20, "Activity(resources)_") == 0) and port.size() > 20) {
        std::string activity(port, 20, std::string::npos);
        if (activities().exist(activity)) {
            std::stringstream out;
            vdec::ActivitiesResourcesConstIteratorPair act = activities().resources(activity);
            out << act;
            return std::unique_ptr<vle::value::Value>(
                new value::String(out.str()));
        }
    }
    return DiscreteTimeDyn::observation(event);
}

}}} // namespace vle ext decision
