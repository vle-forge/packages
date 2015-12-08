
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

#include <vle/recursive/accu_multi.hpp>

namespace vle {
namespace recursive {

enum INPUT_TYPE {MONO, MULTI};
enum CONFIG_PARALLEL_TYPE {THREADS, MVLE, SINGLE};
enum OUTPUT_INTEGRATION_TYPE {LAST, MAX, MSE, ALL};

/*
 * VleInput indetifies an input of the experiment plan
 * For example:
 * id_input_X : condname/portname => id is X
 * Then expects either one of the two elements:
 * - value_X: for a simple value (MONO)
 * - values_X
 */
struct VleInput
{
    VleInput(const std::string& id, const std::string& str,
            const vle::value::Map& config);
    virtual ~VleInput();
    unsigned int nbValues() const;

    std::string id;
    std::string cond;
    std::string port;
    INPUT_TYPE type;
    vle::value::Value* inputValues;

};

/*
 * VleOutput identifies a VleOutput from a configuration str
 * For example with str =
 * max[view/ExBohachevsky:ExBohachevsky.y]
 * =>
 * integrationType= MAX
 * view = view
 * absolutePort = ExBohachevsky:ExBohachevsky.y
 */

struct VleOutput
{
public:
    VleOutput();
    /**
     * @brief VleOutput constructor
     * @param id, id of the input
     * @param config is either a map containing:
     *  - "path" path of the form "coupled:atomic.port"
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

    bool parsePath(const std::string& path);

    /**
     * @brief insert output value from a view map
     */
    void insertReplicate(const vle::value::Map& result);

    /**
     * @brief insert output value from a view
     */
    void insertReplicate(const vle::value::Matrix& outMat);

    void initAggregateResult();

    /**
     * @brief build a vle value (Tuple or Double) from aggregation
     */
    vle::value::Value* buildAggregateResult();

    std::string id;
    std::string view;
    std::string absolutePort;
    OUTPUT_INTEGRATION_TYPE integrationType;
    AccuType aggregationType;
    vle::value::Tuple* mse_times;
    vle::value::Tuple* mse_observations;

    //for mean aggregation
    AccuMono* maccuMono; //for integration with one dimension
    AccuMulti* maccuMulti; //for integration with multiple dimensions
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
    std::vector<VleInput*> mInputs;
    std::string mIdReplica;
    std::pair<std::string, std::string> mReplica; //cond * port
    vle::value::Set* mReplicaValues;//values are Tuple or Set
    std::vector<VleOutput> mOutputs;//view * port
    std::vector<vle::value::Value*> mOutputValues;//values are Tuple or Set
    vle::value::Matrix* mResults;
    std::string mWorkingDir; //only for mvle


public:
    /**
     * @brief MetaManager constructor
     */
    MetaManager();
    /**
     * @brief MetaManager destructor
     */
    virtual ~MetaManager();
    /**
     * @brief Initialize the MetaManager
     * @param init
     */
    void init(const vle::value::Map& init);

    /**
     * @brief Simulates the experiment plan
     * @return the values
     */
    const vle::value::Value& launchSimulations();
    /**
     * @brief Get the results (possibly empty)
     */
    vle::value::Matrix* getResults();




private:


    unsigned int inputsSize() const;
    unsigned int replicasSize() const;
    void postInputs(vle::vpz::Vpz& model) const;
};

}}//namespaces

#endif
