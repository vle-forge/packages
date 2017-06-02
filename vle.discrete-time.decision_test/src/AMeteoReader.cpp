// @@tagdynamic@@
// @@tagdepends: vle.reader, vle.discrete-time @@endtagdepends

/*
 * Copyright (c) 2014-2014 INRA http://www.inra.fr
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to
 * deal in the Software without restriction, including without limitation the
 * rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
 * sell copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
 * IN THE SOFTWARE.
 */

#include <iostream>
#include <sstream>
#include <vle/utils/Tools.hpp>
#include <vle/utils/Context.hpp>
#include <vle/utils/Package.hpp>
#include <vle/utils/DateTime.hpp>
#include <vle/DiscreteTime.hpp>
#include <vle/reader/table_file_reader.hpp>

namespace record {
namespace meteo {

namespace vv = vle::value;
namespace vu = vle::utils;
using namespace vle::discrete_time;


enum METEO_TYPE {TRY_TO_FIND, GENERIC_WITH_HEADER, AGROCLIM};

class MeteoReader : public DiscreteTimeDyn
{
public:
    MeteoReader(const vle::devs::DynamicsInit& init,
            const vle::devs::InitEventList& events)
        : DiscreteTimeDyn(init, events), m_table_reader(),
          meteo_type(GENERIC_WITH_HEADER), line_read(), vars(), year_i(-1),
          month_i(-1),day_i(-1), nb_compute(0),
          begin_date(std::numeric_limits<double>::infinity())
    {
        if (!events.exist("meteo_type")) {
            meteo_type = TRY_TO_FIND;
        } else {
            const std::string& type = events.getString("meteo_type");
            if (type == "try_to_find") {
                meteo_type = TRY_TO_FIND;
            } else if (type == "generic_with_header") {
                meteo_type = GENERIC_WITH_HEADER;
            } else if (type == "agroclim") {
                meteo_type = AGROCLIM;
            } else {
                throw vu::ArgError(vu::format("[%s] Unexpected"
                        " meteo type `%s`" , getModelName().c_str(),
                        type.c_str()));
            }
        }
        if (events.exist("column_separator")) {
            m_table_reader.getParams().separator =
                    events.getString("column_separator");
        } else {
            switch (meteo_type) {
            case GENERIC_WITH_HEADER: {
                m_table_reader.getParams().separator = "\t";
                break;
            } case AGROCLIM: {
                m_table_reader.getParams().separator = ";";
                break;
            } default:
                throw vu::ArgError(vu::format("[%s]: meteo_type"
                        "  not implemented yet", getModelName().c_str()));
            }
        }
        std::string meteo_file_path;
        if (not events.exist("meteo_file")) {
            throw vu::ArgError(vu::format(
                    "[%s]: meteo_file missing)", getModelName().c_str()));
        }
        if (events.exist("PkgName") and events.getString("PkgName") != "") {
            auto ctx = vu::make_context();
            vu::Package pkg(ctx, events.getString("PkgName"));
            meteo_file_path = pkg.getDataFile(events.getString("meteo_file"));
        } else {
            meteo_file_path = events.getString("meteo_file");
        }
        m_table_reader.setFilePath(meteo_file_path);
        //configure table file reader
        switch (meteo_type) {
        case GENERIC_WITH_HEADER: {
            m_table_reader.readLine(line_read);
            vv::Set::iterator itb = line_read.begin();
            vv::Set::iterator ite = line_read.end();
            for (; itb!=ite; itb++) {
                vars.push_back(Var());
                vars[vars.size()-1].init(this, (*itb)->toString().value(), events);
            }
            for (unsigned int i=0; i<vars.size(); i++) {
                m_table_reader.getParams().col_types.push_back(vv::Value::DOUBLE);
            }
            break;
        } case AGROCLIM: {
            std::string line;
            bool skip = true;
            while (skip) {
                m_table_reader.readLine(line);
                if (line.substr(0, 9) == "NUM_POSTE") {
                    std::vector<std::string> strs;
                    vle::utils::tokenize(line, strs, ";", false);
                    for (unsigned int i=0; i<strs.size();i++) {
                        vars.push_back(Var());
                        vars[vars.size()-1].init(this, strs[i], events);
                        m_table_reader.getParams().col_types.push_back(
                                vv::Value::DOUBLE);
                    }
                    skip = false;
                }
            }
            break;
        } default:
            throw vu::ArgError(vu::format("[%s]: meteo_type"
                    " not implemented yet", getModelName().c_str()));
        }

        //jump to the correct line
        if (events.exist("begin_date")) {

            if (events.get("begin_date")->isDouble()) {
                begin_date = events.getDouble("begin_date");
            } else if (events.get("begin_date")->isString()) {
                begin_date = vu::DateTime::toJulianDay(
                        events.getString("begin_date"));
            }


            std::string year_col ="AN";//default agroclim
            std::string month_col ="MOIS";//default agroclim
            std::string day_col = "JOUR";//default agroclim
            if (events.exist("year_column")) {
                year_col.assign(events.getString("year_column"));
            }
            if (events.exist("month_column")) {
                month_col.assign(events.getString("month_column"));
            }
            if (events.exist("day_column")) {
                day_col.assign(events.getString("day_column"));
            }
            std::vector<Var>::const_iterator itb = vars.begin();
            std::vector<Var>::const_iterator ite = vars.end();
            int i =0;
            for (; itb != ite; itb++) {
                if (itb->name == year_col) {
                    year_i = i;
                } else if (itb->name == month_col) {
                    month_i = i;
                } else if (itb->name == day_col) {
                    day_i = i;
                }
                i++;
            }
            if (year_i == -1) {
                throw vu::ArgError(vu::format("[%s] index of"
                        " column `%s` not found", getModelName().c_str(),
                        year_col.c_str()));
            }
            if (month_i == -1) {
                throw vu::ArgError(vu::format("[%s] index of"
                        " column `%s` not found", getModelName().c_str(),
                        month_col.c_str()));
            }
            if (day_i == -1) {
                throw vu::ArgError(vu::format("[%s] index of"
                        " column `%s` not found", getModelName().c_str(),
                        day_col.c_str()));
            }
            bool skip = true;
            while (skip) {
                vle::value::Set lineToFill;
                m_table_reader.readLine(lineToFill);
                if (lineToFill.empty()) {
                    throw vu::ArgError(vu::format("[%s] cannot"
                            " find data in file '%s' for begin date equals: "
                            "`%s` ", getModelName().c_str(),
                            m_table_reader.getFilePath().c_str(),
                            vu::DateTime::toJulianDay(begin_date).c_str()));
                }
                std::stringstream ss;
                ss << lineToFill.getDouble(year_i);
                ss << "-";
                ss << lineToFill.getDouble(month_i);
                ss << "-";
                ss << lineToFill.getDouble(day_i);
                if (double(vu::DateTime::toJulianDayNumber(ss.str()))
                        >= begin_date) {
                    skip = false;
                }
            }

        }
    }

