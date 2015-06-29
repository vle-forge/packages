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
#include <vle/discrete-time/details/DiscreteTimeGen.hpp>
#include <vle/value/Tuple.hpp>
#include <vle/devs/Executive.hpp>

namespace vle {
namespace discrete_time {

namespace vu = vle::utils;
namespace vz = vle::vpz;


Pimpl::Pimpl(TemporalValuesProvider& tempvp,
        const vle::devs::InitEventList&  events):
                tvp(tempvp), devs_state(INIT), devs_options(), devs_guards(),
                devs_internal(), mfirstCompute(true),
                declarationOn(true), currentTimeStep(0)
{
    initializeFromInitEventList(events);
}

void
Pimpl::time_step(double val)
{
    if (devs_internal.initialized) {
        throw vu::ModellingError(
                vle::fmt("[%1%] Error time_step can be set only in the "
                        "constructor '\n") % tvp.get_model_name());
    }
    devs_options.dt = val;
}

void
Pimpl::init_value(const std::string& v, const vle::value::Value& val)
{
    if (devs_internal.initialized) {
        throw vu::ModellingError(
                vle::fmt("[%1%] Error init_value can be used only in the "
                        "constructor '\n") % tvp.get_model_name());
    }
    Variables::iterator itf = tvp.getVariables().find(v);
    if (itf == tvp.getVariables().end()) {
        throw vu::ModellingError(
                vle::fmt("[%1%] Cannot initialize variable '%2%' because "
                        "it is not found '\n") % tvp.get_model_name() % v);
    }
    delete itf->second->init_value;
    itf->second->init_value = val.clone();
}

void
Pimpl::dim(const std::string& v, unsigned int val)
{
    if (devs_internal.initialized) {
        throw vu::ModellingError(
                vle::fmt("[%1%] Error dim can be used only in the "
                        "constructor '\n") % tvp.get_model_name());
    }
    Variables::iterator itf = tvp.getVariables().find(v);
    if (itf == tvp.getVariables().end()) {
        throw vu::ModellingError(
                vle::fmt("[%1%] Cannot initialize dim of variable '%2%' "
                        "because it is not found '\n")
        % tvp.get_model_name() % v);
    }
    if(! itf->second->isVarMulti()) {
        throw vu::ModellingError(
                vle::fmt("[%1%] Cannot initialize dim of variable '%2%' "
                        "because it is not a vect '\n")
        % tvp.get_model_name() % v);
    }
    itf->second->toVarMulti().dim = val;
}

void
Pimpl::history_size(const std::string& v, unsigned int val)
{
    if (devs_internal.initialized) {
        throw vu::ModellingError(
                vle::fmt("[%1%] Error history_size can be used only in the "
                        "constructor '\n") % tvp.get_model_name());
    }
    Variables::iterator itf = tvp.getVariables().find(v);
    if (itf == tvp.getVariables().end()) {
        throw vu::ModellingError(
                vle::fmt("[%1%] Cannot initialize history_size of variable "
                        "'%2%' because it is not found '\n")
        % tvp.get_model_name() % v);
    }
    itf->second->history_size = val;
}

void
Pimpl::sync(const std::string& v, unsigned int val)
{
//    if (devs_internal.initialized) {
//        throw vu::ModellingError(
//                vle::fmt("[%1%] Error sync can be used only in the "
//                        "constructor '\n") % tvp.get_model_name());
//    }
    devs_options.syncs.insert(std::make_pair(v,val));
}

void
Pimpl::output_nil(const std::string& v, bool val)
{
    if (devs_internal.initialized) {
        throw vu::ModellingError(
                vle::fmt("[%1%] Error output_nil can be used only in the "
                        "constructor '\n") % tvp.get_model_name());
    }
    devs_options.outputNils.insert(std::make_pair(v,val));
}

void
Pimpl::output_period(const std::string& v, unsigned int val)
{
    if (devs_internal.initialized) {
        throw vu::ModellingError(
                vle::fmt("[%1%] Error output_period can be used only in the "
                        "constructor '\n") % tvp.get_model_name());
    }
    devs_options.outputPeriods.insert(std::make_pair(v,val));
}

void
Pimpl::allow_update(const std::string& v, bool val)
{
    if (devs_internal.initialized) {
        throw vu::ModellingError(
                vle::fmt("[%1%] Error allow_update can be used only in the "
                        "constructor '\n") % tvp.get_model_name());
    }
    Variables::iterator itf = tvp.getVariables().find(v);
    if (itf == tvp.getVariables().end()) {
        throw vu::ModellingError(
                vle::fmt("[%1%] Cannot initialize allow_update of variable "
                        "'%2%' because it is not found '\n")
        % tvp.get_model_name() % v);
    }
    devs_options.addAllowUpdate(val, v);
}

void
Pimpl::error_no_sync(const std::string& v, bool val)
{
    if (devs_internal.initialized) {
        throw vu::ModellingError(
                vle::fmt("[%1%] Error error_no_sync can be used only in the "
                        "constructor '\n") % tvp.get_model_name());
    }
    Variables::iterator itf = tvp.getVariables().find(v);
    if (itf == tvp.getVariables().end()) {
        throw vu::ModellingError(
                vle::fmt("[%1%] Cannot initialize error_no_sync of variable "
                        "'%2%' because it is not found '\n")
        % tvp.get_model_name() % v);
    }
    itf->second->error_no_sync = val;
}

void
Pimpl::bags_to_eat(unsigned int val)
{
    if (devs_internal.initialized) {
        throw vu::ModellingError(
                vle::fmt("[%1%] Error bags_to_eat can be used only in the "
                        "constructor '\n") % tvp.get_model_name());
    }
    devs_options.bags_to_eat= val;
}

void
Pimpl::global_output_nils(bool val)
{
    devs_options.setGlobalOutputNils(val);
}

unsigned int
Pimpl::dim(const Vect& v) const
{
    return v.itVar->dim;
}

bool
Pimpl::firstCompute() const
{
    return mfirstCompute;
}

DEVS_Options&
Pimpl::getOptions()
{
    return devs_options;
}

void
Pimpl::outputVar(const vle::vpz::AtomicModel& model, const vle::devs::Time& time,
        vle::devs::ExternalEventList& output) const
{
    Variables::const_iterator itb = tvp.getVariables().begin();
    Variables::const_iterator ite = tvp.getVariables().end();
    for (; itb!=ite; itb++) {
        const std::string& var_name = itb->first;
        VarInterface* v = itb->second;
        vle::value::Value* fe = devs_options.getForcingEvent(time, true,
                var_name);
        if (fe) {
            v->update(time, *fe);
            delete fe;
        }
        if (model.existOutputPort(var_name) &&
                devs_options.shouldOutput(this->currentTimeStep, var_name)) {
            vle::devs::ExternalEvent* e =
                    new vle::devs::ExternalEvent(var_name);
            if (devs_options.shouldOutputNil(v->lastUpdateTime(),
                    time, var_name)) {
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
                    vle::value::Tuple::iterator itb =
                            extTuple->value().begin();
                    vle::value::Tuple::iterator ite =
                            extTuple->value().end();
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
        fe = devs_options.getForcingEvent(time, false, var_name);
        if (fe) {
            v->update(time, *fe);
            delete fe;
        }
    }
}

void
Pimpl::updateGuardAllSynchronized(const vle::devs::Time& t)
{
    Variables::iterator itb = tvp.getVariables().begin();
    Variables::iterator ite = tvp.getVariables().end();
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
Pimpl::updateGuardHasSync(const vle::devs::Time& /*t*/)
{
    Variables::iterator itb = tvp.getVariables().begin();
    Variables::iterator ite = tvp.getVariables().end();
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
Pimpl::varOnSyncError(std::string& v)
{
    Variables::iterator itb = tvp.getVariables().begin();
    Variables::iterator ite = tvp.getVariables().end();
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
Pimpl::isSync(const std::string& var_name,
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
void
Pimpl::initializeFromInitEventList(
        const vle::devs::InitEventList&  events)
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
            devs_options.setGlobalOutputNils(itb->second->toBoolean().value());
        } else if (event_name == "output_period") {
            devs_options.setGlobalOutputPeriods(tvp.get_model_name(),
                    itb->second->toInteger().value());
        } else if (event_name == "snapshot_before") {
            devs_options.snapshot_before = itb->second->toBoolean().value();
        } else if (event_name == "snapshot_after") {
            devs_options.snapshot_after = itb->second->toBoolean().value();
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
        } else if (!prefix.assign("forcing_").empty() and
                !event_name.compare(0, prefix.size(), prefix)) {
            var_name.assign(event_name.substr(prefix.size(),
                    event_name.size()));
            std::string tmpEvt = "allow_update_";
            tmpEvt += var_name;
            if (events.exist(tmpEvt)) {
                throw vle::utils::ArgError(vle::fmt("[%1%] forcing_%2% "
                        "parameter cannot be used with allow_update_%2%")
                % tvp.get_model_name() % var_name);
            }
            devs_options.addForcingEvents(tvp.get_model_name(),
                    *itb->second, var_name);
        }
    }
}


vle::devs::Time
Pimpl::init(ComputeInterface* atom, const vle::devs::Time& t)
{
    devs_options.finishInitialization(tvp.getVariables());
    devs_internal.initialized = true;
    devs_state = INIT;
    processIn(atom, t, INTERNAL);
    return timeAdvance();
}

vle::devs::Time
Pimpl::timeAdvance() const
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
Pimpl::internalTransition(ComputeInterface* atom, const vle::devs::Time& t)
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
        % tvp.get_model_name() % varError);
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
    processIn(atom, t, INTERNAL);
}

void
Pimpl::externalTransition(ComputeInterface* atom,
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
                % tvp.get_model_name() % varError);
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
    processIn(atom, t, EXTERNAL);
}

void
Pimpl::confluentTransitions(ComputeInterface* atom,
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
        % tvp.get_model_name() % varError);
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
    processIn(atom, t, EXTERNAL);
}


void
Pimpl::output(const vle::vpz::AtomicModel& model,
        const vle::devs::Time& time,
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
        outputVar(model, time, output);
        break;
    }
}

