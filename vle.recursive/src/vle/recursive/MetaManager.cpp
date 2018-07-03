
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
#include <iomanip>
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

/***********MetaManager*************/

MetaManager::MetaManager(): mIdVpz(), mIdPackage(),
        mConfigParallelType(SINGLE), mRemoveSimulationFiles(true),
        mUseSpawn(false), mConfigParallelNbSlots(2), mrand(), mDefine(),
        mPropagate(), mInputs(), mReplicates(), mOutputs(),
        mWorkingDir(utils::Path::temp_directory_path().string()),
        mCtx(utils::make_context())
{

    mCtx->set_log_priority(3);//erros only

}

MetaManager::MetaManager(utils::ContextPtr ctx): mIdVpz(), mIdPackage(),
        mConfigParallelType(SINGLE), mRemoveSimulationFiles(true),
        mUseSpawn(false), mConfigParallelNbSlots(2), mrand(), mDefine(),
        mPropagate(), mInputs(), mReplicates(), mOutputs(),
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
        out << std::setprecision(std::numeric_limits<double>::digits10)
            << "<double>" << exp.toTuple().at(k) <<"</double>";
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


bool
MetaManager::readCvleIdHeader(std::ifstream& outFile,
            std::string& line, std::vector <std::string>& tokens,
            int& inputId, int& inputRepl)
{
    //read empty lines at the beginning
    if (outFile.eof()) {
        return false;
    }
    std::streampos stream_place = outFile.tellg();
    std::getline(outFile, line);
    while(line.empty()) {
        if (outFile.eof()) {
            return false;
        }
        stream_place = outFile.tellg();
        std::getline(outFile, line);
    }

    //split id_10_2
    tokens.clear();
    utils::tokenize(line, tokens, "_", false);
    if (tokens.size() != 3 or tokens[0] != "id") {
        outFile.seekg(stream_place);
        return false;
    } else {
        inputId = std::stoi(tokens[1]);
        inputRepl = std::stoi(tokens[2]);
        return true;
    }
}

bool
MetaManager::readCvleViewHeader(std::ifstream& outFile,
        std::string& line, std::vector <std::string>& tokens,
        std::string& viewName)
{
    //read empty lines at the beginning
    if (outFile.eof()) {
        return false;
    }
    std::streampos stream_place = outFile.tellg();
    std::getline(outFile, line);
    while(line.empty()) {
        if (outFile.eof()) {
            return false;
        }
        stream_place = outFile.tellg();
        std::getline(outFile, line);
    }

    //split view:viewNoise
    tokens.clear();
    utils::tokenize(line, tokens, ":", false);
    if (tokens.size() != 2 or tokens[0] != "view") {
        outFile.seekg(stream_place);
        return false;
    } else {
        viewName.assign(tokens[1]);
        return true;
    }
}

