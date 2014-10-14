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

#ifndef VLE_RECURSIVE_EmbeddedSimulatorInput_HPP_
#define VLE_RECURSIVE_EmbeddedSimulatorInput_HPP_

#include <boost/algorithm/string/split.hpp>
#include <boost/algorithm/string/classification.hpp>
#include <vle/utils/Exception.hpp>


namespace vle {
namespace recursive {

namespace vu = vle::utils;

/**
 * @brief Class that represent an input for embeded simulators
 * an input is represented as 'cond_name/port_name'
 */
struct EmbeddedSimulatorInput
{
    std::string cond_name;
    std::string port_name;

    /**
     *
     *
     * @brief EmbedSimulation constructor
     */
    EmbeddedSimulatorInput():
        cond_name(""), port_name("")
    {
    }
    EmbeddedSimulatorInput(const std::string& input):
        cond_name(""), port_name("")
    {
        init(input);
    }
    EmbeddedSimulatorInput(const std::string& cond,
            const std::string& port):
        cond_name(cond), port_name(port)
    {
    }

    bool isDefined()
    {
        return !cond_name.empty();
    }

    /**
     * @brief Initialize from string
     * @param input, input on format "condname/portname"
     */
    void init(const std::string& input)
    {
        std::vector <std::string> splitVec;
        boost::split(splitVec,
                input,
                boost::is_any_of("/"),
                boost::token_compress_on);
        if (splitVec.size() == 2) {
            cond_name = splitVec[0];
            port_name = splitVec[1];
        } else {
            throw vu::ArgError(vle::fmt("[EmbeddedSimulatorInput] : the input "
                    " is expected to be of the form "
                    "'cond_name/port_name', got '%1%'") % input);
        }
    }
    /**
     * @brief EmbedSimulation destructor
     */
    virtual ~EmbeddedSimulatorInput()
    {
    }

};

}}//namespaces

#endif
