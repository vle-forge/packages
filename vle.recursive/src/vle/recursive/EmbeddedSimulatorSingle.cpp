
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

#include <vle/vpz/Vpz.hpp>
#include <vle/utils/Exception.hpp>
#include <vle/utils/Package.hpp>
#include <vle/manager/Simulation.hpp>
#include <vle/value/Double.hpp>

#include <vle/recursive/VleAPIfacilities.hpp>
#include <vle/recursive/EmbeddedSimulatorSingle.hpp>
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


EmbeddedSimulatorSingle::EmbeddedSimulatorSingle() :
                EmbeddedSimulator(), mraw_outputs_single(0), mSingleSimulator(0)
{
}

void EmbeddedSimulatorSingle::init(const vv::Map& initSim)
{
    EmbeddedSimulator::init(initSim);
    mSingleSimulator = new vm::Simulation(vm::LOG_NONE, vm::SIMULATION_NONE,
            NULL);
}

EmbeddedSimulatorSingle::~EmbeddedSimulatorSingle()
{
    delete mraw_outputs_single;
    delete mSingleSimulator;
}

void EmbeddedSimulatorSingle::simulate(const vv::Value& input)
{
    delete mraw_outputs_single;
    clearAllCondPort();
    postOneInput(input);
    mraw_outputs_single = mSingleSimulator->run(new vz::Vpz(mvpzDyn),
            mmodules, &merror);
    if (merror.code != 0) {
        throw vle::utils::InternalError(
                vle::fmt("Error in EmbeddedSimulatorSingle::simulateSingle '%1%'")
        % merror.message);
    }
}

void EmbeddedSimulatorSingle::fillWithLastValuesOfOutputs(vv::Value& tofill) const
{
    if (mraw_outputs_single == 0) {
        throw vle::utils::ArgError(vle::fmt("Error in "
                "EmbeddedSimulatorSingle : Null outputs"));
    }
    switch (tofill.getType()) {
    case vv::Value::DOUBLE: {
        if (moutputs.size() != 1) {
            throw vle::utils::ArgError(vle::fmt("Error in "
                    "EmbeddedSimulatorSingle : moutputs.size ('%1%') != 1")
            % moutputs.size());
        }
        const EmbeddedSimulatorOutput& out = moutputs[0];
        if (!mraw_outputs_single->exist(out.view_name)) {
            throw vle::utils::ArgError(vle::fmt("Error in "
                    "EmbeddedSimulatorSingle : no view") % out.view_name);
        }
        const vv::Matrix& v = mraw_outputs_single->getMatrix(out.view_name);
        bool found = false;
        for (unsigned int j =0; j < v.columns(); j++) {
            if (v.getString(j,0) == out.port_name) {
                vv::ConstVectorView col = v.column(j);
                tofill.toDouble().set(
                      ((const vv::Value*)col[col.size()-1])->toDouble().value());
                found = true;
            }
        }
        if (!found) {
            throw vle::utils::ArgError( vle::fmt("[EmbeddedSimulatorSingle] "
                    "fillWithLastValuesOfOutputs : '%1%'") % out.port_name);
        }
        break;
    }
    case vv::Value::MAP: {
        //TODO
        if (moutputs.size() != 1) {
            throw vle::utils::ArgError(vle::fmt("Error in "
                    "EmbeddedSimulatorSingle1 : moutputs.size ('%1%') != 1")
            % moutputs.size());
        }
        vv::Map& tofillMap = tofill.toMap();
        tofillMap.clear();
        const EmbeddedSimulatorOutput& out = moutputs[0];
        const vv::Value* res = VleAPIfacilities::findLastOutputValue(
                *mraw_outputs_single,out.view_name, out.port_name);
        if (!res->isMap()) {
            throw vle::utils::ArgError(vle::fmt("Error in "
                "EmbeddedSimulatorSingle1 : expect a map, got a '%1%'")
                       % res->getType());
        }
        const vv::Map& resMap = res->toMap();
        vv::Map::const_iterator itb = resMap.begin();
        vv::Map::const_iterator ite = resMap.end();
        for (; itb!=ite; itb++) {
            tofillMap.add(itb->first,itb->second->clone());
        }
        break;
    }
    default : {
        throw vle::utils::ArgError( vle::fmt("[EmbeddedSimulatorSingle] "
                "fillWithLastValuesOfOutputs1 : '%1%'") % tofill.getType());
        break;
    }}
}

unsigned int EmbeddedSimulatorSingle::nbOutputs() const
{
    return moutputs.size();
}

vv::ConstVectorView EmbeddedSimulatorSingle::getTimeCol(
        const std::string& outputVar)
{
    EmbeddedSimulatorOutput out(outputVar);
    const vv::Matrix& v = mraw_outputs_single->getMatrix(out.view_name);
    for (unsigned int i =0; i < v.columns(); i++) {
        if (v.getString(i,0) == out.port_name) {
            return v.column(i);
        }
    }
    throw vle::utils::ArgError( vle::fmt("[EmbeddedSimulatorSingle] "
            "Error in getTimecol with idSim=%1%, outputVar=%2%"));
    return v.column(0);
}

void EmbeddedSimulatorSingle::postOneInput(const vv::Value& input)
{
    vz::Conditions& conds = mvpzDyn.project().experiment().conditions();
    switch (input.getType()) {
    case vv::Value::SET : {
        const vv::Set& set = input.toSet();
        if (minputs.size() != set.size()) {
            throw vu::ArgError(vle::fmt("[EmbeddedSimulatorSingle] : "
                    "wrong input format, got a set of size '%1%', "
                    "and expect a set of size '%2%'")
            % set.size() % minputs.size());
        }
        for (unsigned int i=0; i<minputs.size() ; i++){
            vz::Condition& cond = conds.get(minputs[i].cond_name);
            cond.addValueToPort(minputs[i].port_name,
                    set.get(i)->clone());
        }
        break;
    }
    case vv::Value::TUPLE : {
        const vv::Tuple& tuple = input.toTuple();
        if (minputs.size() != tuple.size()) {
            throw vu::ArgError(vle::fmt("[EmbeddedSimulatorSingle] : "
                    "wrong input format, got a tuple of size '%1%', "
                    "and expect a set of size '%2%'")
            % tuple.size() % minputs.size());
        }
        for (unsigned int i=0; i<minputs.size() ; i++){
            vz::Condition& cond = conds.get(minputs[i].cond_name);
            cond.addValueToPort(minputs[i].port_name,
                    new vv::Double(tuple[i]));
        }
        break;
    }
    case vv::Value::INTEGER : {
        if (minputs.size() != 1) {
            throw vu::ArgError(vle::fmt("[EmbeddedSimulatorSingle] : "
                    "wrong input format, got an integer, "
                    "and expect a set of size '%1%'")
             % minputs.size());
        }
        vz::Condition& cond = conds.get(minputs[0].cond_name);
        cond.addValueToPort(minputs[0].port_name, input.clone());

        break;
    }
    default : {
        throw vu::ArgError(vle::fmt("[EmbeddedSimulatorSingle] : wrong "
                "inputs format (got %1%)") % input.getType());
        break;
    }
    }
}
}}//namespaces
