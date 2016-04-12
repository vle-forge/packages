/*
 * Copyright (C) 2009-2015 INRA
 *
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

//@@tagtest@@
//@@tagdepends: vle.recursive @@endtagdepends

#define BOOST_TEST_MAIN
#define BOOST_AUTO_TEST_MAIN
#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE tests_data

#include <stdexcept>
#include <limits>
#include <fstream>
#include <iostream>

#include <boost/test/unit_test.hpp>
#include <boost/test/auto_unit_test.hpp>
#include <boost/test/floating_point_comparison.hpp>

#include <vle/recursive/accu_mono.hpp>

//Accumulators
BOOST_AUTO_TEST_CASE(test_accumulators)
{
    namespace vr = vle::recursive;
    {
        vr::AccuMono acc(vr::STANDARD);
        acc.insert(1);
        acc.insert(5);
        acc.insert(4);
        acc.insert(3.6);
        BOOST_REQUIRE_CLOSE(acc.mean(),3.4,10e-4);
        BOOST_REQUIRE_CLOSE(acc.moment2(),13.74,10e-4);
        BOOST_REQUIRE_CLOSE(acc.stdDeviation(), 1.704895,10e-4);
        BOOST_REQUIRE_CLOSE(acc.variance(), 2.90666,10e-4);
        acc.insert(8);
        acc.insert(3);
        acc.insert(2);
        BOOST_REQUIRE_CLOSE(acc.mean(), 3.8,10e-4);
        BOOST_REQUIRE_CLOSE(acc.moment2(), 18.85143,10e-4);
        BOOST_REQUIRE_CLOSE(acc.stdDeviation(), 2.268627,10e-4);
        BOOST_REQUIRE_CLOSE(acc.variance(), 5.146667,10e-4);

    }
    {
        vr::AccuMono acc(vr::QUANTILE);
        acc.insert(1);
        acc.insert(5);
        acc.insert(4);
        acc.insert(3.6);
        BOOST_REQUIRE_CLOSE(acc.mean(),3.4,10e-4);
        BOOST_REQUIRE_CLOSE(acc.squareSum(),54.96,10e-4);
        acc.insert(8);
        acc.insert(3);
        acc.insert(2);
        BOOST_REQUIRE_CLOSE(acc.mean(),3.8,10e-4);
        BOOST_REQUIRE_CLOSE(acc.squareSum(),131.96,10e-4);
    }
}
