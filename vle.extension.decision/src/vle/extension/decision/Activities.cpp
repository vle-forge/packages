/*
 * @file vle/extension/decision/Activities.cpp
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


#include <vle/extension/decision/Activities.hpp>
#include <vle/utils/Exception.hpp>
#include <vle/utils/i18n.hpp>
#include <numeric>

namespace vle { namespace extension { namespace decision {

Activity& Activities::add(const std::string& name, const Activity& act,
                          const Activity::OutFct& out,
                          const Activity::AckFct& ack)
{
    iterator it(m_lst.find(name));

    if (it != m_lst.end()) {
        throw utils::ArgError(
            vle::fmt(_("Decision: activity '%1%' already exist")) % name);
    }

    Activity& a((*m_lst.insert(value_type(name, act)).first).second);
    if (out) {
        a.addOutputFunction(out);
    }

    if (ack) {
        a.addAcknowledgeFunction(ack);
    }

    return a;
}

Activity& Activities::add(const std::string& name,
                          const Activity::OutFct& out,
                          const Activity::AckFct& ack)
{
    iterator it(m_lst.find(name));

    if (it != m_lst.end()) {
        throw utils::ArgError(
            vle::fmt(_("Decision: activity '%1%' already exist")) % name);
    }

    Activity& a((*m_lst.insert(value_type(name, Activity())).first).second);
    if (out) {
        a.addOutputFunction(out);
    }

    if (ack) {
        a.addAcknowledgeFunction(ack);
    }

    return a;
}

Activity& Activities::add(const std::string& name,
                          const devs::Time& start,
                          const devs::Time& end,
                          const Activity::OutFct& out,
                          const Activity::AckFct& ack)
{
    Activity& result = add(name);
    result.initStartTimeFinishTime(start, end);

    if (out) {
        result.addOutputFunction(out);
    }

    if (ack) {
        result.addAcknowledgeFunction(ack);
    }

    return result;
}

void Activities::remove(const std::string& name)
{
    iterator it(m_lst.find(name));

    if (it != m_lst.end()) {
        throw utils::ArgError(_("Decision: activity does not exist"));
    }

    m_lst.erase(it);
}

void Activities::addStartToStartConstraint(const std::string& acti,
                                           const std::string& actj,
                                           const devs::Time& mintimelag,
                                           const devs::Time& maxtimelag)
{
    addPrecedenceConstraint(
        PrecedenceConstraint(get(acti), get(actj), PrecedenceConstraint::SS,
                             mintimelag, maxtimelag));
}

void Activities::addFinishToFinishConstraint(const std::string& acti,
                                             const std::string& actj,
                                             const devs::Time& mintimelag,
                                             const devs::Time& maxtimelag)
{
    addPrecedenceConstraint(
        PrecedenceConstraint(get(acti), get(actj), PrecedenceConstraint::FF,
                             mintimelag, maxtimelag));
}

void Activities::addFinishToStartConstraint(const std::string& acti,
                                            const std::string& actj,
                                            const devs::Time& mintimelag,
                                            const devs::Time& maxtimelag)
{
    addPrecedenceConstraint(
        PrecedenceConstraint(get(acti), get(actj), PrecedenceConstraint::FS,
                             mintimelag, maxtimelag));
}

devs::Time Activities::nextDate(const devs::Time& time)
{
    devs::Time result = devs::infinity;

    for (iterator activity = begin(); activity != end(); ++activity) {
        result = std::min(result, activity->second.nextTime(time));
    }

    return result;
}

Activities::const_result_t
Activities::beforeTimeHorizonAct(
    const devs::Time& lowerBound,
    const devs::Time& upperBound) const
{
    Activities::const_result_t beforeHorizonAct;

    for (const_iterator activity = begin(); activity != end(); ++activity) {
        switch (activity->second.state()) {
        case Activity::WAIT:
            if (activity->second.isValidHorizonTimeConstraint(lowerBound,
                                                              upperBound))
            {
                beforeHorizonAct.push_back(activity);
            }
            break;
        case Activity::STARTED:
        case Activity::FF:
        case Activity::DONE:
        case Activity::FAILED:
            break;
        default:
            throw utils::InternalError(_("Decision: unknown state"));
        }
    }
    return beforeHorizonAct;
}

void Activities::removeWaitedAct(Activities::iterator it)
{
    removeAct(m_waitedAct, it);
}

void Activities::removeStartedAct(Activities::iterator it)
{
    removeAct(m_startedAct, it);
}

void Activities::removeFailedAct(Activities::iterator it)
{
    removeAct(m_failedAct, it);
}

void Activities::removeFFAct(Activities::iterator it)
{
    removeAct(m_ffAct, it);
}

void Activities::removeEndedAct(Activities::iterator it)
{
    removeAct(m_endedAct, it);
}

void Activities::addWaitedAct(Activities::iterator it)
{
    addAct(m_waitedAct, it);
}

void Activities::addStartedAct(Activities::iterator it)
{
    addAct(m_startedAct, it);
}

void Activities::addFailedAct(Activities::iterator it)
{
    addAct(m_failedAct, it);
}

void Activities::addFFAct(Activities::iterator it)
{
    addAct(m_ffAct, it);
}

void Activities::addEndedAct(Activities::iterator it)
{
    addAct(m_endedAct, it);
}

void Activities::setWaitedAct(Activities::iterator it)
{
    switch (it->second.state()) {
    case Activity::WAIT:
        removeWaitedAct(it);
        addWaitedAct(it);
        updateLatestActivitiesList(m_latestWaitedAct, it);
        break;
    case Activity::STARTED:
        removeStartedAct(it);
        addStartedAct(it);
        updateLatestActivitiesList(m_latestStartedAct, it);
        break;
    case Activity::FF:
        removeFFAct(it);
        addFFAct(it);
        updateLatestActivitiesList(m_latestFFAct, it);
        break;
    case Activity::DONE:
        removeEndedAct(it);
        addEndedAct(it);
        updateLatestActivitiesList(m_latestEndedAct, it);
        break;
    case Activity::FAILED:
        removeFailedAct(it);
        addFailedAct(it);
        updateLatestActivitiesList(m_latestFailedAct, it);
        break;
    }
}

void Activities::setStartedAct(Activities::iterator it)
{
    switch (it->second.state()) {
    case Activity::WAIT:
        removeWaitedAct(it);
        updateLatestActivitiesList(m_latestWaitedAct, it);
        break;
    case Activity::STARTED:
        removeStartedAct(it);
        updateLatestActivitiesList(m_latestStartedAct, it);
        break;
    case Activity::FF:
        removeFFAct(it);
        updateLatestActivitiesList(m_latestFFAct, it);
        break;
    case Activity::DONE:
        removeEndedAct(it);
        updateLatestActivitiesList(m_latestEndedAct, it);
        break;
    case Activity::FAILED:
        removeFailedAct(it);
        updateLatestActivitiesList(m_latestFailedAct, it);
        break;
    }
    addStartedAct(it);
}

void Activities::setFailedAct(Activities::iterator it)
{
    switch (it->second.state()) {
    case Activity::WAIT:
        removeWaitedAct(it);
        updateLatestActivitiesList(m_latestWaitedAct, it);
        break;
    case Activity::STARTED:
        removeStartedAct(it);
        updateLatestActivitiesList(m_latestStartedAct, it);
        break;
    case Activity::FF:
        removeFFAct(it);
        updateLatestActivitiesList(m_latestFFAct, it);
        break;
    case Activity::DONE:
        removeEndedAct(it);
        updateLatestActivitiesList(m_latestEndedAct, it);
        break;
    case Activity::FAILED:
        removeFailedAct(it);
        updateLatestActivitiesList(m_latestFailedAct, it);
        break;
    }
    addFailedAct(it);
}

void Activities::setFFAct(Activities::iterator it)
{
    switch (it->second.state()) {
    case Activity::WAIT:
        removeWaitedAct(it);
        updateLatestActivitiesList(m_latestWaitedAct, it);
        break;
    case Activity::STARTED:
        removeStartedAct(it);
        updateLatestActivitiesList(m_latestStartedAct, it);
        break;
    case Activity::FF:
        removeFFAct(it);
        updateLatestActivitiesList(m_latestFFAct, it);
        break;
    case Activity::DONE:
        removeEndedAct(it);
        updateLatestActivitiesList(m_latestEndedAct, it);
        break;
    case Activity::FAILED:
        removeFailedAct(it);
        updateLatestActivitiesList(m_latestFailedAct, it);
        break;
    }
    addFFAct(it);
}

void Activities::setEndedAct(Activities::iterator it)
{
    switch (it->second.state()) {
    case Activity::WAIT:
        removeWaitedAct(it);
        updateLatestActivitiesList(m_latestWaitedAct, it);
        break;
    case Activity::STARTED:
        removeStartedAct(it);
        updateLatestActivitiesList(m_latestStartedAct, it);
        break;
    case Activity::FF:
        removeFFAct(it);
        updateLatestActivitiesList(m_latestFFAct, it);
        break;
    case Activity::DONE:
        removeEndedAct(it);
        updateLatestActivitiesList(m_latestEndedAct, it);
        break;
    case Activity::FAILED:
        removeFailedAct(it);
        updateLatestActivitiesList(m_latestFailedAct, it);
        break;
    }
    addEndedAct(it);
}

void Activities::removeAct(Activities::result_t& lst, Activities::iterator it)
{
    Activities::result_t::iterator del = std::find(lst.begin(), lst.end(), it);
    if (del != lst.end()) {
        lst.erase(del);
    }
}

void Activities::addAct(Activities::result_t& lst, Activities::iterator it)
{
    lst.push_back(it);
}

void Activities::updateLatestActivitiesList(Activities::result_t& lst,
                                            Activities::iterator it)
{
    removeAct(m_latestWaitedAct, it);
    removeAct(m_latestStartedAct, it);
    removeAct(m_latestEndedAct, it);
    removeAct(m_latestFFAct, it);
    removeAct(m_latestFailedAct, it);

    addAct(lst, it);
}

void Activities::clearLatestActivitiesLists()
{
    m_latestWaitedAct.clear();
    m_latestStartedAct.clear();
    m_latestFFAct.clear();
    m_latestFailedAct.clear();
    m_latestEndedAct.clear();
}

Activities::Result
Activities::process(const devs::Time& time)
{
    devs::Time nextDate = devs::infinity;
    Result update = std::make_pair(false, devs::infinity);
    bool isUpdated = false;

    do {
        m_waitedAct.clear();
        m_startedAct.clear();
        m_ffAct.clear();
        m_failedAct.clear();
        m_endedAct.clear();

        for (iterator activity = begin(); activity != end(); ++activity) {
            switch (activity->second.state()) {
            case Activity::WAIT:
                update = processWaitState(activity, time);
                break;

            case Activity::STARTED:
                update = processStartedState(activity, time);
                break;

            case Activity::FF:
                update = processFFState(activity, time);
                break;

            case Activity::DONE:
                update = processEndedState(activity, time);
                break;

            case Activity::FAILED:
                update = processFailedState(activity, time);
                break;

            default:
                throw utils::InternalError(_("Decision: unknown state"));
            }

            if (not isUpdated and update.first) {
                isUpdated = true;
            }

            if (update.first) {
                activity->second.update(activity->first);
            }

            if (update.second != time and
                update.second != devs::negativeInfinity) {
                nextDate = std::min(nextDate, update.second);
            }

            devs::Time nextActivityDate = activity->second.nextTime(time);
            if (nextActivityDate != time and
                nextActivityDate != devs::negativeInfinity) {
                nextDate = std::min(nextDate, nextActivityDate);
            }
        }
    } while (update.first);

    return std::make_pair(isUpdated, nextDate);
}

Activities::Result
Activities::processWaitState(iterator activity,
                             const devs::Time& time)
{
    PrecedenceConstraint::Result newstate = updateState(activity, time);
    Result update = std::make_pair(false, newstate.second);

    switch (newstate.first) {
    case PrecedenceConstraint::Valid:
    case PrecedenceConstraint::Inapplicable:
        if (activity->second.validRules(activity->first)) {
            activity->second.start(time);
            m_startedAct.push_back(activity);
            m_latestStartedAct.push_back(activity);
            update.first = true;
            break;
        }
    case PrecedenceConstraint::Wait:
        m_waitedAct.push_back(activity);
        update.first = false;
        break;
    case PrecedenceConstraint::Failed:
        activity->second.fail(time);
        m_failedAct.push_back(activity);
        m_latestFailedAct.push_back(activity);
        update.first = true;
        break;
    }

    return update;
}

Activities::Result
Activities::processStartedState(iterator activity,
                                const devs::Time& time)
{
    PrecedenceConstraint::Result newstate = updateState(activity, time);
    Result update = std::make_pair(false, newstate.second);

    switch (newstate.first) {
    case PrecedenceConstraint::Valid:
    case PrecedenceConstraint::Inapplicable:
    case PrecedenceConstraint::Wait:
        m_startedAct.push_back(activity);
        update.first = false;
        break;
    case PrecedenceConstraint::Failed:
        activity->second.fail(time);
        m_failedAct.push_back(activity);
        m_latestFailedAct.push_back(activity);
        update.first = true;
        break;
    }

    return update;
}

Activities::Result
Activities::processFFState(iterator activity,
                             const devs::Time& time)
{
    PrecedenceConstraint::Result newstate = updateState(activity, time);
    Result update = std::make_pair(false, newstate.second);

    switch (newstate.first) {
    case PrecedenceConstraint::Valid:
    case PrecedenceConstraint::Inapplicable:
        activity->second.end(time);
        m_endedAct.push_back(activity);
        m_latestEndedAct.push_back(activity);
        update.first = true;
        break;
    case PrecedenceConstraint::Wait:
        m_ffAct.push_back(activity);
        update.first = false;
        break;
    case PrecedenceConstraint::Failed:
        activity->second.fail(time);
        m_failedAct.push_back(activity);
        m_latestFailedAct.push_back(activity);
        update.first = true;
        break;
    }

    return update;
}

Activities::Result
Activities::processFailedState(iterator activity,
                               const devs::Time& /* time */)
{
    Result update = std::make_pair(false, devs::infinity);
    m_failedAct.push_back(activity);
    return update;
}

