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
#include <string>
#include <stdexcept>
#include <iostream>
#include <vle/manager/Manager.hpp>
#include <vle/manager/Simulation.hpp>
#include <vle/vpz/Vpz.hpp>
#include <vle/utils/Package.hpp>


#include <vle/vle.hpp>
#include <vle/value/Matrix.hpp>


struct F
{
    vle::Init app;

    F()
    {
    }
};

BOOST_GLOBAL_FIXTURE(F);

using namespace vle;

BOOST_AUTO_TEST_CASE(test_gens)
{
    auto ctx = vle::utils::make_context();
    vle::utils::Package pack(ctx, "vle.examples");
    std::unique_ptr<vpz::Vpz> file(new vpz::Vpz(pack.getExpFile("gens.vpz")));

    /* change the output text to storage output */
    vpz::Output& o(file->project().experiment().views().outputs().get("view1"));
    vpz::Output& o2(file->project().experiment().views().outputs().get("view2"));
    o.setStream("", "storage", "vle.output");
    o2.setStream("", "storage", "vle.output");

    /* run the simulation */

    manager::Error error;
    manager::Simulation sim(ctx, manager::LOG_NONE,
            manager::SIMULATION_NONE,
            std::chrono::milliseconds(0), &std::cout);
    std::unique_ptr<value::Map> out = sim.run(std::move(file),
            "vle.examples", &error);

    /* begin check */
    BOOST_REQUIRE_EQUAL(error.code, 0);
    BOOST_REQUIRE(out != NULL);
    BOOST_REQUIRE_EQUAL(out->size(), 2);

    /* get result of simulation */
    value::Matrix& result = out->getMatrix("view1");


    BOOST_REQUIRE_EQUAL(result.columns(),
                        3);
    BOOST_REQUIRE_EQUAL(result.rows(),
                        101);

    BOOST_REQUIRE_EQUAL(result.getDouble(0,0), 0);
    BOOST_REQUIRE_EQUAL(result.getDouble(1,0), 0);
    BOOST_REQUIRE_EQUAL(result.getInt(2,0), 1);

    BOOST_REQUIRE_EQUAL(result.getDouble(0,10), 10);
    BOOST_REQUIRE_EQUAL(result.getDouble(1,10), 55);
    BOOST_REQUIRE_EQUAL(result.getInt(2,10), 11);

    BOOST_REQUIRE_EQUAL(result.getDouble(0,14), 14);
    BOOST_REQUIRE(result.get(1,14) == 0);
    BOOST_REQUIRE_EQUAL(result.getInt(2,14), 15);

    BOOST_REQUIRE_EQUAL(result.getDouble(0,31), 31);
    BOOST_REQUIRE(result.get(1,31) == 0);
    BOOST_REQUIRE_EQUAL(result.getInt(2,31), 32);

    BOOST_REQUIRE_EQUAL(result.getDouble(0,100), 100);
    BOOST_REQUIRE_EQUAL(result.getDouble(1,100), 2550);
    BOOST_REQUIRE_EQUAL(result.getInt(2,100), 1);


}

