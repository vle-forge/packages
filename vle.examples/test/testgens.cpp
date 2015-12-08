/*
 * @file examples/test/testgens.cpp
 *
 * This file is part of VLE, a framework for multi-modeling, simulation
 * and analysis of complex dynamical systems
 * http://www.vle-project.org
 *
 * Copyright (c) 2003-2007 Gauthier Quesnel <quesnel@users.sourceforge.net>
 * Copyright (c) 2003-2011 ULCO http://www.univ-littoral.fr
 * Copyright (c) 2007-2011 INRA http://www.inra.fr
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


#define BOOST_TEST_MAIN
#define BOOST_AUTO_TEST_MAIN
#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE test_gens_model

#include <boost/test/unit_test.hpp>
#include <boost/test/auto_unit_test.hpp>
#include <boost/test/floating_point_comparison.hpp>
#include <boost/lexical_cast.hpp>
#include <stdexcept>
#include <vle/manager/Manager.hpp>
#include <vle/manager/Simulation.hpp>
#include <vle/vpz/Vpz.hpp>
#include <vle/utils/Package.hpp>
#include <vle/utils/Path.hpp>
#include <vle/utils/ModuleManager.hpp>
#include <vle/vle.hpp>
#include <iostream>

struct F
{
    vle::Init app;

    F()
    {
    }
};

BOOST_GLOBAL_FIXTURE(F)

using namespace vle;

BOOST_AUTO_TEST_CASE(test_gens)
{
    vle::utils::Package pack("vle.examples");
    vpz::Vpz *file = new vpz::Vpz(pack.getExpFile("gens.vpz"));

    /* change the output text to storage output */
    vpz::Output& o(file->project().experiment().views().outputs().get("o"));
    vpz::Output& o2(file->project().experiment().views().outputs().get("o2"));
    o.setLocalStream("", "storage", "vle.output");
    o2.setLocalStream("", "storage", "vle.output");

    /* run the simulation */
    utils::ModuleManager man;
    manager::Error error;
    manager::Simulation sim(manager::LOG_NONE,
                            manager::SIMULATION_NONE,
                            NULL);
    value::Map *out = sim.run(file, man, &error);

    /* begin check */
    BOOST_REQUIRE_EQUAL(error.code, 0);
    BOOST_REQUIRE(out != NULL);
    BOOST_REQUIRE_EQUAL(out->size(), 2);

    /* get result of simulation */
    value::Matrix &matrix = out->getMatrix("view1");
    value::MatrixView result(matrix.value());

    BOOST_REQUIRE_EQUAL(result.shape()[0],
                        (value::MatrixView::size_type)3);
    BOOST_REQUIRE_EQUAL(result.shape()[1],
                        (value::MatrixView::size_type)101);

    {
        value::VectorView vectorTime(matrix.column(0));
        BOOST_REQUIRE_EQUAL(vectorTime.shape()[0], 101);
        BOOST_REQUIRE_EQUAL(vectorTime[vectorTime.shape()[0] -
                            1]->toDouble().value(), 100);
    }


    BOOST_REQUIRE_EQUAL(value::toDouble(result[0][0]), 0);
    BOOST_REQUIRE_EQUAL(value::toDouble(result[1][0]), 0);
    BOOST_REQUIRE_EQUAL(value::toInteger(result[2][0]), 1);

    BOOST_REQUIRE_EQUAL(value::toDouble(result[0][10]), 10);
    BOOST_REQUIRE_EQUAL(value::toDouble(result[1][10]), 55);
    BOOST_REQUIRE_EQUAL(value::toInteger(result[2][10]), 11);

    BOOST_REQUIRE_EQUAL(value::toDouble(result[0][14]), 14);
    BOOST_REQUIRE_EQUAL(result[1][14], (value::Value*)0);
    BOOST_REQUIRE_EQUAL(value::toInteger(result[2][14]), 15);

    BOOST_REQUIRE_EQUAL(value::toDouble(result[0][31]), 31);
    BOOST_REQUIRE_EQUAL(result[1][31], (value::Value*)0);
    BOOST_REQUIRE_EQUAL(value::toInteger(result[2][31]), 32);

    BOOST_REQUIRE_EQUAL(value::toDouble(result[0][100]), 100);
    BOOST_REQUIRE_EQUAL(value::toDouble(result[1][100]), 2550);
    BOOST_REQUIRE_EQUAL(value::toInteger(result[2][100]), 1);

    delete out;
}

