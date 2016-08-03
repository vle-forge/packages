
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

#include <vle/reader/vle_results_text_reader.hpp>


#include "MetaManager.hpp"
#include "VleAPIfacilities.hpp"


namespace vle {
namespace recursive {

void MetaManager::split(std::vector<std::string>& elems, const std::string &s, char delim)
{
    std::stringstream ss(s);
    std::string item;
    while (std::getline(ss, item, delim)) {
        elems.push_back(item);
    }
}

VleInput::VleInput(const std::string& _cond, const std::string& _port,
        const vle::value::Value& val):
                cond(_cond), port(_port), type(MONO), nbValues(0)
{
    if (cond.empty() or port.empty()) {
        throw vle::utils::ArgError(vle::utils::format(
                "[MetaManager] : the input has wrong form: '%s.%s'",
                cond.c_str(),  port.c_str()));
    }
    switch (val.getType()) {
        case vle::value::Value::TUPLE:
            nbValues = val.toTuple().size();
            type = MULTI;
            break;
        case vle::value::Value::SET:
            nbValues = val.toSet().size();
            type = MULTI;
            break;
        default:
            nbValues = 1;
            type = MONO;
            break;
        }
}

VleInput::~VleInput()
{
}

const vle::value::Value&
VleInput::values(const vle::value::Map& init, bool replicate)
{
    std::string key("");
    if (replicate) {
        key.append("replicate_");
    } else {
        key.append("input_");
    }
    key.append(getName());
    return *init.get(key);
}

std::string
VleInput::getName()
{
    std::string ret = cond;
    ret.append(".");
    ret.append(port);
    return ret;
}

/************************/

VleOutput::VleOutput() :
   id(), view(), absolutePort(), integrationType(LAST), aggregationType(MEAN),
   mse_times(nullptr), mse_observations(nullptr), maccuMono(nullptr),
   maccuMulti(nullptr), res_value(nullptr)
{
}

VleOutput::VleOutput(const std::string& _id,
        const vle::value::Value& val) :
   id(_id), view(), absolutePort(), integrationType(LAST),
   aggregationType(MEAN), mse_times(nullptr), mse_observations(nullptr),
   maccuMono(nullptr), maccuMulti(nullptr), res_value(nullptr)
{
    std::string tmp;
    if (val.isString()) {
        if (not parsePath(val.toString().value())) {
            throw vle::utils::ArgError(vle::utils::format(
                    "[MetaManager] : error in configuration of the output "
                    "'output_%s' with a string; got: '%s'",
                    id.c_str(),  val.toString().value().c_str()));
        }
    } else if (val.isMap()) {
        const vle::value::Map& m = val.toMap();
        bool error = false;
        if (m.exist("path")) {
            error = not parsePath(m.getString("path"));
        }
        if (m.exist("aggregation")) {
            tmp = m.getString("aggregation");
            if (tmp == "mean") {
                aggregationType = MEAN;
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
                mse_times.reset(new vle::value::Tuple(m.getTuple("mse_times")));
                mse_observations.reset(new vle::value::Tuple(
                        m.getTuple("mse_observations")));
                error = mse_times->size() != mse_observations->size();
            }
        }
        if (error) {
            throw vle::utils::ArgError(vle::utils::format(
                    "[MetaManager] : error in configuration of the output "
                    " '%s%s' with a map",
                    "id_output_",  id.c_str()));
        }
    }
    initAggregateResult();
}

VleOutput::VleOutput(const VleOutput& vleOutput):
      id(vleOutput.id), view(vleOutput.view),
      absolutePort(vleOutput.absolutePort),
      integrationType(vleOutput.integrationType),
      aggregationType(vleOutput.aggregationType), mse_times(nullptr),
      mse_observations(nullptr), maccuMono(nullptr), maccuMulti(nullptr),
      res_value(nullptr)
{
    if (vleOutput.mse_times) {
        mse_times.reset(new value::Tuple(vleOutput.mse_times->toTuple()));
    }
    if (vleOutput.mse_observations) {
        mse_observations.reset(new value::Tuple(
                vleOutput.mse_observations->toTuple()));
    }
    if (vleOutput.maccuMono) {
        maccuMono.reset(new AccuMono(*vleOutput.maccuMono));
    }
    if (vleOutput.maccuMulti) {
        maccuMulti.reset(new AccuMulti(*vleOutput.maccuMulti));
    }
    if (vleOutput.res_value) {
        res_value = vleOutput.res_value->clone();
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
VleOutput::insertReplicate(const vle::value::Map& result)
{
    vle::value::Map::const_iterator it = result.find(view);
    if (it == result.end()) {
        throw vu::ArgError(vle::utils::format(
                "[MetaManager] view '%s' not found)",
                view.c_str()));
    }
    const value::Matrix& outMat = value::toMatrixValue(*it->second);
    insertReplicate(outMat);
}

void
VleOutput::insertReplicate(const vle::value::Matrix& outMat)
{
    unsigned int colIndex = 9999;
    for (unsigned int i=0; i < outMat.columns(); i++) {
        if (outMat.getString(i,0) == absolutePort) {
            colIndex = i;
        }
    }
    if (colIndex == 9999) {
        throw vu::ArgError(vle::utils::format(
                "[MetaManager] view.port '%s' not found)",
                absolutePort.c_str()));
    }
    switch(integrationType) {
    case MAX: {
        double max = -9999;
        for (unsigned int i=1; i < outMat.rows(); i++) {
            double v = outMat.getDouble(colIndex, i);
            if (v > max) {
                max = v;
            }
        }
        maccuMono->insert(max);
        break;
    } case LAST: {
        const std::unique_ptr<vle::value::Value>& res =
                outMat.get(colIndex, outMat.rows() - 1);
        if (res->isDouble()) {
            maccuMono->insert(res->toDouble().value());
        } else {
            res_value = std::move(res->clone());
        }
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
        maccuMono->insert(sum_square_error/nbVal);
        break;
    } case ALL:{
        maccuMulti->insertColumn(outMat, colIndex);
        break;
    }}
}

void
VleOutput::initAggregateResult()
{
    switch(integrationType) {
    case LAST:
    case MAX:
    case MSE: {
        maccuMono.reset(new AccuMono(aggregationType));
        break;
    } case ALL: {
        maccuMulti.reset(new AccuMulti(aggregationType));
    }}
}

std::unique_ptr<vle::value::Value>
VleOutput::buildAggregateResult()
{
    switch (aggregationType) {
    case MEAN: {
        switch (integrationType) {
        case LAST:
        case MAX:
        case MSE: {
            if (res_value) {
                return res_value->clone();
            } else {
                return vle::value::Double::create(maccuMono->mean());
            }
            break;
        } case ALL: {
            std::unique_ptr<vle::value::Tuple> res(
                    new value::Tuple(maccuMulti->size()));
            maccuMulti->mean(*res);
            return res;
        }}
        break;
    } default: {
        throw "error";
        break;
    }}
    return nullptr;
}

/************************/

unsigned int
MetaManager::inputsSize() const
{
    for (unsigned int i=0; i < mInputs.size(); i++) {
        if (mInputs[i]->type == MULTI) {
            return mInputs[i]->nbValues;
        }
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
        mexpe_debug(true), mInputs(), mReplicate(nullptr), mOutputs(),
        mWorkingDir(""), mCtx(utils::make_context())
{

}

MetaManager::~MetaManager()
{
    clear();
}

std::unique_ptr<vle::value::Matrix>
MetaManager::run(const vle::value::Map& init)
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
                throw vle::utils::ArgError("[MetaManager] error for "
                        "mvle config, missing 'working_dir' parameter");
            }
            mWorkingDir.assign(init.getString("working_dir"));
        }  else if (tmp == "single") {
            mConfigParallelType = SINGLE;
        } else {
            throw vle::utils::ArgError("[MetaManager] error for "
                    "configuration type of parallel process");

        }
    }
    if (init.exist("config_parallel_nb_slots")) {
        int tmp;
        tmp = init.getInt("config_parallel_nb_slots");
        if (tmp > 0) {
            mConfigParallelNbSlots = tmp;
        } else {
            throw vle::utils::ArgError("[MetaManager] error for "
                 "configuration type of parallel nb slots)");
        }
    }
    if (init.exist("config_parallel_max_expes")) {
        int tmp;
        tmp = init.getInt("config_parallel_max_expes");
        if (tmp > 0) {
            if ((unsigned int) tmp < mConfigParallelNbSlots) {
                throw vle::utils::ArgError(vle::utils::format(
                        "[MetaManager] error for configuration type of parallel"
                        " max expes, got '%i'which is less than nb slots:'%i'",
                        tmp,  mConfigParallelNbSlots));
            }
            mConfigParallelMaxExpes = tmp;
        } else {
            throw vle::utils::ArgError(vle::utils::format(
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
        throw vle::utils::ArgError("[MetaManager] missing 'package'");
    }
    if (init.exist("vpz")) {
        mIdVpz = init.getString("vpz");
    } else {
        throw vle::utils::ArgError("[MetaManager] missing 'vpz'");
    }

    std::string in_cond;
    std::string in_port;
    std::string out_id;
    vle::value::Map::const_iterator itb = init.begin();
    vle::value::Map::const_iterator ite = init.end();
    for (; itb != ite; itb++) {
        const std::string& conf = itb->first;
        if (MetaManager::parseInput(conf, in_cond, in_port)) {
            mInputs.emplace_back(new VleInput(
                    in_cond, in_port, *itb->second));
        } else if (MetaManager::parseInput(conf, in_cond, in_port, "replicate_")){
            if (not mReplicate == 0) {
                throw vle::utils::ArgError(vle::utils::format(
                        "[MetaManager] : the replica is already defined with "
                        " '%s'", mReplicate->getName().c_str()));
            }
            mReplicate.reset(new VleInput(in_cond, in_port, *itb->second));
        } else if (MetaManager::parseOutput(conf, out_id)){
            mOutputs.push_back(VleOutput(out_id, *itb->second));
        }
    }
    //check
    if (mInputs.size() == 0) {
        throw vle::utils::ArgError("[MetaManager] : error no inputs");
    }
    unsigned int initSize = 0;;
    for (unsigned int i = 0; i< mInputs.size(); i++) {
        const VleInput& vleIn = *mInputs[0];
        if (vleIn.type == MULTI ) {
            if (initSize == 0) {
                initSize = vleIn.nbValues;
            } else {
                if (initSize != vleIn.nbValues) {
                    throw vle::utils::ArgError(vle::utils::format(
                            "[MetaManager]: error in input values: wrong number"
                            " of values 1st input has %u values, %u -th input "
                            "has %u values",
                            initSize, i, vleIn.nbValues));
                }
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

std::unique_ptr<vle::value::Matrix>
MetaManager::runIntern(const vle::value::Map& init)
{
    if (mexpe_debug){
        mCtx->log(1, __FILE__, __LINE__, __FUNCTION__,
                "[vle.recursive] run intern entrance \n");
    }
    vle::utils::Package pkg(mCtx, mIdPackage);
    std::string vpzFile = pkg.getExpFile(mIdVpz, vle::utils::PKG_BINARY);
    std::unique_ptr<vpz::Vpz> model(new vpz::Vpz(vpzFile));
    model->project().experiment().setCombination("linear");

    VleAPIfacilities::changeAllPlugin(*model, "dummy");
    std::vector<VleOutput>::const_iterator itb= mOutputs.begin();
    std::vector<VleOutput>::const_iterator ite= mOutputs.end();
    for (; itb != ite; itb++) {
        switch(mConfigParallelType) {
            case SINGLE:
            case THREADS: {
                VleAPIfacilities::changePlugin(*model, itb->view, "storage");
                break;
            } case MVLE: {
                VleAPIfacilities::changePlugin(*model, itb->view, "file");
                break;
            }
        }
    }

    unsigned int inputSize = inputsSize();
    unsigned int repSize = replicasSize();
    unsigned int outputSize =  mOutputs.size();
    postInputsIntern(*model, init);


    //build output matrix with header
    std::unique_ptr<vle::value::Matrix> results(
            new vle::value::Matrix(outputSize, inputSize+1, 1, 1));
    for (unsigned int j=0; j<outputSize;j++) {
        results->set(j,0, std::unique_ptr<value::String>(
                new vle::value::String(mOutputs[j].id)));
    }


    switch(mConfigParallelType) {
    case SINGLE:
    case THREADS: {

        std::ofstream outstream(mCtx->getHomeFile("metamanager.log").string());
        vle::manager::Manager planSimulator(
                mCtx,
                vle::manager::LOG_NONE,
                vle::manager::SIMULATION_NONE,
                &outstream);
        vle::manager::Error manerror;

        if (mexpe_debug){
            mCtx->log(1, __FILE__, __LINE__, __FUNCTION__,
                    "[vle.recursive] simulation single/threads %d \n",
                    mConfigParallelNbSlots);
        }

        std::unique_ptr<vle::value::Matrix> output_mat =  planSimulator.run(
                std::move(model), mConfigParallelNbSlots, 0, 1, &manerror);
        if (mexpe_debug){
            mCtx->log(1, __FILE__, __LINE__, __FUNCTION__,
                    "[vle.recursive] end simulation single/threads\n");
        }
        if (manerror.code != 0) {
            throw vle::utils::InternalError(vle::utils::format(
                    "Error in MetaManager '%s'",
                    manerror.message.c_str()));
        }

        for (unsigned int out =0; out < outputSize; out++) {
            VleOutput& outId = mOutputs[out];
            for (unsigned int i = 0; i < inputSize*repSize; i+= repSize) {
                outId.initAggregateResult();
                for (unsigned int j = 0; j < repSize; j++) {
                    outId.insertReplicate(output_mat->get(i+j,0)->toMap());
                }
                results->set(out, 1+(i % inputSize),
                        outId.buildAggregateResult());
            }
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
            throw vu::ArgError(vle::utils::format(
                    "Failed to start `%s'", exe.c_str()));
        }
        std::string message;
        bool is_success = true;
        mspawn.wait();
        mspawn.status(&message, &is_success);

        if (! is_success) {
            throw vu::ArgError(vle::utils::format(
                    "Error launching `%s' : %s ",
                    exe.c_str(), message.c_str()));
        }

        for (unsigned int out =0; out < outputSize; out++) {
            VleOutput& outId = mOutputs[out];
            for (unsigned int i = 0; i < inputSize*repSize; i+= repSize) {
                outId.initAggregateResult();
                for (unsigned int j = 0; j < repSize; j++) {
                    std::string vleResultFilePath = mWorkingDir;
                    vleResultFilePath.append(model->project().experiment().name());
                    vleResultFilePath.append("-");
                    vleResultFilePath.append(std::to_string(i+j));
                    vleResultFilePath.append("_");
                    vleResultFilePath.append(outId.view);
                    vleResultFilePath.append(".dat");
                    vle::reader::VleResultsTextReader tfr(vleResultFilePath);
                    vle::value::Matrix mat;
                    tfr.readFile(mat);
                    outId.insertReplicate(mat);
                    if (mRemoveSimulationFiles and out == (outputSize-1)) {
                        utils::Path torm(vleResultFilePath.c_str());
                        torm.remove();
                    }
                }
                results->set(out, 1+(i % inputSize),
                        outId.buildAggregateResult());
            }
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
MetaManager::postInputsIntern(vle::vpz::Vpz& model,
        const vle::value::Map& init)
{
    vle::vpz::Conditions& conds = model.project().experiment().conditions();
    //post replicas
    if (mReplicate) {
        vle::vpz::Condition& condRep = conds.get(mReplicate->cond);
        condRep.clearValueOfPort(mReplicate->port);
        for (unsigned int i=0; i < inputsSize(); i++) {
            for (unsigned int k=0; k < replicasSize(); k++) {
                const vle::value::Value& exp = mReplicate->values(init, true);
                switch(exp.getType()) {
                case vle::value::Value::SET:
                    condRep.addValueToPort(mReplicate->port,
                            exp.toSet().get(k)->clone());
                    break;
                case vle::value::Value::TUPLE:
                    condRep.addValueToPort(mReplicate->port,
                            vle::value::Double::create(exp.toTuple().at(k)));
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
        vle::vpz::Condition& cond = conds.get(tmp_input.cond);
        cond.clearValueOfPort(tmp_input.port);
        const vle::value::Value& exp = tmp_input.values(init, false);
        if (tmp_input.type == MONO) {
            if (not mReplicate) {
                cond.addValueToPort(tmp_input.port, exp.clone());
            } else {
                for (unsigned int k=0; k < replicasSize(); k++) {
                    cond.addValueToPort(tmp_input.port,exp.clone());
                }
            }
        } else {
            switch (exp.getType()) {
            case vle::value::Value::TUPLE: {
                const vle::value::Tuple& tmp_val_tuple = exp.toTuple();
                for (unsigned j=0; j < tmp_val_tuple.size(); j++) {//TODO maxExpe
                    double tmp_j = tmp_val_tuple[j];
                    if (not mReplicate) {
                        cond.addValueToPort(tmp_input.port,
                                vle::value::Double::create(tmp_j));
                    } else {
                        for (unsigned int k=0; k < replicasSize(); k++) {
                            cond.addValueToPort(tmp_input.port,
                                    vle::value::Double::create(tmp_j));
                        }
                    }
                }
                break;
            } case vle::value::Value::SET: {
                const vle::value::Set& tmp_val_set =exp.toSet();
                for (unsigned j=0; j < tmp_val_set.size(); j++) {//TODO maxExpe
                    const vle::value::Value& tmp_j = *tmp_val_set.get(j);
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
            }
            default:
                throw vle::utils::ArgError("[MetaManager] : Internal Error (1)");
                break;
            }
        }
    }
}
void
MetaManager::clear()
{
    mInputs.clear();
    mOutputs.clear();
}

}}//namespaces
