/*
 * This file is part of VLE, a framework for multi-modeling, simulation
 * and analysis of complex dynamical systems
 * http://www.vle-project.org
 *
 * Copyright (c) 2014-2014 INRA http://www.inra.fr
 *
 * See the AUTHORS or Authors.txt file for copyright owners and contributors
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


#ifndef VLE_READER_PARAMS_HPP
#define VLE_READER_PARAMS_HPP 1

#include <string>
#include <vector>
#include <vle/value/Value.hpp>
#include <vle/value/String.hpp>
#include <vle/value/Set.hpp>
#include <vle/value/Double.hpp>
#include <vle/value/Integer.hpp>

namespace vle {
namespace reader {

namespace vv=vle::value;

struct vle_reader_params
{
    vle_reader_params() :
        separator(" "), col_types()
    {
    }

    void set_params(const vv::Map& params)
    {
        if (params.exist("sep")) {
            separator.assign(params.getString("sep"));
        }
        if (params.exist("columns")) {
            const vv::Set& colTypes = params.getSet("columns");
            vv::Set::const_iterator itb = colTypes.begin();
            vv::Set::const_iterator ite = colTypes.end();
            for (; itb!=ite; itb++) {
                const std::string& colType = (*itb)->toString().value();
                if (colType == "double") {
                    col_types.push_back(vle::value::Value::DOUBLE);
                } else if (colType == "int") {
                    col_types.push_back(vle::value::Value::INTEGER);
                } else {
                    col_types.push_back(vle::value::Value::STRING);
                }
            }

        }
    }

    std::string separator;
    std::vector<vle::value::Value::type> col_types;
};

}} // namespaces

#endif
