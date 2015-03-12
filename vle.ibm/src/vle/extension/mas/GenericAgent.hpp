/*
 * @file vle/extension/mas/GenericAgent.hpp
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
#ifndef GENERIC_AGENT_HPP
#define GENERIC_AGENT_HPP

#include <iostream>
#include <unordered_map>

#include <vle/utils/Exception.hpp>
#include <vle/devs/Executive.hpp>

#include <vle/extension/mas/Scheduler.hpp>
#include <vle/extension/mas/Message.hpp>
#include <vle/extension/mas/Effect.hpp>

#include <boost/bind.hpp>
namespace vd = vle::devs;
namespace vu = vle::utils;
using namespace vle::extension::mas;

namespace vle
{
namespace extension
{
namespace mas
{

/** @class GenericAgent
 *  @brief Generic Agent class
 *  It allows user to create an agent model with 3 functions (agent_init,
 *  agent_dynamic, and agent_handleEvent)
 *  @see void agent_dynamic()
 *  @see void agent_init()
 *  @see void agent_handleEvent(const Event&)
 */
class GenericAgent : public vd::Executive
{
public:
    GenericAgent(const vd::ExecutiveInit& init, const vd::InitEventList &events);

    /* vle::devs override functions */
    virtual vd::Time init(const vd::Time&);
    virtual void internalTransition(const vd::Time&);
    virtual vd::Time timeAdvance() const;
    virtual void output(const vd::Time&, vd::ExternalEventList&) const;
    virtual void externalTransition(const vd::ExternalEventList&,
                                    const vd::Time&);

    inline void addEffect(const std::string& name,
                          const Effect::EffectFunction& f)
    {mEffectBinder.insert(std::make_pair(name,f));}

    inline void applyEffect(const std::string& name, const Effect& e)
    {mEffectBinder.at(name)(e);}
protected:
    /** @brief Pure virtual agent functions. Modeler must override them */
    virtual void agent_dynamic() = 0;
    /** @brief Pure virtual agent functions. Modeler must override them */
    virtual void agent_init() = 0;
    /** @brief Pure virtual agent functions. Modeler must override them */
    virtual void agent_handleEvent(const Message&) = 0;

    /* Utils functions */
    inline void sendMessage(Message& m) { mMessagesToSend.push_back(m); }
private:
    /** @brief send all the messages in send buffer */
    void sendMessages(vd::ExternalEventList& event_list) const;

    /** @brief  Copy external events and calls user function
     *  @see    agent_handleEvent*/
    void handleExternalEvents(const vd::ExternalEventList &event_list);
protected:
    static const std::string cOutputPortName;   /**< Agent output port name */
    static const std::string cInputPortName;    /**< Agent input port name */

    Scheduler<Effect> mScheduler;    /**< Agent scheduler */
    double           mCurrentTime;  /**< Last known simulation time */
    double           mLastUpdate;   /**< Last time the model had been updated */
private:
    typedef enum {INIT,   /**< initialization state:initialize vars and behaviour*/
                  IDLE,   /**< idle state : listen network and do dynamic*/
                  OUTPUT  /**< output state : send messages*/
    } states;             /**< states of machine state*/

    states             mState;          /**< Agent current state */
    std::vector<Message> mMessagesToSend;   /**< Events to send whith devs::output*/
    std::unordered_map<std::string,Effect::EffectFunction> mEffectBinder;
};

}}} //namespace vle extension mas
#endif
