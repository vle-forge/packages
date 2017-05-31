
/*
 * Copyright (C) 2015 INRA
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

#include <iostream>
#include <algorithm>
#include <sstream>
#include <cmath>
#include <string>
#include <thread>

#include <vle/vpz/Vpz.hpp>
#include <vle/utils/Tools.hpp>
#include <vle/utils/Exception.hpp>
#include <vle/utils/Package.hpp>
#include <vle/utils/Spawn.hpp>
#include <vle/manager/Simulation.hpp>
#include <vle/manager/Manager.hpp>
#include <vle/value/Double.hpp>
#include <vle/value/Null.hpp>
#include <vle/value/Tuple.hpp>
#include <vle/value/Table.hpp>

#include <vle/reader/vle_results_text_reader.hpp>

#include "MetaManager.hpp"
#include "VleAPIfacilities.hpp"


namespace vle {
namespace recursive {
/*********** local functions *************/

template <class OBJ_TO_PRINT>
std::string toString(const OBJ_TO_PRINT& o)
{
    std::stringstream ss;
    ss << o;
    return ss.str();
}


utils::Path make_temp(const char* format, const std::string& dir)
{
    //utils::Path tmp = utils::Path::temp_directory_path();
    utils::Path tmp(dir);
    tmp /= utils::Path::unique_path(format);
    return tmp;
}


/***********wrapper_init*************/

wrapper_init::wrapper_init(const devs::InitEventList* init_evt_list):
    mInitEventList(init_evt_list), mMap(0), itbI(),
    itbM(), iteI(), iteM(), def()
{
    itbI =  mInitEventList->begin();
    iteI =  mInitEventList->end();
}

wrapper_init::wrapper_init(const value::Map* init_map):
    mInitEventList(0), mMap(init_map), itbI(),
    itbM(), iteI(), iteM(), def()
{
    itbM =  mMap->begin();
    iteM =  mMap->end();
}

wrapper_init::~wrapper_init()
{
    mInitEventList = 0;
    mMap = 0;
}

void
wrapper_init::begin()
{
    if (mInitEventList) {
        itbI =  mInitEventList->begin();
    } else {
        itbM =  mMap->begin();
    }
}

bool
wrapper_init::isEnded() const
{
    if (mInitEventList and itbI != iteI) {
        return false;
    }
    if (mMap and itbM != iteM) {
        return false;
    }
    return true;
}

bool
wrapper_init::next()
{
    if (mInitEventList and itbI != iteI) {
        itbI++ ;
        return true;
    }
    if (mMap and itbM != iteM) {
        itbM++;
        return true;
    }
    return false;
}

const value::Value&
wrapper_init::current(std::string& key, bool& status)
{
    if (mInitEventList and itbI != iteI) {
        const value::Value& res_val = *itbI->second.get();
        status = true;
        key.assign(itbI->first);
        return res_val;
    }
    if (mMap and itbM != iteM) {
        const value::Value& res_val = *itbM->second.get();
        status = true;
        key.assign(itbM->first);
        return res_val;
    }
    key.assign("");
    status = false;
    return def;
}

bool
wrapper_init::exist(const std::string& key, bool& status) const
{
    if (mInitEventList) {
        status = true;
        return mInitEventList->exist(key);
    }
    if (mMap) {
        status = true;
        return mMap->exist(key);
    }
    status = false;
    return false;
}


const value::Value&
wrapper_init::get(const std::string& key, bool& status) const
{
    if (mInitEventList) {
        const std::shared_ptr<const value::Value>& res_val =
                mInitEventList->get(key);
        if (res_val) {
            status = true;
            return *res_val.get();
        }
    }
    if (mMap) {
        const std::unique_ptr<value::Value>& res_val =
                mMap->get(key);
        if (res_val) {
            status = true;
            return *res_val.get();
        }
    }
    status = false;
    return def;
}


std::string
wrapper_init::getString(const std::string& key, bool& status) const
{
    const value::Value& v = get(key, status);
    if (status and v.isString()) {
        return v.toString().value();
    }
    return "";
}

int
wrapper_init::getInt(const std::string& key, bool& status) const
{
    const value::Value& v = get(key, status);
    if (status and v.isInteger()) {
        return v.toInteger().value();
    }
    return 0;
}

int
wrapper_init::getBoolean(const std::string& key, bool& status) const
{
    const value::Value& v = get(key, status);
    if (status and v.isBoolean()) {
        return v.toBoolean().value();
    }
    return 0;
}


/***********VlePropagate*************/

VlePropagate::VlePropagate(const std::string& _cond, const std::string& _port):
                cond(_cond), port(_port)
{
    if (cond.empty() or port.empty()) {
        throw utils::ArgError(utils::format(
                "[MetaManager] : the propagate input has wrong form: '%s.%s'",
                cond.c_str(),  port.c_str()));
    }
}

VlePropagate::~VlePropagate()
{
}

std::string
VlePropagate::getName() const
{
    std::string ret = cond;
    ret.append(".");
    ret.append(port);
    return ret;
}

/***********VleInput*************/

VleInput::VleInput(const std::string& _cond, const std::string& _port,
        const value::Value& val, utils::Rand& rn):
                cond(_cond), port(_port), nbValues(0)
{
    if (cond.empty() or port.empty()) {
        throw utils::ArgError(utils::format(
                "[MetaManager] : the input has wrong form: '%s.%s'",
                cond.c_str(),  port.c_str()));
    }
    switch (val.getType()) {
    case value::Value::TUPLE:
        nbValues = val.toTuple().size();
        break;
    case value::Value::SET:
        nbValues = val.toSet().size();
        break;
    case value::Value::MAP: {
        mvalues = MetaManager::valuesFromDistrib(val.toMap(), rn);
        if (mvalues) {
            nbValues = mvalues->size();
        } else {
            nbValues = 1;
        }
        break;
    } default:
        nbValues = 1;
        break;
    }
}

VleInput::~VleInput()
{
}

const vle::value::Value&
VleInput::values(const wrapper_init& init)
{
    if (mvalues){
        return *mvalues;
    } else {
        std::string key("input_");
        key.append(getName());
        bool status = true;
        return init.get(key, status);
    }
}

std::string
VleInput::getName() const
{
    std::string ret = cond;
    ret.append(".");
    ret.append(port);
    return ret;
}

/***********VleReplicate*************/

VleReplicate::VleReplicate(const std::string& _cond, const std::string& _port,
        const value::Value& val, utils::Rand& rn):
                cond(_cond), port(_port), nbValues(0)
{
    bool err = false;
    if (cond.empty() or port.empty()) {
        throw utils::ArgError(utils::format(
                "[MetaManager] : the replicate has wrong form: '%s.%s'",
                cond.c_str(),  port.c_str()));
    }
    switch (val.getType()) {
    case value::Value::TUPLE:
        nbValues = val.toTuple().size();
        break;
    case value::Value::SET:
        nbValues = val.toSet().size();
        break;
    case value::Value::MAP: {
        mvalues = MetaManager::valuesFromDistrib(val.toMap(), rn);
        if (mvalues) {
            nbValues = mvalues->size();
        } else {
            err = true;
        }
        break;
    } default: {
        err = true;
        break;
    }}
    if (err) {
        throw utils::ArgError(utils::format(
                "[MetaManager] : error in configuration of "
                "'replicate_%s.%s', expect a value::Set or Tuple",
                cond.c_str(),  port.c_str()));
    }
}