vle::value::Value*
Pimpl::observation(
        const vle::devs::ObservationEvent& event) const
{
    const std::string& port = event.getPortName();
    Variables::const_iterator itf = tvp.getVariables().find(port);
    //identify variable name and observation type
    std::string var;
    bool snapshot = false;
    SNAPSHOT_ID snap = SNAP1;
    if (itf != tvp.getVariables().end()) {
        var.assign(port);
        snapshot = false;
    } else {
        std::size_t found = port.find("_before");
        if (found != std::string::npos) {
            itf = tvp.getVariables().find(port.substr(0, found));
            if (itf != tvp.getVariables().end()) {
                var.assign(port.substr(0, found));
                snapshot = true;
                snap = SNAP1;
            }
        } else {
            std::size_t found = port.find("_after");
            if (found != std::string::npos) {
                itf = tvp.getVariables().find(port.substr(0, found));
                if (itf != tvp.getVariables().end()) {
                    var.assign(port.substr(0, found));
                    snapshot = true;
                    snap = SNAP2;
                }
            }
        }
    }
    //build observation
    if (not var.empty()) {
        VarInterface* v = itf->second;
        switch (v->getType()) {
        case MONO: {
            VarMono* vmono = static_cast < VarMono* >(v);
            if (! snapshot) {
                return new vle::value::Double(
                        vmono->getVal(event.getTime(),0));
            } else if (vmono->hasSnapshot(snap)) {
                return new vle::value::Double(vmono->getSnapshot(snap));
            }
            break;
        } case MULTI: {
            VarMulti* vmulti = static_cast < VarMulti* >(v);
            vle::value::Tuple* res = new  vle::value::Tuple(vmulti->dim);
            if (! snapshot) {
                const std::vector<double>& v =
                        vmulti->getVal(event.getTime(), 0);
                for (unsigned int i=0; i < vmulti->dim; i++) {
                    (*res)[i] = v[i];
                }
                return res;
            } else if (vmulti->hasSnapshot(snap)) {
                const std::vector<double>& v = vmulti->getSnapshot(snap);
                for (unsigned int i=0; i < vmulti->dim; i++) {
                    (*res)[i] = v[i];
                }
                return res;
            }
            delete res;
            break;
        } case VALUE_VLE: {
            VarValue* vvalue = static_cast < VarValue* >(v);
            if (! snapshot) {
                vvalue->getVal(event.getTime(),0).clone();
            } else if (vvalue->hasSnapshot(snap)) {
                const vle::value::Value& v = vvalue->getSnapshot(snap);
                return v.clone();
            }
            break;
        }}
    }
    return 0;
}



