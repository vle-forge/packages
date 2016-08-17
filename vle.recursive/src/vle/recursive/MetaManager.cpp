
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
#include <sstream>
#include <cmath>
#include <string>

#include <vle/vpz/Vpz.hpp>
#include <vle/utils/Exception.hpp>
#include <vle/utils/Package.hpp>
#include <vle/utils/Spawn.hpp>
#include <vle/manager/Simulation.hpp>
#include <vle/manager/Manager.hpp>
#include <vle/value/Double.hpp>
#include <vle/value/Null.hpp>
#include <vle/value/Table.hpp>

#include <vle/reader/vle_results_text_reader.hpp>


#include "MetaManager.hpp"
#include "VleAPIfacilities.hpp"


namespace vle {
namespace recursive {

void MetaManager::split(std::vector<std::string>& elems, const std::string &s,
        char delim)
{
    std::stringstream ss(s);
    std::string item;
    while (std::getline(ss, item, delim)) {
        elems.push_back(item);
    }
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

const value::Value&
VlePropagate::value(const value::Map& init)
{
    std::string key("propagate_");
    key.append(getName());
    return *init.get(key);
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
        const value::Value& val):
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
    default:
        nbValues = 1;
        break;
    }
}

VleInput::~VleInput()
{
}

const value::Value&
VleInput::values(const value::Map& init)
{
    std::string key("input_");
    key.append(getName());
    return *init.get(key);
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
        const value::Value& val):
                cond(_cond), port(_port), nbValues(0)
{
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
    default:
        throw utils::ArgError(utils::format(
                "[MetaManager] : error in configuration of "
                "'replicate_%s.%s', expect a value::Set or Tuple",
                cond.c_str(),  port.c_str()));
        break;
    }
}

VleReplicate::~VleReplicate()
{
}

const value::Value&
VleReplicate::values(const value::Map& init)
{
    std::string key("replicate_");
    key.append(getName());
    return *init.get(key);
}

std::string
VleReplicate::getName()
{
    std::string ret = cond;
    ret.append(".");
    ret.append(port);
    return ret;
}

/***********VleOutput*************/

VleOutput::VleOutput() :
   id(), view(), absolutePort(), integrationType(LAST),
   replicateAggregationType(S_mean), inputAggregationType(S_at),
   mse_times(nullptr), mse_observations(nullptr), mreplicateAccuMono(nullptr),
   mreplicateAccuMulti(nullptr), minputAccuMono(nullptr),
   minputAccuMulti(nullptr), mreplicateInserter(nullptr),
   minputInserter(nullptr), manageDoubleValue(true)
{
}

VleOutput::VleOutput(const std::string& _id,
        const value::Value& val) :
   id(_id), view(), absolutePort(), integrationType(LAST),
   replicateAggregationType(S_mean), inputAggregationType(S_at),
   mse_times(nullptr), mse_observations(nullptr), mreplicateAccuMono(nullptr),
   mreplicateAccuMulti(nullptr), minputAccuMono(nullptr),
   minputAccuMulti(nullptr), mreplicateInserter(nullptr),
   minputInserter(nullptr), manageDoubleValue(true)
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

void
VleOutput::insertReplicate(value::Map& result,
        bool initReplicateAccu, bool initInputAccu, unsigned int nbInputs,
        unsigned int nbReplicates)
{
    value::Map::iterator it = result.find(view);
    if (it == result.end()) {
        throw vu::ArgError(utils::format(
                "[MetaManager] view '%s' not found)",
                view.c_str()));
    }
    value::Matrix& outMat = value::toMatrixValue(*it->second);
    insertReplicate(outMat, initReplicateAccu, initInputAccu, nbInputs,
            nbReplicates);
}