BOOST_AUTO_TEST_CASE(test_gens_with_class)
{
    auto ctx = vle::utils::make_context(); vle::utils::Package pack(ctx, "vle.examples");
    std::unique_ptr<vpz::Vpz> file(new vpz::Vpz(
        pack.getExpFile("genswithclass.vpz")));

    /* change the output text to storage output */
    vpz::Output& o(file->project().experiment().views().outputs().get("view1"));
    vpz::Output& o2(file->project().experiment().views().outputs().get("view2"));
    o.setStream("", "storage", "vle.output");
    o2.setStream("", "storage", "vle.output");

    /* run the simulation */

    manager::Error error;
    manager::Simulation sim(ctx, manager::LOG_NONE,
            manager::SIMULATION_NONE,
            std::chrono::milliseconds(0), &std::cout);
    std::unique_ptr<value::Map> out = sim.run(std::move(file),
            "vle.examples", &error);

    /* begin check */
    BOOST_REQUIRE_EQUAL(error.code, 0);
    BOOST_REQUIRE(out != NULL);
    BOOST_REQUIRE_EQUAL(out->size(), 2);

    /* get result of simulation */
    value::Matrix& result = out->getMatrix("view1");


    BOOST_REQUIRE_EQUAL(error.code, 0);
    BOOST_REQUIRE(out != NULL);
    BOOST_REQUIRE_EQUAL(out->size(), 2);

    BOOST_REQUIRE_EQUAL(result.columns(),
                        3);
    BOOST_REQUIRE_EQUAL(result.rows(),
                        101);

    BOOST_REQUIRE_EQUAL(result.getDouble(0,0), 0);
    BOOST_REQUIRE_EQUAL(result.getDouble(1,0), 0);
    BOOST_REQUIRE_EQUAL(result.getInt(2,0), 2);

    BOOST_REQUIRE_EQUAL(result.getDouble(0,5), 5);
    BOOST_REQUIRE_EQUAL(result.getDouble(1,5), 63);
    BOOST_REQUIRE_EQUAL(result.getInt(2,5), 7);

    BOOST_REQUIRE_EQUAL(result.getDouble(0,7), 7);
    BOOST_REQUIRE(result.get(1,7) == 0);
    BOOST_REQUIRE_EQUAL(result.getInt(2,7), 9);

    BOOST_REQUIRE_EQUAL(result.getDouble(0,15), 15);
    BOOST_REQUIRE_EQUAL(result.getDouble(1,15), 528);
    BOOST_REQUIRE_EQUAL(result.getInt(2,15), 17);

    BOOST_REQUIRE_EQUAL(result.getDouble(0,100), 100);
    BOOST_REQUIRE_EQUAL(result.getDouble(1,100), 11400);
    BOOST_REQUIRE_EQUAL(result.getInt(2,100), 2);


}

BOOST_AUTO_TEST_CASE(test_gens_with_graph)
{
    auto ctx = vle::utils::make_context(); vle::utils::Package pack(ctx, "vle.examples");
    std::unique_ptr<vpz::Vpz> file(new vpz::Vpz(
        pack.getExpFile("gensgraph.vpz")));

    /* change the output text to storage output */
    vpz::Output& o(file->project().experiment().views().outputs().get("view1"));
    o.setStream("", "storage", "vle.output");

    /* run the simulation */

    manager::Error error;
    manager::Simulation sim(ctx, manager::LOG_NONE,
            manager::SIMULATION_NONE,
            std::chrono::milliseconds(0), &std::cout);
    std::unique_ptr<value::Map> out = sim.run(std::move(file),
            "vle.examples", &error);

    /* begin check */
    BOOST_REQUIRE_EQUAL(error.code, 0);
    BOOST_REQUIRE(out != NULL);
    BOOST_REQUIRE_EQUAL(out->size(), 1);

    /* get result of simulation */
    value::Matrix& result = out->getMatrix("view1");



    BOOST_REQUIRE_EQUAL(result.columns(),
                        2);
    BOOST_REQUIRE_EQUAL(result.rows(),
                        101);

    BOOST_REQUIRE_EQUAL(result.getDouble(0,0), 0);
    BOOST_REQUIRE_EQUAL(result.getDouble(1,0), 5);

    BOOST_REQUIRE_EQUAL(result.getDouble(0,5), 5);
    BOOST_REQUIRE_EQUAL(result.getDouble(1,5), 30);

    BOOST_REQUIRE_EQUAL(result.getDouble(0,20), 20);
    BOOST_REQUIRE(result.get(1,20) == 0);

    BOOST_REQUIRE_EQUAL(result.getDouble(0,99), 99);
    BOOST_REQUIRE_EQUAL(result.getDouble(1,99), 500);

    BOOST_REQUIRE_EQUAL(result.getDouble(0,100), 100);
    BOOST_REQUIRE_EQUAL(result.getDouble(1,100), 505);


}


BOOST_AUTO_TEST_CASE(test_gens_delete_connection)
{
    auto ctx = vle::utils::make_context(); vle::utils::Package pack(ctx, "vle.examples");
    std::unique_ptr<vpz::Vpz> file(new vpz::Vpz(
        pack.getExpFile("gensdelete.vpz")));

    /* change the output text to storage output */
    vpz::Output& o(file->project().experiment().views().outputs().get("view1"));
    o.setStream("", "storage", "vle.output");

    /* run the simulation */

    manager::Error error;
    manager::Simulation sim(ctx, manager::LOG_NONE,
            manager::SIMULATION_NONE,
            std::chrono::milliseconds(0), &std::cout);
    std::unique_ptr<value::Map> out = sim.run(std::move(file),
            "vle.examples", &error);

    /* begin check */
    BOOST_REQUIRE_EQUAL(error.code, 0);


}
