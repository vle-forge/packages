
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
