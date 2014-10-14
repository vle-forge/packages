
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
#include <vle/utils/Rand.hpp>
#include <vle/manager/Manager.hpp>
#include <vle/value/Double.hpp>

#include <vle/recursive/VleAPIfacilities.hpp>
#include <vle/recursive/EmbeddedSimulatorReplicas.hpp>
#include <vle/recursive/EmbeddedSimulatorInput.hpp>
#include <vle/recursive/EmbeddedSimulatorOutput.hpp>

namespace vle {
namespace recursive {


namespace vu = vle::utils;
namespace vm = vle::manager;
namespace vz = vle::vpz;
namespace vv = vle::value;
namespace vo = vle::oov;

EmbeddedSimulatorReplicas::EmbeddedSimulatorReplicas() :
                EmbeddedSimulator(), mraw_outputs_plan(0), mnbthreads(1),
                minputRand(), mPlanSimulator(0), mrand()
{
}

void EmbeddedSimulatorReplicas::init(const vv::Map& initSim)
{
    EmbeddedSimulator::init(initSim);
    //handle inputsRand
    if(initSim.exist("inputRand")){
        minputRand.init(initSim.getString("inputRand"));
    }
    //handle parameters
    if(initSim.exist("nb-threads")){
        mnbthreads = initSim.getInt("nb-threads");
    }
    mPlanSimulator = new vm::Manager(vm::LOG_NONE, vm::SIMULATION_NONE,
            NULL);
}

EmbeddedSimulatorReplicas::~EmbeddedSimulatorReplicas()
{
    delete mraw_outputs_plan;
    delete mPlanSimulator;
}

void EmbeddedSimulatorReplicas::simulateReplicas(const vv::Value& input, unsigned int nbReplicas,
        bool commonSeed)
{
    delete mraw_outputs_plan;
    clearAllCondPort();
    clearAllCondPortRand();
    for (unsigned int i=0; i < nbReplicas; i++) {
        postInputs(input);
    }
    unsigned int nbcomb = nbComb(input);
    postSeeds(nbcomb, nbReplicas, commonSeed);
    mraw_outputs_plan= mPlanSimulator->run(new vz::Vpz(mvpzDyn), mmodules,
            mnbthreads, 0, 1, &merror);
    if (merror.code != 0) {
        throw vle::utils::InternalError(
                vle::fmt("Error2 in EmbeddedSimulatorReplicas::simulateReplicas '%1%'")
                % merror.message);
    }
}

unsigned int EmbeddedSimulatorReplicas::nbSimus() const
{
    if (mraw_outputs_plan) {
        return mraw_outputs_plan->columns();
    }
    return 0;
}

unsigned int EmbeddedSimulatorReplicas::nbInputs() const
{
    return minputs.size();
}

unsigned int EmbeddedSimulatorReplicas::nbOutputs() const
{
    return moutputs.size();
}

const vv::Map& EmbeddedSimulatorReplicas::getSimulationResults(
        unsigned int idComb, unsigned int idRepl) const
{
    unsigned int idSim = idRepl * idComb + idComb;
    return mraw_outputs_plan->getMap(idSim,0);
}

const vv::Value& EmbeddedSimulatorReplicas::lastValue(unsigned int idComb,
        unsigned int idRepl, unsigned int idOutput) const
{
    const vv::Map& views = getSimulationResults(idComb, idRepl);
    if (moutputs.size() < idOutput-1) {
        throw vle::utils::ArgError( vle::fmt("[EmbeddedSimulatorReplicas] "
                "moutputs.size() < idOutput-1 (%1% < %2%)") %
                moutputs.size() % idOutput);
    }
    const EmbeddedSimulatorOutput& out = moutputs[idOutput-1];
    const vv::Matrix& v = views.getMatrix(out.view_name);
    for (unsigned int i =0; i < v.columns(); i++) {
        if (v.getString(i,0) == out.port_name) {
            vv::ConstVectorView col = v.column(i);
            return *col[col.size()-1];
        }
    }
    throw vle::utils::ArgError( vle::fmt("[EmbeddedSimulatorReplicas] "
            "Error in getTimecol with idSim=%1%, outputVar=%2%"));
}

void EmbeddedSimulatorReplicas::fillWithLastValuesOfOutputs(vv::Set& tofill,
        unsigned int idComb, unsigned int idRepl) const
{
    tofill.clear();
    const vv::Map& views = getSimulationResults(idComb, idRepl);
    for (unsigned int i=0; i<moutputs.size();i++) {
        const EmbeddedSimulatorOutput& out = moutputs[i];
        const vv::Matrix& v = views.getMatrix(out.view_name);
        for (unsigned int j =0; j < v.columns(); j++) {
            if (v.getString(j,0) == out.port_name) {
                vv::ConstVectorView col = v.column(j);
                tofill.add(((const vv::Value*) col[col.size()-1])->clone());
            }
        }
    }
}

void EmbeddedSimulatorReplicas::setNbThreads(unsigned int nbThreads)
{
    mnbthreads = nbThreads;
}

void EmbeddedSimulatorReplicas::clearAllCondPortRand()
{
    if (minputRand.isDefined()) {
        vz::Conditions& conds = mvpzDyn.project().experiment().conditions();
        conds.get(minputRand.cond_name).clearValueOfPort(minputRand.port_name);
    }

}

unsigned int EmbeddedSimulatorReplicas::nbComb(const vv::Value& input) const
{
    switch (input.getType()) {
    case vv::Value::TABLE : {
        const vv::Table& table = input.toTable();
        if (minputs.size() != (unsigned int) table.height()) {
            throw vu::ArgError(vle::fmt("[EmbeddedSimulatorReplicas] : wrong "
                    "input format, got a table with '%1%' rows and expect "
                    " '%2%' inputs") % table.height() % minputs.size());
        }
        return table.width();
        break;
    }
    case vv::Value::SET : {
        const vv::Set& set = input.toSet();
        switch (set.get(0)->getType()) {
        case vv::Value::TUPLE: {
            if ((minputs.size() != 1) &&
                    (minputs.size() != set.getTuple(0).size())) {
                throw vu::ArgError(vle::fmt("[EmbeddedSimulatorReplicas] : wrong "
                        "input format, got a tuple and expect data for %1% "
                        "inputs") % minputs.size());
            }
            return set.size();
            break;
        }
        case vv::Value::SET: {
            for (unsigned int i=0; i<set.size() ; i++){
                const vv::Set& seti = set.getSet(i);
                if (minputs.size() != seti.size()) {
                    throw vu::ArgError(vle::fmt("[EmbeddedSimulatorReplicas] : "
                            "wrong input format, got a set of size '%1%', "
                            "and expect a set of size '%2%'")
                    % seti.size() % minputs.size());
                }
                return set.size();
            }
            break;
        }
        default : {
            throw vu::ArgError(vle::fmt("[EmbeddedSimulatorReplicas] : wrong "
                    "input format (got %1%)") % input);
            break;
        }}//end switch
        break;
    }
    default : {
        throw vu::ArgError(vle::fmt("[EmbeddedSimulatorReplicas] : wrong "
                "format for intput set (got %1%)") % input.getType());
        break;
    }}//end switch
    return 0;
}

void EmbeddedSimulatorReplicas::postSeeds(unsigned int nbComb, unsigned int nbRepl,
        bool commonSeed)
{
    if (minputRand.isDefined()) {
        vz::Conditions& conds = mvpzDyn.project().experiment().conditions();
        vz::Condition& cond = conds.get(minputRand.cond_name);
        const std::string& portname = minputRand.port_name;
        unsigned int seed = 1;
        for (unsigned int i =0; i< nbRepl; i++) {
            for (unsigned int j =0; j< nbComb; j++) {
                if ((i == 0) || (!commonSeed)) {
                    seed = mrand.getInt();
                }
                cond.addValueToPort(portname,new vv::Integer(seed));
            }
        }
    }
}

void EmbeddedSimulatorReplicas::postInputs(const vv::Value& input)
{
    vz::Conditions& conds = mvpzDyn.project().experiment().conditions();
    switch (input.getType()) {
    case vv::Value::TABLE : {
        const vv::Table& table = input.toTable();
        if (minputs.size() != (unsigned int) table.height()) {
            throw vu::ArgError(vle::fmt("[EmbeddedSimulatorReplicas] : wrong "
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
            const vv::Tuple& tuple0 = set.getTuple(0);
            if (minputs.size() == 1) {
                for (unsigned int i=0; i<set.size() ; i++){
                    const vv::Tuple& tuple = set.getTuple(i);
                    vz::Condition& cond = conds.get(minputs[0].cond_name);
                    cond.addValueToPort(minputs[0].port_name,tuple.clone());
                }
            } else if (minputs.size() == tuple0.size()) {
                for (unsigned int i=0; i< set.size() ; i++){
                    const vv::Tuple& tuplei = set.getTuple(i);
                    for (unsigned int j=0; j< minputs.size() ; j++){
                        vz::Condition& condj = conds.get(minputs[j].cond_name);
                        condj.addValueToPort(minputs[j].port_name,
                                new vv::Double(tuplei[j]));
                    }
                }
            } else {
                throw vu::ArgError(vle::fmt("[EmbeddedSimulatorReplicas] : wrong "
                        "input format, got a tuple of size '%1%' and expect "
                        "data for '%0%' inputs") % tuple0.size() % minputs.size());
            }

            break;
        }
        case vv::Value::SET: {
            for (unsigned int i=0; i<set.size() ; i++){
                const vv::Set& seti = set.getSet(i);
                if (minputs.size() != seti.size()) {
                    throw vu::ArgError(vle::fmt("[EmbeddedSimulatorReplicas] : "
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
            throw vu::ArgError(vle::fmt("[EmbeddedSimulatorReplicas] : wrong "
                    "input format (got %1%)") % input);
            break;
        }
        }
        break;
    }
    default : {
        throw vu::ArgError(vle::fmt("[EmbeddedSimulatorReplicas] : wrong set of "
                "inputs format (got %1%)") % input.getType());
        break;
    }
    }
}

}}//namespaces
