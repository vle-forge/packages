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

#ifndef VLE_UTILS_TABLE_FILE_READER_HPP
#define VLE_UTILS_TABLE_FILE_READER_HPP 1

#include <vector>
#include <string>
#include <ostream>
#include <fstream>
#include <regex>
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


class TableFileReader
{
public:

    TableFileReader() :
        file_path(), filestream(0), params_parser(), report(), stream_place(0)
    {
    }

    TableFileReader(const std::string& filepath) :
        file_path(filepath), filestream(0), params_parser(), report(),
        stream_place()
    {
    }

    virtual ~TableFileReader()
    {
        clearFileStream();
    }

    void setParams(const vv::Map& params)
    {
        params_parser.set_params(params);
    }

    bool readLine(std::string& lineStr)
    {
        if (filestream == 0) {
            openFileStream();
        }
        std::getline(*filestream, lineStr);
        if (filestream->eof() or not filestream->good()) {
            clearFileStream();
        }
        return true;
    }

    bool readLine(vv::Set& lineToFill, const std::string& sep)
    {
        if (filestream == 0) {
            openFileStream();
        }
        params_parser.separator = sep;
        params_parser.col_types.clear();
        std::string line;



        lineToFill.clear();
        stream_place = filestream->tellg();
        std::getline(*filestream, line);
        bool res = vle_line_parser::parseLine(params_parser, line, lineToFill);

        if (filestream->eof() or not filestream->good()) {
            clearFileStream();
        }
        return res;
    }

    bool readLine(vv::Set& lineToFill)
    {
        if (filestream == 0) {
            openFileStream();
        }

        std::string line;

        lineToFill.clear();
        stream_place = filestream->tellg();
        std::getline(*filestream, line);
        bool res = vle_line_parser::parseLine(params_parser, line, lineToFill);

        if (filestream->eof() or not filestream->good()) {
            clearFileStream();
        }
        return res;
    }

    bool readLineUndo()
    {
        if (filestream != 0) {
            filestream->seekg(stream_place);
            return true;
        } else {
            return false;
        }
    }

    bool readFile(vv::Matrix& matrixToFill)
    {
        clearFileStream();
        openFileStream();

        std::string line;
        bool res = true;
        vle::value::Set lineToFill;
        do {
            stream_place = filestream->tellg();
            std::getline(*filestream, line);
            if (not line.empty()){
                res = res and vle_line_parser::parseLine(
                        params_parser,line,lineToFill);
                if (matrixToFill.rows() == 0) {
                    if (params_parser.col_types.size() > 0) {
                        matrixToFill.resize(lineToFill.size(), 1);
                    }
                } else {
                    matrixToFill.addRow();
                }
                for (unsigned int i=0;i<matrixToFill.columns();i++) {
                    matrixToFill.set(i, matrixToFill.rows()-1, std::move(
                            lineToFill.give(i)));
                }
            }
        } while (not filestream->eof() and filestream->good()
                 and not line.empty());
        clearFileStream();
        return res;
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

    void fillWithError(std::string& s)
    {
        for (unsigned int i=0; i < report.size(); i++) {
            s.append(report[i]);
            s.append("\n");
        }
    }

    const std::string& getFilePath() const
    {
        return file_path;
    }

    void setFilePath(const std::string& filePath)
    {
        clearFileStream();
        file_path.assign(filePath);
    }

    vle_reader_params& getParams()
    {
        return params_parser;
    }

private:
    std::string file_path;
    std::ifstream* filestream;
    vle_reader_params params_parser;
    std::vector<std::string> report;
    int stream_place;


    void clearFileStream()
    {
        if (filestream != 0) {
            filestream->close();
            delete filestream;
            filestream = 0;
        }
        stream_place = -1;
    }
    void openFileStream()
    {
        filestream = new std::ifstream(file_path.c_str());
        if (not filestream->good() or not filestream->is_open()) {
            throw vu::ArgError(vle::utils::format(
                    "vle.reader: fails to topen %s ",
                    file_path.c_str()));
        }
        stream_place = filestream->tellg();
    }
};



}} // namespaces

#endif
