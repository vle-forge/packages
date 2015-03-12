/*
 * @file vle/extension/mas/GenericAgent.cpp
 *
 * This file is part of VLE, a framework for multi-modeling, simulation
 * and analysis of complex dynamical systems
 * http://www.vle-project.org
 *
 * Copyright (c) 2013-2015 INRA http://www.inra.fr
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
#include <vle/extension/mas/GenericAgent.hpp>

namespace vle {
namespace extension {
namespace mas {

const std::string GenericAgent::cOutputPortName = "agent_output";
const std::string GenericAgent::cInputPortName =  "agent_input";


GenericAgent::GenericAgent(const vd::ExecutiveInit& init,
                           const vd::InitEventList &events)
    :vd::Executive(init,events),mCurrentTime(0.0),mState(INIT)
{ }

vd::Time GenericAgent::init(const vd::Time &t)
{
    mCurrentTime = t;
    switch(mState) {
        case INIT:
            /* Call internal transition */
            return 0.0;
        break;
        case IDLE:
            throw vu::InternalError("function init called in state"\
                                    " IDLE : forbidden state");
        break;
        case OUTPUT:
            throw vu::InternalError("function init called in state"\
                                    " OUTPUT : forbidden state");
    }
    /* Avoid compiler warning*/
    return vd::infinity;
}

void GenericAgent::internalTransition(const vd::Time &t)
{
    mCurrentTime = t;
    switch(mState) {
        case INIT:
            /* model initialization */
            agent_init();
            mState = IDLE;
            mLastUpdate = t;
        break;
        case IDLE:
            /* model behaviour */
            agent_dynamic();
            mLastUpdate = t;
        break;
        case OUTPUT:
            /* remove messages (they have been sent!)*/
            mState = IDLE;
            mMessagesToSend.clear();
        break;
    }

    /* Send all the messages */
    if(mMessagesToSend.size() > 0)
        mState = OUTPUT;
}

vd::Time GenericAgent::timeAdvance() const
{
    switch(mState) {
        case INIT:
            throw vle::utils::InternalError("function timeAdvance called "\
                                            "in state IDLE : forbidden "\
                                            "state");
        break;
        case IDLE:
            if (mScheduler.empty()) {
                /* Waiting state */
                return vd::infinity;
            } else {
                /* Wake me when next event is ready*/
                double ta = mScheduler.nextEffect().getDate() - mCurrentTime;
                if (ta < 0) {
                    return 0;
                } else {
                    return ta;
                }
            }
        break;
        case OUTPUT:
            /* Call vle::devs::output */
            return 0.0;
        break;
    }

    /* Avoid compiler warning */
    return vd::infinity;
}

void GenericAgent::output(const vd::Time& /*t*/,
                          vd::ExternalEventList& event_list) const
{
    switch(mState) {
    case INIT:
        break;
    case IDLE:
        break;
    case OUTPUT:
        /* Send ALL the messages */
        sendMessages(event_list);
        break;
    }
}

void GenericAgent::externalTransition(const vd::ExternalEventList &event_list,
                                      const vd::Time &t)
{
    mCurrentTime = t;
    switch(mState) {
        case INIT:
        case IDLE:
        case OUTPUT:
            /* Handle external event in any case*/
            handleExternalEvents(event_list);
        break;
    }

    /* Send all the messages */
    if(mMessagesToSend.size() > 0)
        mState = OUTPUT;
}


void GenericAgent::sendMessages(vd::ExternalEventList& event_list) const
{
    for (const auto& messageToSend : mMessagesToSend) {
        vd::ExternalEvent* DEVS_event = new vd::ExternalEvent(messageToSend.getReceiver() + "_toPerturb");
        for (const auto& p_name : messageToSend.getInformations()) {
            vv::Value *v = p_name.second.get()->clone();
            DEVS_event << vd::attribute(p_name.first, v);
        }
        event_list.push_back(DEVS_event);
    }
}


void GenericAgent::handleExternalEvents(
                                    const vd::ExternalEventList &event_list)
{
    for (const auto& event : event_list) {
        std::string receiver = "";
        std::string sender = event->getPortName();
        std::string subject = "";
        Message incomingM(sender,receiver,subject);

        for (const auto& attribute : event->getAttributes()) {
            incomingM.add(attribute.first,attribute.second->clone());
        }
        agent_handleEvent(incomingM);
    }
}


}}} //namespace vle extension mas
