
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

#ifndef VLE_RECURSIVE_EmbeddedSimulatorOutput_HPP_
#define VLE_RECURSIVE_EmbeddedSimulatorOutput_HPP_

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
struct EmbeddedSimulatorOutput
{
    std::string view_name;
    std::string port_name;
    std::string complete_name;

    /**
     * @brief EmbeddedSimulatorOutput constructor
     */
    EmbeddedSimulatorOutput(const std::string& input):
        view_name(""), port_name(""), complete_name(input)
    {
        std::vector <std::string> splitVec;
        boost::split(splitVec,
                     input,
                     boost::is_any_of("/"),
                     boost::token_compress_on);
        if (splitVec.size() == 2) {
            view_name = splitVec[0];
            port_name = splitVec[1];
        } else {
            throw vu::ArgError(vle::fmt("[EmbeddedSimulatorOutput] : the output "
                    "(here = '%1%') is expected to be of the form "
                    "'view_name/port_name'") % input);
        }
    }
    EmbeddedSimulatorOutput(const std::string& view,
            const std::string& port):
        view_name(view), port_name(port), complete_name("")
    {
        complete_name.assign(view_name);
        complete_name.append("/");
        complete_name.append("port_name");
    }
    /**
     * @brief EmbedSimulation destructor
     */
    virtual ~EmbeddedSimulatorOutput()
    {
    }
    /*
     *
     */

};

}}//namespaces

#endif
