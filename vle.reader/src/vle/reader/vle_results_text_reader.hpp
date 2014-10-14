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
#include <boost/algorithm/string/replace.hpp>
#include <boost/algorithm/string/split.hpp>
#include <boost/algorithm/string/classification.hpp>
#include <vle/value/Matrix.hpp>
#include <vle/value/Set.hpp>
#include <vle/value/Map.hpp>
#include <vle/utils/Exception.hpp>
#include <vle/value/Map.hpp>
#include <vle/reader/details/TFR_line_grammar.hpp>

namespace vle {
namespace reader {

namespace vv = vle::value;

class VleResultsTextReader
{
public:
    VleResultsTextReader(const std::string& filepath) :
                file_path(filepath), filestream(0)
    {
        boost::replace_all(file_path, "\r\n", "");
        boost::replace_all(file_path, "\n", "");
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
            throw vu::ArgError(vle::fmt("vle.reader: fails to topen %1% ") %
                    file_path);
        }
        matrixToFill.clear();

        //read the first line to get the number of columns
        std::string line;
        std::getline(*filestream, line);
        std::vector<std::string> strs;
        boost::split(strs,line,boost::is_any_of("\t"));
        //build parser parameter
        vle_reader_params p;
        p.separator.assign("\t");
        for (unsigned int i=0; i < strs.size(); i++) {
            if (not strs[i].empty()) {
                p.col_types.push_back(vv::Value::DOUBLE);
            }
        }
        TFR_matrix_tofill tofill(p, matrixToFill);
        matrixToFill.addRow();
        for (unsigned int i=0; i < strs.size(); i++) {
            if (not strs[i].empty()) {
                boost::replace_all(strs[i], "\"", "");
                boost::replace_all(strs[i], "#", "");
                matrixToFill.set(i,0, new vv::String(strs[i]));
            }
        }

        TFR_line_grammar g(p, tofill);
        tofill.nextRow();
        do {
            parseLine(g,line);
            tofill.nextRow();
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


    bool parseLine(TFR_line_grammar& g, std::string& line)
    {
        std::getline(*filestream, line);
        if (not line.empty()) {
            parse_info<> resParsing =
                    BOOST_SPIRIT_CLASSIC_NS::parse(line.c_str(), g);
            if (! resParsing.full) {
                std::ostringstream report_line;
                report_line << line << " (failed to parse, reason: "
                        << resParsing.stop << ")";
                report.push_back(report_line.str());
            } else if (g.tofill.params.col_types.size() != g.tofill.currColIndex){
                std::ostringstream report_line;
                report_line << line << " (wrong number of elements, expected: "
                        << g.tofill.params.col_types.size() <<  "; got "
                        << g.tofill.currColIndex << ")";
                report.push_back(report_line.str());
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
    }
};



}} // namespaces

#endif
