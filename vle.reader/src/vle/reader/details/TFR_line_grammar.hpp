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


#ifndef VLE_READER_DETAILS_TFR_LINE_GRAMMAR_HPP
#define VLE_READER_DETAILS_TFR_LINE_GRAMMAR_HPP 1


#include <fstream>

#define BOOST_SPIRIT_THREADSAFE
#include <boost/lexical_cast.hpp>
#include <boost/algorithm/string/replace.hpp>
#include <boost/spirit/include/classic_core.hpp>
#include <boost/spirit/include/classic_push_back_actor.hpp>
using namespace BOOST_SPIRIT_CLASSIC_NS;

#include <vle/utils/Exception.hpp>
#include <vle/value/Set.hpp>
#include <vle/value/Double.hpp>
#include <vle/value/String.hpp>
#include <vle/reader/details/vle_reader_params.hpp>


namespace vle {
namespace reader {

namespace vv = vle::value;
namespace vu = vle::utils;


struct TFR_data_tofill
{
    const vle_reader_params& params;
    unsigned int currColIndex;

    TFR_data_tofill(const vle_reader_params& p):
        params(p), currColIndex(0)
    {
    }

    virtual ~TFR_data_tofill(){}
    virtual void fill_with(const char* beg, const char* end) = 0;
    vv::Value* buildNewValueFromChar(const char* beg, const char* end,
            unsigned int colIndex)
    {
        std::string valStr(beg, end);
        if (params.col_types.size() > 0) {
            switch (params.col_types[colIndex]) {
            case vv::Value::DOUBLE : {
                double val;
                if (valStr.empty()) {
                    val = NAN;
                } else {
                    val = boost::lexical_cast<double>(valStr);
                }
                return new vv::Double(val);
                break;
            } case vv::Value::INTEGER : {
                int val;
                if (valStr.empty()) {
                    val = NAN;
                } else {
                    val = boost::lexical_cast<int>(valStr);
                }
                return new vv::Integer(val);
                break;
            } case vv::Value::STRING : {
                return new vv::String(valStr);
                break;
            } default: {
                return new vv::String(valStr);
                break;
            }}
        } else {
            return new vv::String(valStr);
        }
    }
};

struct TFR_matrix_tofill : public TFR_data_tofill
{
    vv::Matrix& data;
    unsigned int currRowIndex;


    TFR_matrix_tofill(const vle_reader_params& p, vv::Matrix& d) :
        TFR_data_tofill(p), data(d), currRowIndex(0)
    {
        data.clear();
        for (unsigned int i=0; i < params.col_types.size(); i++) {
            data.addColumn();
        }
    }

    void nextRow()
    {
        currRowIndex++;
        currColIndex=0;
    }

    void fill_with(const char* beg, const char* end)
    {
        if (currColIndex == 0) {
            data.addRow();
        }
        data.set(currColIndex, currRowIndex,
           TFR_data_tofill::buildNewValueFromChar(beg, end, currColIndex));
        currColIndex++;
    }



};

struct TFR_set_tofill : public TFR_data_tofill
{
    vv::Set& data;

    TFR_set_tofill(const vle_reader_params& p, vv::Set& d) :
        TFR_data_tofill(p), data(d)
    {
        data.clear();
    }

    void fill_with(const char* beg, const char* end)
    {
        data.add(
          TFR_data_tofill::buildNewValueFromChar(beg, end, currColIndex));
        currColIndex++;
    }



};


struct TFR_line_grammar : public grammar<TFR_line_grammar>
{
    struct fill_char
    {
        fill_char(TFR_data_tofill& tf): tofill(tf)
        {
        }
        void operator()(const char* beg, const char* end) const
        {
            tofill.fill_with(beg, end);
        }
        TFR_data_tofill& tofill;
    };



    const vle_reader_params& params;
    TFR_data_tofill& tofill;
    fill_char fc;


    TFR_line_grammar(const vle_reader_params& p, TFR_data_tofill& tf) :
        params(p), tofill(tf), fc(tf)
    {
    }

    template <typename ScannerT>
    struct definition
    {
        char sep;

        //lexcial elements
        rule<ScannerT>  lex_string_quoted;
        rule<ScannerT>  lex_string_quoted_interior;
        rule<ScannerT>  lex_string_not_quoted;
        rule<ScannerT>  lex_string_empty;
        rule<ScannerT>  lex_string_not_empty;

        rule<ScannerT>  lex_string;
        //main rule
        rule<ScannerT>  rule_root;

        //elements to fill
        fill_char const& fc;

        //parser parameters
        const vle_reader_params& params;

        definition(TFR_line_grammar const& self) :
            fc(self.fc), params(self.params)
        {
            if (params.separator.size() > 0) {
                sep = params.separator.c_str()[0];
            } else {
                sep =' ';
            }

            //lexcial elements
            lex_string_not_quoted = +(~ch_p('"') & ~ch_p(sep) & ~blank_p) ;
            lex_string_quoted_interior = +((blank_p | graph_p) & ~ch_p('"'));
            lex_string_quoted = ch_p('"') >> lex_string_quoted_interior[fc] >> ch_p('"');
            lex_string_not_empty =  lex_string_quoted | lex_string_not_quoted[fc] ;
            lex_string_empty =  str_p("")[fc] ;
            lex_string = lex_string_not_empty | lex_string_empty;

            //root level
            rule_root =
              (
                lex_string >> *(ch_p(sep) >> lex_string)
              );
        }
        rule<ScannerT> const& start() const { return rule_root; }
    };
};

}}//namespaces

#endif