Activities::Result
Activities::processEndedState(iterator activity,
                              const devs::Time& /* time */)
{
    Result update = std::make_pair(false, devs::infinity);
    m_endedAct.push_back(activity);
    return update;
}

PrecedenceConstraint::Result Activities::updateState(iterator activity,
                                                     const devs::Time& time)
{
    PrecedenceConstraint::Result newstate;
    newstate.first = PrecedenceConstraint::Valid;
    newstate.second = devs::infinity;

    if (activity->second.isBeforeTimeConstraint(time)) {
        newstate.first = PrecedenceConstraint::Wait;
    } else if (activity->second.isAfterTimeConstraint(time)) {
        newstate.first = PrecedenceConstraint::Failed;
    } else {
        PrecedencesGraph::findIn in = m_graph.findPrecedenceIn(activity);
        PrecedencesGraph::iteratorIn it;

        if (activity->second.waitAllFsBeforeStart()) {
            it = in.first;
            while (newstate.first == PrecedenceConstraint::Valid and
                   it != in.second) {
                PrecedenceConstraint::Result r = it->isValid(time);

                switch (r.first) {
                case PrecedenceConstraint::Wait:
                    newstate.first = r.first;
                    newstate.second = std::min(newstate.second, r.second);
                    break;
                case PrecedenceConstraint::Failed:
                    newstate.first = r.first;
                    newstate.second = std::min(newstate.second, r.second);
                    break;
                case PrecedenceConstraint::Valid:
                case PrecedenceConstraint::Inapplicable:
                    newstate.first = PrecedenceConstraint::Valid;
                    newstate.second = std::min(newstate.second, r.second);
                    break;
                }
                ++it;
            }
        } else {
            it = in.first;
            while (it != in.second and
                   newstate.first != PrecedenceConstraint::Valid) {
                PrecedenceConstraint::Result r = it->isValid(time);

                switch (r.first) {
                case PrecedenceConstraint::Wait:
                    newstate.first = r.first;
                    newstate.second = std::min(newstate.second, r.second);
                    break;
                case PrecedenceConstraint::Failed:
                    newstate.first = r.first;
                    newstate.second = std::min(newstate.second, r.second);
                    break;
                case PrecedenceConstraint::Valid:
                    newstate.first = r.first;
                    newstate.second = std::min(newstate.second, r.second);
                    break;
                case PrecedenceConstraint::Inapplicable:
                    newstate.first = r.first;
                    newstate.second = std::min(newstate.second, r.second);
                    break;
                }
                ++it;
            }
        }
    }
    return newstate;
}

}}} // namespace vle model decision
