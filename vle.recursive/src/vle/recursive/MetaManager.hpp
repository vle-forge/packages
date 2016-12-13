
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

#ifndef VLE_RECURSIVE_METAMANAGER_HPP_
#define VLE_RECURSIVE_METAMANAGER_HPP_

#include <iostream>
#include <sstream>
#include <fstream>
#include <cmath>
#include <string>
#include <thread>

#include <vle/devs/InitEventList.hpp>
#include <vle/value/Map.hpp>
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
#include <vle/vpz/Vpz.hpp>
#include <vle/manager/Types.hpp>
#include <vle/utils/Context.hpp>
#include <vle/utils/Rand.hpp>
#include <vle/recursive/accu_multi.hpp>

#include "VleAPIfacilities.hpp"

namespace vle {
namespace recursive {

enum CONFIG_PARALLEL_TYPE {THREADS, MVLE, CVLE,  SINGLE};
enum INTEGRATION_TYPE {LAST, MAX, MSE, ALL};

struct VlePropagate
{
    /*
     * @brief VlePropagate specifies the value to set for all simulations
     *        to a port condition of the embedded simulator
     * @param cond, id of the cond
     * @param port, id of the port
     */
    VlePropagate(const std::string& cond, const std::string& port);
    virtual ~VlePropagate();

    /**
     * @brief Gets the value for this propagation input
     * @param init, the initialization map
     *            (either devs:InitEventList or value::Map)
     */
    template <class InitializationMap>
    const vle::value::Value& value(const InitializationMap& init)
    {
        std::string key("propagate_");
        key.append(getName());
        return *init.get(key);
    }

    std::string getName() const;

    std::string cond;
    std::string port;

};

struct VleInput
{
    /*
     * @brief VleInput identifies an input of the experiment plan
     * @param cond, id of the cond
     * @param port, id of the port
     * @param val, the value given on port conf_name, is either
     *  a value::Set or value::Tuple that identifies an experiment plan
     * @param rn, a random number generator
     */
    VleInput(const std::string& cond, const std::string& port,
            const vle::value::Value& val, utils::Rand& rn);
    virtual ~VleInput();

    /**
     * @brief Gets the experiment plan from initialization map
     * @param init, the initialization map
     */
    template <class InitializationMap>
    const vle::value::Value& values(const InitializationMap& init)
    {
        if (mvalues){
            return *mvalues;
        } else {
            std::string key("input_");
            key.append(getName());
            return *init.get(key);
        }
    }
    std::string getName() const;

    std::string cond;
    std::string port;
    unsigned int nbValues;
    //only for distribution configuration
    std::unique_ptr<value::Tuple> mvalues;

};

struct VleReplicate
{
    /*
     * @brief VleReplicate identifies the replicate input
     * @param cond, id of the cond
     * @param port, id of the port
     * @param val, the value given on port conf_name, is either
     * a value::Set or value::Tuple that identifies an experiment plan
     * @param rn, a random number generator
     */
    VleReplicate(const std::string& cond, const std::string& port,
            const vle::value::Value& val, utils::Rand& rn);
    virtual ~VleReplicate();

    /**
     * @brief Gets the experiment plan from initialization map
     * @param init, the initialization map
     */
    template <class InitializationMap>
    const vle::value::Value& values(const InitializationMap& init)
    {
        if (mvalues){
            return *mvalues;
        } else {
            std::string key("replicate_");
            key.append(getName());
            return *init.get(key);
        }
    }
    std::string getName();

    std::string cond;
    std::string port;
    unsigned int nbValues;
    //only for distribution configuration
    std::unique_ptr<value::Tuple> mvalues;
};


class VleOutput;
/**
 * Delegate output functionnalities
 */
class DelegateOut
{
public:
    DelegateOut(VleOutput& vleout, bool managedouble);
    virtual ~DelegateOut();

    virtual std::unique_ptr<vle::value::Value>
    insertReplicate(vle::value::Matrix& outMat, unsigned int currInput) = 0;

    /**
     * Temporal integration, shared with other delegates
     * @param [in] the vle output
     * @param [in] the output matrix
     * @return the temporal integration of the output
     */
    static std::unique_ptr<value::Value> integrateReplicate(VleOutput& vleout,
            vle::value::Matrix& outMat);

    static AccuMulti& getAccu(std::map<int, std::unique_ptr<AccuMulti>>& accu,
            unsigned int index, AccuStat s);


    static AccuMono& getAccu(std::map<int, std::unique_ptr<AccuMono>>& accu,
            unsigned int index, AccuStat s);