VleReplicate::~VleReplicate()
{
}

const vle::value::Value&
VleReplicate::values(const wrapper_init& init)
{
    if (mvalues){
        return *mvalues;
    } else {
        std::string key("replicate_");
        key.append(getName());
        bool status = true;
        return init.get(key, status);
    }
}

std::string
VleReplicate::getName() const
{
    std::string ret = cond;
    ret.append(".");
    ret.append(port);
    return ret;
}

/***********DelegateOutput********/
DelegateOut::DelegateOut(VleOutput& vleout, bool managedouble):
        vleOut(vleout), manageDouble(managedouble)
{
}
DelegateOut::~DelegateOut()
{
}

std::unique_ptr<value::Value>
DelegateOut::integrateReplicate(VleOutput& vleout, vle::value::Matrix& outMat)
{
    switch(vleout.integrationType) {
    case MAX: {
        double max = -9999;
        for (unsigned int i=1; i < outMat.rows(); i++) {
            double v = outMat.getDouble(vleout.colIndex, i);
            if (v > max) {
                max = v;
            }
        }
        return value::Double::create(max);
        break;
    } case LAST: {
        if (vleout.shared) {
            const std::unique_ptr<value::Value>& res =
                            outMat.get(vleout.colIndex, outMat.rows() - 1);
            return res->clone();
        } else {
            return std::move(outMat.give(vleout.colIndex, outMat.rows() - 1));
        }
        break;
    } case MSE: {
        double sum_square_error = 0;
        double nbVal = 0;
        for (unsigned int i=0; i< vleout.mse_times->size(); i++) {
            int t = std::floor(vleout.mse_times->at(i));
            if (t > 0 and t< (int) outMat.rows()) {
                sum_square_error += std::pow(
                        (outMat.getDouble(vleout.colIndex,t)
                                - vleout.mse_observations->at(i)), 2);
                nbVal++;
            }
        }
        return value::Double::create(sum_square_error/nbVal);
        break;
    } default:{
        //not possible
        break;
    }}
    return nullptr;
}

AccuMulti&
DelegateOut::getAccu(std::map<int, std::unique_ptr<AccuMulti>>& accus,
        unsigned int index, const VleOutput& vleout)
{
    std::map<int, std::unique_ptr<AccuMulti>>::iterator itf =
            accus.find(index);
    if (itf != accus.end()) {
        return *(itf->second);
    }
    std::unique_ptr<AccuMulti> ptr(new AccuMulti(
            vleout.replicateAggregationType));
    AccuMulti& ref = *ptr;
    ref.setDefaultQuantile(vleout.replicateAggregationQuantile);
    accus.insert(std::make_pair(index, std::move(ptr)));
    return ref;
}

AccuMono&
DelegateOut::getAccu(std::map<int, std::unique_ptr<AccuMono>>& accus,
        unsigned int index, const VleOutput& vleout)
{
    std::map<int, std::unique_ptr<AccuMono>>::iterator itf =
            accus.find(index);
    if (itf != accus.end()) {
        return *(itf->second);
    }
    std::unique_ptr<AccuMono> ptr(new AccuMono(
            vleout.replicateAggregationType));
    AccuMono& ref = *ptr;
    ref.setDefaultQuantile(vleout.replicateAggregationQuantile);
    accus.insert(std::make_pair(index, std::move(ptr)));
    return ref;
}


DelOutStd::DelOutStd(VleOutput& vleout): DelegateOut(vleout, true)
{
    minputAccu.reset(new AccuMono(vleOut.inputAggregationType));
}

std::unique_ptr<value::Value>
DelOutStd::insertReplicate(
            vle::value::Matrix& outMat, unsigned int currInput)
{
    //start insertion for double management only
    std::unique_ptr<value::Value> intVal = std::move(
            integrateReplicate(vleOut, outMat));
    if (vleOut.nbReplicates == 1) {
        minputAccu->insert(intVal->toDouble().value());
    } else {
        AccuMono& accuRepl = DelegateOut::getAccu(mreplicateAccu, currInput,
                vleOut);
        accuRepl.insert(intVal->toDouble().value());
        //test if aggregating replicates is finished
        if (accuRepl.count() == vleOut.nbReplicates) {
            minputAccu->insert(accuRepl.getStat(
                    vleOut.replicateAggregationType));
            mreplicateAccu.erase(currInput);

        }
    }
    //test if aggregating inputs is finished
    if (minputAccu->count() == vleOut.nbInputs) {
        double res = minputAccu->getStat(vleOut.inputAggregationType);
        minputAccu.reset(nullptr);
        return value::Double::create(res);
    }
    return nullptr;
};


DelOutIntAggrALL::DelOutIntAggrALL(VleOutput& vleout, bool managedouble):
        DelegateOut(vleout, managedouble), mreplicateAccu(),
        minputAccu(nullptr), nbInputsFilled(0)
{

}
std::unique_ptr<value::Value>
DelOutIntAggrALL::insertReplicate(
            vle::value::Matrix& outMat, unsigned int currInput)
{
    if (not minputAccu) {
        if (manageDouble) {
            minputAccu.reset(new value::Table(vleOut.nbInputs,
                    outMat.rows()-1));
        } else {
            minputAccu.reset(new value::Matrix(vleOut.nbInputs,
                    outMat.rows()-1, 10, 10));
        }
    }
    if (vleOut.nbReplicates == 1){//one can put directly into results
        for (unsigned int i=1; i < outMat.rows(); i++) {
            if (manageDouble) {
                minputAccu->toTable().get(currInput, i-1) =
                        outMat.getDouble(vleOut.colIndex, i);
            } else if (vleOut.shared) {
                minputAccu->toMatrix().set(currInput, i-1,
                        outMat.get(vleOut.colIndex, i)->clone());
            } else {
                minputAccu->toMatrix().set(currInput, i-1,
                        std::move(outMat.give(vleOut.colIndex, i)));
            }
        }
        nbInputsFilled++;
    } else {
        AccuMulti& accuRepl = DelegateOut::getAccu(mreplicateAccu, currInput,
                vleOut);
        accuRepl.insertColumn(outMat, vleOut.colIndex);
        if (accuRepl.count() == vleOut.nbReplicates) {
            accuRepl.fillStat(minputAccu->toTable(),
                    currInput, vleOut.replicateAggregationType);
            mreplicateAccu.erase(currInput);
            nbInputsFilled++;
        }
    }
    if (nbInputsFilled == vleOut.nbInputs) {
        return std::move(minputAccu);
    }
    return nullptr;
}

