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
// @@tagdepends: vle.reader @@endtagdepends

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
#include <vle/utils/Context.hpp>

#include <vle/reader/vle_results_text_reader.hpp>

#include <iostream>

BOOST_AUTO_TEST_CASE(test_vle_results_text_reader)
{
    {
        auto ctx = vle::utils::make_context();
        vle::utils::Package pkg(ctx, "vle.reader_test");
        vle::reader::VleResultsTextReader tfr(
                pkg.getDataFile("Bohachevsky_view.dat"));
        vle::value::Matrix mat;
        tfr.readFile(mat);
        std::cout << " mat res:\n" << mat << "\n";
        BOOST_REQUIRE((mat.rows() == 2));
        BOOST_REQUIRE((mat.columns() == 2));
        BOOST_REQUIRE_CLOSE(mat.getDouble(0,1), 1, 10e-5);
        BOOST_REQUIRE((mat.getString(1,0)
                == "ExBohachevsky:ExBohachevsky.y"));
    }
}