BOOST_AUTO_TEST_CASE(test_gens_with_class)
{
    vle::utils::Package pack("vle.examples");
    vpz::Vpz* file = new vpz::Vpz(
        pack.getExpFile("genswithclass.vpz"));

    /* change the output text to storage output */
    vpz::Output& o(file->project().experiment().views().outputs().get("o"));
    vpz::Output& o2(file->project().experiment().views().outputs().get("o2"));
    o.setLocalStream("", "storage", "vle.output");
    o2.setLocalStream("", "storage", "vle.output");

    /* run the simulation */
    utils::ModuleManager man;
    manager::Error error;
    manager::Simulation sim(manager::LOG_NONE,
                            manager::SIMULATION_NONE,
                            NULL);
    value::Map *out = sim.run(file, man, &error);

    /* begin check */
    BOOST_REQUIRE_EQUAL(error.code, 0);
    BOOST_REQUIRE(out != NULL);
    BOOST_REQUIRE_EQUAL(out->size(), 2);

    /* get result of simulation */
    value::Matrix &matrix = out->getMatrix("view1");
    value::MatrixView result(matrix.value());

    BOOST_REQUIRE_EQUAL(error.code, 0);
    BOOST_REQUIRE(out != NULL);
    BOOST_REQUIRE_EQUAL(out->size(), 2);

    BOOST_REQUIRE_EQUAL(result.shape()[0],
                        (value::MatrixView::size_type)3);
    BOOST_REQUIRE_EQUAL(result.shape()[1],
                        (value::MatrixView::size_type)101);

    BOOST_REQUIRE_EQUAL(value::toDouble(result[0][0]), 0);
    BOOST_REQUIRE_EQUAL(value::toDouble(result[1][0]), 0);
    BOOST_REQUIRE_EQUAL(value::toInteger(result[2][0]), 2);

    BOOST_REQUIRE_EQUAL(value::toDouble(result[0][5]), 5);
    BOOST_REQUIRE_EQUAL(value::toDouble(result[1][5]), 63);
    BOOST_REQUIRE_EQUAL(value::toInteger(result[2][5]), 7);

    BOOST_REQUIRE_EQUAL(value::toDouble(result[0][7]), 7);
    BOOST_REQUIRE_EQUAL(result[1][7], (value::Value*)0);
    BOOST_REQUIRE_EQUAL(value::toInteger(result[2][7]), 9);

    BOOST_REQUIRE_EQUAL(value::toDouble(result[0][15]), 15);
    BOOST_REQUIRE_EQUAL(value::toDouble(result[1][15]), 528);
    BOOST_REQUIRE_EQUAL(value::toInteger(result[2][15]), 17);

    BOOST_REQUIRE_EQUAL(value::toDouble(result[0][100]), 100);
    BOOST_REQUIRE_EQUAL(value::toDouble(result[1][100]), 11400);
    BOOST_REQUIRE_EQUAL(value::toInteger(result[2][100]), 2);

    delete out;
}