DelOutIntALL::DelOutIntALL(VleOutput& vleout): DelegateOut(vleout, true),
        mreplicateAccu(), minputAccu(nullptr)
{

}
std::unique_ptr<value::Value>
DelOutIntALL::insertReplicate(
            vle::value::Matrix& outMat, unsigned int currInput)
{
    if (not minputAccu) {
        minputAccu.reset(new AccuMulti(vleOut.inputAggregationType));
    }
    if (vleOut.nbReplicates == 1){//one can put directly into results
        minputAccu->insertColumn(outMat, vleOut.colIndex);
    } else {
        AccuMulti& accuRepl = DelegateOut::getAccu(mreplicateAccu, currInput,
                vleOut);
        accuRepl.insertColumn(outMat, vleOut.colIndex);
        if (accuRepl.count() == vleOut.nbReplicates) {
            minputAccu->insertAccuStat(accuRepl,
                    vleOut.replicateAggregationType);
            mreplicateAccu.erase(currInput);
        }
    }
    if (minputAccu->count() == vleOut.nbInputs) {
        std::unique_ptr<value::Table> res(new value::Table(1,
                minputAccu->size()));
        minputAccu->fillStat(*res, 0, vleOut.inputAggregationType);
        return std::move(res);
    }
    return nullptr;
}

DelOutAggrALL::DelOutAggrALL(VleOutput& vleout, bool managedouble):
        DelegateOut(vleout, managedouble), mreplicateAccu(),
        minputAccu(nullptr), nbInputsFilled(0)
{
    if (manageDouble) {
        minputAccu.reset(new value::Table(vleOut.nbInputs,1));
    } else {
        minputAccu.reset(new value::Matrix(vleOut.nbInputs,1,10,10));
    }
}
std::unique_ptr<value::Value>
DelOutAggrALL::insertReplicate(
            vle::value::Matrix& outMat, unsigned int currInput)
{
    std::unique_ptr<value::Value> intVal = std::move(
            integrateReplicate(vleOut, outMat));

    if (vleOut.nbReplicates == 1){//one can put directly into results
        if (manageDouble) {
            minputAccu->toTable().get(currInput, 0) =
                    intVal->toDouble().value();
        } else {
            minputAccu->toMatrix().set(currInput, 0, std::move(intVal));

        }

        nbInputsFilled++;
    } else {
        AccuMono& accuRepl = DelegateOut::getAccu(mreplicateAccu, currInput,
                vleOut);
        accuRepl.insert(intVal->toDouble().value());
        if (accuRepl.count() == vleOut.nbReplicates) {
            minputAccu->toTable().get(currInput, 0)=
                    accuRepl.getStat(vleOut.replicateAggregationType);
            mreplicateAccu.erase(currInput);
            nbInputsFilled++;
        }
    }
    if (nbInputsFilled == vleOut.nbInputs) {
        return std::move(minputAccu);
    }
    return nullptr;
}


/***********VleOutput*************/

VleOutput::VleOutput() :
   id(), view(), absolutePort(), colIndex(-1), shared(true),
   integrationType(LAST), replicateAggregationType(S_mean),
   inputAggregationType(S_at), nbInputs(0), nbReplicates(0), delegate(nullptr),
   mse_times(nullptr), mse_observations(nullptr),
   replicateAggregationQuantile(0.5)
{
}

VleOutput::VleOutput(const std::string& _id,
        const value::Value& val) :
   id(_id), view(), absolutePort(),  colIndex(-1), shared(true),
   integrationType(LAST), replicateAggregationType(S_mean),
   inputAggregationType(S_at), nbInputs(0), nbReplicates(0), delegate(nullptr),
   mse_times(nullptr), mse_observations(nullptr),
   replicateAggregationQuantile(0.5)
{
    std::string tmp;
    if (val.isString()) {
        if (not parsePath(val.toString().value())) {
            throw utils::ArgError(utils::format(
                    "[MetaManager] : error in configuration of the output "
                    "'output_%s' with a string; got: '%s'",
                    id.c_str(),  val.toString().value().c_str()));
        }
    } else if (val.isMap()) {
        const value::Map& m = val.toMap();
        bool error = false;
        if (m.exist("path")) {
            error = not parsePath(m.getString("path"));
        }
        if (m.exist("aggregation_replicate")) {
            tmp = m.getString("aggregation_replicate");
            if (tmp == "mean") {
                replicateAggregationType = S_mean;
            } else if (tmp == "quantile"){
                replicateAggregationType = S_quantile;
                if (m.exist("replicate_quantile")) {
                    replicateAggregationQuantile = m.getDouble(
                            "replicate_quantile");
                }
            } else if (tmp == "variance"){
                replicateAggregationType = S_variance;
            } else {
                error = true;
            }
        }
        if (m.exist("aggregation_input")) {
            tmp = m.getString("aggregation_input");
            if (tmp == "mean") {
                inputAggregationType = S_mean;
            } else if (tmp == "quantile"){
                inputAggregationType = S_quantile;
            } else if (tmp == "max"){
                inputAggregationType = S_max;
            } else if (tmp == "all"){
                inputAggregationType = S_at;
            } else {
                error = true;
            }
        }
        if (not error) {
            if (m.exist("integration")) {
                tmp = m.getString("integration");
                if (tmp == "last") {
                    integrationType = LAST;
                } else if(tmp == "max") {
                    integrationType = MAX;
                } else if(tmp == "mse") {
                    integrationType = MSE;
                } else if(tmp == "all") {
                    integrationType = ALL;
                } else {
                    error = true;
                }
            } else {
                integrationType = LAST;
            }
        }
        if (not error and integrationType == MSE) {
            if (not m.exist("mse_times") or not m.exist("mse_observations")) {
                error = true;
            } else {
                mse_times.reset(new value::Tuple(m.getTuple("mse_times")));
                mse_observations.reset(new value::Tuple(
                        m.getTuple("mse_observations")));
                error = mse_times->size() != mse_observations->size();
            }
        }
        if (error) {
            throw utils::ArgError(utils::format(
                    "[MetaManager] : error in configuration of the output "
                    " '%s%s' with a map",
                    "id_output_",  id.c_str()));
        }
    }
}

VleOutput::~VleOutput()
{
}

bool
VleOutput::parsePath(const std::string& path)
{
    std::vector<std::string> splvec;
    MetaManager::split(splvec, path, '/');
    if (splvec.size() == 2) {
        view.assign(splvec[0]);
        absolutePort.assign(splvec[1]);
        return true;
    } else {
        return false;
    }
}

std::unique_ptr<value::Value>
VleOutput::insertReplicate(value::Map& result, unsigned int currInput,
        unsigned int nbInputs, unsigned int nbReplicates)
{
    value::Map::iterator it = result.find(view);
    if (it == result.end()) {
        throw vu::ArgError(utils::format(
                "[MetaManager] view '%s' not found)",
                view.c_str()));
    }
    value::Matrix& outMat = value::toMatrixValue(*it->second);
    return insertReplicate(outMat, currInput, nbInputs, nbReplicates);
}

