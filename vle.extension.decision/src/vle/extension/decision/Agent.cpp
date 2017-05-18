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


#include <vle/extension/decision/Agent.hpp>
#include <vle/value/String.hpp>
#include <cassert>
#include <sstream>

namespace vle { namespace extension { namespace decision {

devs::Time Agent::init(devs::Time time)
{
    mState = Output;
    mCurrentTime = time;
    mNextChangeTime = processChanges(time);
    waiter = mWait;

    return 0.0;
}

void Agent::output(devs::Time time,
                   devs::ExternalEventList& output) const
{
    if (mState == Output) {
        mCurrentTime = time;

        {
            const Agent::ActivityList& lst = latestStartedActivities();
            Agent::ActivityList::const_iterator it = lst.begin();
            for (; it != lst.end(); ++it) {
                (*it)->second.output((*it)->first, output);
            }
        }
        {
            const Agent::ActivityList& lst = latestFailedActivities();
            Agent::ActivityList::const_iterator it = lst.begin();
            for (; it != lst.end(); ++it) {
                (*it)->second.output((*it)->first, output);
            }
        }
        {
            const Agent::ActivityList& lst = latestDoneActivities();
            Agent::ActivityList::const_iterator it = lst.begin();
            for (; it != lst.end(); ++it) {
                (*it)->second.output((*it)->first, output);
            }
        }
        {
            const Agent::ActivityList& lst = latestEndedActivities();
            Agent::ActivityList::const_iterator it = lst.begin();
            for (; it != lst.end(); ++it) {
                (*it)->second.output((*it)->first, output);
            }
        }
    }
}

devs::Time Agent::timeAdvance() const
{
    switch (mState) {
    case Init:
    case Output:
    case UpdateFact:
        return 0.0;
    case Process:
        if (mNextChangeTime.second == devs::negativeInfinity or
            mNextChangeTime.first == true or
            haveActivityInLatestActivitiesLists()) {
            return 0.0;
        } else {
            return mNextChangeTime.second - mCurrentTime;
        }
    }

    throw utils::InternalError("[Decision:Agent] timeAdvance");
}

void Agent::internalTransition(devs::Time time)
{
    mCurrentTime = time;

    switch (mState) {
    case Output:
        clearLatestActivitiesLists();
    case Init:
    case UpdateFact:
        waiter--;
        if (waiter < 0) {
            mNextChangeTime = processChanges(time);
            mState = Process;
            waiter = mWait;
        } else {
            mState = UpdateFact;
        }
        break;
    case Process:
        mState = Output;
        break;
    }
}

void Agent::externalTransition(
    const devs::ExternalEventList& events,
    devs::Time time)
{
    mCurrentTime = time;

    for (devs::ExternalEventList::const_iterator it = events.begin();
         it != events.end(); ++it) {
        const std::string& port(it->getPortName());


        if (port == "ack") {
            const value::Map& atts = it->attributes()->toMap();
            const std::string& activity(atts.getString("name"));
            const std::string& order(atts.getString("value"));

            if (order == "done") {
                setActivityDone(activity, time);
            } else if (order == "fail") {
                setActivityFailed(activity, time);
            } else {
                throw utils::ModellingError(
                    vle::utils::format("Decision: unknown order `%s'",
                            order.c_str()));
            }
        } else {
            if (it->attributes()->isMap()) {
                const value::Map& atts = it->attributes()->toMap();
                value::Map::const_iterator jt = atts.value().find("value");
                if (jt == atts.end()) {
                    jt = atts.value().find("init");
                }

                if (jt == atts.end() or not jt->second) {
                    throw utils::ModellingError(
                            "Decision: no value in this message");
                }

                if (mPortMode) {
                    applyFact(port, *jt->second);
                } else {
                    const std::string& fact(atts.getString("name"));
                    applyFact(fact, *jt->second);
                }
            } else {
                applyFact(port, *it->attributes());
            }
        }
    }

    mState = UpdateFact;
}

void Agent::confluentTransitions(
    devs::Time time,
    const devs::ExternalEventList& extEventlist)
{
    externalTransition(extEventlist, time);
    internalTransition(time);
}

std::unique_ptr<vle::value::Value> Agent::observation(
    const devs::ObservationEvent& event) const
{
    const std::string port = event.getPortName();

    if (port == "KnowledgeBase") {
        std::stringstream out;
        out << *this;
        return std::unique_ptr<vle::value::Value>(new value::String(out.str()));
    } else if (port == "Activities") {
        std::stringstream out;
        out << activities();
        return std::unique_ptr<vle::value::Value>(new value::String(out.str()));
    } else if ((port.compare(0, 9, "Activity_") == 0) and port.size() > 9) {
        std::string activity(port, 9, std::string::npos);
        if (activities().exist(activity)) {
            const Activity& act(activities().get(activity)->second);
            std::stringstream out;
            out << act.state();
            return std::unique_ptr<vle::value::Value>(
                new value::String(out.str()));
        }
    } else if ((port.compare(0, 16, "Activity(state)_") == 0) and port.size() > 16) {
        std::string activity(port, 16, std::string::npos);
        if (activities().exist(activity)) {
            const Activity& act(activities().get(activity)->second);
            std::stringstream out;
            out << act.state();
            return std::unique_ptr<vle::value::Value>(
                new value::String(out.str()));
        }
    } else if ((port.compare(0, 20, "Activity(resources)_") == 0) and port.size() > 20) {
        std::string activity(port, 20, std::string::npos);
        if (activities().exist(activity)) {
            std::stringstream out;
            out << activities().resources(activity);
            return std::unique_ptr<vle::value::Value>(
                new value::String(out.str()));
        }
    }

    return vle::devs::Dynamics::observation(event);
}

void Agent::finish()
{
}

}}} // namespace vle ext decision