    VleOutput& vleOut;
    bool manageDouble;
};

/**
 * Delegate output for standard (no all integration nor all aggregation_input)
 */
class DelOutStd : public DelegateOut
{
public:
    DelOutStd(VleOutput& vleout);

    std::unique_ptr<vle::value::Value>
    insertReplicate(vle::value::Matrix& outMat,
            unsigned int currInput) override;




    //for replicate aggregation for current input index
    std::map<int, std::unique_ptr<AccuMono>> mreplicateAccu;
    std::unique_ptr<AccuMono> minputAccu;
};

/**
 * Delegate output for both integration and aggregation_input to 'all'
 */
class DelOutIntAggrALL : public DelegateOut
{
public:
    DelOutIntAggrALL(VleOutput& vleout, bool managedouble);

    std::unique_ptr<vle::value::Value>
    insertReplicate(vle::value::Matrix& outMat,
            unsigned int currInput) override;

    //for replicate aggregation for current input index
    std::map<int, std::unique_ptr<AccuMulti>> mreplicateAccu;
    std::unique_ptr<value::Value> minputAccu;
    unsigned int nbInputsFilled;
};

/**
 * Delegate output for both integration set to 'all'
 */
class DelOutIntALL : public DelegateOut
{
public:
    DelOutIntALL(VleOutput& vleout);

    std::unique_ptr<vle::value::Value>
    insertReplicate(vle::value::Matrix& outMat,
            unsigned int currInput) override;

    //for replicate aggregation for current input index
    std::map<int, std::unique_ptr<AccuMulti>> mreplicateAccu;
    std::unique_ptr<AccuMulti> minputAccu;
};

/**
 * Delegate output for both integration set to 'all'
 */
class DelOutAggrALL : public DelegateOut
{
public:
    DelOutAggrALL(VleOutput& vleout, bool managedouble);

    std::unique_ptr<vle::value::Value>
    insertReplicate(vle::value::Matrix& outMat,
            unsigned int currInput) override;

    //for replicate aggregation for current input index
    std::map<int, std::unique_ptr<AccuMono>> mreplicateAccu;
    std::unique_ptr<value::Value> minputAccu;
    unsigned int nbInputsFilled;
};

/**
 * Default interface for VleOutput
 */
class VleOutput
{
public:
    VleOutput();

    VleOutput(const std::string& id, const vle::value::Value& config);
    ~VleOutput();

    /**
     * @brief insert a replicate from a map of views
     * @param result, one simulation result (map of views)
     * @param currInput, current input index
     * @param nbInputs, nb inputs of the experiment plan (for allocation)
     * @param nbReplicates, nb replicates of the experiment plan
     * @return the input aggregated value if all inputs and all replicates
     * have aggregated
     */
    std::unique_ptr<value::Value>
    insertReplicate(value::Map& result, unsigned int currInput,
            unsigned int nbInputs, unsigned int nbReplicates);
    /**
     * @brief insert a replicate from a view
     * @param result, one view (matrix) from one simulation result
     * @param currInput, current input index
     * @param nbInputs, nb inputs of the experiment plan (for allocation)
     * @param nbReplicates, nb replicates of the experiment plan
     * @return the input aggregated value if all inputs and all replicates
     * have aggregated
     */
    std::unique_ptr<value::Value>
    insertReplicate(vle::value::Matrix& outMat, unsigned int currInput,
            unsigned int nbInputs, unsigned int nbReplicates);

public:
    bool parsePath(const std::string& path);

    std::string id;
    std::string view;
    std::string absolutePort;
    int colIndex;
    bool shared;
    INTEGRATION_TYPE integrationType;
    AccuStat replicateAggregationType;
    AccuStat inputAggregationType;
    unsigned int nbInputs;
    unsigned int nbReplicates;
    std::unique_ptr<DelegateOut> delegate;
    //optionnal for integration == MSE only
    std::unique_ptr<vle::value::Tuple> mse_times;
    std::unique_ptr<vle::value::Tuple> mse_observations;
};

/**
 * @brief Class that implements a meta manager, ie an API for performing
 * simulation of experiment plans
 */
class MetaManager
{
private:
    std::string mIdVpz;
    std::string mIdPackage;
    CONFIG_PARALLEL_TYPE mConfigParallelType;
    bool mRemoveSimulationFiles;
    unsigned int mConfigParallelNbSlots;
    unsigned int mConfigParallelMaxExpes;
    bool mexpe_debug;
    utils::Rand mrand;
    std::vector<std::unique_ptr<VlePropagate>> mPropagate;
    std::vector<std::unique_ptr<VleInput>> mInputs;
    std::unique_ptr<VleReplicate> mReplicate;
    std::vector<std::unique_ptr<VleOutput>> mOutputs;//view * port
    std::vector<std::unique_ptr<value::Value>>
      mOutputValues;//values are Tuple or Set
    std::string mWorkingDir; //only for mvle
    utils::ContextPtr mCtx;

public:

    /**
     * @brief MetaManager constructor
     */
    MetaManager();
    /**
     * @brief MetaManager destructor
     */
    virtual ~MetaManager();

    utils::Rand& random_number_generator();

    /**
     * @brief Simulates the experiment plan
     * @param[in] init, the initialization map
     * @param[out] init, an error structure
     * @return the simulated values
     */
    template <class InitializationMap>
    std::unique_ptr<value::Map> run(const InitializationMap& init,
            manager::Error& err)
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
                    err.code = -1;
                    err.message = "[MetaManager] error for "
                            "mvle config, missing 'working_dir' parameter";
                    return nullptr;
                }
                mWorkingDir.assign(init.getString("working_dir"));
            } else if (tmp == "cvle") {
                mConfigParallelType = CVLE;
                if (! init.exist("working_dir")) {
                    err.code = -1;
                    err.message = "[MetaManager] error for "
                            "mvle config, missing 'working_dir' parameter";
                    return nullptr;
                }
                mWorkingDir.assign(init.getString("working_dir"));
            } else if (tmp == "single") {
                mConfigParallelType = SINGLE;
            } else {
                err.code = -1;
                err.message = "[MetaManager] error for configuration type of "
                        "parallel process";
                return nullptr;
            }
        }
        if (init.exist("config_parallel_nb_slots")) {
            int tmp = init.getInt("config_parallel_nb_slots");
            if (tmp > 0) {
                mConfigParallelNbSlots = tmp;
            } else {
                err.code = -1;
                err.message = "[MetaManager] error for "
                        "configuration type of parallel nb slots)";
                return nullptr;
            }
        }
        if (init.exist("config_parallel_max_expes")) {
            int tmp;
            tmp = init.getInt("config_parallel_max_expes");
            if (tmp > 0) {
                if ((unsigned int) tmp < mConfigParallelNbSlots) {
                    err.code = -1;
                    err.message = vle::utils::format(
                            "[MetaManager] error for configuration type of parallel"
                            " max expes, got '%i'which is less than nb slots:'%i'",
                            tmp,  mConfigParallelNbSlots);
                    return nullptr;
                }
                mConfigParallelMaxExpes = tmp;
            } else {
                err.code = -1;
                err.message = vle::utils::format(
                        "[MetaManager] error for configuration type of parallel "
                        "max expes, got '%i'", tmp);
                return nullptr;
            }
        }
        if (init.exist("expe_debug")) {
            mexpe_debug = init.getBoolean("expe_debug");
            if (mexpe_debug) {
                mCtx->set_log_priority(7);
            }
        }
        if (init.exist("expe_seed")) {
            mrand.seed(init.getInt("expe_seed"));
        }
        if (init.exist("config_parallel_rm_files")) {
            mRemoveSimulationFiles = init.getBoolean("config_parallel_rm_files");
        }
        if (init.exist("package")) {
            mIdPackage = init.getString("package");
        } else {
            err.code = -1;
            err.message = "[MetaManager] missing 'package'";
            return nullptr;
        }
        if (init.exist("vpz")) {
            mIdVpz = init.getString("vpz");
        } else {
            err.code = -1;
            err.message = "[MetaManager] missing 'vpz'";
            return nullptr;
        }

        std::string in_cond;
        std::string in_port;
        std::string out_id;

        auto itb = init.begin();
        auto ite = init.end();
        try {
            for (; itb != ite; itb++) {
                const std::string& conf = itb->first;
                if (MetaManager::parseInput(conf, in_cond, in_port, "propagate_")) {
                    mPropagate.emplace_back(new VlePropagate(in_cond, in_port));
                } else if (MetaManager::parseInput(conf, in_cond, in_port)) {
                    mInputs.emplace_back(new VleInput(
                            in_cond, in_port, *itb->second, mrand));
                } else if (MetaManager::parseInput(conf, in_cond, in_port,
                        "replicate_")){
                    if (not mReplicate == 0) {
                        err.code = -1;
                        err.message = vle::utils::format(
                                "[MetaManager] : the replica is already defined "
                                "with '%s'", mReplicate->getName().c_str());
                        clear();
                        return nullptr;
                    }
                    mReplicate.reset(new VleReplicate(in_cond, in_port,
                            *itb->second, mrand));
                } else if (MetaManager::parseOutput(conf, out_id)){
                    mOutputs.emplace_back(new VleOutput(out_id, *itb->second));
                }
            }
        } catch (const std::exception& e){
            err.code = -1;
            err.message = "[MetaManager] ";
            err.message += e.what();
            clear();
            return nullptr;
        }
        //check

        unsigned int initSize = 0;
        for (unsigned int i = 0; i< mInputs.size(); i++) {
            const VleInput& vleIn = *mInputs[i];
            //check size which has to be consistent
            if (initSize == 0 and vleIn.nbValues > 1) {
                initSize = vleIn.nbValues;
            } else {
                if (vleIn.nbValues > 1 and initSize > 0
                        and initSize != vleIn.nbValues) {
                    err.code = -1;
                    err.message = utils::format(
                            "[MetaManager]: error in input values: wrong number"
                            " of values 1st input has %u values,  input %s has %u "
                            "values", initSize, vleIn.getName().c_str(),
                            vleIn.nbValues);
                    clear();
                    return nullptr;
                }
            }
            //check if already exist in replicate or propagate
            if (mReplicate and (mReplicate->getName() == vleIn.getName())) {
                err.code = -1;
                err.message = utils::format(
                        "[MetaManager]: error input '%s' is also the replicate",
                        vleIn.getName().c_str());
                clear();
                return nullptr;
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
        return runIntern(init, err);
    }


    //split a string with a char
    static void split(std::vector<std::string>& elems,
            const std::string &s, char delim);

    /**
     * @brief Parse a string of the type that should identify an input, eg. :
     *   input_cond.port
     *   replicate_cond.port
     *   cond.port
     *
     * @param[in]  conf, the string to parse
     * @param[out] cond, the condition of the input or empty if not parsed
     * @param[out] port, the port of the input or empty if not parsed
     * @param[in] tells the prefix to parse.
     *
     * @return if parsing was successfull
     */
    static bool parseInput(const std::string& conf,
            std::string& cond, std::string& port,
            const std::string& prefix ="input_");

    /**
     * @brief Parse a string of the type output_idout that should identify
     * an output
     *
     * @param[in]  conf, the string to parse
     * @param[out] idout, id of the output
     *
     * @return if parsing was successfull
     */
    static bool parseOutput(const std::string& conf, std::string& idout);

    /**
     * @brief Build a tuple value from a distribution
     *
     * @param[in]     distr, vle::value::Map specifying distribution and params
     * @param[in/out] rn, the random number generator
     *
     * @return the double avalues generated form distribution
     */
    static std::unique_ptr<value::Tuple> valuesFromDistrib(
            const value::Map& distrib, utils::Rand& rn);


private:

    unsigned int inputsSize() const;
    unsigned int replicasSize() const;

    //file the matrix with the result of one simulation
    void readResultFile(const std::string& filePath, value::Matrix&);

    template <class InitializationMap>
    std::unique_ptr<value::Map> runIntern(
            const InitializationMap& init,
            manager::Error& err)
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
                }
            }
        }

        unsigned int inputSize = inputsSize();
        unsigned int repSize = replicasSize();
        unsigned int outputSize =  mOutputs.size();

        //build output matrix with header
        std::unique_ptr<value::Map> results(new value::Map());
        for (unsigned int j=0; j<outputSize;j++) {
            results->add(mOutputs[j]->id, value::Null::create());
        }


        switch(mConfigParallelType) {
        case SINGLE:
        case THREADS: {
            postInputsIntern(*model, init);
            vle::manager::Manager planSimulator(
                    mCtx,
                    vle::manager::LOG_NONE,
                    vle::manager::SIMULATION_NONE,
                    nullptr);
            vle::manager::Error manerror;
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
            if (mexpe_debug){
                mCtx->log(1, __FILE__, __LINE__, __FUNCTION__,
                        "[vle.recursive] aggregation finished\n");
            }
            return results;
            break;

        } case MVLE: {
            postInputsIntern(*model, init);
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

            if (mexpe_debug){
                mCtx->log(1, __FILE__, __LINE__, __FUNCTION__,
                        "[vle.recursive] simulation mvle %d \n",
                        mConfigParallelNbSlots);
                std::string messageDbg ="";
                for (const auto& s : argv ) {
                    messageDbg += " ";
                    messageDbg += s;
                }
                messageDbg += "\n";
                mCtx->log(1, __FILE__, __LINE__, __FUNCTION__,
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
            if (mexpe_debug){
                mCtx->log(1, __FILE__, __LINE__, __FUNCTION__,
                        "[vle.recursive] aggregation finished \n");
            }
            return results;
            break;
        } case CVLE: {
            //post propagate
            vpz::Conditions& conds = model->project().experiment().conditions();
            for (unsigned int i=0; i < mPropagate.size(); i++) {
                VlePropagate& tmp_propagate = *mPropagate[i];
                vpz::Condition& cond = conds.get(tmp_propagate.cond);
                cond.clearValueOfPort(tmp_propagate.port);
                const value::Value& exp = tmp_propagate.value(init);
                cond.addValueToPort(tmp_propagate.port, exp.clone());
            }
            //save vpz
            vu::Package pkg(mCtx, "vle.recursive");//TODO should be saved outside
            std::string tempvpzPath = pkg.getExpDir(vu::PKG_BINARY);
            tempvpzPath.append("/temp_gen_CVLE.vpz");
            model->write(tempvpzPath);

            //build in.csv file
            utils::Path inPath(mWorkingDir);
            inPath /= "in.csv";
            std::ofstream inFile;
            inFile.open (inPath.string());
            //write header
            for (unsigned int in=0; in < mInputs.size(); in++) {
                VleInput& tmp_input = *mInputs[in];
                inFile << tmp_input.getName() << ",";
            }
            if (mReplicate) {
                inFile << mReplicate->getName() << ",";
            }
            inFile << "id\n";
            //write content
            for (unsigned int i = 0; i < inputSize; i++) {
                for (unsigned int j = 0; j < repSize; j++) {
                    for (unsigned int in=0; in < mInputs.size(); in++) {
                        VleInput& tmp_input = *mInputs[in];
                        const value::Value& exp = tmp_input.values(init);
                        switch(exp.getType()) {
                        case value::Value::SET:
                            exp.toSet().get(i)->writeFile(inFile);
                            inFile << ",";
                            break;
                        case value::Value::TUPLE:
                            inFile << exp.toTuple().at(i) <<",";
                            break;
                        default:
                            //error already detected
                            break;
                        }
                    }
                    if (mReplicate) {
                        const value::Value& exp = mReplicate->values(init);
                        switch(exp.getType()) {
                        case value::Value::SET:
                            exp.toSet().get(j)->writeFile(inFile);
                            inFile << ",";
                            break;
                        case value::Value::TUPLE:
                            inFile << exp.toTuple().at(j) <<",";
                            break;
                        default:
                            //error already detected
                            break;
                        }
                    }
                    inFile << "id_" << i << "_" << j << "\n";
                }
            }
            inFile.close();
            //launch mpirun
            vu::Spawn mspawn(mCtx);
            std::string exe = mCtx->findProgram("mpirun").string();

            std::vector < std::string > argv;
            argv.push_back("-np");
            std::stringstream ss;
            {
                ss << mConfigParallelNbSlots;
                argv.push_back(ss.str());
            }
            argv.push_back("cvle");
            argv.push_back("-P");//TODO should be simulated outside the rr package
            argv.push_back("vle.recursive");
            argv.push_back("temp_gen_CVLE.vpz");
            argv.push_back("-i");
            argv.push_back("in.csv");
            argv.push_back("-o");
            argv.push_back("out.csv");

            if (mexpe_debug){
                mCtx->log(1, __FILE__, __LINE__, __FUNCTION__,
                        "[vle.recursive] simulation cvle %d \n",
                        mConfigParallelNbSlots);
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
                err.code = -1;
                err.message = "[MetaManager] ";
                err.message += vle::utils::format("Error launching `%s' : %s ",
                        exe.c_str(), message.c_str());
                model.reset(nullptr);
                results.reset(nullptr);
                clear();
                return nullptr;
            }
            //read output file
            utils::Path outPath(mWorkingDir);
            outPath /= "out.csv";
            std::ifstream outFile;
            outFile.open (outPath.string(), std::ios_base::in);
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
            return results;
            break;
        }}
        return nullptr;
    }

    template <class InitializationMap>
    void postInputsIntern(vpz::Vpz& model, const InitializationMap& init)
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
    void clear();
};

}}//namespaces

#endif