std::unique_ptr<value::Value>
VleOutput::insertReplicate(value::Matrix& outMat, unsigned int currInput,
        unsigned int nbIn, unsigned int nbRepl)
{
    if (not delegate){
        nbReplicates = nbRepl;
        nbInputs = nbIn;
        //performs some checks on output matrix
        if (outMat.rows() < 2){
            throw vu::ArgError("[MetaManager] expect at least 2 rows");
        }
        //get col index
        colIndex = 9999;
        for (unsigned int i=0; i < outMat.columns(); i++) {
            if (outMat.getString(i,0) == absolutePort) {
                colIndex = i;
            }
        }
        if (colIndex == 9999) {
            throw vu::ArgError(utils::format(
                    "[MetaManager] view.port '%s' not found)",
                    absolutePort.c_str()));
        }
        bool manageDouble = true;
        if (not outMat.get(colIndex,1)->isDouble()) {
            if (nbReplicates != 1 or
                (integrationType != ALL  and integrationType != LAST) or
                (inputAggregationType != S_at)){
                throw vu::ArgError(utils::format(
                        "[MetaManager] since data is not double no "
                        "aggregation is possible for output '%s'",
                        id.c_str()));
            }
            manageDouble = false;
        }
        if (integrationType == ALL) {
            if (inputAggregationType == S_at) {
                delegate.reset(new DelOutIntAggrALL(*this, manageDouble));
            } else {
                delegate.reset(new DelOutIntALL(*this));
            }
        } else {
            if (inputAggregationType == S_at) {
                delegate.reset(new DelOutAggrALL(*this, manageDouble));
            } else {
                delegate.reset(new DelOutStd(*this));
            }
        }
    }
    return delegate->insertReplicate(outMat, currInput);
}

/***********MetaManager*************/

MetaManager::MetaManager(): mIdVpz(), mIdPackage(),
        mConfigParallelType(SINGLE), mRemoveSimulationFiles(true),
        mConfigParallelNbSlots(2), mrand(), mPropagate(), mInputs(),
        mReplicates(), mOutputs(),
        mWorkingDir(utils::Path::temp_directory_path().string()),
        mCtx(utils::make_context())
{

    mCtx->set_log_priority(3);//erros only

}

MetaManager::MetaManager(utils::ContextPtr ctx): mIdVpz(), mIdPackage(),
        mConfigParallelType(SINGLE), mRemoveSimulationFiles(true),
        mConfigParallelNbSlots(2), mrand(), mPropagate(), mInputs(),
        mReplicates(), mOutputs(),
        mWorkingDir(utils::Path::temp_directory_path().string()),
        mCtx(ctx)
{

}


MetaManager::~MetaManager()
{
    mCtx.reset();
    clear();
}

utils::Rand&
MetaManager::random_number_generator()
{
    return mrand;
}


unsigned int
MetaManager::inputsSize() const
{
    if (mInputs.size() > 0) {
        return mInputs[0]->nbValues;
    }
    return 1;
}


unsigned int
MetaManager::replicasSize() const
{
    if (mReplicates.size() == 0) {
        return 1;
    }
    return mReplicates[0]->nbValues;
}

//write kth element of exp (a Set or a Tuple) as xml into out
bool
produceXml(const value::Value& exp, unsigned int k, std::ostream& out,
        manager::Error& err)
{
    switch(exp.getType()) {
    case value::Value::SET:
        exp.toSet().get(k)->writeXml(out);
        return true;
        break;
    case value::Value::TUPLE:
        out << "<double>" << exp.toTuple().at(k) <<"</double>";
        return true;
        break;
    default:
        if (k == 0){
            out << exp.writeToXml();
            return true;
        }
        break;
    }
    err.code = -1;
    err.message = "[vle.recursive] error while producing cvle file";
    return false;
}

void
MetaManager::produceCvleInFile(const wrapper_init& init,
        const std::string& inPath, manager::Error& err)
{
    std::ofstream inFile;
    inFile.open (inPath, std::ios_base::trunc);
    //write header
    inFile << "_cvle_complex_values\n";
    //write content
    std::string curr_cond = "";
    bool has_simulation_engine = false;
    std::set<std::string> conds_seen;
    for (unsigned int i = 0; i < inputsSize(); i++) {
        for (unsigned int j = 0; j < replicasSize(); j++) {
            curr_cond = "";
            has_simulation_engine = false;
            inFile << "<?xml version='1.0' encoding='UTF-8'?>"
                    "<vle_project date=\"\" version=\"1.0\" author=\"cvle\">"
                    "<experiment name=\"cvle_exp\"><conditions>";
            for (unsigned int in=0; in < mInputs.size(); in++) {
                VleInput& tmp_input = *mInputs[in];
                has_simulation_engine = has_simulation_engine or
                        (tmp_input.cond == "simulation_engine");
                if (curr_cond != "" and tmp_input.cond != curr_cond){
                    inFile << "</condition>";
                }
                if (tmp_input.cond != curr_cond) {
                    inFile << "<condition name =\"" << tmp_input.cond << "\">";
                }
                curr_cond = tmp_input.cond;

                inFile << "<port name =\"" << tmp_input.port << "\">";
                produceXml(tmp_input.values(init), i, inFile, err);
                inFile << "</port>";
                //write replicate if the same cond name
                for (unsigned int k=0; k<mReplicates.size(); k++) {
                    VleReplicate& tmp_repl = *mReplicates[k];
                    if (tmp_repl.cond == curr_cond) {
                        inFile << "<port name =\"" << tmp_repl.port << "\">";
                        produceXml(tmp_repl.values(init), j, inFile, err);
                        inFile << "</port>";

                    }
                }
                if ((int) in == ((int) mInputs.size())-1) {
                    inFile << "</condition>";
                }
                conds_seen.emplace(curr_cond);
            }
            //write replicates if not already written
            for (unsigned int k=0; k<mReplicates.size(); k++) {
                VleReplicate& tmp_repl = *mReplicates[k];
                if (conds_seen.find(tmp_repl.cond) == conds_seen.end()) {
                    has_simulation_engine = has_simulation_engine or
                            (tmp_repl.cond == "simulation_engine");
                    inFile << "<condition name =\"" << tmp_repl.cond << "\">";
                    inFile << "<port name =\"" << tmp_repl.port << "\">";
                    produceXml(tmp_repl.values(init), j, inFile, err);
                    inFile << "</port>";
                    inFile << "</condition>";
                }
            }
            //write _cvle_cond
            inFile << "<condition name =\"_cvle_cond\">";
            inFile << "<port name =\"id\">";
            inFile << "<string>id_" << i << "_" << j << "</string>";
            inFile << "</port>";
            inFile << "</condition>";
            inFile << "</conditions>";
            inFile << "</experiment>";
            inFile << "</vle_project>";
            inFile <<"\n";
        }
    }
    inFile.close();
}


