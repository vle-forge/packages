
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

#ifndef VLE_RECURSIVE_EmbeddedSimulatorPlan_HPP_
#define VLE_RECURSIVE_EmbeddedSimulatorPlan_HPP_

#include <vle/value/Map.hpp>
#include <vle/recursive/EmbeddedSimulator.hpp>

namespace vle {
namespace recursive {

namespace vu = vle::utils;
namespace vm = vle::manager;
namespace vz = vle::vpz;
namespace vv = vle::value;
namespace vo = vle::oov;

/**
 * @brief Class that implements an abstract simulator of
 * the simOptim library based on a VLE simulator.
 */
class EmbeddedSimulatorPlan : public EmbeddedSimulator
{

public:
    /**
     * @brief EmbeddedSimulatorPlan constructor
     */
    EmbeddedSimulatorPlan();
    /**
     * @brief Initialize the Simulator
     * @param initSim
     */
    void init(const vv::Map& initSim);
    /**
     * @brief EmbeddedSimulatorPlan destructor
     */
    virtual ~EmbeddedSimulatorPlan();
    /**
     * @brief Simulates an input plan
     * @param input, the container of inputs to simulate (either a vv::Tuple,
     * a vv::Table, or a vv::Set)
     */
    void simulatePlan(const vv::Value& input);
    /**
     * @brief Number of simulations
     * @return the number of simulations performed for the last
     * simulations
     */
    unsigned int nbSimus();
    /**
     * @brief get a column
     * @param idSim, index of simulation
     * @param outputVar, the outputVar under the form 'view/variable'
     * @return a const view on the column
     */
    vv::ConstVectorView getTimeCol(unsigned int idSim,
                const std::string& outputVar);
    /**
     * @brief Set the number of threads for simulation
     */
    void setNbThreads(unsigned int nbThreads);

    /**
     *  @brief Fills a vv::Tuple with outputs of the last simulation
     *  if moutputs=1
     * @param tofill, the vv::Value to fill
     */
    void fillWithLastValuesOfOutputs(vv::Value& tofill) const;
    /**
     * @brief Number of simulations
     * @return the number of simulations performed for the last
     * simulations
     */
    unsigned int nbSimus() const;

private:

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
     * @brief the Simulation engine for plans
     */
    vm::Manager* mPlanSimulator;

};

}}//namespaces

#endif
