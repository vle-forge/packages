
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

struct VleInput
{
    /*
     * @brief VleInput identifies an input of the experiment plan
     * @param conf, a string of the form 'condname.portname'
     * @param val, the value given on port conf_name, is either
     *  * a value::Set or value::Tuple that identifies an experiment plan
     *  * any other value type that identifies a single value
     */
    VleInput(const std::string& conf, const vle::value::Value& config);
    virtual ~VleInput();
    /**
     * @brief Gets the experiment plan from initialization map
     * @param init, the initialization map
     * @param replicate, true if this is the replicate and not an input
     * @return a reference to the set of values
     */
    const vle::value::Value& values(const vle::value::Map& init,
            bool replicate=false);
    std::string getName();

    std::string cond;
    std::string port;
    INPUT_TYPE type;
    unsigned int nbValues;

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
    VleInput* mReplicate;
    std::vector<VleOutput> mOutputs;//view * port
    std::vector<vle::value::Value*> mOutputValues;//values are Tuple or Set
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
     * @brief Simulates the experiment plan
     * @param init, the initialization map
     * @return the values
     */
    vle::value::Matrix* run(const vle::value::Map& init);

private:

    unsigned int inputsSize() const;
    unsigned int replicasSize() const;
    vle::value::Matrix* runIntern(const vle::value::Map& init);
    void postInputsIntern(vle::vpz::Vpz& model, const vle::value::Map& init);
};

}}//namespaces

#endif