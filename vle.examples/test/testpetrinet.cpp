/*
 * @file examples/test/testpetrinet.cpp
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
#define BOOST_TEST_MODULE test_petrinet_extension

#include <boost/test/unit_test.hpp>
#include <boost/test/auto_unit_test.hpp>
#include <boost/test/floating_point_comparison.hpp>
#include <string>
#include <boost/version.hpp>
#include <stdexcept>
#include <vle/manager/Manager.hpp>
#include <vle/manager/Simulation.hpp>
#include <vle/vpz/Vpz.hpp>
#include <vle/utils/Package.hpp>


#include <vle/value/Matrix.hpp>
#include <vle/vle.hpp>
#include <iostream>
#include <iomanip>

struct F
{
    vle::Init app;

    F()
    {
    }
};

BOOST_GLOBAL_FIXTURE(F);

using namespace vle;

BOOST_AUTO_TEST_CASE(test_petrinet_and)
{
    auto ctx = vle::utils::make_context(); vle::utils::Package pack(ctx, "vle.examples");
    std::unique_ptr<vpz::Vpz> file(
            new vpz::Vpz(pack.getExpFile("petrinet-and.vpz")));

    vpz::Output& o(file->project().experiment().views().outputs().get("view"));
    o.setLocalStream("", "storage", "vle.output");


    manager::Error error;
    manager::Simulation sim(ctx, manager::LOG_NONE,
                            manager::SIMULATION_NONE,
                            NULL);
    std::unique_ptr<value::Map> out = sim.run(std::move(file), &error);

    BOOST_REQUIRE_EQUAL(error.code, 0);
    BOOST_REQUIRE(out != NULL);
    BOOST_REQUIRE_EQUAL(out->size(), 1);

    value::Matrix& result = out->getMatrix("view");


    BOOST_REQUIRE_EQUAL(result.columns(),
                        5);
    //due to appoximation
    BOOST_REQUIRE(result.rows() >= 11);
    BOOST_REQUIRE(result.rows() <= 12);

    BOOST_REQUIRE_CLOSE(result.getDouble(3,10), 0.75, 10e-5);
    BOOST_REQUIRE_CLOSE(result.getDouble(4,10), 1.0, 10e-5);
}

BOOST_AUTO_TEST_CASE(test_petrinet_or)
{
    auto ctx = vle::utils::make_context(); vle::utils::Package pack(ctx, "vle.examples");
    std::unique_ptr<vpz::Vpz> file(
            new vpz::Vpz(pack.getExpFile("petrinet-or.vpz")));

    vpz::Output& o(file->project().experiment().views().outputs().get("view"));
    o.setLocalStream("", "storage", "vle.output");


    manager::Error error;
    manager::Simulation sim(ctx, manager::LOG_NONE,
                            manager::SIMULATION_NONE,
                            NULL);
    std::unique_ptr<value::Map> out = sim.run(std::move(file), &error);

    BOOST_REQUIRE_EQUAL(error.code, 0);
    BOOST_REQUIRE(out != NULL);
    BOOST_REQUIRE_EQUAL(out->size(), 1);

    value::Matrix& result = out->getMatrix("view");


    BOOST_REQUIRE_EQUAL(result.columns(),
                        5);
    //due to appoximation
    BOOST_REQUIRE(result.rows() >= 11);
    BOOST_REQUIRE(result.rows() <= 12);


    BOOST_REQUIRE_CLOSE(result.getDouble(3,10), 0.75, 10e-5);
    BOOST_REQUIRE_CLOSE(result.getDouble(4,10), 2.0, 10e-5);
}

BOOST_AUTO_TEST_CASE(test_petrinet_nand1)
{
    auto ctx = vle::utils::make_context(); vle::utils::Package pack(ctx, "vle.examples");
    std::unique_ptr<vpz::Vpz> file(
            new vpz::Vpz(pack.getExpFile("petrinet-nand1.vpz")));

    vpz::Output& o(file->project().experiment().views().outputs().get("view"));
    o.setLocalStream("", "storage", "vle.output");


    manager::Error error;
    manager::Simulation sim(ctx, manager::LOG_NONE,
                            manager::SIMULATION_NONE,
                            NULL);
    std::unique_ptr<value::Map> out = sim.run(std::move(file), &error);

    BOOST_REQUIRE_EQUAL(error.code, 0);
    BOOST_REQUIRE(out != NULL);
    BOOST_REQUIRE_EQUAL(out->size(), 1);

    value::Matrix& result = out->getMatrix("view");


    BOOST_REQUIRE_EQUAL(result.columns(),
                        5);
    //due to appoximation
    BOOST_REQUIRE(result.rows() >= 11);
    BOOST_REQUIRE(result.rows() <= 12);


    BOOST_REQUIRE_CLOSE(result.getDouble(3,10), 0., 10e-5);
    BOOST_REQUIRE_CLOSE(result.getDouble(4,10), 0., 10e-5);
}

BOOST_AUTO_TEST_CASE(test_petrinet_nand2)
{
    auto ctx = vle::utils::make_context(); vle::utils::Package pack(ctx, "vle.examples");
    std::unique_ptr<vpz::Vpz> file(
            new vpz::Vpz(pack.getExpFile("petrinet-nand2.vpz")));

    vpz::Output& o(file->project().experiment().views().outputs().get("view"));
    o.setLocalStream("", "storage", "vle.output");


    manager::Error error;
    manager::Simulation sim(ctx, manager::LOG_NONE,
                            manager::SIMULATION_NONE,
                            NULL);
    std::unique_ptr<value::Map> out = sim.run(std::move(file), &error);

    BOOST_REQUIRE_EQUAL(error.code, 0);
    BOOST_REQUIRE(out != NULL);
    BOOST_REQUIRE_EQUAL(out->size(), 1);

    value::Matrix& result = out->getMatrix("view");


    BOOST_REQUIRE_EQUAL(result.columns(),
                        5);
    //due to appoximation
    BOOST_REQUIRE(result.rows() >= 11);
    BOOST_REQUIRE(result.rows() <= 12);


    BOOST_REQUIRE_CLOSE(result.getDouble(3,10), 0.5, 10e-5);
    BOOST_REQUIRE_CLOSE(result.getDouble(4,10), 1., 10e-5);
}

BOOST_AUTO_TEST_CASE(test_petrinet_and_timed)
{
    auto ctx = vle::utils::make_context(); vle::utils::Package pack(ctx, "vle.examples");
    std::unique_ptr<vpz::Vpz> file(
            new vpz::Vpz(pack.getExpFile("petrinet-and-timed.vpz")));

    vpz::Output& o(file->project().experiment().views().outputs().get("view"));
    o.setLocalStream("", "storage", "vle.output");


    manager::Error error;
    manager::Simulation sim(ctx, manager::LOG_NONE,
                            manager::SIMULATION_NONE,
                            NULL);
    std::unique_ptr<value::Map> out = sim.run(std::move(file), &error);

    BOOST_REQUIRE_EQUAL(error.code, 0);
    BOOST_REQUIRE(out != NULL);
    BOOST_REQUIRE_EQUAL(out->size(), 1);

    value::Matrix& result = out->getMatrix("view");


    BOOST_REQUIRE_EQUAL(result.columns(),
                        5);
    //due to appoximation
    BOOST_REQUIRE(result.rows() >= 20);
    BOOST_REQUIRE(result.rows() <= 21);


    BOOST_REQUIRE_CLOSE(result.getDouble(3,19), 1.7, 10e-5);
    BOOST_REQUIRE_CLOSE(result.getDouble(4,19), 3., 10e-5);
}

BOOST_AUTO_TEST_CASE(test_petrinet_or_priority)
{
    auto ctx = vle::utils::make_context(); vle::utils::Package pack(ctx, "vle.examples");
    std::unique_ptr<vpz::Vpz> file(
            new vpz::Vpz(pack.getExpFile("petrinet-or-priority.vpz")));

    vpz::Output& o(file->project().experiment().views().outputs().get("view"));
    o.setLocalStream("", "storage", "vle.output");


    manager::Error error;
    manager::Simulation sim(ctx, manager::LOG_NONE,
                            manager::SIMULATION_NONE,
                            NULL);
    std::unique_ptr<value::Map> out = sim.run(std::move(file), &error);

    BOOST_REQUIRE_EQUAL(error.code, 0);
    BOOST_REQUIRE(out != NULL);
    BOOST_REQUIRE_EQUAL(out->size(), 1);

    value::Matrix& result = out->getMatrix("view");


    BOOST_REQUIRE_EQUAL(result.columns(),
                        5);
    //due to appoximation
    BOOST_REQUIRE(result.rows() >= 11);
    BOOST_REQUIRE(result.rows() <= 12);


    BOOST_REQUIRE_CLOSE(result.getDouble(3,10), 1., 10e-5);
    BOOST_REQUIRE_CLOSE(result.getDouble(4,10), 6., 10e-5);
}

BOOST_AUTO_TEST_CASE(test_petrinet_meteo)
{
    auto ctx = vle::utils::make_context(); vle::utils::Package pack(ctx, "vle.examples");
    std::unique_ptr<vpz::Vpz> file(
            new vpz::Vpz(pack.getExpFile("petrinet-meteo.vpz")));

    vpz::Output& o(file->project().experiment().views().outputs().get("view"));
    o.setLocalStream("", "storage", "vle.output");


    manager::Error error;
    manager::Simulation sim(ctx, manager::LOG_NONE,
                            manager::SIMULATION_NONE,
                            NULL);
    std::unique_ptr<value::Map> out = sim.run(std::move(file), &error);

    BOOST_REQUIRE_EQUAL(error.code, 0);
    BOOST_REQUIRE(out != NULL);
    BOOST_REQUIRE_EQUAL(out->size(), 1);

    value::Matrix& result = out->getMatrix("view");


    BOOST_REQUIRE_EQUAL(result.columns(),
                        6);
    BOOST_REQUIRE_EQUAL(result.rows(),
                        31);

//#if BOOST_VERSION < 104000
    BOOST_REQUIRE_CLOSE(result.getDouble(4,30), 29., 10e-5);
//#else
    //BOOST_REQUIRE_CLOSE(result.getDouble(4,30), 28., 10e-5);
//#endif
    BOOST_REQUIRE_CLOSE(result.getDouble(5,30), 4., 10e-5);
}

BOOST_AUTO_TEST_CASE(test_petrinet_inout)
{
    auto ctx = vle::utils::make_context(); vle::utils::Package pack(ctx, "vle.examples");
    std::unique_ptr<vpz::Vpz> file(
            new vpz::Vpz(pack.getExpFile("petrinet-inout.vpz")));

    vpz::Output& o(file->project().experiment().views().outputs().get("view"));
    o.setLocalStream("", "storage", "vle.output");


    manager::Error error;
    manager::Simulation sim(ctx, manager::LOG_NONE,
                            manager::SIMULATION_NONE,
                            NULL);
    std::unique_ptr<value::Map> out = sim.run(std::move(file), &error);

    BOOST_REQUIRE_EQUAL(error.code, 0);
    BOOST_REQUIRE(out != NULL);
    BOOST_REQUIRE_EQUAL(out->size(), 1);

    value::Matrix& result = out->getMatrix("view");


    BOOST_REQUIRE_EQUAL(result.columns(),
                        5);
    //due to appoximation
    BOOST_REQUIRE(result.rows() >= 11);
    BOOST_REQUIRE(result.rows() <= 12);

    BOOST_REQUIRE_CLOSE(result.getDouble(0,10), 1., 1.);

    BOOST_REQUIRE(result.getInt(2,10) == 6);
    BOOST_REQUIRE_CLOSE(result.getDouble(3,10), 1., 10e-5);
    BOOST_REQUIRE_CLOSE(result.getDouble(4,10), 6., 10e-5);
}

BOOST_AUTO_TEST_CASE(test_petrinet_conflict)
{
    auto ctx = vle::utils::make_context(); vle::utils::Package pack(ctx, "vle.examples");
    std::unique_ptr<vpz::Vpz> file(
            new vpz::Vpz(pack.getExpFile("petrinet-conflict.vpz")));

    vpz::Output& o(file->project().experiment().views().outputs().get("view"));
    o.setLocalStream("", "storage", "vle.output");


    manager::Error error;
    manager::Simulation sim(ctx, manager::LOG_NONE,
                            manager::SIMULATION_NONE,
                            NULL);
    std::unique_ptr<value::Map> out = sim.run(std::move(file), &error);

    BOOST_REQUIRE_EQUAL(error.code, 0);
    BOOST_REQUIRE(out != NULL);
    BOOST_REQUIRE_EQUAL(out->size(), 1);

    value::Matrix& result = out->getMatrix("view");


    BOOST_REQUIRE_EQUAL(result.columns(),
                        5);
    //due to appoximation
    BOOST_REQUIRE(result.rows() >= 11);
    BOOST_REQUIRE(result.rows() <= 12);


    BOOST_REQUIRE(result.getInt(2,10) == 1);
    BOOST_REQUIRE_CLOSE(result.getDouble(3,10), 1., 10e-5);
    BOOST_REQUIRE_CLOSE(result.getDouble(4,10), 5., 10e-5);
}
