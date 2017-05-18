/*
 * This file is part of VLE, a framework for multi-modeling, simulation
 * and analysis of complex dynamical systems
 * http://www.vle-project.org
 *
 * Copyright (c) 2013-2013 INRA http://www.inra.fr
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


#include <vle/discrete-time/DiscreteTimeDyn.hpp>
#include <vle/value/Tuple.hpp>

namespace vle {
namespace discrete_time {

namespace vu = vle::utils;
namespace vz = vle::vpz;

DiscreteTimeDyn::DiscreteTimeDyn(
        const vle::devs::DynamicsInit& model,
        const vle::devs::InitEventList& events):
        vle::devs::Dynamics(model, events),
        TemporalValuesProvider(this->getModelName(), events),
        mpimpl(new Pimpl(static_cast<TemporalValuesProvider&>(*this), events))

{
}

DiscreteTimeDyn::~DiscreteTimeDyn()
{
    delete mpimpl;
}

vle::devs::Dynamics*
DiscreteTimeDyn::toDynamics()
{
    return (vle::devs::Dynamics*) this;
}


void
DiscreteTimeDyn::time_step(double val)
{
    mpimpl->time_step(val);
}

 void
DiscreteTimeDyn::init_value(const std::string& v,
        const vle::value::Value& val)
{
     mpimpl->init_value(v, val);
}

 void
DiscreteTimeDyn::dim(const std::string& v, unsigned int val)
{
     mpimpl->dim(v, val);
}

 void
DiscreteTimeDyn::history_size(const std::string& v, unsigned int val)
{
     mpimpl->history_size(v, val);
}

 void
DiscreteTimeDyn::sync(const std::string& v, unsigned int val)
{
     mpimpl->sync(v, val);
}

 void
DiscreteTimeDyn::output_nil(const std::string& v, bool val)
{
     mpimpl->output_nil(v, val);
}

 void
DiscreteTimeDyn::output_period(const std::string& v, unsigned int val)
{
     mpimpl->output_period(v, val);
}

 void
DiscreteTimeDyn::allow_update(const std::string& v, bool val)
{
     mpimpl->allow_update(v, val);
}

 void
DiscreteTimeDyn::error_no_sync(const std::string& v, bool val)
{
     mpimpl->error_no_sync(v, val);
}

void
DiscreteTimeDyn::bags_to_eat(unsigned int val)
{
    mpimpl->bags_to_eat(val);
}

void
DiscreteTimeDyn::global_output_nils(bool val)
{
    mpimpl->global_output_nils(val);
}

double
DiscreteTimeDyn::time_step() const
{
    return mpimpl->time_step();
}


unsigned int
DiscreteTimeDyn::dim(const Vect& v) const
{
    return mpimpl->dim(v);
}

bool
DiscreteTimeDyn::firstCompute() const
{
    return mpimpl->firstCompute();
}

DEVS_Options&
DiscreteTimeDyn::getOptions()
{
    return mpimpl->getOptions();
}

void
DiscreteTimeDyn::outputVar(const vle::vpz::AtomicModel& model,
                           const vle::devs::Time& time,
                           vle::devs::ExternalEventList& output)
{
    mpimpl->outputVar(model, time, output);
}

void
DiscreteTimeDyn::outputVar(const vle::devs::Time& time,
        vle::devs::ExternalEventList& output) const
{
     mpimpl->outputVar(getModel(), time, output);
}

 void
DiscreteTimeDyn::updateGuardAllSynchronized(const vle::devs::Time& t)
{
     mpimpl->updateGuardAllSynchronized(t);
}

void
DiscreteTimeDyn::updateGuardHasSync(const vle::devs::Time& t)
{
     mpimpl->updateGuardHasSync(t);
}

void
DiscreteTimeDyn::varOnSyncError(std::string& v)
{
    mpimpl->varOnSyncError(v);
}

bool
DiscreteTimeDyn::isSync(const std::string& var_name,
        unsigned int currTimeStep) const
{
    return mpimpl->isSync(var_name, currTimeStep);
}

void
DiscreteTimeDyn::initializeFromInitEventList(
        const vle::devs::InitEventList&  events)
{
     mpimpl->initializeFromInitEventList(events);
}

vle::devs::Time
DiscreteTimeDyn::init(vle::devs::Time t)
{
    return mpimpl->init(this, t);
}

vle::devs::Time
DiscreteTimeDyn::timeAdvance() const
{
    return mpimpl->timeAdvance();
}

 void
DiscreteTimeDyn::internalTransition(vle::devs::Time t)
{
     mpimpl->internalTransition(t);
}

 void
DiscreteTimeDyn::externalTransition(
    const vle::devs::ExternalEventList& event,
    vle::devs::Time t)
{
     mpimpl->externalTransition(event, t);
}

 void
DiscreteTimeDyn::confluentTransitions(
    vle::devs::Time t,
    const vle::devs::ExternalEventList& event)
{
     mpimpl->confluentTransitions(t, event);
}


 void
DiscreteTimeDyn::output(vle::devs::Time time,
                        vle::devs::ExternalEventList& output) const
{
     mpimpl->output(getModel(), time, output);
}

std::unique_ptr<vle::value::Value>
DiscreteTimeDyn::observation(const vle::devs::ObservationEvent& event) const
{
     return mpimpl->observation(event);
}

void
DiscreteTimeDyn::processIn(const vle::devs::Time& t,
        DEVS_TransitionType trans)
{
     mpimpl->processIn( t, trans);
}

void
DiscreteTimeDyn::processOut(const vle::devs::Time& t,
        DEVS_TransitionType trans)
{
     mpimpl->processOut(t, trans);
}

void
DiscreteTimeDyn::updateGuards(const vle::devs::Time& t,
        DEVS_TransitionType trans)
{
     mpimpl->updateGuards(t, trans);
}

void
DiscreteTimeDyn::handleExtEvt(const vle::devs::Time& t,
        const vle::devs::ExternalEventList& ext)
{
     mpimpl->handleExtEvt(t, ext);
}

void
DiscreteTimeDyn::handleExtVar(const vle::devs::Time& t,
        const std::string& port, const vle::value::Value& attrs)
{
     mpimpl->handleExtVar(t, port, attrs);
}


}} // namespace