void
VleOutput::insertReplicate(value::Matrix& outMat,
        bool initReplicateAccu, bool initInputAccu, unsigned int nbInputs,
        unsigned int nbReplicates)
{

    //performs some checks on output matrix
    if (outMat.rows() < 2){
        throw vu::ArgError("[MetaManager] expect at least 2 rows");
    }
    //get col index
    unsigned int colIndex = 9999;
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

    //performs some check on initialization
    if (initInputAccu and not outMat.get(colIndex,1)->isDouble()) {
        //replica size has to be 1
        if (nbReplicates != 1){
            throw vu::ArgError(utils::format(
                    "[MetaManager] since data is not double no replicate "
                    "aggregation is possible for output '%s'",
                    id.c_str()));
        }
        //integration should be ALL or LAST
        if (integrationType != ALL and integrationType != LAST) {
            throw vu::ArgError(utils::format(
                    "[MetaManager] integration for output '%s' should be all"
                    "or last in order to manage not double values ",
                    id.c_str()));
        }
        //aggregation should be S_at
        if (inputAggregationType != S_at) {
            throw vu::ArgError(utils::format(
                    "[MetaManager] aggregation_input for output '%s' "
                    "should be all in order to manage not double values ",
                    id.c_str()));
        }
        manageDoubleValue = false;
    }

    //Initialization of replicate accu
    if (initReplicateAccu) {
        mreplicateAccuMono.reset(nullptr);
        mreplicateAccuMulti.reset(nullptr);
        mreplicateInserter.reset(nullptr);
        if (manageDoubleValue) {
            if(integrationType != ALL) {
                mreplicateAccuMono.reset(
                        new AccuMono(replicateAggregationType));
            } else {
                mreplicateAccuMulti.reset(
                        new AccuMulti(replicateAggregationType));
            }
        } //else integration and aggregation is performed directly in the
          //input accu initialization

    }
    //Initialization of initInput accu
    if (initInputAccu) {
        minputAccuMono.reset(nullptr);
        minputAccuMulti.reset(nullptr);
        minputInserter.reset(nullptr);
        if (not manageDoubleValue) {
            if(integrationType == ALL) {
                minputInserter.reset(new value::Set(outMat.rows()-1));
                for (unsigned int i=1; i < outMat.rows(); i++) {
                    minputInserter->toSet().set(i-1,
                            std::move(outMat.give(colIndex, i)));
                }
            } else {//integration is LAST
                minputInserter = std::move(
                        outMat.give(colIndex, outMat.rows()-1));
            }
            return ;//management of non double values is performed
        } else if (integrationType != ALL) {
            if (inputAggregationType != S_at) {
                minputAccuMono.reset(new AccuMono(inputAggregationType));
            } else {
                minputInserter.reset(new value::Table(nbInputs, 1));
            }
        } else {
            if (inputAggregationType != S_at) {
                minputAccuMulti.reset(new AccuMulti(inputAggregationType));
                minputInserter.reset(
                        new value::Table(1, outMat.rows()-1));
            } else {
                minputInserter.reset(
                        new value::Table(nbInputs, outMat.rows()-1));
            }
        }
    }

    //start insertion for double management only
    switch(integrationType) {
    case MAX: {
        double max = -9999;
        for (unsigned int i=1; i < outMat.rows(); i++) {
            double v = outMat.getDouble(colIndex, i);
            if (v > max) {
                max = v;
            }
        }
        mreplicateAccuMono->insert(max);
        break;
    } case LAST: {
        const std::unique_ptr<value::Value>& res =
                outMat.get(colIndex, outMat.rows() - 1);
        mreplicateAccuMono->insert(res->toDouble().value());
        break;
    } case MSE: {
        double sum_square_error = 0;
        double nbVal = 0;
        for (unsigned int i=0; i< mse_times->size(); i++) {
            int t = std::floor(mse_times->at(i));
            if (t > 0 and t< (int) outMat.rows()) {
                sum_square_error += std::pow((outMat.getDouble(colIndex,t)
                         - mse_observations->at(i)), 2);
                nbVal++;
            }
        }
        mreplicateAccuMono->insert(sum_square_error/nbVal);
        break;
    } case ALL:{
        mreplicateAccuMulti->insertColumn(outMat, colIndex);
        break;
    }}
}


void
VleOutput::insertInput(unsigned int currentInput)
{
    if (not manageDoubleValue) {
        return;
    }
    if (integrationType != ALL) {
        if (inputAggregationType != S_at) {
            minputAccuMono->insert(
                    mreplicateAccuMono->getStat(replicateAggregationType));
        } else {
            minputInserter->toTable()(currentInput, 0) =
                    mreplicateAccuMono->getStat(replicateAggregationType);
        }
    } else {
        if (inputAggregationType != S_at) {
            minputAccuMulti->insertAccuStat(*mreplicateAccuMulti,
                                            replicateAggregationType);
        } else {
            mreplicateAccuMulti->fillStat(minputInserter->toTable(),
                    currentInput, replicateAggregationType);
        }
    }
    mreplicateAccuMono.reset(nullptr);
    mreplicateAccuMulti.reset(nullptr);
    mreplicateInserter.reset(nullptr);
}