void
Pimpl::processIn(ComputeInterface* atom, const vle::devs::Time& t,
        DEVS_TransitionType /*trans*/)
{
    switch (devs_state) {
    case INIT:
        declarationOn = false;
        tvp.setCurrentTime(t);
        tvp.initHistory(t);
        if (devs_options.snapshot_after) {
            tvp.snapshot(SNAP2);
        }
        break;
    case WAIT:
        break;
    case WAIT_SYNC:
        break;
    case WAIT_BAGS:
        break;
    case COMPUTE:
        currentTimeStep ++;
        tvp.setCurrentTime(t);
        if (devs_options.snapshot_before) {
            tvp.snapshot(SNAP1);
        }
        atom->compute(t);
        mfirstCompute=false;
        if (devs_options.snapshot_after) {
            tvp.snapshot(SNAP2);
        }
        break;
    }
}

void
Pimpl::processOut(const vle::devs::Time& t,
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
Pimpl::updateGuards(const vle::devs::Time& t,
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
Pimpl::handleExtEvt(const vle::devs::Time& t,
        const vle::devs::ExternalEventList& ext)
{
    vle::devs::ExternalEventList::const_iterator itb = ext.begin();
    vle::devs::ExternalEventList::const_iterator ite = ext.end();
    for (; itb != ite; itb++) {
        handleExtEvt(t, (*itb)->getPortName(), (*itb)->attributes());
    }
}

void
Pimpl::handleExtEvt(const vle::devs::Time& t,
        const std::string& port, const vle::value::Map& attrs)
{
    Variables::iterator it = tvp.getVariables().find(port);
    if(it == tvp.getVariables().end()){
        throw vu::InternalError(
                vle::fmt("[%1%] Unrecognised port '%2%' "
                        "which does not match a variable \n")
        % tvp.get_model_name() % port);
    }
    VarInterface* var = it->second;
    if (attrs.exist("value")) {
        const vle::value::Value& varValue = *attrs.get("value");
        var->update(t,varValue);
    }
}

DEVS_TransitionGuards::DEVS_TransitionGuards():
         has_sync(false), all_synchronized(false), bags_to_eat_eq_0(true),
         bags_eaten_eq_bags_to_eat(true), LWUt_sup_NCt(false),
         LWUt_eq_NCt(false)
{
}

DEVS_Options::DEVS_Options():
        bags_to_eat(0), dt(1.0), syncs(), outputPeriods(), outputNils(),
        forcingEvents(0), allowUpdates(0), outputPeriodsGlobal(0),
        outputNilsGlobal(0), snapshot_before(false), snapshot_after(false)
{
}

DEVS_Options::~DEVS_Options()
{
    delete outputNilsGlobal;
    delete outputPeriodsGlobal;
    delete forcingEvents;
    delete allowUpdates;
}

void
DEVS_Options::setGlobalOutputNils(bool nil)
{
    delete outputNilsGlobal;
    outputNilsGlobal = new vle::value::Boolean(nil);
}

void
DEVS_Options::setGlobalOutputPeriods(
        const std::string& dtd, int period)
{
    if (period <= 0) {
        throw vle::utils::ArgError(vle::fmt("[%1%] output_period parameter "
                "must be positive") % dtd);
    }
    delete outputPeriodsGlobal;
    outputPeriodsGlobal = new vle::value::Integer(period);
}

void
DEVS_Options::addForcingEvents(const std::string& dtd,
        const vle::value::Value& fe, const std::string& varname)
{
    switch (fe.getType()) {
    case vle::value::Value::MAP: {
        if (forcingEvents == 0) {
            forcingEvents = new ForcingEvents();
        }
        vle::value::Set& fevents = (*forcingEvents)[varname];
        fevents.add(fe.toMap().clone());
        break;
    } case vle::value::Value::SET: {
        if (forcingEvents == 0) {
            forcingEvents = new ForcingEvents();
        }
        vle::value::Set& fevents = (*forcingEvents)[varname];
        fevents.clear();
        vle::value::Set::const_iterator itb = fe.toSet().begin();
        vle::value::Set::const_iterator ite = fe.toSet().end();
        for (; itb != ite; itb++) {
            fevents.add((*itb)->clone());
        }
        break;
    } default: {
        throw vle::utils::ArgError(vle::fmt("[%1%] forcing_%2% parameter "
                "must be a vle::value::Set or a vle::value::Map")
            % dtd % varname);
        break;
    }}
    addAllowUpdate(true, varname);
}

void
DEVS_Options::addAllowUpdate(bool allowUpdate,  const std::string& varname)
{
    if (allowUpdates == 0) {
        allowUpdates = new AllowUpdates();
    }
    (*allowUpdates)[varname] = allowUpdate;
}

bool
DEVS_Options::shouldOutput(
        unsigned int currentTimeStep, const std::string& varname) const
{
    OutputPeriods::const_iterator itf = outputPeriods.find(varname);
    if (itf == outputPeriods.end()) {
        return true;
    }
    return ((currentTimeStep % itf->second) == 0);
}

bool
DEVS_Options::shouldOutputNil(double lastUpdateTime,
        double currentTime, const std::string& varname) const
{
    if (!(lastUpdateTime < currentTime)) return false;
    OutputNils::const_iterator itf = outputNils.find(varname);
    return (itf != outputNils.end() && itf->second);
}

vle::value::Value*
DEVS_Options::getForcingEvent(double currentTime, bool beforeCompute,
        const std::string& varname) const
{
    if (forcingEvents == 0){
        return 0;
    }
    ForcingEvents::const_iterator itf = forcingEvents->find(varname);
    if (itf ==  forcingEvents->end()) {
        return 0;
    }
    vle::value::Set::const_iterator itb = itf->second.begin();
    vle::value::Set::const_iterator ite = itf->second.end();
    for (; itb != ite; itb++) {
        const vle::value::Map& m = (*itb)->toMap();
        //TODO manage double errors
        if (std::abs(m.getDouble("time") - currentTime) < dt/std::pow(10,9)) {
            if (m.exist("before_output") and
                    (m.getBoolean("before_output") == beforeCompute)) {
                return m.get("value")->clone();
            }
            if (! beforeCompute) {
                return m.get("value")->clone();
            }
        }
    }
    return 0;
}

void
DEVS_Options::finishInitialization(Variables& vars)
{
    if (outputPeriodsGlobal) {
        Variables::const_iterator itb = vars.begin();
        Variables::const_iterator ite = vars.end();
        for (;itb != ite; itb++) {
            if (outputPeriods.find(itb->first) != outputPeriods.end()) {
                outputPeriods.insert(std::pair<std::string, unsigned int>(
                        itb->first,
                        (unsigned int) outputPeriodsGlobal->value()));
            }
        }
    }
    if (outputNilsGlobal) {
        Variables::const_iterator itb = vars.begin();
        Variables::const_iterator ite = vars.end();
        for (;itb != ite; itb++) {
            if (outputNils.find(itb->first) != outputNils.end()) {
                outputNils.insert(std::pair<std::string, bool>(
                        itb->first, outputNilsGlobal->value()));
            }
        }
    }
    if (allowUpdates) {
        AllowUpdates::const_iterator itb = allowUpdates->begin();
        AllowUpdates::const_iterator ite = allowUpdates->end();
        for (;itb != ite; itb++) {
            Variables::iterator itf = vars.find(itb->first);
            if (itf != vars.end()) {
                itf->second->allow_update = itb->second;
            }
        }
    }
}

}} // namespace
