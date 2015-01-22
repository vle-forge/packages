/*
 * @file vle/extension/decision/Agent.hpp
 *
 * This file is part of VLE, a framework for multi-modeling, simulation
 * and analysis of complex dynamical systems
 * http://www.vle-project.org
 *
 * Copyright (c) 2014 INRA http://www.inra.fr
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


#ifndef VLE_DISCRETE_TIME_DECISION_AGENTDT_HPP
#define VLE_DISCRETE_TIME_DECISION_AGENTDT_HPP 1

#include <vle/extension/decision/KnowledgeBase.hpp>
#include <vle/extension/decision/Activities.hpp>
#include <vle/discrete-time/DiscreteTimeDyn.hpp>

namespace vle {
namespace discrete_time {
namespace decision {

namespace vdt = vle::discrete_time;
namespace vdec = vle::extension::decision;

class AgentDT : public vdt::DiscreteTimeDyn, public vdec::KnowledgeBase
{
public:
    typedef extension::decision::Activities::result_t ActivityList;

    AgentDT(const devs::DynamicsInit& mdl,
          const devs::InitEventList& evts);

    virtual ~AgentDT();

    //AgentDT functions
    const devs::Time& currentTime() const;

    //DicreteTimeDyn functions overload
    virtual void compute(const vle::devs::Time& t);

    void outputVar(const vle::devs::Time& time,
            vle::devs::ExternalEventList& output) const;

    void handleExtEvt(const vle::devs::Time& t,
                const vle::devs::ExternalEventList& ext);

    //Dynamics functions overload
    vle::value::Value* observation(
            const vle::devs::ObservationEvent& event) const;

    //double mCurrentTime;

};

}}} // namespace vle ext decision

#endif
