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
#include <boost/algorithm/string/replace.hpp>
#include <vle/value/Matrix.hpp>
#include <vle/value/Set.hpp>
#include <vle/value/Map.hpp>
#include <vle/utils/Exception.hpp>
#include <vle/value/Map.hpp>
#include <vle/reader/details/TFR_line_grammar.hpp>
#include <vle/reader/details/vle_reader_params.hpp>


namespace vle {
namespace reader {

namespace vv = vle::value;


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
        boost::replace_all(file_path, "\r\n", "");
        boost::replace_all(file_path, "\n", "");
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
        TFR_set_tofill tofill(params_parser, lineToFill);
        TFR_line_grammar g(params_parser, tofill);
        std::string line;
        bool res = parseLine(g, line);
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
        TFR_set_tofill tofill(params_parser, lineToFill);
        TFR_line_grammar g(params_parser, tofill);
        std::string line;
        bool res = parseLine(g, line);
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

        TFR_matrix_tofill tofill(params_parser, matrixToFill);
        TFR_line_grammar g(params_parser, tofill);
        std::string line;
        bool res = true;
        do {
            res = res & parseLine(g,line);
            tofill.nextRow();
        } while (not filestream->eof() and filestream->good());
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


    bool parseLine(TFR_line_grammar& g, std::string& line)
    {
        stream_place = filestream->tellg();
        std::getline(*filestream, line);
        if (line.empty()) {
            return false;
        }
        if (not line.empty()) {
            parse_info<> resParsing =
                    BOOST_SPIRIT_CLASSIC_NS::parse(line.c_str(), g);
            if (! resParsing.full) {
                std::ostringstream report_line;
                report_line << line << " (failed to parse, reason: "
                        << resParsing.stop << ")";
                report.push_back(report_line.str());
                return false;
            } else if ((g.tofill.params.col_types.size() > 0) &&
                    (g.tofill.params.col_types.size()!= g.tofill.currColIndex)){
                std::ostringstream report_line;
                report_line << line << " (wrong number of elements, expected: "
                        << g.tofill.params.col_types.size() <<  "; got "
                        << g.tofill.currColIndex << ")";
                report.push_back(report_line.str());
                return false;
            }
        }
        return true;
    }

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
            throw vu::ArgError(vle::fmt("vle.reader: fails to topen %1% ") %
                    file_path);
        }
        stream_place = filestream->tellg();
    }
};



}} // namespaces

#endif
