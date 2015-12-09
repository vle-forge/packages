
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
#include <cmath>
#include <boost/algorithm/string/split.hpp>
#include <boost/algorithm/string/classification.hpp>
#include <boost/filesystem.hpp>

#include <vle/vpz/Vpz.hpp>
#include <vle/utils/Exception.hpp>
#include <vle/utils/Package.hpp>
#include <vle/utils/Path.hpp>
#include <vle/utils/Spawn.hpp>
#include <vle/manager/Simulation.hpp>
#include <vle/manager/Manager.hpp>
#include <vle/value/Double.hpp>

#include <vle/reader/vle_results_text_reader.hpp>


#include "MetaManager.hpp"
#include "VleAPIfacilities.hpp"

//#include <boost/lexical_cast.hpp>

namespace vle {
namespace recursive {

MetaManager::MetaManager(): mIdVpz(), mIdPackage(),
        mConfigParallelType(SINGLE), mRemoveSimulationFiles(true),
        mConfigParallelNbSlots(1), mConfigParallelMaxExpes(1),
        mInputs(), mReplicate(0), mOutputs(), mOutputValues(), mResults(0),
        mWorkingDir("")
{
}

MetaManager::~MetaManager()
{
    {
        std::vector<VleInput*>::iterator itb = mInputs.begin();
        std::vector<VleInput*>::iterator ite = mInputs.end();
        for (; itb != ite; itb ++) {
            delete *itb;
        }
    }
    delete mReplicate;
    {
        std::vector<vle::value::Value*>::iterator itb = mOutputValues.begin();
        std::vector<vle::value::Value*>::iterator ite = mOutputValues.end();
        for (; itb != ite; itb ++) {
            delete *itb;
        }
    }
    delete mResults;
}

void
MetaManager::init(const vle::value::Map& init)
{
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
            throw vle::utils::ArgError(vle::fmt("[MetaManager] error for "
                    "configuration type of parallel process, got '%1%'")
            % (*init.get("config_parallel_type")));
        }
    }
    if (init.exist("config_parallel_nb_slots")) {
        int tmp;
        tmp = init.getInt("config_parallel_nb_slots");
        if (tmp > 0) {
            mConfigParallelNbSlots = tmp;
        } else {
            throw vle::utils::ArgError(vle::fmt("[MetaManager] error for "
                 "configuration type of parallel nb slots, got '%1%'")
            % (*init.get("config_parallel_nb_slots")));
        }
    }
    if (init.exist("config_parallel_max_expes")) {
        int tmp;
        tmp = init.getInt("config_parallel_max_expes");
        if (tmp > 0) {
            if ((unsigned int) tmp < mConfigParallelNbSlots) {
                throw vle::utils::ArgError(vle::fmt("[MetaManager] error for "
                        "configuration type of parallel max expes, got '%1%'"
                        "which is less than nb slots: '%2%'")
                % tmp % mConfigParallelNbSlots);
            }
            mConfigParallelMaxExpes = tmp;
        } else {
            throw vle::utils::ArgError(vle::fmt("[MetaManager] error for "
                 "configuration type of parallel max expes, got '%1%'")
            % (*init.get("config_parallel_max_expes")));
        }
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

    std::string prefix;
    std::string varname;
    vle::value::Map::const_iterator itb = init.begin();
    vle::value::Map::const_iterator ite = init.end();
    for (; itb != ite; itb++) {
        const std::string& conf_name = itb->first;
        if (!prefix.assign("input_").empty() and
                !conf_name.compare(0, prefix.size(), prefix)) {
            varname.assign(conf_name.substr(prefix.size(), conf_name.size()));
            mInputs.push_back(new VleInput(varname, *itb->second));
        } else if (!prefix.assign("output_").empty() and
                        !conf_name.compare(0, prefix.size(), prefix)) {
            varname.assign(conf_name.substr(prefix.size(), conf_name.size()));
            mOutputs.push_back(VleOutput(varname, *itb->second));
        } else if (!prefix.assign("replicate_").empty() and
                !conf_name.compare(0, prefix.size(), prefix)) {
            if (not mReplicate == 0) {
                throw vle::utils::ArgError(vle::fmt("[MetaManager] : the"
                        " replica is already defined with '%1%'")
                   % mReplicate->getName());
            }
            varname.assign(conf_name.substr(prefix.size(), conf_name.size()));
            mReplicate = new VleInput(varname, *itb->second);
        }
    }
    //check
    if (mInputs.size() == 0) {
        throw vle::utils::ArgError("[MetaManager] : error no inputs");
    }
    unsigned int initSize = 0;;
    for (unsigned int i = 0; i< mInputs.size(); i++) {
        VleInput* vleIn = mInputs[0];
        if (vleIn->type == MULTI ) {
            if (initSize == 0) {
                initSize = vleIn->nbValues();
            } else {
                if (initSize != vleIn->nbValues()) {
                    throw vle::utils::ArgError(vle::fmt("[MetaManager]: error "
                            "in input values: wrong number of values 1st "
                            " input has %1% values, %2%th input has %3% values")
                            % initSize % i % vleIn->nbValues());
                }
            }
        }
    }
}

/**
 * @brief Simulates the experiment plan
 * @return the values
 */
const vle::value::Value&
MetaManager::launchSimulations()
{
    vle::vpz::Vpz model;
    vle::utils::Package pkg(mIdPackage);
    std::string vpzFile = pkg.getExpFile(mIdVpz, vle::utils::PKG_BINARY);
    model.parseFile(vpzFile);
    model.project().experiment().setCombination("linear");

    VleAPIfacilities::changeAllPlugin(model, "dummy");
    std::vector<VleOutput>::const_iterator itb= mOutputs.begin();
    std::vector<VleOutput>::const_iterator ite= mOutputs.end();
    for (; itb != ite; itb++) {
        switch(mConfigParallelType) {
            case SINGLE:
            case THREADS: {
                VleAPIfacilities::changePlugin(model, itb->view, "storage");
                break;
            } case MVLE: {
                VleAPIfacilities::changePlugin(model, itb->view, "file");
                break;
            }
        }
    }

    unsigned int inputSize = inputsSize();
    unsigned int repSize = replicasSize();
    unsigned int outputSize =  mOutputs.size();
    postInputs(model);


    switch(mConfigParallelType) {
    case SINGLE:
    case THREADS: {
        std::ofstream outstream(
                vle::utils::Path::path().getHomeFile("metamanager.log").c_str());
        vle::manager::Manager planSimulator(vle::manager::LOG_NONE,
                vle::manager::SIMULATION_NONE,
                &outstream);
        vle::utils::ModuleManager modules;
        vle::manager::Error manerror;

        vle::value::Matrix* output_mat =  planSimulator.run(
                new vle::vpz::Vpz(model), modules, mConfigParallelNbSlots,
                0, 1, &manerror);
        if (manerror.code != 0) {
            throw vle::utils::InternalError(
                    vle::fmt("Error in MetaManager '%1%'")
            % manerror.message);
        }

        delete mResults;
        mResults= new vle::value::Matrix(outputSize, inputSize, 1, 1);


        for (unsigned int out =0; out < outputSize; out++) {
            VleOutput& outId = mOutputs[out];
            for (unsigned int i = 0; i < inputSize*repSize; i+= repSize) {
                outId.initAggregateResult();
                for (unsigned int j = 0; j < repSize; j++) {
                    outId.insertReplicate(output_mat->get(i+j,0)->toMap());
                }
                mResults->set(out, i % inputSize, outId.buildAggregateResult());
            }
        }
        delete output_mat;
        return *mResults;
        break;
    } case MVLE: {

        vu::Package pkg("vle.recursive");//TODO should be saved outside the rr package
        std::string tempvpzPath = pkg.getExpDir(vu::PKG_BINARY);
        tempvpzPath.append("/temp_gen_MPI.vpz");
        model.write(tempvpzPath);
        vu::Spawn mspawn;
        std::string exe = vu::Path::findProgram("mpirun");

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
        boost::system::error_code fsPathError;
        boost::filesystem::path currentPath(
                boost::filesystem::current_path(fsPathError));

        bool started = mspawn.start(exe, mWorkingDir, argv);
        if (not started) {
            throw vu::ArgError(vle::fmt(_("Failed to start `%1%'")) % exe);
        }
        std::string message;
        bool is_success = true;
        mspawn.wait();
        mspawn.status(&message, &is_success);
        boost::filesystem::current_path(currentPath, fsPathError);

        if (! is_success) {
            throw vu::ArgError(vle::fmt(_("Error launching `%1%' : %2% "))
            % exe % message);
        }
        delete mResults;
        mResults= new vle::value::Matrix(outputSize, inputSize, 1, 1);
        for (unsigned int out =0; out < outputSize; out++) {
            VleOutput& outId = mOutputs[out];
            for (unsigned int i = 0; i < inputSize*repSize; i+= repSize) {
                outId.initAggregateResult();
                for (unsigned int j = 0; j < repSize; j++) {
                    std::string vleResultFilePath = mWorkingDir;
                    vleResultFilePath.append(model.project().experiment().name());
                    vleResultFilePath.append("-");
                    vleResultFilePath.append(boost::lexical_cast<std::string>(i+j));
                    vleResultFilePath.append("_");
                    vleResultFilePath.append(outId.view);
                    vleResultFilePath.append(".dat");
                    vle::reader::VleResultsTextReader tfr(vleResultFilePath);
                    vle::value::Matrix mat;
                    tfr.readFile(mat);
                    outId.insertReplicate(mat);
                    if (mRemoveSimulationFiles and out == (outputSize-1)) {
                        boost::filesystem::remove(vleResultFilePath.c_str());
                    }
                }
                mResults->set(out, i % inputSize, outId.buildAggregateResult());
            }
        }
        return *mResults;
        break;
    }
    }
    return *mResults;

}

vle::value::Matrix*
MetaManager::getResults()
{
    return mResults;
}


//private functions
VleInput::VleInput(const std::string& conf,
        const vle::value::Value& val):
        cond(), port(), type(MONO), inputValues()
{
    std::vector <std::string> splvec;
    boost::split(splvec, conf, boost::is_any_of("."),
            boost::token_compress_on);
    if (splvec.size() == 2) {
        cond.assign(splvec[0]);
        port.assign(splvec[1]);
    } else {
        throw vle::utils::ArgError(vle::fmt("[MetaManager] : the input"
                " is expected to be of the form "
                "'cond_name.port_name', got '%1%'") % conf);
    }
    switch (val.getType()) {
    case vle::value::Value::TUPLE:
    case vle::value::Value::SET:
        inputValues = val.clone();
        type = MULTI;
        break;
    default:
        inputValues = val.clone();
        type = MONO;
        break;
    }
}

VleInput::~VleInput()
{
    delete inputValues;
}

unsigned int
VleInput::nbValues() const
{
    switch (inputValues->getType()) {
    case vle::value::Value::TUPLE:
        return inputValues->toTuple().size();
        break;
    case vle::value::Value::SET:
        return inputValues->toSet().size();
        break;
    default:
        throw vle::utils::ArgError(vle::fmt("[MetaManager] : Input values"
                " types not handled for input '%1%.%2%'") % cond % port);
        break;
    }
    return 1;
}

std::string
VleInput::getName()
{
    std::string ret = cond;
    ret.append(".");
    ret.append(port);
    return ret;
}

VleOutput::VleOutput() :
   id(), view(), absolutePort(), integrationType(LAST), aggregationType(MEAN),
   mse_times(0), mse_observations(0), maccuMono(0), maccuMulti(0)
{
}

VleOutput::VleOutput(const std::string& _id,
        const vle::value::Value& val) :
   id(_id), view(), absolutePort(), integrationType(LAST),
   aggregationType(MEAN), mse_times(0), mse_observations(0), maccuMono(0),
   maccuMulti(0)
{
    std::string tmp;
    if (val.isString()) {
        if (not parsePath(val.toString().value())) {
            throw vle::utils::ArgError(vle::fmt("[MetaManager] : error in "
                           "configuration of the output 'output_%1%' with "
                           "a string; got: '%2%'") % id
                            % val.toString().value());
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
                mse_times = new vle::value::Tuple(m.getTuple("mse_times"));
                mse_observations = new vle::value::Tuple(
                        m.getTuple("mse_observations"));
                error = mse_times->size() != mse_observations->size();
            }
        }
        if (error) {
            throw vle::utils::ArgError(vle::fmt("[MetaManager] : error in "
                    "configuration of the output '%1%%2%' with "
                    "a map; got: '%3%'") % "id_output_" % id
                    % val);
        }
    }
    initAggregateResult();
}