std::unique_ptr<value::Map>
MetaManager::run(wrapper_init& init,
        manager::Error& err)
{
    clear();
    bool status = true;
    if (init.exist("config_parallel_type", status)) {
        std::string tmp;
        tmp.assign(init.getString("config_parallel_type", status));
        if (tmp == "threads") {
            mConfigParallelType = THREADS;
        } else if (tmp == "mvle") {
            mConfigParallelType = MVLE;
            if (! init.exist("working_dir", status)) {
                err.code = -1;
                err.message = "[MetaManager] error for "
                        "mvle config, missing 'working_dir' parameter";
                return nullptr;
            }
            mWorkingDir.assign(init.getString("working_dir", status));
        } else if (tmp == "cvle") {
            mConfigParallelType = CVLE;
            if (! init.exist("working_dir", status)) {
                err.code = -1;
                err.message = "[MetaManager] error for "
                        "mvle config, missing 'working_dir' parameter";
                return nullptr;
            }
            mWorkingDir.assign(init.getString("working_dir", status));
        } else if (tmp == "single") {
            mConfigParallelType = SINGLE;
        } else {
            err.code = -1;
            err.message = "[MetaManager] error for configuration type of "
                    "parallel process";
            return nullptr;
        }
    }
    if (init.exist("config_parallel_nb_slots", status)) {
        mConfigParallelNbSlots =
                init.getInt("config_parallel_nb_slots", status);
    }
    if (init.exist("expe_log", status)) {
        mCtx->set_log_priority(init.getInt("expe_log", status));
    }
    if (init.exist("expe_seed", status)) {
        mrand.seed(init.getInt("expe_seed", status));
    }
    if (init.exist("config_parallel_rm_files", status)) {
        mRemoveSimulationFiles = init.getBoolean(
                "config_parallel_rm_files", status);
    }
    if (init.exist("package", status)) {
        mIdPackage = init.getString("package", status);
    } else {
        err.code = -1;
        err.message = "[MetaManager] missing 'package'";
        return nullptr;
    }
    if (init.exist("vpz", status)) {
        mIdVpz = init.getString("vpz", status);
    } else {
        err.code = -1;
        err.message = "[MetaManager] missing 'vpz'";
        return nullptr;
    }

    std::string in_cond;
    std::string in_port;
    std::string out_id;

    try {
        std::string conf = "";
        for (init.begin(); not init.isEnded(); init.next()) {
            const value::Value& val = init.current(conf, status);
            if (MetaManager::parseInput(conf, in_cond, in_port, "propagate_")) {
                mPropagate.emplace_back(new VlePropagate(in_cond, in_port));
            } else if (MetaManager::parseInput(conf, in_cond, in_port)) {
                mInputs.emplace_back(new VleInput(
                        in_cond, in_port, val, mrand));
            } else if (MetaManager::parseInput(conf, in_cond, in_port,
                    "replicate_")){
                mReplicates.emplace_back(new VleReplicate(in_cond, in_port,
                        val, mrand));
            } else if (MetaManager::parseOutput(conf, out_id)){
                mOutputs.emplace_back(new VleOutput(out_id, val));
            }
        }
        std::sort(mPropagate.begin(), mPropagate.end(),
                VlePropagateSorter());
        std::sort(mInputs.begin(), mInputs.end(), VleInputSorter());
        std::sort(mOutputs.begin(), mOutputs.end(), VleOutputSorter());
    } catch (const std::exception& e){
        err.code = -1;
        err.message = "[MetaManager] ";
        err.message += e.what();
        clear();
        return nullptr;
    }

    //check Inputs
    unsigned int inputSize = 0;
    for (unsigned int i = 0; i< mInputs.size(); i++) {
        const VleInput& vleIn = *mInputs[i];
        //check input size which has to be consistent
        if (inputSize == 0 and vleIn.nbValues > 1) {
            inputSize = vleIn.nbValues;
        } else {
            if (vleIn.nbValues > 1 and inputSize > 0
                    and inputSize != vleIn.nbValues) {
                err.code = -1;
                err.message = utils::format(
                        "[MetaManager]: error in input values: wrong number"
                        " of values 1st input has %u values,  input %s has %u "
                        "values", inputSize, vleIn.getName().c_str(),
                        vleIn.nbValues);
                clear();
                return nullptr;
            }
        }
        //check if already exist in replicate or propagate
        for (unsigned int j=0; j<mReplicates.size(); j++) {
            const VleReplicate& vleRepl = *mReplicates[j];
            if (vleRepl.getName() == vleIn.getName()) {
                err.code = -1;
                err.message = utils::format(
                        "[MetaManager]: error input '%s' is also a replicate",
                        vleIn.getName().c_str());
                clear();
                return nullptr;
            }
        }
        for (unsigned int j=0; j<mPropagate.size(); j++) {
            const VlePropagate& vleProp = *mPropagate[j];
            if (vleProp.getName() == vleIn.getName()) {
                err.code = -1;
                err.message = utils::format(
                        "[MetaManager]: error input '%s' is also a propagate",
                        vleIn.getName().c_str());
                clear();
                return nullptr;
            }
        }
    }
    //check Replicates
    unsigned int replSize = 0;
    for (unsigned int j=0; j<mReplicates.size(); j++) {
        const VleReplicate& vleRepl = *mReplicates[j];
        //check repl size which has to be consistent
        if (replSize == 0 and vleRepl.nbValues > 1) {
            replSize = vleRepl.nbValues;
        } else {
            if (vleRepl.nbValues > 1 and replSize > 0
                    and replSize != vleRepl.nbValues) {
                err.code = -1;
                err.message = utils::format(
                        "[MetaManager]: error in replicate values: wrong number"
                        " of values 1st replicate has %u values, replicate %s "
                        "has %u values", replSize, vleRepl.getName().c_str(),
                        vleRepl.nbValues);
                clear();
                return nullptr;
            }
        }
        //check if already exist in replicate or propagate
        for (unsigned int j=0; j<mPropagate.size(); j++) {
            const VlePropagate& vleProp = *mPropagate[j];
            if (vleProp.getName() == vleRepl.getName()) {
                err.code = -1;
                err.message = utils::format(
                        "[MetaManager]: error replicate '%s' is also a "
                        "propagate", vleRepl.getName().c_str());
                clear();
                return nullptr;
            }
        }
    }

    //check nb slots
    if (mConfigParallelNbSlots < 2 and mConfigParallelType == CVLE) {
        err.code = -1;
        err.message = "[MetaManager] error for "
                "configuration of cvle for the nb of slots (< 2)";
        return nullptr;
    }


    //launch simulations
    switch(mConfigParallelType) {
    case SINGLE:
    case THREADS: {
        return run_with_threads(init, err);
        break;
    } case MVLE: {
        return run_with_mvle(init, err);
        break;
    } case CVLE: {
        return run_with_cvle(init, err);
        break;
    }}
    return nullptr;
}


void
MetaManager::readResultFile(const std::string& filePath, value::Matrix& mat)
{
    reader::VleResultsTextReader tfr(filePath);
    tfr.readFile(mat);
}