std::unique_ptr<value::Value>
VleOutput::buildAggregateResult()
{
    if (not manageDoubleValue) {
        return std::move(minputInserter);
    }
    std::unique_ptr<value::Value> res;
    if (integrationType != ALL) {
        if (inputAggregationType != S_at) {
            res.reset(new value::Double(
                    minputAccuMono->getStat(inputAggregationType)));
        } else {
            res = std::move(minputInserter);
        }
    } else {
        if (inputAggregationType != S_at) {
            minputAccuMulti->fillStat(minputInserter->toTable(),
                    0, inputAggregationType);
        }
        res = std::move(minputInserter);
    }
    minputAccuMono.reset(nullptr);
    minputAccuMulti.reset(nullptr);
    minputInserter.reset(nullptr);
    return std::move(res);
}
/***********MetaManager*************/

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
    if (not mReplicate) {
        return 1;
    }
    return mReplicate->nbValues;
}



MetaManager::MetaManager(): mIdVpz(), mIdPackage(),
        mConfigParallelType(SINGLE), mRemoveSimulationFiles(true),
        mConfigParallelNbSlots(1), mConfigParallelMaxExpes(1),
        mexpe_debug(true), mPropagate(), mInputs(), mReplicate(nullptr),
        mOutputs(), mWorkingDir(""), mCtx(utils::make_context())
{

}

MetaManager::~MetaManager()
{
    clear();
}

