
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

#include <vle/value/Map.hpp>
#include <vle/vpz/Vpz.hpp>
#include <vle/manager/Types.hpp>
#include <vle/utils/Context.hpp>
#include <vle/utils/Rand.hpp>
#include <vle/recursive/accu_multi.hpp>

namespace vle {
namespace recursive {

enum CONFIG_PARALLEL_TYPE {THREADS, MVLE, SINGLE};
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
     */
    const vle::value::Value& value(const vle::value::Map& init);
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
    const vle::value::Value& values(const vle::value::Map& init);
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
    const vle::value::Value& values(const vle::value::Map& init);
    std::string getName();

    std::string cond;
    std::string port;
    unsigned int nbValues;
    //only for distribution configuration
    std::unique_ptr<value::Tuple> mvalues;
};

struct VleOutput
{
public:
    VleOutput();
    /**
     * @brief VleOutput identifies an output of the experiment plan
     * @param id,  a string that is an id for the output
     * @param config is either a map containing:
     *  - "path": of the form 'view/coupled:atomic.port'
     *  - "integration" type of integration
     *  - "mse_times" required if integration=mse
     *  - "mse_observations" required if integration=mse
     *  - "aggregation" type of aggregation
     *  or a string identifying the "path" parameter defined above,
     *  default values are then:
     *  - integration ="last"
     *  - mse_times =NULL
     *  - mse_observations=NULL
     *  - aggregation = mean
     */
    VleOutput(const std::string& id, const vle::value::Value& config);
    ~VleOutput();

    bool parsePath(const std::string& path);


    /**
     * @brief insert a replicate from a map of views
     * @param result, one simulation result (map of views)
     * @param initReplicateAccu, if true accumulators are initialized
     * @param initInputAccu, if true accumulators are initialized
     * @param nbInputs, nb inputs of the experiment plan (for allocation)
     * @param nbReplicates, nb replicates of the experiment plan (for check)
     */
    void insertReplicate(vle::value::Map& result,
            bool initReplicateAccu, bool initInputAccu, unsigned int nbInputs,
            unsigned int nbReplicates);
    /**
     * @brief insert a replicate from a view
     * @param result, one view (matrix) from one simulation result
     * @param initReplicateAccu, if true accumulators are initialized
     * @param initInputAccu, if true accumulators are initialized
     * @param nbInputs, nb inputs of the experiment plan (for allocation)
     * @param nbReplicates, nb replicates of the experiment plan (for check)
     */
    void insertReplicate(vle::value::Matrix& outMat,
            bool initReplicateAccu, bool initInputAccu, unsigned int nbInputs,
            unsigned int nbReplicates);

    /**
     * @brief insert result of an input combination
     * @note rely on replicate accumulators previously filled
     */
    void insertInput(unsigned int currentInput);

    /**
     * @brief build a vle value from input aggregation
     */
    std::unique_ptr<vle::value::Value> buildAggregateResult();

    std::string id;
    std::string view;
    std::string absolutePort;
    INTEGRATION_TYPE integrationType;
    AccuStat replicateAggregationType;
    AccuStat inputAggregationType;
    std::unique_ptr<vle::value::Tuple> mse_times;
    std::unique_ptr<vle::value::Tuple> mse_observations;

    //for replicate aggregation (one dimension or more)
    std::unique_ptr<AccuMono> mreplicateAccuMono;
    std::unique_ptr<AccuMulti> mreplicateAccuMulti;

    //for input aggregation (one dimension or more)
    std::unique_ptr<AccuMono> minputAccuMono;
    std::unique_ptr<AccuMulti> minputAccuMulti;

    //for replicate aggregation in the case values are not double
    std::unique_ptr<vle::value::Value> mreplicateInserter;
    //for aggregation in the case integration or inputAgreggation is ALL
    std::unique_ptr<vle::value::Value> minputInserter;

    //true if one manage double values
    bool manageDoubleValue;

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
    std::unique_ptr<vle::value::Map> run(const vle::value::Map& init,
            vle::manager::Error& err);


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

    std::unique_ptr<vle::value::Map> runIntern(
            const vle::value::Map& init,
            vle::manager::Error& err);
    void postInputsIntern(vle::vpz::Vpz& model, const vle::value::Map& init);
    void clear();
};

}}//namespaces

#endif