std::unique_ptr<vpz::Vpz>
MetaManager::init_embedded_model(const wrapper_init& init)
{
    vle::utils::Package pkg(mCtx, mIdPackage);
    std::string vpzFile = pkg.getExpFile(mIdVpz, vle::utils::PKG_BINARY);

    std::unique_ptr<vpz::Vpz> model(new vpz::Vpz(vpzFile));
    model->project().experiment().setCombination("linear");

    VleAPIfacilities::changeAllPlugin(*model, "dummy");
    std::vector<std::unique_ptr<VleOutput>>::const_iterator itb =
            mOutputs.begin();
    std::vector<std::unique_ptr<VleOutput>>::const_iterator ite =
            mOutputs.end();
    for (; itb != ite; itb++) {
        switch(mConfigParallelType) {
        case SINGLE:
        case THREADS:
        case CVLE: {
            VleAPIfacilities::changePlugin(*model, (*itb)->view, "storage");
            break;
        } case MVLE: {
            VleAPIfacilities::changePlugin(*model, (*itb)->view, "file");
            break;
        }}
    }
    post_propagates(*model, init);
    return model;
}

std::unique_ptr<value::Map>
MetaManager::init_results()
{
    //build output matrix with header
    std::unique_ptr<value::Map> results(new value::Map());
    for (unsigned int j=0; j<mOutputs.size();j++) {
        results->add(mOutputs[j]->id, value::Null::create());
    }
    return results;
}


std::unique_ptr<value::Map>
MetaManager::run_with_threads(const wrapper_init& init, manager::Error& err)
{
    std::unique_ptr<vpz::Vpz> model = init_embedded_model(init);
    std::unique_ptr<value::Map> results = init_results();

    unsigned int inputSize = inputsSize();
    unsigned int repSize = replicasSize();
    unsigned int outputSize =  mOutputs.size();
    post_inputs(*model, init);
    vle::manager::Manager planSimulator(
            mCtx,
            vle::manager::LOG_NONE,
            vle::manager::SIMULATION_NONE,
            nullptr);
    vle::manager::Error manerror;

    mCtx->log(7, "", __LINE__, "",
            "[vle.recursive] simulation single/threads(%d slots) "
            "nb simus: %u \n", mConfigParallelNbSlots,
            (repSize*inputSize));


    //        //for dbg
    //        std::string tempvpzPath = pkg.getExpDir(vu::PKG_BINARY);
    //        tempvpzPath.append("/temp_saved.vpz");
    //        model->write(tempvpzPath);
    //        //
    std::unique_ptr<value::Matrix> output_mat =  planSimulator.run(
            std::move(model), mConfigParallelNbSlots, 0, 1, &manerror);

    mCtx->log(7, "", __LINE__, "",
            "[vle.recursive] end simulation single/threads\n");

    if (manerror.code != 0) {
        err.code = -1;
        err.message = "[MetaManager] ";
        err.message += manerror.message;
        model.reset(nullptr);
        results.reset(nullptr);
        clear();
        return nullptr;
    }
    std::unique_ptr<value::Value> aggrValue;
    for (unsigned int out =0; out < outputSize; out++) {
        VleOutput& outId = *mOutputs[out];
        for (unsigned int i = 0; i < inputSize*repSize; i+= repSize) {
            for (unsigned int j = 0; j < repSize; j++) {
                aggrValue = std::move(outId.insertReplicate(
                        output_mat->get(i+j,0)->toMap(),
                        i/repSize, inputSize, repSize));
            }
        }
        results->set(outId.id, std::move(aggrValue));
    }

    mCtx->log(7, "", __LINE__, "",
            "[vle.recursive] aggregation finished\n");

    return results;
}

std::unique_ptr<value::Map>
MetaManager::run_with_mvle(const wrapper_init& init, manager::Error& err)
{
    std::unique_ptr<vpz::Vpz> model = init_embedded_model(init);
    std::unique_ptr<value::Map> results = init_results();

    unsigned int inputSize = inputsSize();
    unsigned int repSize = replicasSize();
    unsigned int outputSize =  mOutputs.size();

    post_inputs(*model, init);

    vu::Package pkg(mCtx, "vle.recursive");//TODO should be saved outside
    std::string tempvpzPath = pkg.getExpDir(vu::PKG_BINARY);
    tempvpzPath.append("/temp_gen_MPI.vpz");
    model->write(tempvpzPath);
    vu::Spawn mspawn(mCtx);
    std::string exe = mCtx->findProgram("mpirun").string();

    std::vector < std::string > argv;
    //use mpi_warn_nf_forgk since mvle launches simulation with fork
    argv.push_back("--mca");
    argv.push_back("mpi_warn_on_fork");
    argv.push_back("0");
    //        //set more log for mpirun
    //        argv.push_back("--mca");
    //        argv.push_back("ras_gridengine_verbose");
    //        argv.push_back("1");
    //        argv.push_back("--mca");
    //        argv.push_back("plm_gridengine_verbose");
    //        argv.push_back("1");
    //        argv.push_back("--mca");
    //        argv.push_back("ras_gridengine_show_jobid");
    //        argv.push_back("1");
    //        argv.push_back("--mca");
    //        argv.push_back("plm_gridengine_debug");
    //        argv.push_back("1");

    argv.push_back("-np");
    std::stringstream ss;
    {
        ss << mConfigParallelNbSlots;
        argv.push_back(ss.str());
    }
    argv.push_back("mvle");
    argv.push_back("-P");//TODO should be simulated outside the rr package
    argv.push_back("vle.recursive");
    argv.push_back("temp_gen_MPI.vpz");

    //log before run
    if (mCtx->get_log_priority() >= 7) {
        mCtx->log(7, "", __LINE__, "",
                "[vle.recursive] simulation mvle %d \n",
                mConfigParallelNbSlots);
        std::string messageDbg ="";
        for (const auto& s : argv ) {
            messageDbg += " ";
            messageDbg += s;
        }
        messageDbg += "\n";
        mCtx->log(1, "", __LINE__, "",
                "[vle.recursive] launching in dir %s: %s %s",
                mWorkingDir.c_str(), exe.c_str(), messageDbg.c_str());
    }

    bool started = mspawn.start(exe, mWorkingDir, argv);
    if (not started) {
        err.code = -1;
        err.message = vle::utils::format(
                "[MetaManager] Failed to start `%s'", exe.c_str());
        model.reset(nullptr);
        results.reset(nullptr);
        clear();
        return nullptr;
    }
    bool is_success = true;
    std::string message, output, error;
    while (not mspawn.isfinish()) {
        if (mspawn.get(&output, &error)) {
            if (not error.empty() and message.empty()){
                //TODO info such as Context are written into error.
                //                    is_success = false;
                //                    message.assign(error);
            }
            output.clear();
            error.clear();
            std::this_thread::sleep_for(std::chrono::microseconds(200));
        } else {
            break;
        }
    }
    mspawn.wait();
    if (! is_success) {
        err.code = -1;
        err.message = "[MetaManager] ";
        err.message += message;
        model.reset(nullptr);
        results.reset(nullptr);
        clear();
        return nullptr;
    }
    mspawn.status(&message, &is_success);

    if (! is_success) {
        //TODO with mpi_warn_on_fork=0, the forks can lead to an error.
        //            err.code = -1;
        //            err.message = "[MetaManager] ";
        //            err.message += vle::utils::format("Error launching `%s' : %s ",
        //                    exe.c_str(), message.c_str());
        //            model.reset(nullptr);
        //            results.reset(nullptr);
        //            clear();
        //            return nullptr;
    }

    std::unique_ptr<value::Value> aggrValue;
    for (unsigned int out =0; out < outputSize; out++) {
        VleOutput& outId = *mOutputs[out];
        for (unsigned int i = 0; i < inputSize*repSize; i+= repSize) {
            for (unsigned int j = 0; j < repSize; j++) {
                std::string vleResultFilePath = mWorkingDir;
                vleResultFilePath.append(model->project().experiment().name());
                vleResultFilePath.append("-");
                vleResultFilePath.append(std::to_string(i+j));
                vleResultFilePath.append("_");
                vleResultFilePath.append(outId.view);
                vleResultFilePath.append(".dat");
                value::Matrix mat;
                readResultFile(vleResultFilePath, mat);
                aggrValue = std::move(outId.insertReplicate(mat, i/repSize,
                        inputSize, repSize));
                if (mRemoveSimulationFiles and out == (outputSize-1)) {
                    utils::Path torm(vleResultFilePath.c_str());
                    torm.remove();
                }
            }
        }
        results->set(outId.id, std::move(aggrValue));
    }

    mCtx->log(7, "", __LINE__, "",
            "[vle.recursive] aggregation finished \n");

    return results;
}


