
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

#ifndef VLE_RECURSIVE_EmbeddedSimulatorSingle_HPP_
#define VLE_RECURSIVE_EmbeddedSimulatorSingle_HPP_

#include <vle/vpz/Vpz.hpp>
#include <vle/utils/Exception.hpp>
#include <vle/utils/Package.hpp>
#include <vle/manager/Simulation.hpp>
#include <vle/value/Double.hpp>

#include <vle/recursive/VleAPIfacilities.hpp>
#include <vle/recursive/EmbeddedSimulator.hpp>
#include <vle/recursive/EmbeddedSimulatorInput.hpp>
#include <vle/recursive/EmbeddedSimulatorOutput.hpp>


//#include <boost/lexical_cast.hpp>

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
class EmbeddedSimulatorSingle : public EmbeddedSimulator
{

public:
    /**
     * @brief EmbeddedSimulatorSingle constructor
     */
    EmbeddedSimulatorSingle();

    /**
     * @brief Initialize the Simulator
     * @param initSim
     */
    void init(const vv::Map& initSim);


    /**
     * @brief EmbeddedSimulatorSingle destructor
     */
    virtual ~EmbeddedSimulatorSingle();

    /**
     * @brief Simulates a simple simulation
     * @param input, the input (any vle::value)
     */
    void simulate(const vv::Value& input);

    /**
     *  @brief Fills a vv::Set with outputs of the last simulation
     * @param tofill, the vv::Value to fill
     */
    void fillWithLastValuesOfOutputs(vv::Value& tofill) const;

    /**
     * @brief get a column
     * @param outputVar, the outputVar under the form 'view/variable'
     * @return a const view on the column
     */
    vv::ConstVectorView getTimeCol(const std::string& outputVar);
    /**
     * @brief Number of outputs
     * @return the number of outputs declared
     */
    unsigned int nbOutputs() const;

private:

    /**
     * @brief Fills the vpz cond port from one input
     * @param input a vv::Value containing the value of an input
     * (any vle::value)
     */
    void postOneInput(const vv::Value& input);

    /**
     * @brief Raw output, a map issued from single simulation
     */
    vv::Map* mraw_outputs_single;
    /**
     * @brief the Simulation engine for plans
     */
    vm::Simulation* mSingleSimulator;
};

}}//namespaces

#endif