bool
VleOutput::parsePath(const std::string& path)
{
    std::vector<std::string> splvec;
    boost::split(splvec, path, boost::is_any_of("/"), boost::token_compress_on);
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
        throw vu::ArgError(vle::fmt("[MetaManager] view '%1%' not found)")
          % view);
    }
    const vv::Matrix& outMat = vv::toMatrixValue(*it->second);
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
        throw vu::ArgError(vle::fmt("[MetaManager] view.port '%1%' not found)")
        % absolutePort);
    }
    const vv::ConstVectorView& outVec = outMat.column(colIndex);
    switch(integrationType) {
    case MAX: {
        double max = -9999;
        for (unsigned int i=1; i < outVec.size(); i++) {
            if (outVec[i]->toDouble().value() > max) {
                max = outVec[i]->toDouble().value();
            }
        }
        maccuMono->insert(max);
        break;
    } case LAST: {
        maccuMono->insert(outVec[outVec.size() - 1]->toDouble().value());
        break;
    } case MSE: {
        double sum_square_error = 0;
        for (unsigned int i=0; i< mse_times->size(); i++) {
            sum_square_error += std::pow(
                 (outVec[std::floor((*mse_times).at(i))]->toDouble().value()
                         - mse_observations->at(i))
                 , 2);
        }
        maccuMono->insert(sum_square_error/mse_times->size());
        break;
    } case ALL:{
        maccuMulti->insert(outVec);
        break;
    }}
}