std::unique_ptr<value::Map>
MetaManager::run_with_cvle(const wrapper_init& init, manager::Error& err)
{
    std::unique_ptr<vpz::Vpz> model = init_embedded_model(init);
    std::unique_ptr<value::Map> results = init_results();

    unsigned int inputSize = inputsSize();
    unsigned int repSize = replicasSize();

    mCtx->log(6, "", __LINE__, "",
            "[vle.recursive] simulation cvle (%d slots) "
            "nb simus: %u \n", mConfigParallelNbSlots,
            (repSize*inputSize));

    //save vpz
    vu::Package pkg(mCtx, "vle.recursive");//TODO should be saved outside
    utils::Path vleRecPath(pkg.getExpDir(vu::PKG_BINARY));
    utils::Path tempVpzPath = make_temp(
            "vle-rec-%%%%-%%%%-%%%%-%%%%.vpz", vleRecPath.string());
    model->write(tempVpzPath.string());
    std::string tempVpzFile = tempVpzPath.filename();

    //write content in in.csv file
    utils::Path tempInCsv = make_temp(
            "vle-rec-%%%%-%%%%-%%%%-%%%%-in.csv", mWorkingDir);
    produceCvleInFile(init, tempInCsv.string(), err);
    if (err.code == -1) {
        return nullptr;
    }

    //find output
    utils::Path  tempOutCsv = make_temp(
            "vle-rec-%%%%-%%%%-%%%%-%%%%-out.csv", mWorkingDir);

    //launch mpirun
    vu::Spawn mspawn(mCtx);
    std::string exe = mCtx->findProgram("mpirun").string();

    std::vector < std::string > argv;
    argv.push_back("-np");
    argv.push_back(toString(mConfigParallelNbSlots));
    argv.push_back("cvle");
    argv.push_back("--block-size");
    argv.push_back(toString(
            (int(inputSize*repSize)/int(mConfigParallelNbSlots-1))+1));
    argv.push_back("--withoutspawn");
    argv.push_back("--package");//TODO required by cvle but unused
    argv.push_back("vle.recursive");//TODO required by cvle but unused
    argv.push_back(tempVpzFile);
    argv.push_back("-i");
    argv.push_back(tempInCsv.string());
    argv.push_back("-o");
    argv.push_back(tempOutCsv.string());

    if (mCtx->get_log_priority() >= 7) {
        std::string messageDbg ="";
        for (const auto& s : argv ) {
            messageDbg += " ";
            messageDbg += s;
        }
        messageDbg += "\n";
        mCtx->log(7, "", __LINE__, "",
                "[vle.recursive] launching in dir %s: %s %s",
                mWorkingDir.c_str(), exe.c_str(), messageDbg.c_str());
    }

    bool started = mspawn.start(exe, mWorkingDir, argv);
    if (not started) {
        err.code = -1;
        err.message = vle::utils::format(
                "[MetaManager] Failed to start `%s'", exe.c_str());
        model.reset(nullptr);
        results.reset(nullptr);
        clear();
        return nullptr;
    }
    bool is_success = true;
    std::string message, output, error;
    while (not mspawn.isfinish()) {
        if (mspawn.get(&output, &error)) {
            if (not error.empty() and message.empty()){
                //TODO info such as Context are written into error.
                //                    is_success = false;
                //                    message.assign(error);
            }
            output.clear();
            error.clear();
            std::this_thread::sleep_for(std::chrono::microseconds(200));
        } else {
            break;
        }
    }
    mspawn.wait();
    if (! is_success) {
        err.code = -1;
        err.message = "[vle.recursive] ";
        err.message += message;
        model.reset(nullptr);
        results.reset(nullptr);
        clear();
        return nullptr;
    }
    mspawn.status(&message, &is_success);

    if (! is_success) {
        err.code = -1;
        err.message = "[vle.recursive] ";
        err.message += vle::utils::format("Error launching `%s' : %s ",
                exe.c_str(), message.c_str());
        model.reset(nullptr);
        results.reset(nullptr);
        clear();
        return nullptr;
    }
    //read output file
    std::ifstream outFile;
    outFile.open (tempOutCsv.string(), std::ios_base::in);
    std::string line;

    int inputId = -1;
    std::string viewName;
    std::map<std::string, int> insightsViewRows;
    std::unique_ptr<value::Matrix> viewMatrix;

    std::vector <std::string> tokens;
    bool finishMatrixLine = true;
    bool finishViews = true;
    bool finishIds = false;
    while(not finishIds){
        //read id_1_0
        if (not finishViews) {
            finishViews = true;
            if (not std::getline(outFile,line)) {//read empty line
                break;
            }
        } else {
            std::getline(outFile,line);
        }
        tokens.clear();
        utils::tokenize(line, tokens, "_", false);
        if (tokens.size() != 3 or tokens[0] != "id") {
            finishIds = true;
            break;
        }
        inputId = std::stoi(tokens[1]);
        //inputRepl = std::stoi(tokens[2]);
        finishViews = false;
        while(not finishViews){
            //read view:viewName
            if (not finishMatrixLine) {
                finishMatrixLine = true;
            } else {
                std::getline(outFile,line);
            }
            tokens.clear();
            utils::tokenize(line, tokens, ":", false);
            if (tokens.size() != 2 or tokens[0] != "view") {
                break;
            }
            viewName.assign(tokens[1]);
            //read view header and instantiate matrix view
            std::getline(outFile,line);
            tokens.clear();
            utils::tokenize(line, tokens, " ", true);
            unsigned int nbCols = tokens.size();
            bool getInsight = (insightsViewRows.find(viewName) !=
                    insightsViewRows.end());
            if (not getInsight) {
                insightsViewRows.insert(std::make_pair(viewName,100));
            }
            int rows = insightsViewRows[viewName];
            viewMatrix.reset(new value::Matrix(nbCols,1,nbCols, rows));
            for (unsigned int c=0; c<nbCols; c++){
                viewMatrix->set(c,0,value::String::create(tokens[c]));
            }

            finishMatrixLine = false;
            while(not finishMatrixLine){
                std::getline(outFile,line);
                tokens.clear();
                utils::tokenize(line, tokens, " ", true);
                if (tokens.size() != nbCols) {
                    break;
                }
                viewMatrix->addRow();
                for (unsigned int c=0; c<nbCols; c++){
                    if (c == 0 and tokens[c] == "inf") {
                        viewMatrix->set(c,viewMatrix->rows()-1,
                                value::Double::create(
                                        std::numeric_limits<double>::max()));
                    } else {
                        viewMatrix->set(c,viewMatrix->rows()-1,
                                value::Double::create(
                                        std::stod(tokens[c])));
                    }
                }
            }
            if (not getInsight) {
                insightsViewRows[viewName] = viewMatrix->rows();
            }
            //insert replicate for
            std::unique_ptr<value::Value> aggrValue;
            for (unsigned int o=0; o< mOutputs.size(); o++) {
                VleOutput& outId = *mOutputs[o];
                if (outId.view == viewName) {
                    for (unsigned int p=0; p<viewMatrix->columns(); p++) {
                        if (viewMatrix->getString(p,0) ==
                                outId.absolutePort) {
                            aggrValue = std::move(outId.insertReplicate(
                                    *viewMatrix, inputId, inputSize,
                                    repSize));
                            if (aggrValue) {
                                results->set(outId.id,
                                        std::move(aggrValue));
                            }
                        }
                    }
                }
            }
        }
    }
    if (mRemoveSimulationFiles) {
        tempOutCsv.remove();
        tempInCsv.remove();
        tempVpzPath.remove();
    }

    return results;
}