std::unique_ptr<value::Map>
MetaManager::run(const value::Map& init)
{
    clear();

    if (init.exist("config_parallel_type")) {
        std::string tmp;
        tmp.assign(init.getString("config_parallel_type"));
        if (tmp == "threads") {
            mConfigParallelType = THREADS;
        } else if (tmp == "mvle") {
            mConfigParallelType = MVLE;
            if (! init.exist("working_dir")) {
                throw utils::ArgError("[MetaManager] error for "
                        "mvle config, missing 'working_dir' parameter");
            }
            mWorkingDir.assign(init.getString("working_dir"));
        }  else if (tmp == "single") {
            mConfigParallelType = SINGLE;
        } else {
            throw utils::ArgError("[MetaManager] error for "
                    "configuration type of parallel process");

        }
    }
    if (init.exist("config_parallel_nb_slots")) {
        int tmp;
        tmp = init.getInt("config_parallel_nb_slots");
        if (tmp > 0) {
            mConfigParallelNbSlots = tmp;
        } else {
            throw utils::ArgError("[MetaManager] error for "
                 "configuration type of parallel nb slots)");
        }
    }
    if (init.exist("config_parallel_max_expes")) {
        int tmp;
        tmp = init.getInt("config_parallel_max_expes");
        if (tmp > 0) {
            if ((unsigned int) tmp < mConfigParallelNbSlots) {
                throw utils::ArgError(utils::format(
                        "[MetaManager] error for configuration type of parallel"
                        " max expes, got '%i'which is less than nb slots:'%i'",
                        tmp,  mConfigParallelNbSlots));
            }
            mConfigParallelMaxExpes = tmp;
        } else {
            throw utils::ArgError(utils::format(
                    "[MetaManager] error for configuration type of parallel "
                    "max expes, got '%i'", tmp));
        }
    }
    if (init.exist("expe_debug")) {
        mexpe_debug = init.getBoolean("expe_debug");
    }
    if (init.exist("config_parallel_rm_files")) {
        mRemoveSimulationFiles = init.getBoolean("config_parallel_rm_files");
    }
    if (init.exist("package")) {
        mIdPackage = init.getString("package");
    } else {
        throw utils::ArgError("[MetaManager] missing 'package'");
    }
    if (init.exist("vpz")) {
        mIdVpz = init.getString("vpz");
    } else {
        throw utils::ArgError("[MetaManager] missing 'vpz'");
    }

    std::string in_cond;
    std::string in_port;
    std::string out_id;
    value::Map::const_iterator itb = init.begin();
    value::Map::const_iterator ite = init.end();
    for (; itb != ite; itb++) {
        const std::string& conf = itb->first;
        if (MetaManager::parseInput(conf, in_cond, in_port, "propagate_")) {
            mPropagate.emplace_back(new VlePropagate(in_cond, in_port));
        } else if (MetaManager::parseInput(conf, in_cond, in_port)) {
            mInputs.emplace_back(new VleInput(
                    in_cond, in_port, *itb->second));
        } else if (MetaManager::parseInput(conf, in_cond, in_port,
                                           "replicate_")){
            if (not mReplicate == 0) {
                throw utils::ArgError(utils::format(
                        "[MetaManager] : the replica is already defined with "
                        " '%s'", mReplicate->getName().c_str()));
            }
            mReplicate.reset(new VleReplicate(in_cond, in_port, *itb->second));
        } else if (MetaManager::parseOutput(conf, out_id)){
            mOutputs.emplace_back(new VleOutput(out_id, *itb->second));
        }
    }
    //check
    unsigned int initSize = 0;;
    for (unsigned int i = 0; i< mInputs.size(); i++) {
        const VleInput& vleIn = *mInputs[i];
        //check size which has to be consistent
        if (initSize == 0 and vleIn.nbValues > 1) {
            initSize = vleIn.nbValues;
        } else {
            if (vleIn.nbValues > 1 and initSize > 0
                    and initSize != vleIn.nbValues) {
                throw utils::ArgError(utils::format(
                        "[MetaManager]: error in input values: wrong number"
                        " of values 1st input has %u values, %u -th input "
                        "has %u values",
                        initSize, i, vleIn.nbValues));
            }
        }
        //check if already exist in replicate or propagate
        if (mReplicate and (mReplicate->getName() == vleIn.getName())) {
            throw utils::ArgError(utils::format(
                    "[MetaManager]: error input '%s' is also the replicate",
                     vleIn.getName().c_str()));
        }
        for (unsigned int j=0; j<mPropagate.size(); j++) {
            const VlePropagate& vleProp = *mPropagate[j];
            if (vleProp.getName() == vleIn.getName()) {
                throw utils::ArgError(utils::format(
                    "[MetaManager]: error input '%s' is also a propagate",
                    vleIn.getName().c_str()));
            }
        }
    }
    return runIntern(init);
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

std::unique_ptr<value::Map>
MetaManager::runIntern(const value::Map& init)
{
    if (mexpe_debug){
        mCtx->log(1, __FILE__, __LINE__, __FUNCTION__,
                "[vle.recursive] run intern entrance \n");
    }
    utils::Package pkg(mCtx, mIdPackage);
    std::string vpzFile = pkg.getExpFile(mIdVpz, utils::PKG_BINARY);
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
            case THREADS: {
                VleAPIfacilities::changePlugin(*model, (*itb)->view, "storage");
                break;
            } case MVLE: {
                VleAPIfacilities::changePlugin(*model, (*itb)->view, "file");
                break;
            }
        }
    }

    unsigned int inputSize = inputsSize();
    unsigned int repSize = replicasSize();
    unsigned int outputSize =  mOutputs.size();
    postInputsIntern(*model, init);


    //build output matrix with header
    std::unique_ptr<value::Map> results(new value::Map());
    for (unsigned int j=0; j<outputSize;j++) {
        results->add(mOutputs[j]->id, value::Null::create());
    }


    switch(mConfigParallelType) {
    case SINGLE:
    case THREADS: {

        std::ofstream outstream(mCtx->getHomeFile("metamanager.log").string());
        manager::Manager planSimulator(
                mCtx,
                manager::LOG_NONE,
                manager::SIMULATION_NONE,
                &outstream);
        manager::Error manerror;

        if (mexpe_debug){
            mCtx->log(1, __FILE__, __LINE__, __FUNCTION__,
                    "[vle.recursive] simulation single/threads(%d slots) "
                    "nb simus: %u \n", mConfigParallelNbSlots,
                    (repSize*inputSize));
        }

//        //for dbg
//        std::string tempvpzPath = pkg.getExpDir(vu::PKG_BINARY);
//        tempvpzPath.append("/temp_saved.vpz");
//        model->write(tempvpzPath);
//        //
        std::unique_ptr<value::Matrix> output_mat =  planSimulator.run(
                std::move(model), mConfigParallelNbSlots, 0, 1, &manerror);
        if (mexpe_debug){
            mCtx->log(1, __FILE__, __LINE__, __FUNCTION__,
                    "[vle.recursive] end simulation single/threads\n");
        }
        if (manerror.code != 0) {
            throw utils::InternalError(utils::format(
                    "Error in MetaManager '%s'",
                    manerror.message.c_str()));
        }

        for (unsigned int out =0; out < outputSize; out++) {
            VleOutput& outId = *mOutputs[out];
            for (unsigned int i = 0; i < inputSize*repSize; i+= repSize) {
                for (unsigned int j = 0; j < repSize; j++) {
                    outId.insertReplicate(output_mat->get(i+j,0)->toMap(),
                            j==0, i==0, inputSize, repSize);
                }
                outId.insertInput(i % inputSize);
            }
            results->set(outId.id, std::move(outId.buildAggregateResult()));
        }
        if (mexpe_debug){
            mCtx->log(1, __FILE__, __LINE__, __FUNCTION__,
                    "[vle.recursive] aggregation finished\n");
        }
        return results;
        break;
    } case MVLE: {

        vu::Package pkg(mCtx, "vle.recursive");//TODO should be saved outside the rr package
        std::string tempvpzPath = pkg.getExpDir(vu::PKG_BINARY);
        tempvpzPath.append("/temp_gen_MPI.vpz");
        model->write(tempvpzPath);
        vu::Spawn mspawn(mCtx);
        std::string exe = mCtx->findProgram("mpirun").string();

        std::vector < std::string > argv;
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

        if (mexpe_debug){
            mCtx->log(1, __FILE__, __LINE__, __FUNCTION__,
                    "[vle.recursive] simulation mvle %d \n",
                    mConfigParallelNbSlots);
        }
        bool started = mspawn.start(exe, mWorkingDir, argv);
        if (not started) {
            throw vu::ArgError(utils::format(
                    "Failed to start `%s'", exe.c_str()));
        }
        std::string message;
        bool is_success = true;
        mspawn.wait();
        mspawn.status(&message, &is_success);

        if (! is_success) {
            throw vu::ArgError(utils::format(
                    "Error launching `%s' : %s ",
                    exe.c_str(), message.c_str()));
        }

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
                    reader::VleResultsTextReader tfr(vleResultFilePath);
                    value::Matrix mat;
                    tfr.readFile(mat);
                    outId.insertReplicate(mat, j==0, i==0, inputSize, repSize);
                    if (mRemoveSimulationFiles and out == (outputSize-1)) {
                        utils::Path torm(vleResultFilePath.c_str());
                        torm.remove();
                    }
                }
                outId.insertInput(i % inputSize);
            }
            results->set(outId.id, std::move(outId.buildAggregateResult()));
        }
        if (mexpe_debug){
            mCtx->log(1, __FILE__, __LINE__, __FUNCTION__,
                    "[vle.recursive] aggregation finished \n");
        }
        return results;
        break;
    }}
    return 0;
}


