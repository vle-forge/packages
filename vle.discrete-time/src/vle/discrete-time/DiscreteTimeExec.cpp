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


#include <vle/discrete-time/DiscreteTimeExec.hpp>
#include <vle/value/Tuple.hpp>

namespace vle {
namespace discrete_time {

namespace vu = vle::utils;
namespace vz = vle::vpz;

DiscreteTimeExec::DiscreteTimeExec(
        const vle::devs::ExecutiveInit& model,
        const vle::devs::InitEventList& events):
        vle::devs::Executive(model, events),
        TemporalValuesProvider(this->getModelName(), events),
        mpimpl(new Pimpl(static_cast<TemporalValuesProvider&>(*this), events))

{
}

DiscreteTimeExec::~DiscreteTimeExec()
{
    delete mpimpl;
}

vle::devs::Dynamics*
DiscreteTimeExec::toDynamics()
{
    return (vle::devs::Dynamics*) this;
}

void
DiscreteTimeExec::time_step(double val)
{
    mpimpl->time_step(val);
}

 void
DiscreteTimeExec::init_value(const std::string& v,
        const vle::value::Value& val)
{
     mpimpl->init_value(v, val);
}

 void
DiscreteTimeExec::dim(const std::string& v, unsigned int val)
{
     mpimpl->dim(v, val);
}

 void
DiscreteTimeExec::history_size(const std::string& v, unsigned int val)
{
     mpimpl->history_size(v, val);
}

 void
DiscreteTimeExec::sync(const std::string& v, unsigned int val)
{
     mpimpl->sync(v, val);
}

 void
DiscreteTimeExec::output_nil(const std::string& v, bool val)
{
     mpimpl->output_nil(v, val);
}

 void
DiscreteTimeExec::output_period(const std::string& v, unsigned int val)
{
     mpimpl->output_period(v, val);
}

 void
DiscreteTimeExec::allow_update(const std::string& v, bool val)
{
     mpimpl->allow_update(v, val);
}

 void
DiscreteTimeExec::error_no_sync(const std::string& v, bool val)
{
     mpimpl->error_no_sync(v, val);
}

void
DiscreteTimeExec::bags_to_eat(unsigned int val)
{
    mpimpl->bags_to_eat(val);
}

void
DiscreteTimeExec::global_output_nils(bool val)
{
    mpimpl->global_output_nils(val);
}


unsigned int
DiscreteTimeExec::dim(const Vect& v) const
{
    return mpimpl->dim(v);
}

bool
DiscreteTimeExec::firstCompute() const
{
    return mpimpl->firstCompute();
}

DEVS_Options&
DiscreteTimeExec::getOptions()
{
    return mpimpl->getOptions();
}

 void
DiscreteTimeExec::outputVar(const vle::devs::Time& time,
        vle::devs::ExternalEventList& output) const
{
     mpimpl->outputVar(getModel(), time, output);
}

 void
DiscreteTimeExec::updateGuardAllSynchronized(const vle::devs::Time& t)
{
     mpimpl->updateGuardAllSynchronized(t);
}

void
DiscreteTimeExec::updateGuardHasSync(const vle::devs::Time& t)
{
     mpimpl->updateGuardHasSync(t);
}

void
DiscreteTimeExec::varOnSyncError(std::string& v)
{
    mpimpl->varOnSyncError(v);
}

bool
DiscreteTimeExec::isSync(const std::string& var_name,
        unsigned int currTimeStep) const
{
    return mpimpl->isSync(var_name, currTimeStep);
}

void
DiscreteTimeExec::initializeFromInitEventList(
        const vle::devs::InitEventList&  events)
{
     mpimpl->initializeFromInitEventList(events);
}

vle::devs::Time
DiscreteTimeExec::init(vle::devs::Time t)
{
    return mpimpl->init(this, t);
}

vle::devs::Time
DiscreteTimeExec::timeAdvance() const
{
    return mpimpl->timeAdvance();
}

 void
DiscreteTimeExec::internalTransition(vle::devs::Time t)
{
     mpimpl->internalTransition(t);
}

 void
DiscreteTimeExec::externalTransition(
    const vle::devs::ExternalEventList& event,
    vle::devs::Time t)
{
     mpimpl->externalTransition(event, t);
}

void
DiscreteTimeExec::confluentTransitions(
    vle::devs::Time t,
    const vle::devs::ExternalEventList& event)
{
     mpimpl->confluentTransitions(t, event);
}


 void
DiscreteTimeExec::output(vle::devs::Time time,
                        vle::devs::ExternalEventList& output) const
{
     mpimpl->output(getModel(), time, output);
}

std::unique_ptr<vle::value::Value>
DiscreteTimeExec::observation(const vle::devs::ObservationEvent& event) const
{
     return mpimpl->observation(event);
}

void
DiscreteTimeExec::processIn(const vle::devs::Time& t,
        DEVS_TransitionType trans)
{
     mpimpl->processIn(t, trans);
}

void
DiscreteTimeExec::processOut(const vle::devs::Time& t,
        DEVS_TransitionType trans)
{
     mpimpl->processOut(t, trans);
}

void
DiscreteTimeExec::updateGuards(const vle::devs::Time& t,
        DEVS_TransitionType trans)
{
     mpimpl->updateGuards(t, trans);
}

void
DiscreteTimeExec::handleExtEvt(const vle::devs::Time& t,
        const vle::devs::ExternalEventList& ext)
{
     mpimpl->handleExtEvt(t, ext);
}

void
DiscreteTimeExec::handleExtVar(const vle::devs::Time& t,
        const std::string& port, const vle::value::Map& attrs)
{
     mpimpl->handleExtVar(t, port, attrs);
}


}} // namespace
