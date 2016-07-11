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


#ifndef VLE_LINE_PARSER_HPP
#define VLE_LINE_PARSER_HPP 1

#include <string>
#include <vector>
#include <regex>
#include <iostream>
#include <vle/value/Value.hpp>
#include <vle/reader/details/vle_reader_params.hpp>

namespace vle {
namespace reader {

namespace vv=vle::value;

struct vle_line_parser
{
    vle_line_parser()
    {
    }

    static bool parseLine(const vle_reader_params& params, std::string& line,
            vv::Set& lineToFill)
    {
        lineToFill.clear();
        if (line.empty()) {
            return false;
        }
        //split string according separator or \"
        std::regex regex(
                "[^"+params.separator+"\"]+|["+params.separator+"\"]");

        std::string current = "";
        bool inString = false;

        std::sregex_iterator next(line.begin(), line.end(), regex);
        std::sregex_iterator end;
        while (next != end) {
            std::smatch match = *next;
            if (match.str() == params.separator) {
                if (not inString) {
                    if (params.col_types.size() > 0) {
                        if (params.col_types.size() < lineToFill.size()+1) {
                            return false;
                        }
                        lineToFill.add(std::move(convertToValue(current,
                                params.col_types[lineToFill.size()])));
                    } else {
                        lineToFill.add(std::move(convertToValue(current,
                                value::Value::STRING)));
                    }
                    current.erase();
                } else {
                    current += match.str();
                }
            } else if(match.str() == "\"") {
                inString = not inString;
            } else {
                current += match.str();
            }
            next++;
        }

        if (inString) {
            std::cout << " Erro parsing \n"; //TODO
        }

        if (params.col_types.size() > 0) {
            if (params.col_types.size() < lineToFill.size()+1) {
                return false;
            }
            lineToFill.add(std::move(convertToValue(current,
                    params.col_types[lineToFill.size()])));
        } else {
            lineToFill.add(std::move(convertToValue(current,
                    value::Value::STRING)));
        }

        return true;
    }

    static std::unique_ptr<value::Value>
    convertToValue(const std::string& str, value::Value::type t)
    {
        switch (t) {
        case vv::Value::DOUBLE : {
            double val;
            if (str.empty() or str == "NA") {
                val = NAN;
            } else {
                std::istringstream iss (str);
                iss.imbue (std::locale("C"));
                iss >> val;
            }
            return std::unique_ptr<value::Value>(new vv::Double(val));
            break;
        } case vv::Value::INTEGER : {
            int val;
            if (str.empty()) {
                val = -999;
            } else {
                val = std::stoi(str);
            }
            return std::unique_ptr<value::Value>(new vv::Integer(val));
            break;
        } case vv::Value::STRING : {
            return std::unique_ptr<value::Value>(new vv::String(str));
            break;
        } default: {
            return std::unique_ptr<value::Value>(new vv::String(str));
            break;
        }}
    }


    std::string separator;
    std::vector<vle::value::Value::type> col_types;
};

}} // namespaces

#endif