void
MetaManager::postInputsIntern(vpz::Vpz& model,
        const value::Map& init)
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
    //post replicas
    if (mReplicate) {
        vpz::Condition& condRep = conds.get(mReplicate->cond);
        condRep.clearValueOfPort(mReplicate->port);
        for (unsigned int i=0; i < inputsSize(); i++) {
            for (unsigned int k=0; k < replicasSize(); k++) {
                const value::Value& exp = mReplicate->values(init);
                switch(exp.getType()) {
                case value::Value::SET:
                    condRep.addValueToPort(mReplicate->port,
                            exp.toSet().get(k)->clone());
                    break;
                case value::Value::TUPLE:
                    condRep.addValueToPort(mReplicate->port,
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
                if (not mReplicate) {
                    cond.addValueToPort(tmp_input.port,
                            value::Double::create(tmp_j));
                } else {
                    for (unsigned int k=0; k < replicasSize(); k++) {
                        cond.addValueToPort(tmp_input.port,
                                value::Double::create(tmp_j));
                    }
                }
            }
            break;
        } case value::Value::SET: {
            const value::Set& tmp_val_set =exp.toSet();
            for (unsigned j=0; j < tmp_val_set.size(); j++) {//TODO maxExpe
                const value::Value& tmp_j = *tmp_val_set.get(j);
                if (not mReplicate) {
                    cond.addValueToPort(tmp_input.port,tmp_j.clone());
                } else {
                    for (unsigned int k=0; k < replicasSize(); k++) {
                        cond.addValueToPort(tmp_input.port,
                                tmp_j.clone());
                    }
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
MetaManager::clear()
{
    mInputs.clear();
    mOutputs.clear();
}

}}//namespaces