BOOST_AUTO_TEST_CASE(test_gens_with_graph)
{
    vle::utils::Package pack("vle.examples");
    vpz::Vpz* file = new vpz::Vpz(
        pack.getExpFile("gensgraph.vpz"));

    /* change the output text to storage output */
    vpz::Output& o(file->project().experiment().views().outputs().get("o"));
    o.setLocalStream("", "storage", "vle.output");

    /* run the simulation */
    utils::ModuleManager man;
    manager::Error error;
    manager::Simulation sim(manager::LOG_NONE,
                            manager::SIMULATION_NONE,
                            NULL);
    value::Map *out = sim.run(file, man, &error);

    /* begin check */
    BOOST_REQUIRE_EQUAL(error.code, 0);
    BOOST_REQUIRE(out != NULL);
    BOOST_REQUIRE_EQUAL(out->size(), 1);

    /* get result of simulation */
    value::Matrix &matrix = out->getMatrix("view1");
    value::MatrixView result(matrix.value());


    BOOST_REQUIRE_EQUAL(result.shape()[0],
                        (value::MatrixView::size_type)2);
    BOOST_REQUIRE_EQUAL(result.shape()[1],
                        (value::MatrixView::size_type)101);

    BOOST_REQUIRE_EQUAL(value::toDouble(result[0][0]), 0);
    BOOST_REQUIRE_EQUAL(value::toDouble(result[1][0]), 5);

    BOOST_REQUIRE_EQUAL(value::toDouble(result[0][5]), 5);
    BOOST_REQUIRE_EQUAL(value::toDouble(result[1][5]), 30);

    BOOST_REQUIRE_EQUAL(value::toDouble(result[0][20]), 20);
    BOOST_REQUIRE_EQUAL(result[1][20], (value::Value*)0);

    BOOST_REQUIRE_EQUAL(value::toDouble(result[0][99]), 99);
    BOOST_REQUIRE_EQUAL(value::toDouble(result[1][99]), 500);

    BOOST_REQUIRE_EQUAL(value::toDouble(result[0][100]), 100);
    BOOST_REQUIRE_EQUAL(value::toDouble(result[1][100]), 505);

    delete out;
}


BOOST_AUTO_TEST_CASE(test_gens_delete_connection)
{
    vle::utils::Package pack("vle.examples");
    vpz::Vpz* file = new vpz::Vpz(
        pack.getExpFile("gensdelete.vpz"));

    /* change the output text to storage output */
    vpz::Output& o(file->project().experiment().views().outputs().get("o"));
    o.setLocalStream("", "storage", "vle.output");

    /* run the simulation */
    utils::ModuleManager man;

    manager::Error error;
    manager::Simulation sim(manager::LOG_NONE,
                            manager::SIMULATION_NONE,
                            NULL);
    value::Map *out = sim.run(file, man, &error);

    /* begin check */
    BOOST_REQUIRE_EQUAL(error.code, 0);

    delete out;
}

BOOST_AUTO_TEST_CASE(test_gens_ordereddeleter)
{
    vle::utils::Package pack("vle.examples");

    for (int s = 0, es = 100; s != es; ++s) {
        vpz::Vpz *file = new vpz::Vpz(pack.getExpFile("ordereddeleter.vpz"));
        utils::ModuleManager man;
        manager::Error error;
        manager::Simulation sim(manager::LOG_NONE,
                                manager::SIMULATION_NONE,
                                NULL);
        value::Map *out = sim.run(file, man, &error);

        /* begin check */
        BOOST_REQUIRE_EQUAL(error.code, 0);
        BOOST_REQUIRE(out != NULL);
        BOOST_REQUIRE_EQUAL(out->size(), 1);

        /* get result of simulation */
        value::Matrix &matrix = out->getMatrix("view1");
        value::MatrixView result(matrix.value());

        BOOST_REQUIRE_EQUAL(result.shape()[0],
                            (value::MatrixView::size_type)2);

        for (value::MatrixView::size_type i = 0, ei = 10; i != ei; ++i) {
            BOOST_REQUIRE_EQUAL(value::toDouble(result[0][i]), i);
            BOOST_REQUIRE_EQUAL(value::toDouble(result[1][i]), 0);
        }

        delete out;
    }
}
