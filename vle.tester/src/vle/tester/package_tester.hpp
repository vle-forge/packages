/*
 * This file is part of VLE, a framework for multi-modeling, simulation
 * and analysis of complex dynamical systems.
 * http://www.vle-project.org
 *
 * Copyright (c) 20014-2014 INRA http://www.inra.fr
 *
 * See the AUTHORS or Authors.txt file for copyright owners and
 * contributors
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or (at
 * your option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef _VLE_UTILS_PACKAGE_TESTER_HPP
#define _VLE_UTILS_PACKAGE_TESTER_HPP 1

#include <iostream>
#include <string>
#include <exception>

#include <boost/lexical_cast.hpp>
#include <vle/utils/Package.hpp>
#include <vle/value/Map.hpp>
#include <vle/value/Set.hpp>
#include <vle/value/Matrix.hpp>
#include <vle/reader/table_file_reader.hpp>
#include <vle/tester/package_tester.hpp>
#include <vle/tester/details/pkg_tester_simulation.hpp>

namespace vle {
namespace tester {

namespace vu = vle::utils;
namespace vv = vle::value;
namespace vz = vle::vpz;


class PackageTester
{
public:
    PackageTester()
    {
    }

    virtual ~PackageTester()
    {
    }

    static bool check(const std::string& pkgname,
            const std::string& test_file_name)
    {
        std::cout << " check=" << test_file_name << std::endl;

        std::vector <std::string> test_report;
        bool fail = false;
        vu::Package pkg(pkgname);
        std::string test_file_path = pkg.getDataFile(test_file_name);
        vv::Map params;
        std::string separator = " ";
        params.addString("sep",separator);
        vv::Set& cols = params.addSet("columns");
        cols.addString("string");//pkg name
        cols.addString("string");//vpz name
        cols.addString("string");//view name
        cols.addString("string");//col name
        cols.addString("int");//line index
        cols.addString("string");//expected double or string
        cols.addString("string");//precision for double or NA for string

        vle::reader::TableFileReader tfr(test_file_path);
        tfr.setParams(params);
        vv::Matrix resParsing;
        tfr.readFile(resParsing);

        if (tfr.hasError()) {
            tfr.printError(std::cout);
            fail = true;
            return fail;
        }

        std::string pkgtotest("NA");
        std::string vpzname("NA");
        std::string view;
        std::string colname;
        unsigned int lineIndex;
        std::string valExpected;
        std::string precision;

        TesterSimulation* tester_sim(0);
        vv::Map* sim_outputs(0);



        for (unsigned int i = 0; i < resParsing.rows(); i++) {
            bool fail_i = false;
            const std::string& newpkg = resParsing.getString(0,i);
            const std::string& newvpz = resParsing.getString(1,i);
            view = resParsing.getString(2,i);
            colname = resParsing.getString(3,i);
            lineIndex = resParsing.getInt(4,i);
            precision = resParsing.getString(5,i);
            valExpected = resParsing.getString(6,i);

            std::ostringstream report_line;
            report_line << "test_" << i;
            report_line << ": ";
            vv::Value* simulated_val = 0;

            if (not (newpkg == pkgtotest and newvpz == vpzname)) {

                delete tester_sim;
                delete sim_outputs;
                sim_outputs = 0;
                tester_sim = 0;

                try {
                    tester_sim = new TesterSimulation(newpkg, newvpz);
                } catch (const std::exception& e) {
                    report_line << "fail to open " << newpkg << "/"
                            << newvpz;
                    fail_i = true;
                }
                if (!fail_i){
                    try {
                        sim_outputs = tester_sim->simulates();
                    } catch (const std::exception& e){
                        report_line << "running during simulation "
                                << newpkg << "/" << newvpz << ": ";
                        fail_i = true;
                    }
                }
                if (!fail_i && tester_sim->getError().code) {
                    report_line << "running error of "
                            << newpkg << "/" << newvpz << ": "
                            << tester_sim->getError().message;
                    fail_i = true;
                }
                pkgtotest.assign(newpkg);
                vpzname.assign(newvpz);

            }
            if (!fail_i && sim_outputs == 0) {
                report_line << "sim_outputs null of "
                        << newpkg << "/" << newvpz;
                fail_i = true;
            }
            if (!fail_i) {
                try {
                    simulated_val =  tester_sim->getColElt(*sim_outputs,
                            view, colname, lineIndex);
                } catch (const vu::ArgError&) {
                    fail_i = true;
                    report_line << "test fail of " << newpkg << "/" << newvpz
                            << ": error in getting " << lineIndex
                            <<"th value of col '"<< colname << "' from view '"
                            << view << "' ";
                }
            }
            if (!fail_i && !performs_one_test(precision, valExpected,
                    *simulated_val)) {
                report_line << "test fail of " << newpkg << "/"
                        << newvpz << ": expected=" << valExpected
                        << "; got= " << *simulated_val;
                fail_i = true;
            }
            if (!fail_i) {
                report_line << "test ok of " << newpkg << "/" << newvpz
                        << ": view=" << view << "; col="<< colname
                        << "; expected=" << valExpected
                        << "; got=" << *simulated_val;
            }
            test_report.push_back(report_line.str());
            fail = fail or fail_i;
        }
        if (tester_sim != 0) {
            delete tester_sim;
            delete sim_outputs;
        }

        for (unsigned int i=0; i< test_report.size(); i++) {
            std::cout << test_report[i] << std::endl;
        }
        return fail;
    }

private:
    static bool performs_one_test(const std::string& precision,
            const std::string& valExpected, vv::Value& res)
    {
        if (precision == "NA") {
            if (res.toString().value() != valExpected) {
                return false;
            }
        } else {
            double prec = boost::lexical_cast<double>(precision);            
            double valSim = res.toDouble().value();
            if (isnan(valSim)) {
                return (valExpected == "nan");
            }
            double valExpect = boost::lexical_cast<double>(valExpected);
            if ((valSim < valExpect - prec) || (valSim > valExpect + prec)) {
                return false;
            }
        }
        return true;
    }
};

#define VLE_UTILS_TEST(pkg,vpz)                             \
  int main(int /*argc*/, char ** /*argv*/) {                   \
        return vle::tester::PackageTester::check(pkg,vpz);  \
  }
}} // namespaces

#endif
