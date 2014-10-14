
/*
 * Copyright (C) 2014 INRA
 *
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


#include "EmbeddedSimulatorPlan.hpp"

#include <vle/vpz/Vpz.hpp>
#include <vle/utils/Exception.hpp>
#include <vle/utils/Package.hpp>
#include <vle/manager/Manager.hpp>
#include <vle/value/Double.hpp>

#include <vle/recursive/VleAPIfacilities.hpp>
#include <vle/recursive/EmbeddedSimulator.hpp>
#include <vle/recursive/EmbeddedSimulatorInput.hpp>
#include <vle/recursive/EmbeddedSimulatorOutput.hpp>


//#include <boost/lexical_cast.hpp>

namespace vle {
namespace recursive {


namespace vu = vle::utils;
namespace vm = vle::manager;
namespace vz = vle::vpz;
namespace vv = vle::value;
namespace vo = vle::oov;

EmbeddedSimulatorPlan::EmbeddedSimulatorPlan() :
                EmbeddedSimulator(), mraw_outputs_plan(0), mnbthreads(1),
                mPlanSimulator(0)
{
}

void EmbeddedSimulatorPlan::init(const vv::Map& initSim)
{
    EmbeddedSimulator::init(initSim);
    //handle parameters
    if(initSim.exist("nb-threads")){
        mnbthreads = initSim.getInt("nb-threads");
    }
    mPlanSimulator = new vm::Manager(vm::LOG_NONE, vm::SIMULATION_NONE,
            NULL);
}

EmbeddedSimulatorPlan::~EmbeddedSimulatorPlan()
{
    delete mraw_outputs_plan;
    delete mPlanSimulator;
}

void EmbeddedSimulatorPlan::simulatePlan(const vv::Value& input)
{
    delete mraw_outputs_plan;
    clearAllCondPort();
    postInputs(input);
    mraw_outputs_plan= mPlanSimulator->run(new vz::Vpz(mvpzDyn), mmodules,
            mnbthreads, 0, 1, &merror);
    if (merror.code != 0) {
        throw vle::utils::InternalError(
                vle::fmt("Error in EmbeddedSimulatorPlan::simulatePlan '%1%'")
        % merror.message);
    }
}

unsigned int EmbeddedSimulatorPlan::nbSimus()
{
    if (mraw_outputs_plan) {
        return mraw_outputs_plan->columns();
    }
    return 0;
}

vv::ConstVectorView EmbeddedSimulatorPlan::getTimeCol(unsigned int idSim,
        const std::string& outputVar)
{
    EmbeddedSimulatorOutput out(outputVar);
    const vv::Map& views = mraw_outputs_plan->getMap(idSim,0);
    const vv::Matrix& v = views.getMatrix(out.view_name);
    for (unsigned int i =0; i < v.columns(); i++) {
        if (v.getString(i,0) == out.port_name) {
            return v.column(i);
        }
    }
    throw vle::utils::ArgError( vle::fmt("[EmbeddedSimulatorPlan] "
            "Error in getTimecol with idSim=%1%, outputVar=%2%"));
    return v.column(0);
}

void EmbeddedSimulatorPlan::setNbThreads(unsigned int nbThreads)
{
    mnbthreads = nbThreads;
}

void EmbeddedSimulatorPlan::postInputs(const vv::Value& input)
{
    vz::Conditions& conds = mvpzDyn.project().experiment().conditions();
    switch (input.getType()) {
    case vv::Value::TABLE : {
        const vv::Table& table = input.toTable();
        if (minputs.size() != (unsigned int) table.height()) {
            throw vu::ArgError(vle::fmt("[EmbeddedSimulatorPlan] : wrong "
                    "input format, got a table with '%1%' rows and expect "
                    " '%2%' inputs") % table.height() % minputs.size());
        }
        for (unsigned int i=0; i< table.height() ; i ++) {
            vz::Condition& cond = conds.get(minputs[i].cond_name);
            const std::string& portname = minputs[i].port_name;
            for (unsigned int j=0; j< table.width() ; j ++) {
                cond.addValueToPort(portname,new vv::Double(
                        table.get(j,i)));
            }
        }
        break;
    }
    case vv::Value::SET : {
        const vv::Set& set = input.toSet();
        switch (set.get(0)->getType()) {
        case vv::Value::TUPLE: {
            if (minputs.size() != 1) {
                throw vu::ArgError(vle::fmt("[EmbeddedSimulatorPlan] : wrong "
                        "input format, got a tuple and expect data for %1% "
                        "inputs") % minputs.size());
            }
            for (unsigned int i=0; i<set.size() ; i++){
                const vv::Tuple& tuple = set.getTuple(i);
                vz::Condition& cond = conds.get(minputs[0].cond_name);
                cond.addValueToPort(minputs[0].port_name,tuple.clone());
            }
            break;
        }
        case vv::Value::SET: {
            for (unsigned int i=0; i<set.size() ; i++){
                const vv::Set& seti = set.getSet(i);
                if (minputs.size() != seti.size()) {
                    throw vu::ArgError(vle::fmt("[EmbeddedSimulatorPlan] : "
                            "wrong input format, got a set of size '%1%', "
                            "and expect a set of size '%2%'")
                    % seti.size() % minputs.size());
                }
                for (unsigned int j=0; j<minputs.size() ; j++){
                    vz::Condition& cond = conds.get(minputs[j].cond_name);
                    cond.addValueToPort(minputs[j].port_name,
                            seti.get(j)->clone());
                }
            }
            break;
        }
        default : {
            throw vu::ArgError(vle::fmt("[EmbeddedSimulatorPlan] : wrong "
                    "input format (got %1%)") % input);
            break;
        }
        }
        break;
    }
    default : {
        throw vu::ArgError(vle::fmt("[EmbeddedSimulatorPlan] : wrong set of "
                "inputs format (got %1%)") % input.getType());
        break;
    }
    }
}

unsigned int EmbeddedSimulatorPlan::nbSimus() const
{
    if (mraw_outputs_plan) {
        return mraw_outputs_plan->columns();
    }
    return 0;
}

void EmbeddedSimulatorPlan::fillWithLastValuesOfOutputs(
        vv::Value& tofill) const
{
    switch (tofill.getType()) {
    case vv::Value::TUPLE: {
        if ((moutputs.size() != 1) and (nbSimus() > 1)) {
            throw vu::ArgError(vle::fmt("[EmbeddedSimulatorPlan] : got a tuple "
              " and expect 1 output (got %1%)") % moutputs.size());
        }
        vv::Tuple& tofillTuple = tofill.toTuple();
        tofillTuple.value().resize(nbSimus());
        const EmbeddedSimulatorOutput& out = moutputs[0];
        for (unsigned int i=0; i< nbSimus(); i++) {
            const vv::Value* res = VleAPIfacilities::findLastOutputValue(
                    mraw_outputs_plan->getMap(i,0),
                    out.view_name, out.port_name);
            if (!res->isDouble()) {
                throw vu::ArgError(vle::fmt("[EmbeddedSimulatorPlan] : can "
                     "fill tuples only with Doubles"));
            }
            tofillTuple[i] = res->toDouble().value();
        }
        break;
    }
    default : {
            throw vle::utils::ArgError( vle::fmt("[EmbeddedSimulatorPlan] "
                    "fillWithLastValuesOfOutputs1 : '%1%'") % tofill.getType());
            break;
        }
    }
}

}}//namespaces
