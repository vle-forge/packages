/*
 * This file is part of VLE, a framework for multi-modeling, simulation
 * and analysis of complex dynamical systems.
 * http://www.vle-project.org
 *
 * Copyright (c) 2014-2014 INRA http://www.inra.fr
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

//@@tagtest@@
//@@tagdepends: vle.reader @@endtagdepends

#define BOOST_TEST_MAIN
#define BOOST_AUTO_TEST_MAIN
#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE table_file_reader
#include <boost/test/unit_test.hpp>
#include <boost/test/auto_unit_test.hpp>
#include <boost/test/floating_point_comparison.hpp>

#include <vle/value/Matrix.hpp>
#include <vle/value/Map.hpp>
#include <vle/value/Set.hpp>
#include <vle/utils/Package.hpp>

#include <iostream>
#include <vle/reader/table_file_reader.hpp>


BOOST_AUTO_TEST_CASE(test_table_file_reader)
{
    {
        vle::value::Map params;
        params.addString("sep"," ");
        vle::value::Set& columns = params.addSet("columns");
        columns.addString("double");
        columns.addString("double");
        columns.addString("double");
        vle::utils::Package pkg("vle.reader_test");
        vle::reader::TableFileReader tfr(pkg.getDataFile("data.txt"));
        tfr.setParams(params);
        vle::value::Matrix mat;
        tfr.readFile(mat);
        BOOST_REQUIRE_CLOSE(mat.getDouble(2,1), 10.3, 10e-5);
        vle::reader::TableFileReader tfr2(pkg.getDataFile("data.txt"));
        tfr2.setParams(params);
        vle::value::Set set;
        tfr2.readLine(set);
        BOOST_REQUIRE_CLOSE(set.getDouble(2), 6, 10e-5);
        tfr2.readLine(set);
        BOOST_REQUIRE_CLOSE(set.getDouble(2), 10.3, 10e-5);
    }
    {
        vle::value::Map params;
        params.addString("sep",",");
        vle::value::Set& columns = params.addSet("columns");
        columns.addString("double");
        columns.addString("double");
        columns.addString("double");
        vle::utils::Package pkg("vle.reader_test");
        vle::reader::TableFileReader tfr(
                pkg.getDataFile("dataWithCommaSep.txt"));
        tfr.setParams(params);
        vle::value::Matrix mat;
        tfr.readFile(mat);
        BOOST_REQUIRE_CLOSE(mat.getDouble(2,1), 10.3, 10e-5);
        vle::reader::TableFileReader tfr2(
                pkg.getDataFile("dataWithCommaSep.txt"));
        tfr2.setParams(params);
        vle::value::Set set;
        tfr2.readLine(set);
        BOOST_REQUIRE_CLOSE(set.getDouble(2), 25, 10e-5);
        tfr2.readLine(set);
        BOOST_REQUIRE_CLOSE(set.getDouble(2), 10.3, 10e-5);
    }
    {
        vle::utils::Package pkg("vle.reader_test");
        vle::reader::TableFileReader tfr(
                pkg.getDataFile("dataWithHeader.txt"));
        vle::value::Set header;
        tfr.readLine(header, " ");
        BOOST_REQUIRE((header.size() == 3));
        BOOST_REQUIRE((header.getString(2) == "V3"));
    }
}
