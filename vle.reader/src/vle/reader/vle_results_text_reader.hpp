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

#ifndef VLE_READER_VLE_RESULTS_TEXT_READER_HPP
#define VLE_READER_VLE_RESULTS_TEXT_READER_HPP 1

#include <vector>
#include <string>
#include <ostream>
#include <fstream>
#include <vle/utils/Exception.hpp>
#include <vle/utils/Tools.hpp>
#include <vle/value/Matrix.hpp>
#include <vle/value/Set.hpp>
#include <vle/value/Map.hpp>
#include <vle/value/Map.hpp>
#include <vle/reader/details/vle_line_parser.hpp>

namespace vle {
namespace reader {

namespace vv = vle::value;
namespace vu = vle::utils;

class VleResultsTextReader
{
public:
    VleResultsTextReader(const std::string& filepath) :
                file_path(filepath), filestream(0)
    {
    }

    virtual ~VleResultsTextReader()
    {
        clearFileStream();
    }

    bool readFile(vv::Matrix& matrixToFill)
    {
        clearFileStream();
        filestream = new std::ifstream(file_path.c_str());
        if (not filestream->good() or not filestream->is_open()) {
            throw vu::ArgError(vle::utils::format(
                    "vle.reader: fails to open %s ",
                    file_path.c_str()));
        }
        matrixToFill.clear();

        //read the first line to get the number of columns
        std::string line;
        std::getline(*filestream, line);
        value::Set lineToFill;
        vle_reader_params p;
        p.separator="\t";
        vle_line_parser::parseLine(p, line, lineToFill);
        lineToFill.remove(lineToFill.size()-1);//extra \t at the end of header

        //build parser parameter

        for (unsigned int i=0; i < lineToFill.size(); i++) {
            if (not lineToFill.getString(i).empty()) {
                p.col_types.push_back(vv::Value::DOUBLE);
            }
        }
        matrixToFill.resize(p.col_types.size(), 1);

        for (unsigned int i=0; i < lineToFill.size(); i++) {
            if (not lineToFill.getString(i).empty()) {

                matrixToFill.set(i,0, std::move(lineToFill.give(i)));
            }
        }


        do {
            std::getline(*filestream, line);
            if (not line.empty()) {
                vle_line_parser::parseLine(p,line,lineToFill);
                matrixToFill.addRow();
                for (unsigned int i=0;i<matrixToFill.columns();i++) {
                    matrixToFill.set(i, matrixToFill.rows()-1, std::move(
                            lineToFill.give(i)));
                }
            }

        } while (not filestream->eof() and filestream->good());

        clearFileStream();
        return true;
    }

    bool hasError()
    {
        return report.size() != 0;
    }

    std::ostream& printError(std::ostream& ios)
    {
        for (unsigned int i=0; i < report.size(); i++) {
            ios << report[i] << std::endl;
        }
        return ios;
    }

private:
    std::string file_path;
    std::ifstream* filestream;
    std::vector<std::string> report;

    void clearFileStream()
    {
        if (filestream != 0) {
            filestream->close();
            delete filestream;
            filestream = 0;
        }
    }
};



}} // namespaces

#endif
