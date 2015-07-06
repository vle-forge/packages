
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

namespace vle {
namespace recursive {

enum INPUT_TYPE {MONO, MULTI};
enum CONFIG_PARALLEL_TYPE {THREADS, MVLE, SINGLE};
enum OUTPUT_STAT {MEAN};
enum OUTPUT_INTEGRATION_TYPE {LAST, MAX};

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
    VleOutput();
    VleOutput(const std::string& id, const std::string& str);
    double findOutputValue(const vle::value::Map& result) const;
    std::string id;
    std::string view;
    std::string absolutePort;
    OUTPUT_INTEGRATION_TYPE integrationType;

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
    unsigned int mConfigParallelNbSlots;
    unsigned int mConfigParallelMaxExpes;
    std::vector<VleInput*> mInputs;
    std::string mIdReplica;
    std::pair<std::string, std::string> mReplica; //cond * port
    vle::value::Set* mReplicaValues;//values are Tuple or Set
    std::vector<VleOutput> mOutputs;//view * port
    std::vector<OUTPUT_STAT> mOutputStats;
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