void
VleOutput::initAggregateResult()
{
    delete maccuMulti;
    delete maccuMono;
    switch(integrationType) {
    case LAST:
    case MAX:
    case MSE: {
        maccuMono = new AccuMono(aggregationType);
        break;
    } case ALL: {
        maccuMulti = new AccuMulti(aggregationType);
    }}
}

vle::value::Value*
VleOutput::buildAggregateResult()
{
    switch (aggregationType) {
    case MEAN: {
        switch (integrationType) {
        case LAST:
        case MAX:
        case MSE: {
            return new vle::value::Double(maccuMono->mean());
            break;
        } case ALL: {
            vle::value::Tuple* res = new vle::value::Tuple(maccuMulti->size());
            maccuMulti->mean(*res);
            return res;
        }}
        break;
    } default: {
        throw "error";
        break;
    }}
    return 0;
}

unsigned int
MetaManager::inputsSize() const
{

    for (unsigned int i=0; i < mInputs.size(); i++) {
        if (mInputs[i]->type == MULTI) {
            return mInputs[i]->nbValues();
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

    unsigned int repsSize = 0;
    switch (mReplicate->inputValues->getType()) {
    case vle::value::Value::TUPLE:
        repsSize = mReplicate->inputValues->toTuple().size();
        break;
    case vle::value::Value::SET:
        repsSize = mReplicate->inputValues->toSet().size();
        break;
    default:
        throw vle::utils::ArgError(
                "[MetaManager]: Replicate types not handled");
        break;
    }
    return repsSize;
}

void
MetaManager::postInputs(vle::vpz::Vpz& model) const
{
    vle::vpz::Conditions& conds = model.project().experiment().conditions();
    //post replicas
    if (mReplicate) {
        vle::vpz::Condition& condRep = conds.get(mReplicate->cond);
        condRep.clearValueOfPort(mReplicate->port);
        for (unsigned int i=0; i < inputsSize(); i++) {
            for (unsigned int k=0; k < replicasSize(); k++) {
                switch(mReplicate->inputValues->getType()) {
                case vle::value::Value::SET:
                    condRep.addValueToPort(mReplicate->port,
                            mReplicate->inputValues->toSet().get(k)->clone());
                    break;
                case vle::value::Value::TUPLE:
                    condRep.addValueToPort(mReplicate->port,
                            new vle::value::Double(
                                    mReplicate->inputValues->toTuple().at(k)));
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
        VleInput* tmp_input = mInputs[i];
        vle::vpz::Condition& cond = conds.get(tmp_input->cond);
        cond.clearValueOfPort(tmp_input->port);
        if (tmp_input->type == MONO) {
            if (not mReplicate) {
                cond.addValueToPort(tmp_input->port,
                        tmp_input->inputValues->clone());
            } else {
                for (unsigned int k=0; k < replicasSize(); k++) {
                    cond.addValueToPort(tmp_input->port,
                            tmp_input->inputValues->clone());
                }
            }
        } else {
            switch (tmp_input->inputValues->getType()) {
            case vle::value::Value::TUPLE: {
                const vle::value::Tuple& tmp_val_tuple =
                        tmp_input->inputValues->toTuple();
                for (unsigned j=0; j < tmp_val_tuple.size(); j++) {//TODO maxExpe
                    double tmp_j = tmp_val_tuple[j];
                    if (not mReplicate) {
                        cond.addValueToPort(tmp_input->port,
                                new vle::value::Double(tmp_j));
                    } else {
                        for (unsigned int k=0; k < replicasSize(); k++) {
                            cond.addValueToPort(tmp_input->port,
                                    new vle::value::Double(tmp_j));
                        }
                    }
                }
                break;
            } case vle::value::Value::SET: {
                const vle::value::Set& tmp_val_set =
                        tmp_input->inputValues->toSet();
                for (unsigned j=0; j < tmp_val_set.size(); j++) {//TODO maxExpe
                    const vle::value::Value* tmp_j = tmp_val_set.get(j);
                    if (not mReplicate) {
                        cond.addValueToPort(tmp_input->port,tmp_j->clone());
                    } else {
                        for (unsigned int k=0; k < replicasSize(); k++) {
                            cond.addValueToPort(tmp_input->port,
                                    tmp_j->clone());
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

}}//namespaces