void
MetaManager::post_propagates(vpz::Vpz& model, const wrapper_init& init)
{
    vpz::Conditions& conds = model.project().experiment().conditions();
    //post propagate
    for (unsigned int i=0; i < mPropagate.size(); i++) {
        VlePropagate& tmp_propagate = *mPropagate[i];
        vpz::Condition& cond = conds.get(tmp_propagate.cond);
        cond.clearValueOfPort(tmp_propagate.port);
        const value::Value& exp = tmp_propagate.value(init);
        cond.addValueToPort(tmp_propagate.port, exp.clone());
    }
}

/******************* Static public functions ****************************/



void
MetaManager::post_inputs(vpz::Vpz& model, const wrapper_init& init)
{
    vpz::Conditions& conds = model.project().experiment().conditions();
    //post replicas
    for (unsigned int i=0; i < mReplicates.size(); i++) {
        VleReplicate& tmp_repl = *mReplicates[i];
        vpz::Condition& condRep = conds.get(tmp_repl.cond);
        condRep.clearValueOfPort(tmp_repl.port);
        for (unsigned int i=0; i < inputsSize(); i++) {
            for (unsigned int k=0; k < replicasSize(); k++) {
                const value::Value& exp = tmp_repl.values(init);
                switch(exp.getType()) {
                case value::Value::SET:
                    condRep.addValueToPort(tmp_repl.port,
                            exp.toSet().get(k)->clone());
                    break;
                case value::Value::TUPLE:
                    condRep.addValueToPort(tmp_repl.port,
                            value::Double::create(exp.toTuple().at(k)));
                    break;
                default:
                    //error already detected
                    break;
                }
            }
        }
    }
    //post inputs
    for (unsigned int i=0; i < mInputs.size(); i++) {
        VleInput& tmp_input = *mInputs[i];
        vpz::Condition& cond = conds.get(tmp_input.cond);
        cond.clearValueOfPort(tmp_input.port);
        const value::Value& exp = tmp_input.values(init);

        switch (exp.getType()) {
        case value::Value::TUPLE: {
            const value::Tuple& tmp_val_tuple = exp.toTuple();
            for (unsigned j=0; j < tmp_val_tuple.size(); j++) {//TODO maxExpe
                double tmp_j = tmp_val_tuple[j];
                for (unsigned int k=0; k < replicasSize(); k++) {
                    cond.addValueToPort(tmp_input.port,
                            value::Double::create(tmp_j));
                }
            }
            break;
        } case value::Value::SET: {
            const value::Set& tmp_val_set =exp.toSet();
            for (unsigned j=0; j < tmp_val_set.size(); j++) {//TODO maxExpe
                const value::Value& tmp_j = *tmp_val_set.get(j);
                for (unsigned int k=0; k < replicasSize(); k++) {
                    cond.addValueToPort(tmp_input.port,
                            tmp_j.clone());
                }
            }
            break;
        } default: {
            cond.addValueToPort(tmp_input.port,exp.clone());
            break;
        }}
    }
}

void
MetaManager::split(std::vector<std::string>& elems, const std::string &s,
        char delim)
{
    std::stringstream ss(s);
    std::string item;
    while (std::getline(ss, item, delim)) {
        elems.push_back(item);
    }
}

bool
MetaManager::parseInput(const std::string& conf,
        std::string& cond, std::string& port,
        const std::string& prefix)
{
    std::string varname;
    cond.clear();
    port.clear();
    if (prefix.size() > 0) {
        if (conf.compare(0, prefix.size(), prefix) != 0) {
            return false;
        }
        varname.assign(conf.substr(prefix.size(), conf.size()));
    } else {
        varname.assign(conf);
    }
    std::vector <std::string> splvec;
    MetaManager::split(splvec, varname, '.');
    if (splvec.size() != 2) {
        return false;
    }
    cond.assign(splvec[0]);
    port.assign(splvec[1]);
    return not cond.empty() and not port.empty();
}

bool
MetaManager::parseOutput(const std::string& conf, std::string& idout)
{
    idout.clear();
    std::string prefix = "output_";
    if (conf.compare(0, prefix.size(), prefix) != 0) {
        return false;
    }
    idout.assign(conf.substr(prefix.size(), conf.size()));
    return not idout.empty();
}


std::unique_ptr<value::Tuple>
MetaManager::valuesFromDistrib(const value::Map& distrib, utils::Rand& rn)
{

    if (distrib.exist("distribution")
            and distrib.get("distribution")->isString()) {
        if (distrib.getString("distribution") == "uniform") {
            if (distrib.exist("nb") and distrib.exist("min")
                    and distrib.exist("max")
                    and distrib.get("nb")->isInteger()
                    and distrib.get("min")->isDouble()
                    and distrib.get("max")->isDouble()) {
                std::unique_ptr<value::Tuple> res(new value::Tuple(
                        distrib.getInt("nb")));
                double min = distrib.getDouble("min");
                double max = distrib.getDouble("max");
                std::vector<double>::iterator itb = res->value().begin();
                std::vector<double>::iterator ite = res->value().end();
                for (; itb != ite; itb++) {
                    *itb = rn.getDouble(min, max);
                }
                return std::move(res);
            }
        }
    }
    return nullptr;
}

void
MetaManager::clear()
{
    mPropagate.clear();
    mInputs.clear();
    mReplicates.clear();
    mOutputs.clear();
    mOutputValues.clear();
}

}}//namespaces
