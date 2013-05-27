/*
 * @file vle/extension/decision/Agent.hpp
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


#ifndef VLE_EXT_DECISION_AGENT_HPP
#define VLE_EXT_DECISION_AGENT_HPP 1

#include <vle/extension/decision/KnowledgeBase.hpp>
#include <vle/devs/Dynamics.hpp>

namespace vle { namespace extension { namespace decision {

class Agent : public devs::Dynamics,
                                   public decision::KnowledgeBase
{
public:
    Agent(const devs::DynamicsInit& mdl,
          const devs::InitEventList& evts)
        : devs::Dynamics(mdl, evts), mState(Init), mCurrentTime(0.0),
        mPortMode(true)
    {}

    virtual ~Agent() {}

    /**
     * @brief Return the simulation time.
     * @return The simulation time.
     */
    const devs::Time& currentTime() const { return mCurrentTime; }

    /* * * * * Update methods * * * * */

    typedef Activities::result_t ActivityList;

    /* * * * * DEVS methods * * * * */

    virtual devs::Time init(const devs::Time& time);

    /**
     * @brief Process the output function: compute the output function.
     * @param time the time of the occurrence of output function.
     * @param output the list of external events (output parameter).
     */
    virtual void output(
        const devs::Time& time,
        devs::ExternalEventList& output) const;

    /**
     * @brief Process the time advance function: compute the duration of the
     * current state.
     * @return duration of the current state.
     */
    virtual devs::Time timeAdvance() const;

    /**
     * @brief Process an internal transition: compute the new state of the
     * model with the internal transition function.
     * @param time the date of occurence of this event.
     */
    virtual void internalTransition(const devs::Time& time);

    /**
     * @brief Process an external transition: compute the new state of the
     * model when an external event occurs.
     * @param event the external event with of the port.
     * @param time the date of occurrence of this event.
     */
    virtual void externalTransition(
        const devs::ExternalEventList& events,
        const devs::Time& time);

    /**
     * @brief Process the confluent transition: select the transition to
     * call when an internal and one or more external event appear in the
     * same time.
     * @param internal the internal event.
     * @param extEventlist the external events list.
     * @return Event::INTERNAL if internal is priority or Event::EXTERNAL.
     */
    virtual void confluentTransitions(
        const devs::Time& time,
        const devs::ExternalEventList& extEventlist);

    /**
     * @brief Process an observation event: compute the current state of the
     * model at a specified time and for a specified port.
     * @param event the state event with of the port
     * @return the value of state variable
     */
    virtual value::Value* observation(
        const devs::ObservationEvent& event) const;

    /**
     * @brief When the simulation of the atomic model is finished, the
     * finish method is invoked.
     */
    virtual void finish();

protected:
    /**
     * @brief DEVS phase of the autotmate.
     */
    enum State {
        Init, /**< The initial state. */
        Process, /**< call the process function of the KnowledgeBase. */
        UpdateFact, /**< Update the facts. */
        Output /**< State to output result. */
    };

    State mState;
    mutable devs::Time mCurrentTime; /* To assign mCurrentTime in output
                                        function. */

    KnowledgeBase::Result mNextChangeTime;

    bool mPortMode;
};

}}} // namespace vle ext decision

#endif