    virtual ~MeteoReader()
    {
    }

    void compute(const vle::devs::Time& t) override
    {
        nb_compute++;
        bool res = m_table_reader.readLine(line_read);
        if (!res) {
            std::string report;
            m_table_reader.fillWithError(report);
            throw vu::ModellingError(vu::format("[%s] error"
                    " reading line at time %s; report error:{\n %s}",
                    getModelName().c_str(),
                    vu::DateTime::toJulianDay(t).c_str(),
                    report.c_str()));
        }
        for (unsigned int i=0; i<vars.size(); i++) {
            vars[i] = line_read.getDouble(i);
        }
    }

    std::unique_ptr<vle::value::Value> observation(
        const vle::devs::ObservationEvent& event) const override
    {
        if (event.onPort("current_date_str")) {
            if (begin_date == std::numeric_limits<double>::infinity()) {
                return vv::String::create("no_begin_date");
            }
            return vv::String::create(vu::DateTime::toJulianDayNumber(
                    (unsigned long) (begin_date+nb_compute)));
        }
        if (event.onPort("current_date")) {
            return vv::Double::create(begin_date+ nb_compute);
        }

        return DiscreteTimeDyn::observation(event);
    }

    vle::reader::TableFileReader m_table_reader;
    METEO_TYPE meteo_type;
    vv::Set line_read;
    std::vector<Var> vars;
    int year_i;
    int month_i;
    int day_i;
    double nb_compute;
    double begin_date;
};

}}

DECLARE_DYNAMICS(record::meteo::MeteoReader)
