
/*
 * Copyright (C) 2014 INRA
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

#ifndef VLE_RECURSIVE_EmbeddedSimulatorReplicas_HPP_
#define VLE_RECURSIVE_EmbeddedSimulatorReplicas_HPP_

#include <vle/manager/Manager.hpp>
#include <vle/value/Map.hpp>
#include <vle/value/Matrix.hpp>
#include <vle/recursive/EmbeddedSimulator.hpp>


namespace vle {
namespace recursive {


namespace vu = vle::utils;
namespace vm = vle::manager;
namespace vz = vle::vpz;
namespace vv = vle::value;
namespace vo = vle::oov;

/**
 * @brief Class that implements an simulator of replicas * combinations
 */
class EmbeddedSimulatorReplicas : public EmbeddedSimulator
{

public:
    /**
     * @brief EmbeddedSimulatorReplicas constructor
     */
    EmbeddedSimulatorReplicas();

    /**
     * @brief Initialize the Simulator
     * @param initSim
     */
    void init(const vv::Map& initSim);


    /**
     * @brief EmbeddedSimulatorReplicas destructor
     */
    virtual ~EmbeddedSimulatorReplicas();

    /**
     * @brief Simulates an input plan
     * @param input, the container of inputs to simulate (either a vv::Tuple,
     * a vv::Table, or a vv::Set)
     * @param nbReplicas, number of replicates to
     * @param commonSeed, if true all combinations are simulated
     * with the same set of seeds
     */
    void simulateReplicas(const vv::Value& input, unsigned int nbReplicas,
            bool commonSeed);

    /**
     * @brief Number of simulations
     * @return the number of simulations performed for the last
     * simulations
     */
    unsigned int nbSimus() const;

    /**
     * @brief Number of outputs
     * @return the number of simulation inputs
     */
    unsigned int nbInputs() const;

    /**
     * @brief Number of outputs
     * @return the number of simulation outputs
     */
    unsigned int nbOutputs() const;

    /**
     * @brief get a map resulting from one simulation
     * @param idComb, index of combination
     * @param idRepl, index of replicate
     * @return a const view on the column
     */
    const vv::Map& getSimulationResults(unsigned int idComb,
                unsigned int idRepl) const;

    /**
     * @brief get last value of an output variable
     * @param idComb, index of combination
     * @param idRepl, index of replicate
     * @param idOutput, index of output
     * @return a const value
     */
    const vv::Value& lastValue(unsigned int idComb,
            unsigned int idRepl, unsigned int idOutput) const;

    /**
     * @brief Fills a vv::Set with outputs of a simulation
     * @param tofill, the set to fill
     * @param idComb, index of combination
     * @param idRepl, index of replicate
     *
     */
    void fillWithLastValuesOfOutputs(vv::Set& tofill, unsigned int idComb,
            unsigned int idRepl) const;

    /**
     * @brief Set the number of threads for simulation
     */
    void setNbThreads(unsigned int nbThreads);

private:

    /**
     * @brief Clear all condition values
     */
    void clearAllCondPortRand();

    /**
     * @brief Number of combinations
     * @param input a vv::Value containing the values of an input
     */

    unsigned int nbComb(const vv::Value& input) const;
    /**
     * @brief Fills the vpz cond port inputRand with seeds
     * @param nbComb, the set osf seed to simulate
     * @param nbRepl, the number of replicates
     * @param commonSeed, if true all combinations are simulated
     * with the same set of seeds
     */
    void postSeeds(unsigned int nbComb, unsigned int nbRepl,
            bool commonSeed);

    /**
     * @brief Fills the vpz cond port from inputs
     * @param input a vv::Value containing the values of an input
     * (either a vv::Tuple or a vv::Set)
     */
    void postInputs(const vv::Value& input);

    /**
     * @brief Raw output, a matrix issued from plan simulation
     */
    vv::Matrix* mraw_outputs_plan;
    /**
     *@brief a view on raw outputs
     */
    unsigned int mnbthreads;
    /**
     *@brief Index of random inputs
     */
    EmbeddedSimulatorInput minputRand;

    /**
     * @brief the Simulation engine for plans
     */
    vm::Manager* mPlanSimulator;
    /**
     * @brief Random number generator
     */
    vu::Rand mrand;
};

}}//namespaces

#endif