std::unique_ptr<vv::Matrix>
MetaManager::readCvleMatrix(std::ifstream& outFile, std::string& line,
        std::vector <std::string>& tokens, unsigned int nb_rows)
{
    std::unique_ptr<vv::Matrix> viewMatrix(nullptr);

    //read empty lines at the beginning
    std::streampos stream_place = outFile.tellg();
    std::getline(outFile, line);
    while(line.empty()) {
        stream_place = outFile.tellg();
        std::getline(outFile, line);
    }

    //fill matrix header
    tokens.clear();
    utils::tokenize(line, tokens, ",", true);
    unsigned int nbCols = tokens.size();
    viewMatrix.reset(new value::Matrix(nbCols,1,nbCols, nb_rows));
    for (unsigned int c=0; c<nbCols; c++){
        viewMatrix->set(c,0,value::String::create(tokens[c]));
    }

    //read content
    while(true){
        stream_place = outFile.tellg();
        std::getline(outFile,line);
        tokens.clear();
        utils::tokenize(line, tokens, ",", true);
        if (tokens.size() != nbCols) {
            outFile.seekg(stream_place);
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
    return viewMatrix;
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
        } else if (tmp == "cvle") {
            mConfigParallelType = CVLE;
            if (! init.exist("working_dir", status)) {
                err.code = -1;
                err.message = "[MetaManager] error for "
                        "cvle config, missing 'working_dir' parameter";
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
    if (init.exist("config_parallel_spawn", status)) {
        mUseSpawn = init.getBoolean(
                "config_parallel_spawn", status);
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
            if (parseInput(conf, in_cond, in_port, "define_")) {
                mDefine.emplace_back(new VleDefine(in_cond, in_port, val));
            } else if (parseInput(conf, in_cond, in_port, "propagate_")) {
                mPropagate.emplace_back(new VlePropagate(in_cond, in_port));
            } else if (parseInput(conf, in_cond, in_port)) {
                mInputs.emplace_back(new VleInput(
                        in_cond, in_port, val, mrand));
            } else if (parseInput(conf, in_cond, in_port,
                    "replicate_")){
                mReplicates.emplace_back(new VleReplicate(in_cond, in_port,
                        val, mrand));
            } else if (parseOutput(conf, out_id)){
                mOutputs.emplace_back(new VleOutput(out_id, val));
            }
        }
        std::sort(mDefine.begin(), mDefine.end(), VleDefineSorter());
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

    //check Define
    for (unsigned int i = 0; i< mDefine.size(); i++) {
        const VleDefine& vleDef = *mDefine[i];
        //check if exist in Input
        bool found = false;
        for (unsigned int j=0; j<mInputs.size() and not found; j++) {
            const VleInput& vleIn = *mInputs[j];
            if (vleDef.getName() == vleIn.getName()) {
                if (vleDef.to_add) {
                    found = true;
                } else {
                    err.code = -1;
                    err.message = utils::format(
                            "[MetaManager]: error in define_X '%s': it cannot "
                            "be removed and declared as input at the same time",
                            vleDef.getName().c_str());
                    clear();
                    return nullptr;
                }
            }
        }
        //check if exist in Propagate
        for (unsigned int j=0; j<mPropagate.size() and not found; j++) {
            const VlePropagate& vleProp = *mPropagate[j];
            if (vleDef.getName() == vleProp.getName()) {
                if (vleDef.to_add) {
                    found = true;
                } else {
                    err.code = -1;
                    err.message = utils::format(
                            "[MetaManager]: error in define_X '%s': it cannot "
                            "be removed and declared as propagate at the same "
                            "time", vleDef.getName().c_str());
                    clear();
                    return nullptr;
                }
            }
        }
        //check if initialized
        if (vleDef.to_add and not found) {
            err.code = -1;
            err.message = utils::format(
                    "[MetaManager]: error in define_X '%s': it cannot "
                    "be added without initialization",
                    vleDef.getName().c_str());
            clear();
            return nullptr;
        }
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
    } case CVLE: {
        return run_with_cvle(init, err);
        break;
    }}
    return nullptr;
}

std::unique_ptr<vpz::Vpz>
MetaManager::init_embedded_model(const wrapper_init& init, manager::Error& err)
{
    vle::utils::Package pkg(mCtx, mIdPackage);
    std::string vpzFile = pkg.getExpFile(mIdVpz, vle::utils::PKG_BINARY);

    std::unique_ptr<vpz::Vpz> model(new vpz::Vpz(vpzFile));
    model->project().experiment().setCombination("linear");

    {//remove useless views, observables and observables ports.
        vle::vpz::BaseModel* baseModel = model->project().model().node();
        std::set<std::string> viewsToKeep;
        std::set<std::string> obsAndPortTokeep;//of the form obs,port
        std::string atomPath;
        std::string obsPort;

        for (auto& o : mOutputs) {
            viewsToKeep.insert(o->view);
            if (not o->extractAtomPathAndPort(atomPath, obsPort)) {
                err.code = -1;
                err.message = utils::format(
                        "[vle.recursive]: error in analysis of output '%s/%s'",
                        o->view.c_str(), o->absolutePort.c_str());
                clear();
                return nullptr;
            }
            vle::vpz::AtomicModel* atomMod = baseModel->findModelFromPath(
                    atomPath)->toAtomic();
            obsPort = atomMod->observables() + "," + obsPort;
            obsAndPortTokeep.insert(obsPort);
        }
        VleAPIfacilities::keepOnly(*model, viewsToKeep, obsAndPortTokeep);


    }
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
        }}
    }
    post_define(*model);
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
    std::unique_ptr<vpz::Vpz> model = init_embedded_model(init, err);
    if (err.code) {
        return nullptr;
    }
    std::unique_ptr<value::Map> results = init_results();

    unsigned int inputSize = inputsSize();
    unsigned int repSize = replicasSize();
    unsigned int outputSize =  mOutputs.size();
    post_inputs(*model, init);

    vle::manager::SimulationOptions optSim = vle::manager::SIMULATION_NONE;
    if (mUseSpawn) {
        optSim = vle::manager::SIMULATION_SPAWN_PROCESS;
    }
    vle::manager::Manager planSimulator(
            mCtx,
            vle::manager::LOG_NONE,
            optSim,
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
MetaManager::run_with_cvle(const wrapper_init& init, manager::Error& err)
{
    std::unique_ptr<vpz::Vpz> model = init_embedded_model(init, err);
    if (err.code) {
        return nullptr;
    }
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
    argv.push_back(vle::utils::to(mConfigParallelNbSlots));
    argv.push_back("cvle");
    argv.push_back("--block-size");
    argv.push_back(vle::utils::to(
            (int(inputSize*repSize)/int(mConfigParallelNbSlots-1))+1));
    if (not mUseSpawn) {
        argv.push_back("--withoutspawn");
    }
    argv.push_back("--more-output-details");
    ////use mpi_warn_nf_forgk since cvle launches simulation with fork
    //// if whithspawn (?)
    //argv.push_back("--mca");
    //argv.push_back("mpi_warn_on_fork");
    //argv.push_back("0");
    ////set more log for mpirun
    //argv.push_back("--mca");
    //argv.push_back("ras_gridengine_verbose");
    //argv.push_back("1");
    //argv.push_back("--mca");
    //argv.push_back("plm_gridengine_verbose");
    //argv.push_back("1");
    //argv.push_back("--mca");
    //argv.push_back("ras_gridengine_show_jobid");
    //argv.push_back("1");
    //argv.push_back("--mca");
    //argv.push_back("plm_gridengine_debug");
    //argv.push_back("1");
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
    int inputRepl = -1;
    std::string viewName;
    std::map<std::string, int> insightsViewRows;
    std::unique_ptr<value::Matrix> viewMatrix;

    std::vector <std::string> tokens;
    bool finishViews = false;
    bool finishIds = false;
    unsigned int nbSimus = 0;
    while(not finishIds){
        //read id_1_0
        if (not readCvleIdHeader(outFile, line, tokens, inputId, inputRepl)) {
            finishIds = true;
            break;
        }
        finishViews = false;
        while(not finishViews){
            //read view:viewName
            if (not readCvleViewHeader(outFile, line, tokens, viewName)) {
                finishViews= true;
                break;
            }
            //get/set insight of the number of rows
            bool getInsight = (insightsViewRows.find(viewName) !=
                    insightsViewRows.end());
            if (not getInsight) {
                insightsViewRows.insert(std::make_pair(viewName,100));
            }
            int rows = insightsViewRows[viewName];
            //read matrix of values
            viewMatrix = std::move(readCvleMatrix(outFile, line, tokens, rows));
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
        nbSimus ++;
    }
    if (nbSimus != inputSize*repSize) {
        err.code = -1;
        err.message = "[vle.recursive] ";
        err.message += vle::utils::format("Error in simu id=%d, repl=%d ",
                inputId, inputRepl);
        std::string prefix_id = "id_";
        bool stop = false;
        while (not stop) {
            std::getline(outFile, line);
            if (line.substr(0, prefix_id.size()) == prefix_id) {
                stop = true;
            } else {
                err.message +=  line+" ";
            }
            stop = stop or outFile.eof();
        }
        model.reset(nullptr);
        results.reset(nullptr);
        clear();
    }
    outFile.close();
    if (mRemoveSimulationFiles) {
        tempOutCsv.remove();
        tempInCsv.remove();
        tempVpzPath.remove();
    }
    return results;
}

void
MetaManager::post_define(vpz::Vpz& model)
{
    vpz::Conditions& conds = model.project().experiment().conditions();
    for (unsigned int i=0; i < mDefine.size(); i++) {
        VleDefine& tmp_def = *mDefine[i];
        vpz::Condition& cond = conds.get(tmp_def.cond);
        if (cond.exist(tmp_def.port)) {
            if (not tmp_def.to_add) {
                cond.del(tmp_def.port);
            }
        } else {
            if (tmp_def.to_add) {
                cond.add(tmp_def.port);
            }
        }
    }

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
MetaManager::clear()
{
    mPropagate.clear();
    mInputs.clear();
    mReplicates.clear();
    mOutputs.clear();
    mOutputValues.clear();
}

}}//namespaces
