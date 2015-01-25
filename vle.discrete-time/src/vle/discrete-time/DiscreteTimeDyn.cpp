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


#include <vle/discrete-time/TemporalValues.hpp>
#include <vle/discrete-time/DiscreteTimeDyn.hpp>
#include <vle/value/Tuple.hpp>

namespace vle {
namespace discrete_time {


namespace vu = vle::utils;
namespace vz = vle::vpz;

DiscreteTimeDyn::DEVS_TransitionGuards::DEVS_TransitionGuards():
         has_sync(false), all_synchronized(false), bags_to_eat_eq_0(true),
         bags_eaten_eq_bags_to_eat(true), LWUt_sup_NCt(false),
         LWUt_eq_NCt(false)
{
}

DiscreteTimeDyn::DEVS_Options::DEVS_Options():
        bags_to_eat(0), dt(1.0), syncs(), outputPeriods(), outputNils(),
        outputPeriodsGlobal(0), outputNilsGlobal(0)
{
}

DiscreteTimeDyn::DEVS_Options::~DEVS_Options()
{
    delete outputNilsGlobal;
    delete outputPeriodsGlobal;
}

void
DiscreteTimeDyn::DEVS_Options::setGlobalOutputNils(
        const DiscreteTimeDyn& /*dtd*/, bool nil)
{
    delete outputNilsGlobal;
    outputNilsGlobal = new vle::value::Boolean(nil);
}

void
DiscreteTimeDyn::DEVS_Options::setGlobalOutputPeriods(
        const DiscreteTimeDyn& dtd, int period)
{
    if (period <= 0) {
        throw vle::utils::ArgError(vle::fmt("[%1%] output_period parameter "
                "must be positive") % dtd.getModelName());
    }
    delete outputPeriodsGlobal;
    outputPeriodsGlobal = new vle::value::Integer(period);
}

void
DiscreteTimeDyn::DEVS_Options::finishInitialization(
        const DiscreteTimeDyn& dtd)
{
    if (outputPeriodsGlobal) {
        Variables::const_iterator itb = dtd.getVariables().begin();
        Variables::const_iterator ite = dtd.getVariables().end();
        for (;itb != ite; itb++) {
            if (outputPeriods.find(itb->first) != outputPeriods.end()) {
                outputPeriods.insert(std::pair<std::string, unsigned int>(
                        itb->first,
                        (unsigned int) outputPeriodsGlobal->value()));
            }
        }
    }
    if (outputNilsGlobal) {
        Variables::const_iterator itb = dtd.getVariables().begin();
        Variables::const_iterator ite = dtd.getVariables().end();
        for (;itb != ite; itb++) {
            if (outputNils.find(itb->first) != outputNils.end()) {
                outputNils.insert(std::pair<std::string, bool>(
                        itb->first, outputNilsGlobal->value()));
            }
        }
    }
}

bool
DiscreteTimeDyn::DEVS_Options::shouldOutput(
        const DiscreteTimeDyn& dtd, const std::string& varname) const
{
    OutputPeriods::const_iterator itf = outputPeriods.find(varname);
    if (itf == outputPeriods.end()) {
        return true;
    }
    return ((dtd.currentTimeStep % itf->second) == 0);
}

bool
DiscreteTimeDyn::DEVS_Options::shouldOutputNil(const DiscreteTimeDyn& /*dtd*/,
        double lastUpdateTime,
        double currentTime,
        const std::string& varname) const
{
    if (!(lastUpdateTime < currentTime)) return false;
    OutputNils::const_iterator itf = outputNils.find(varname);
    return (itf != outputNils.end() && itf->second);
}

DiscreteTimeDyn::DiscreteTimeDyn(const vle::devs::DynamicsInit& model,
    const vle::devs::InitEventList& events): vle::devs::Dynamics(model, events),
    TemporalValuesProvider(getModelName(), events), devs_state(INIT),
    devs_options(), devs_guards(), devs_internal(), mfirstCompute(true),
    declarationOn(true), currentTimeStep(0)
{
    vle::devs::InitEventList::const_iterator itb = events.begin();
    vle::devs::InitEventList::const_iterator ite = events.end();
    std::string prefix;
    std::string var_name;
    //first init
    for (; itb != ite; itb++) {
        const std::string& event_name = itb->first;
        if (event_name == "bags_to_eat") {
            devs_options.bags_to_eat = itb->second->toInteger().value();
        } else if (event_name == "time_step") {
            devs_options.dt = itb->second->toDouble().value();
        } else if (event_name == "syncs") {
            const vle::value::Set& syncs = itb->second->toSet();
            vle::value::Set::const_iterator isb = syncs.begin();
            vle::value::Set::const_iterator ise = syncs.end();
            for (; isb != ise; isb++) {
                devs_options.syncs.insert(
                        std::make_pair((*isb)->toString().value(), true));
            }
        } else if (event_name == "output_nil") {
            devs_options.setGlobalOutputNils(*this,
                    itb->second->toBoolean().value());
        } else if (event_name == "output_period") {
            devs_options.setGlobalOutputPeriods(*this,
                    itb->second->toInteger().value());
        }
    }
    //2nd init (prior)
    itb = events.begin();
    for (; itb != ite; itb++) {
        const std::string& event_name = itb->first;
        if (!prefix.assign("sync_").empty() and
                !event_name.compare(0, prefix.size(), prefix)) {
            var_name.assign(event_name.substr(prefix.size(),
                    event_name.size()));
            int sync;
            if (itb->second->isInteger()) {
                sync = itb->second->toInteger().value();
            } else {
                if (itb->second->toBoolean().value()) {
                    sync = 1;
                } else {
                    sync = 0;
                }
            }
            devs_options.syncs.insert(std::make_pair(var_name, sync));
        } else if (!prefix.assign("output_nil_").empty() and
                !event_name.compare(0, prefix.size(), prefix)) {
            var_name.assign(event_name.substr(prefix.size(),
                    event_name.size()));
            devs_options.outputNils.insert(std::pair<std::string, bool>(
                    var_name, itb->second->toBoolean().value()));
        } else if (!prefix.assign("output_period_").empty() and
                !event_name.compare(0, prefix.size(), prefix)) {
            var_name.assign(event_name.substr(prefix.size(),
                    event_name.size()));
            devs_options.outputPeriods.insert(std::pair<std::string, bool>(
                    var_name, itb->second->toInteger().value()));
        }
    }
}

DiscreteTimeDyn::~DiscreteTimeDyn()
{
}

bool DiscreteTimeDyn::firstCompute() const
{
    return mfirstCompute;
}

void
DiscreteTimeDyn::outputVar(const vle::devs::Time& time,
        vle::devs::ExternalEventList& output) const
{
    Variables::const_iterator itb = getVariables().begin();
    Variables::const_iterator ite = getVariables().end();
    for (; itb!=ite; itb++) {
        const std::string& var_name = itb->first;
        if (getModel().existOutputPort(var_name) &&
                devs_options.shouldOutput(*this, var_name)) {
            vle::devs::ExternalEvent* e =
                    new vle::devs::ExternalEvent(var_name);
            VarInterface* v = itb->second;
            if (devs_options.shouldOutputNil(
                    *this, v->lastUpdateTime(), time, var_name)) {
                e->putAttribute("value", new vle::value::Null);
            } else {
                switch (v->getType()) {
                case MONO:{
                    VarMono* vmono = static_cast < VarMono* >(v);
                    e->putAttribute("value",
                            new vle::value::Double(vmono->getVal(time,0)));
                    break;
                } case MULTI: {
                    VarMulti* vmulti = static_cast < VarMulti* >(v);
                    vle::value::Tuple* extTuple =
                            new vle::value::Tuple(vmulti->dim);
                    vle::value::Tuple::iterator itb = extTuple->value().begin();
                    vle::value::Tuple::iterator ite = extTuple->value().end();
                    std::vector<double>::const_iterator itval =
                            vmulti->getVal(time,0).begin();
                    for (; itb!=ite; itb++, itval++) {
                        *itb = *itval;
                    }
                    e->putAttribute("value", extTuple);
                    break;
                } case VALUE_VLE: {
                    VarValue* vval = static_cast < VarValue* >(v);
                    e->putAttribute("value", vval->getVal(time,0).clone());
                    break;
                }}
            }
            output.push_back(e);
        }
    }
}

void
DiscreteTimeDyn::updateGuardAllSynchronized(const vle::devs::Time& t)
{
    Variables::iterator itb = getVariables().begin();
    Variables::iterator ite = getVariables().end();
    for (;itb!=ite;itb++) {
        const std::string& var_name = itb->first;
        if (isSync(var_name, currentTimeStep+1)) {
            if (itb->second->lastUpdateTime() < t) {
                devs_guards.all_synchronized = false;
                return ;
            }
        }
    }
    devs_guards.all_synchronized = true;
}

void
DiscreteTimeDyn::updateGuardHasSync(const vle::devs::Time& /*t*/)
{
    Variables::iterator itb = getVariables().begin();
    Variables::iterator ite = getVariables().end();
    for (;itb!=ite;itb++) {
        const std::string& var_name = itb->first;
        if (isSync(var_name, currentTimeStep+1)) {
            devs_guards.has_sync = true;
            return;
        }
    }
    devs_guards.has_sync = false;
}

void
DiscreteTimeDyn::varOnSyncError(std::string& v)
{
    Variables::iterator itb = getVariables().begin();
    Variables::iterator ite = getVariables().end();
    for (;itb!=ite;itb++) {
        const std::string& var_name = itb->first;
        if (isSync(var_name, currentTimeStep)) {
            if (itb->second->lastUpdateTime() < devs_internal.NCt) {
                v.assign(var_name);
                return ;
            }
        }
    }
    v.assign("");
}

bool
DiscreteTimeDyn::isSync(const std::string& var_name,
        unsigned int currTimeStep) const
{
    DEVS_Options::SyncsType::const_iterator itf =
            devs_options.syncs.find(var_name);
    DEVS_Options::SyncsType::const_iterator ite = devs_options.syncs.end();
    if ((itf == ite) or (itf->second == 0)) {
        return false;
    }
    return (currTimeStep % itf->second) == 0 ;
}

vle::devs::Time
DiscreteTimeDyn::init(const vle::devs::Time& t)
{
    devs_options.finishInitialization(*this);
    devs_state = INIT;
    processIn(t, INTERNAL);
    return timeAdvance();
}

vle::devs::Time
DiscreteTimeDyn::timeAdvance() const
{
    switch (devs_state) {
    case INIT:
        return 0;
        break;
    case WAIT:
        return devs_internal.NCt - devs_internal.LWUt;
        break;
    case WAIT_SYNC:
        return devs_internal.NCt + devs_options.dt/2.0 - devs_internal.LWUt;
        break;
    case WAIT_BAGS:
        return 0;
        break;
    case COMPUTE:
        return 0;
        break;
    }
    return 0;
}


void
DiscreteTimeDyn::internalTransition(const vle::devs::Time& t)
{
    processOut(t, INTERNAL);
    updateGuards(t, INTERNAL);

    switch (devs_state) {
    case INIT:
        if (devs_guards.has_sync) {
            devs_state = WAIT_SYNC;
        } else {
            devs_state = WAIT;
        }
        break;
    case WAIT:
        if (devs_guards.bags_to_eat_eq_0) {
            devs_state = COMPUTE;
        } else {
            devs_state = WAIT_BAGS;
        }
        break;
    case WAIT_SYNC:
        {
            std::string varError;
            varOnSyncError(varError);
            throw vu::InternalError(
                    vle::fmt("[%1%] Error missing sync: '%2%'\n")
                    % getModelName() % varError);
        }
        break;
    case WAIT_BAGS:
        if (devs_guards.bags_eaten_eq_bags_to_eat) {
            devs_state = COMPUTE;
        } else {
            devs_state = WAIT_BAGS;
        }
        break;
    case COMPUTE:
        if (devs_guards.has_sync) {
            devs_state = WAIT_SYNC;
        } else {
            devs_state = WAIT;
        }
        break;
    }
    processIn(t, INTERNAL);
}

void
DiscreteTimeDyn::externalTransition(
    const vle::devs::ExternalEventList& event,
    const vle::devs::Time& t)
{
    processOut(t, EXTERNAL);
    handleExtEvt(t, event);
    updateGuards(t, EXTERNAL);

    switch (devs_state) {
    case INIT:
        throw vu::InternalError("Error DEVS \n");
        break;
    case WAIT:
        devs_state = WAIT;
        break;
    case WAIT_SYNC:
        if (devs_guards.LWUt_sup_NCt) {
            {
                std::string varError;
                varOnSyncError(varError);
                throw vu::InternalError(
                        vle::fmt("[%1%] Error missing sync: '%2%'\n")
                        % getModelName() % varError);
            }
        } else if (devs_guards.LWUt_eq_NCt and devs_guards.all_synchronized) {
            if (devs_guards.bags_to_eat_eq_0) {
                devs_state = COMPUTE;
            } else {
                devs_state = WAIT_BAGS;
            }
        } else {
            devs_state = WAIT_SYNC;
        }
        break;
    case WAIT_BAGS:
        throw vu::InternalError("Error DEVS \n");
        break;
    case COMPUTE:
        throw vu::InternalError("Error DEVS \n");
        break;
    }
    processIn(t, EXTERNAL);
}

void
DiscreteTimeDyn::confluentTransitions(
    const vle::devs::Time& t,
    const vle::devs::ExternalEventList& event)
{
    processOut(t, CONFLUENT);
    handleExtEvt(t, event);
    updateGuards(t, CONFLUENT);

    switch (devs_state) {
    case INIT:
        throw vu::InternalError("Error Unhandled error \n");
        break;
    case WAIT:
        if (devs_guards.bags_to_eat_eq_0) {
            devs_state = COMPUTE;
        } else {
            devs_state = WAIT_BAGS;
        }
        break;
    case WAIT_SYNC:
        {
            std::string varError;
            varOnSyncError(varError);
            throw vu::InternalError(
                    vle::fmt("[%1%] Error missing sync: '%2%'\n")
            % getModelName() % varError);
        }
        break;
    case WAIT_BAGS:
        if (devs_guards.bags_eaten_eq_bags_to_eat) {
            devs_state = COMPUTE;
        } else {
            devs_state = WAIT_BAGS;
        }
        break;
    case COMPUTE:
        if (devs_guards.has_sync) {
            devs_state = WAIT_SYNC;
        } else {
            devs_state = WAIT;
        }
        break;
    }
    processIn(t, EXTERNAL);
}


void
DiscreteTimeDyn::output(const vle::devs::Time& time,
                        vle::devs::ExternalEventList& output) const
{
    switch (devs_state) {
    case INIT:
        break;
    case WAIT:
        break;
    case WAIT_SYNC:
        break;
    case WAIT_BAGS:
        break;
    case COMPUTE:
        outputVar(time, output);
        break;
    }
}

vle::value::Value*
DiscreteTimeDyn::observation(const vle::devs::ObservationEvent& event) const
{
    const std::string& port = event.getPortName();
    Variables::const_iterator itf =
            getVariables().find(port);

    if (itf != getVariables().end()) {
        VarInterface* v = itf->second;
        switch (v->getType()) {
        case MONO: {
            VarMono* vmono =
                    static_cast < VarMono* >(v);
            return new vle::value::Double(vmono->snapshot);
            break;
        } case MULTI: {
            VarMulti* vmulti =
                    static_cast < VarMulti* >(v);
            vle::value::Tuple* res = new  vle::value::Tuple(vmulti->dim);
            for (unsigned int i=0; i < vmulti->dim; i++) {
                (*res)[i] = vmulti->snapshot[i];
            }
            return res;
            break;
        } case VALUE_VLE: {
            VarValue* vvalue =
                    static_cast < VarValue* >(v);
                return vvalue->snapshot->clone();
            break;
        }}
    }
    return 0;
}



void
DiscreteTimeDyn::processIn(const vle::devs::Time& t,
        DEVS_TransitionType /*trans*/)
{
    switch (devs_state) {
    case INIT:
        declarationOn = false;
        setCurrentTime(t);
        initHistory(t);
        snapshot();
        break;
    case WAIT:
        break;
    case WAIT_SYNC:
        break;
    case WAIT_BAGS:
        break;
    case COMPUTE:
        currentTimeStep ++;
        setCurrentTime(t);
        compute(t);
        mfirstCompute=false;
        snapshot();
        break;
    }
}

void
DiscreteTimeDyn::processOut(const vle::devs::Time& t,
        DEVS_TransitionType /*trans*/)
{
    switch (devs_state) {
    case INIT:
        devs_internal.LWUt = t;
        devs_internal.NCt = t + devs_options.dt;
        break;
    case WAIT:
        devs_internal.LWUt = t;
        devs_internal.bags_eaten = 0;
        break;
    case WAIT_SYNC:
        devs_internal.LWUt = t;
        devs_internal.bags_eaten = 0;
        break;
    case WAIT_BAGS:
        devs_internal.LWUt = t;
        devs_internal.bags_eaten ++;
        break;
    case COMPUTE:
        devs_internal.LWUt = t;
        devs_internal.NCt = t + devs_options.dt;
        break;
    }
}

void
DiscreteTimeDyn::updateGuards(const vle::devs::Time& t,
        DEVS_TransitionType /*trans*/)
{
    switch (devs_state) {
    case INIT: {
        devs_guards.bags_to_eat_eq_0 = (devs_options.bags_to_eat == 0);
        updateGuardHasSync(t);
        break;
    } case WAIT: {
        break;
    } case WAIT_SYNC: {
        devs_guards.LWUt_eq_NCt = (devs_internal.LWUt == devs_internal.NCt);
        devs_guards.LWUt_sup_NCt = (devs_internal.LWUt > devs_internal.NCt);
        updateGuardAllSynchronized(t);
        break;
    } case WAIT_BAGS: {
        devs_guards.bags_eaten_eq_bags_to_eat =
                (devs_internal.bags_eaten == devs_options.bags_to_eat);
        break;
    } case COMPUTE: {
        updateGuardHasSync(t);
        break;
    }}
}

void
DiscreteTimeDyn::handleExtEvt(const vle::devs::Time& t,
        const vle::devs::ExternalEventList& ext)
{
    vle::devs::ExternalEventList::const_iterator itb = ext.begin();
    vle::devs::ExternalEventList::const_iterator ite = ext.end();
    for (; itb != ite; itb++) {
        handleExtEvt(t, (*itb)->getPortName(), (*itb)->attributes());
    }
}

void
DiscreteTimeDyn::handleExtEvt(const vle::devs::Time& t,
        const std::string& port, const vle::value::Map& attrs)
{
    Variables::iterator it = getVariables().find(port);
    if(it == getVariables().end()){
        throw vu::InternalError(
                vle::fmt("[%1%] Unrecognised port '%2%' "
                        "which does not match a variable \n")
        % getModelName() % port);
    }
    VarInterface* var = it->second;
    if (attrs.exist("value")) {
        const vle::value::Value& varValue = *attrs.get("value");
        var->update(t,varValue);
    }
}

}} // namespace
