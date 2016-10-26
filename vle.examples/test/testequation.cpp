/*
 * @file examples/test/testequation.cpp
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
#define BOOST_TEST_MODULE test_difference_equation_extension

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

#include <vle/value/Matrix.hpp>
#include <vle/vle.hpp>
#include <iostream>

struct F
{
    vle::Init app;

    F()
    {
    }
};

BOOST_GLOBAL_FIXTURE(F);

using namespace vle;

BOOST_AUTO_TEST_CASE(test_equation1)
{
    std::cout << " test_equation1 " << std::endl;
    auto ctx = vle::utils::make_context(); vle::utils::Package pack(ctx, "vle.examples");
    std::unique_ptr<vpz::Vpz> file(
    		new vpz::Vpz(pack.getExpFile("equation1.vpz")));
    vpz::Output& o(file->project().experiment().views().outputs().get("view"));
    o.setStream("", "storage", "vle.output");


    manager::Error error;
    manager::Simulation sim(ctx, manager::LOG_NONE,
            manager::SIMULATION_NONE,
            std::chrono::milliseconds(0), &std::cout);
    std::unique_ptr<value::Map> out = sim.run(std::move(file),
            "vle.examples", &error);

    BOOST_REQUIRE_EQUAL(error.code, 0);
    BOOST_REQUIRE(out != NULL);
    BOOST_REQUIRE_EQUAL(out->size(), 1);

    value::Matrix& result = out->getMatrix("view");


    BOOST_REQUIRE_EQUAL(result.columns(), 6);
    //due to appoximation
    BOOST_REQUIRE(result.rows() >= 11);
    BOOST_REQUIRE(result.rows() <= 12);


    BOOST_REQUIRE_CLOSE(result.getDouble(1,10), 10., 10e-5);
    BOOST_REQUIRE_CLOSE(result.getDouble(2,10), 52., 10e-5);
    BOOST_REQUIRE_CLOSE(result.getDouble(3,10), 146., 10e-5);
    BOOST_REQUIRE_CLOSE(result.getDouble(4,10), 461., 10e-5);
    BOOST_REQUIRE_CLOSE(result.getDouble(5,10), 669., 10e-5);


}

BOOST_AUTO_TEST_CASE(test_equation2)
{
    std::cout << " test_equation2 " << std::endl;
    auto ctx = vle::utils::make_context(); vle::utils::Package pack(ctx, "vle.examples");
    std::unique_ptr<vpz::Vpz> file(
    		new vpz::Vpz(pack.getExpFile("equation2.vpz")));
    vpz::Output& o(file->project().experiment().views().outputs().get("view"));
    o.setStream("", "storage", "vle.output");


    manager::Error error;
    manager::Simulation sim(ctx, manager::LOG_NONE,
            manager::SIMULATION_NONE,
            std::chrono::milliseconds(0), &std::cout);
    std::unique_ptr<value::Map> out = sim.run(std::move(file),
            "vle.examples", &error);

    BOOST_REQUIRE_EQUAL(error.code, 0);
    BOOST_REQUIRE(out != NULL);
    BOOST_REQUIRE_EQUAL(out->size(), 1);

    value::Matrix& result = out->getMatrix("view");

    BOOST_REQUIRE_EQUAL(result.columns(), 5);
    //due to appoximation
    BOOST_REQUIRE(result.rows() >= 11);
    BOOST_REQUIRE(result.rows() <= 12);

    BOOST_REQUIRE_CLOSE(result.getDouble(1,10), 2045., 10e-5);
    BOOST_REQUIRE_CLOSE(result.getDouble(2,10), 4092., 10e-5);
    BOOST_REQUIRE_CLOSE(result.getDouble(3,10), 4094., 10e-5);
    BOOST_REQUIRE_CLOSE(result.getDouble(4,10), 4093., 10e-5);


}

BOOST_AUTO_TEST_CASE(test_equation3)
{
    std::cout << " test_equation3 " << std::endl;
    auto ctx = vle::utils::make_context(); vle::utils::Package pack(ctx, "vle.examples");
    std::unique_ptr<vpz::Vpz> file(
    		new vpz::Vpz(pack.getExpFile("equation3.vpz")));
    vpz::Output& o(file->project().experiment().views().outputs().get("view"));
    o.setStream("", "storage", "vle.output");


    manager::Error error;
    manager::Simulation sim(ctx, manager::LOG_NONE,
            manager::SIMULATION_NONE,
            std::chrono::milliseconds(0), &std::cout);
    std::unique_ptr<value::Map> out = sim.run(std::move(file),
            "vle.examples", &error);

    BOOST_REQUIRE_EQUAL(error.code, 0);
    BOOST_REQUIRE(out != NULL);
    BOOST_REQUIRE_EQUAL(out->size(), 1);

    value::Matrix& result = out->getMatrix("view");


    BOOST_REQUIRE_EQUAL(result.columns(), 6);
    //due to appoximation
    BOOST_REQUIRE(result.rows() >= 11);
    BOOST_REQUIRE(result.rows() <= 12);


    BOOST_REQUIRE_CLOSE(result.getDouble(1,10), 461., 10e-5);
    BOOST_REQUIRE_CLOSE(result.getDouble(2,10), 10., 10e-5);
    BOOST_REQUIRE_CLOSE(result.getDouble(3,10), 52., 10e-5);
    BOOST_REQUIRE_CLOSE(result.getDouble(4,10), 146., 10e-5);
    BOOST_REQUIRE_CLOSE(result.getDouble(5,10), 669., 10e-5);


}

BOOST_AUTO_TEST_CASE(test_equation4)
{
    std::cout << " test_equation4 " << std::endl;
    auto ctx = vle::utils::make_context(); vle::utils::Package pack(ctx, "vle.examples");
    std::unique_ptr<vpz::Vpz> file(
    		new vpz::Vpz(pack.getExpFile("equation4.vpz")));
    vpz::Output& o(file->project().experiment().views().outputs().get("view"));
    o.setStream("", "storage", "vle.output");


    manager::Error error;
    manager::Simulation sim(ctx, manager::LOG_NONE,
            manager::SIMULATION_NONE,
            std::chrono::milliseconds(0), &std::cout);
    std::unique_ptr<value::Map> out = sim.run(std::move(file),
            "vle.examples", &error);

    BOOST_REQUIRE_EQUAL(error.code, 0);
    BOOST_REQUIRE(out != NULL);
    BOOST_REQUIRE_EQUAL(out->size(), 1);

    value::Matrix& result = out->getMatrix("view");


    BOOST_REQUIRE_EQUAL(result.columns(), 6);
    //due to appoximation
    BOOST_REQUIRE(result.rows() >= 11);
    BOOST_REQUIRE(result.rows() <= 12);

    BOOST_REQUIRE_CLOSE(result.getDouble(1,10), 626., 10e-5);
    BOOST_REQUIRE_CLOSE(result.getDouble(2,10), 10., 10e-5);
    BOOST_REQUIRE_CLOSE(result.getDouble(3,10), 55., 10e-5);
    BOOST_REQUIRE_CLOSE(result.getDouble(4,10), 176., 10e-5);
    BOOST_REQUIRE_CLOSE(result.getDouble(5,10), 867., 10e-5);


}

BOOST_AUTO_TEST_CASE(test_equation5)
{
    std::cout << " test_equation5 " << std::endl;
    auto ctx = vle::utils::make_context(); vle::utils::Package pack(ctx, "vle.examples");
    std::unique_ptr<vpz::Vpz> file(
    		new vpz::Vpz(pack.getExpFile("equation5.vpz")));
    vpz::Output& o(file->project().experiment().views().outputs().get("view"));
    o.setStream("", "storage", "vle.output");


    manager::Error error;
    manager::Simulation sim(ctx, manager::LOG_NONE,
            manager::SIMULATION_NONE,
            std::chrono::milliseconds(0), &std::cout);
    std::unique_ptr<value::Map> out = sim.run(std::move(file),
            "vle.examples", &error);

    BOOST_REQUIRE_EQUAL(error.code, 0);
    BOOST_REQUIRE(out != NULL);
    BOOST_REQUIRE_EQUAL(out->size(), 1);

    value::Matrix& result = out->getMatrix("view");


    BOOST_REQUIRE_EQUAL(result.columns(),
                        5);
    BOOST_REQUIRE_EQUAL(result.rows(),
                        21);

    BOOST_REQUIRE_CLOSE(result.getDouble(1,5), 10., 10e-5);
    BOOST_REQUIRE_CLOSE(result.getDouble(2,5), 12., 10e-5);
    BOOST_REQUIRE_CLOSE(result.getDouble(3,5), 10., 10e-5);
    BOOST_REQUIRE_CLOSE(result.getDouble(4,5), 30., 10e-5);

    BOOST_REQUIRE_CLOSE(result.getDouble(1,10), 0., 10e-5);
    BOOST_REQUIRE_CLOSE(result.getDouble(2,10), 77., 10e-5);
    BOOST_REQUIRE_CLOSE(result.getDouble(3,10), 195., 10e-5);
    BOOST_REQUIRE_CLOSE(result.getDouble(4,10), 565., 10e-5);

    BOOST_REQUIRE_CLOSE(result.getDouble(1,15), 10., 10e-5);
    BOOST_REQUIRE_CLOSE(result.getDouble(2,15), 92., 10e-5);
    BOOST_REQUIRE_CLOSE(result.getDouble(3,15), 605., 10e-5);
    BOOST_REQUIRE_CLOSE(result.getDouble(4,15), 2770., 10e-5);

    BOOST_REQUIRE_CLOSE(result.getDouble(1,20), 0., 10e-5);
    BOOST_REQUIRE_CLOSE(result.getDouble(2,20), 157., 10e-5);
    BOOST_REQUIRE_CLOSE(result.getDouble(3,20), 1190., 10e-5);
    BOOST_REQUIRE_CLOSE(result.getDouble(4,20), 7480., 10e-5);


}

BOOST_AUTO_TEST_CASE(test_equation6)
{
    std::cout << " test_equation6 " << std::endl;
    auto ctx = vle::utils::make_context(); vle::utils::Package pack(ctx, "vle.examples");
    std::unique_ptr<vpz::Vpz> file(
    		new vpz::Vpz(pack.getExpFile("equation6.vpz")));
    vpz::Output& o(file->project().experiment().views().outputs().get("view"));
    o.setStream("", "storage", "vle.output");


    manager::Error error;
    manager::Simulation sim(ctx, manager::LOG_NONE,
            manager::SIMULATION_NONE,
            std::chrono::milliseconds(0), &std::cout);
    std::unique_ptr<value::Map> out = sim.run(std::move(file),
            "vle.examples", &error);

    BOOST_REQUIRE_EQUAL(error.code, 0);
    BOOST_REQUIRE(out != NULL);
    BOOST_REQUIRE_EQUAL(out->size(), 1);

    value::Matrix& result = out->getMatrix("view");


    BOOST_REQUIRE_EQUAL(result.columns(),
                        5);
    BOOST_REQUIRE_EQUAL(result.rows(),
                        21);

    BOOST_REQUIRE_CLOSE(result.getDouble(1,5), 10., 10e-5);
    BOOST_REQUIRE_CLOSE(result.getDouble(2,5), 57., 10e-5);
    BOOST_REQUIRE_CLOSE(result.getDouble(3,5), 59., 10e-5);
    BOOST_REQUIRE_CLOSE(result.getDouble(4,5), 58., 10e-5);

    BOOST_REQUIRE_CLOSE(result.getDouble(1,10), 0., 10e-5);
    BOOST_REQUIRE_CLOSE(result.getDouble(2,10), 975., 10e-5);
    BOOST_REQUIRE_CLOSE(result.getDouble(3,10), 977., 10e-5);
    BOOST_REQUIRE_CLOSE(result.getDouble(4,10), 976., 10e-5);

    BOOST_REQUIRE_CLOSE(result.getDouble(1,15), 10., 10e-5);
    BOOST_REQUIRE_CLOSE(result.getDouble(2,15), 15673., 10e-5);
    BOOST_REQUIRE_CLOSE(result.getDouble(3,15), 15675., 10e-5);
    BOOST_REQUIRE_CLOSE(result.getDouble(4,15), 15674., 10e-5);

    BOOST_REQUIRE_CLOSE(result.getDouble(1,20), 0., 10e-5);
    BOOST_REQUIRE_CLOSE(result.getDouble(2,20), 250831., 10e-5);
    BOOST_REQUIRE_CLOSE(result.getDouble(3,20), 250833., 10e-5);
    BOOST_REQUIRE_CLOSE(result.getDouble(4,20), 250832., 10e-5);


}

BOOST_AUTO_TEST_CASE(test_equation7)
{
    std::cout << " test_equation7 " << std::endl;
    auto ctx = vle::utils::make_context(); vle::utils::Package pack(ctx, "vle.examples");
    std::unique_ptr<vpz::Vpz> file(
    		new vpz::Vpz(pack.getExpFile("equation7.vpz")));
    vpz::Output& o(file->project().experiment().views().outputs().get("view"));
    o.setStream("", "storage", "vle.output");


    manager::Error error;
    manager::Simulation sim(ctx, manager::LOG_NONE,
            manager::SIMULATION_NONE,
            std::chrono::milliseconds(0), &std::cout);
    std::unique_ptr<value::Map> out = sim.run(std::move(file),
            "vle.examples", &error);

    BOOST_REQUIRE_EQUAL(error.code, 0);
    BOOST_REQUIRE(out != NULL);
    BOOST_REQUIRE_EQUAL(out->size(), 1);

    value::Matrix& result = out->getMatrix("view");


    BOOST_REQUIRE_EQUAL(result.columns(),
                        4);
    BOOST_REQUIRE_EQUAL(result.rows(),
                        21);

    BOOST_REQUIRE_CLOSE(result.getDouble(1,5), 10., 10e-5);
    BOOST_REQUIRE_CLOSE(result.getDouble(2,5), 22., 10e-5);
    BOOST_REQUIRE_CLOSE(result.getDouble(3,5), 41., 10e-5);

    BOOST_REQUIRE_CLOSE(result.getDouble(1,10), 0., 10e-5);
    BOOST_REQUIRE_CLOSE(result.getDouble(2,10), 77., 10e-5);
    BOOST_REQUIRE_CLOSE(result.getDouble(3,10), 341., 10e-5);

    BOOST_REQUIRE_CLOSE(result.getDouble(1,15), 10., 10e-5);
    BOOST_REQUIRE_CLOSE(result.getDouble(2,15), 102., 10e-5);
    BOOST_REQUIRE_CLOSE(result.getDouble(3,15), 786., 10e-5);

    BOOST_REQUIRE_CLOSE(result.getDouble(1,20), 0., 10e-5);
    BOOST_REQUIRE_CLOSE(result.getDouble(2,20), 157., 10e-5);
    BOOST_REQUIRE_CLOSE(result.getDouble(3,20), 1486., 10e-5);


}

BOOST_AUTO_TEST_CASE(test_equation8)
{
    std::cout << " test_equation8 " << std::endl;
    auto ctx = vle::utils::make_context(); vle::utils::Package pack(ctx, "vle.examples");
    std::unique_ptr<vpz::Vpz> file(
    		new vpz::Vpz(pack.getExpFile("equation8.vpz")));
    vpz::Output& o(file->project().experiment().views().outputs().get("view"));
    o.setStream("", "storage", "vle.output");


    manager::Error error;
    manager::Simulation sim(ctx, manager::LOG_NONE,
            manager::SIMULATION_NONE,
            std::chrono::milliseconds(0), &std::cout);
    std::unique_ptr<value::Map> out = sim.run(std::move(file),
            "vle.examples", &error);

    BOOST_REQUIRE_EQUAL(error.code, 0);
    BOOST_REQUIRE(out != NULL);
    BOOST_REQUIRE_EQUAL(out->size(), 1);

    value::Matrix& result = out->getMatrix("view");


    BOOST_REQUIRE_EQUAL(result.columns(),
                        5);
    BOOST_REQUIRE_EQUAL(result.rows(),
                        21);

    BOOST_REQUIRE_CLOSE(result.getDouble(1,5), 10., 10e-5);
    BOOST_REQUIRE_CLOSE(result.getDouble(2,5), 22., 10e-5);
    BOOST_REQUIRE_CLOSE(result.getDouble(3,5), 10., 10e-5);
    BOOST_REQUIRE_CLOSE(result.getDouble(4,5), 31., 10e-5);

    BOOST_REQUIRE_CLOSE(result.getDouble(1,10), 0., 10e-5);
    BOOST_REQUIRE_CLOSE(result.getDouble(2,10), 77., 10e-5);
    BOOST_REQUIRE_CLOSE(result.getDouble(3,10), 0., 10e-5);
    BOOST_REQUIRE_CLOSE(result.getDouble(4,10), 561., 10e-5);

    BOOST_REQUIRE_CLOSE(result.getDouble(1,15), 10., 10e-5);
    BOOST_REQUIRE_CLOSE(result.getDouble(2,15), 102., 10e-5);
    BOOST_REQUIRE_CLOSE(result.getDouble(3,15), 10., 10e-5);
    BOOST_REQUIRE_CLOSE(result.getDouble(4,15), 1411., 10e-5);

    BOOST_REQUIRE_CLOSE(result.getDouble(1,20), 0., 10e-5);
    BOOST_REQUIRE_CLOSE(result.getDouble(2,20), 157., 10e-5);
    BOOST_REQUIRE_CLOSE(result.getDouble(3,20), 0., 10e-5);
    BOOST_REQUIRE_CLOSE(result.getDouble(4,20), 2741., 10e-5);


}

BOOST_AUTO_TEST_CASE(test_equation9)
{
    std::cout << " test_equation9 " << std::endl;
    auto ctx = vle::utils::make_context(); vle::utils::Package pack(ctx, "vle.examples");
    std::unique_ptr<vpz::Vpz> file(
    		new vpz::Vpz(pack.getExpFile("equation9.vpz")));
    vpz::Output& o(file->project().experiment().views().outputs().get("view"));
    o.setStream("", "storage", "vle.output");


    manager::Error error;
    manager::Simulation sim(ctx, manager::LOG_NONE,
            manager::SIMULATION_NONE,
            std::chrono::milliseconds(0), &std::cout);
    std::unique_ptr<value::Map> out = sim.run(std::move(file),
            "vle.examples", &error);

    BOOST_REQUIRE_EQUAL(error.code, 0);
    BOOST_REQUIRE(out != NULL);
    BOOST_REQUIRE_EQUAL(out->size(), 1);

    value::Matrix& result = out->getMatrix("view");


    BOOST_REQUIRE_EQUAL(result.columns(),
                        5);
    BOOST_REQUIRE_EQUAL(result.rows(),
                        21);

    BOOST_REQUIRE_CLOSE(result.getDouble(1,5), 11., 10e-5);
    BOOST_REQUIRE_CLOSE(result.getDouble(2,5), 18., 10e-5);
    BOOST_REQUIRE_CLOSE(result.getDouble(3,5), 10., 10e-5);
    BOOST_REQUIRE_CLOSE(result.getDouble(4,5), 31., 10e-5);

    BOOST_REQUIRE_CLOSE(result.getDouble(1,10), 3., 10e-5);
    BOOST_REQUIRE_CLOSE(result.getDouble(2,10), 49., 10e-5);
    BOOST_REQUIRE_CLOSE(result.getDouble(3,10), 196., 10e-5);
    BOOST_REQUIRE_CLOSE(result.getDouble(4,10), 605., 10e-5);

    BOOST_REQUIRE_CLOSE(result.getDouble(1,15), 13., 10e-5);
    BOOST_REQUIRE_CLOSE(result.getDouble(2,15), 94., 10e-5);
    BOOST_REQUIRE_CLOSE(result.getDouble(3,15), 511., 10e-5);
    BOOST_REQUIRE_CLOSE(result.getDouble(4,15), 2500., 10e-5);

    BOOST_REQUIRE_CLOSE(result.getDouble(1,20), 11., 10e-5);
    BOOST_REQUIRE_CLOSE(result.getDouble(2,20), 115., 10e-5);
    BOOST_REQUIRE_CLOSE(result.getDouble(3,20), 1006., 10e-5);
    BOOST_REQUIRE_CLOSE(result.getDouble(4,20), 6541., 10e-5);


}

BOOST_AUTO_TEST_CASE(test_equation10)
{
    std::cout << " test_equation10 " << std::endl;
    auto ctx = vle::utils::make_context(); vle::utils::Package pack(ctx, "vle.examples");
    std::unique_ptr<vpz::Vpz> file(
    		new vpz::Vpz(pack.getExpFile("equation10.vpz")));
    vpz::Output& o(file->project().experiment().views().outputs().get("view"));
    o.setStream("", "storage", "vle.output");


    manager::Error error;
    manager::Simulation sim(ctx, manager::LOG_NONE,
            manager::SIMULATION_NONE,
            std::chrono::milliseconds(0), &std::cout);
    std::unique_ptr<value::Map> out = sim.run(std::move(file),
            "vle.examples", &error);

    BOOST_REQUIRE_EQUAL(error.code, 0);
    BOOST_REQUIRE(out != NULL);
    BOOST_REQUIRE_EQUAL(out->size(), 1);

    value::Matrix& result = out->getMatrix("view");


    BOOST_REQUIRE_EQUAL(result.columns(),
                        5);
    BOOST_REQUIRE_EQUAL(result.rows(),
                        21);

    BOOST_REQUIRE_CLOSE(result.getDouble(1,5), 75., 10e-5);
    BOOST_REQUIRE_CLOSE(result.getDouble(2,5), 10., 10e-5);
    BOOST_REQUIRE_CLOSE(result.getDouble(3,5), 15., 10e-5);
    BOOST_REQUIRE_CLOSE(result.getDouble(4,5), 25., 10e-5);

    BOOST_REQUIRE_CLOSE(result.getDouble(1,10), 730., 10e-5);
    BOOST_REQUIRE_CLOSE(result.getDouble(2,10), 0., 10e-5);
    BOOST_REQUIRE_CLOSE(result.getDouble(3,10), 80., 10e-5);
    BOOST_REQUIRE_CLOSE(result.getDouble(4,10), 225., 10e-5);

    BOOST_REQUIRE_CLOSE(result.getDouble(1,15), 3130., 10e-5);
    BOOST_REQUIRE_CLOSE(result.getDouble(2,15), 10., 10e-5);
    BOOST_REQUIRE_CLOSE(result.getDouble(3,15), 95., 10e-5);
    BOOST_REQUIRE_CLOSE(result.getDouble(4,15), 650., 10e-5);

    BOOST_REQUIRE_CLOSE(result.getDouble(1,20), 8110., 10e-5);
    BOOST_REQUIRE_CLOSE(result.getDouble(2,20), 0., 10e-5);
    BOOST_REQUIRE_CLOSE(result.getDouble(3,20), 160., 10e-5);
    BOOST_REQUIRE_CLOSE(result.getDouble(4,20), 1250., 10e-5);


}

BOOST_AUTO_TEST_CASE(test_equation11)
{
    std::cout << " test_equation11 " << std::endl;
    auto ctx = vle::utils::make_context(); vle::utils::Package pack(ctx, "vle.examples");
    std::unique_ptr<vpz::Vpz> file(
    		new vpz::Vpz(pack.getExpFile("equation11.vpz")));
    vpz::Output& o(file->project().experiment().views().outputs().get("view"));
    o.setStream("", "storage", "vle.output");


    manager::Error error;
    manager::Simulation sim(ctx, manager::LOG_NONE,
            manager::SIMULATION_NONE,
            std::chrono::milliseconds(0), &std::cout);
    std::unique_ptr<value::Map> out = sim.run(std::move(file),
            "vle.examples", &error);

    BOOST_REQUIRE_EQUAL(error.code, 0);
    BOOST_REQUIRE(out != NULL);
    BOOST_REQUIRE_EQUAL(out->size(), 1);

    value::Matrix& result = out->getMatrix("view");


    BOOST_REQUIRE_EQUAL(result.columns(),
                        5);
    BOOST_REQUIRE_EQUAL(result.rows(),
                        21);

    BOOST_REQUIRE_CLOSE(result.getDouble(1,5), 60., 10e-5);
    BOOST_REQUIRE_CLOSE(result.getDouble(2,5), 10., 10e-5);
    BOOST_REQUIRE_CLOSE(result.getDouble(3,5), 15., 10e-5);
    BOOST_REQUIRE_CLOSE(result.getDouble(4,5), 10., 10e-5);

    BOOST_REQUIRE_CLOSE(result.getDouble(1,10), 430., 10e-5);
    BOOST_REQUIRE_CLOSE(result.getDouble(2,10), 0., 10e-5);
    BOOST_REQUIRE_CLOSE(result.getDouble(3,10), 80., 10e-5);
    BOOST_REQUIRE_CLOSE(result.getDouble(4,10), 0., 10e-5);

    BOOST_REQUIRE_CLOSE(result.getDouble(1,15), 1290., 10e-5);
    BOOST_REQUIRE_CLOSE(result.getDouble(2,15), 10., 10e-5);
    BOOST_REQUIRE_CLOSE(result.getDouble(3,15), 95., 10e-5);
    BOOST_REQUIRE_CLOSE(result.getDouble(4,15), 10., 10e-5);

    BOOST_REQUIRE_CLOSE(result.getDouble(1,20), 2460., 10e-5);
    BOOST_REQUIRE_CLOSE(result.getDouble(2,20), 0., 10e-5);
    BOOST_REQUIRE_CLOSE(result.getDouble(3,20), 160., 10e-5);
    BOOST_REQUIRE_CLOSE(result.getDouble(4,20), 0., 10e-5);


}

BOOST_AUTO_TEST_CASE(test_equation12)
{
    std::cout << " test_equation12 " << std::endl;
    auto ctx = vle::utils::make_context(); vle::utils::Package pack(ctx, "vle.examples");
    std::unique_ptr<vpz::Vpz> file(
    		new vpz::Vpz(pack.getExpFile("equation12.vpz")));
    vpz::Output& o(file->project().experiment().views().outputs().get("view"));
    o.setStream("", "storage", "vle.output");


    manager::Error error;
    manager::Simulation sim(ctx, manager::LOG_NONE,
            manager::SIMULATION_NONE,
            std::chrono::milliseconds(0), &std::cout);
    std::unique_ptr<value::Map> out = sim.run(std::move(file),
            "vle.examples", &error);

    BOOST_REQUIRE_EQUAL(error.code, 0);
    BOOST_REQUIRE(out != NULL);
    BOOST_REQUIRE_EQUAL(out->size(), 1);

    value::Matrix& result = out->getMatrix("view");


    BOOST_REQUIRE_EQUAL(result.columns(),
                        5);
    BOOST_REQUIRE_EQUAL(result.rows(),
                        21);

    BOOST_REQUIRE_CLOSE(result.getDouble(1,5), 7., 10e-5);
    BOOST_REQUIRE_CLOSE(result.getDouble(2,5), 12., 10e-5);
    BOOST_REQUIRE_CLOSE(result.getDouble(3,5), 10., 10e-5);
    BOOST_REQUIRE_CLOSE(result.getDouble(4,5), 27., 10e-5);

    BOOST_REQUIRE_CLOSE(result.getDouble(1,10), 14., 10e-5);
    BOOST_REQUIRE_CLOSE(result.getDouble(2,10), 62., 10e-5);
    BOOST_REQUIRE_CLOSE(result.getDouble(3,10), 165., 10e-5);
    BOOST_REQUIRE_CLOSE(result.getDouble(4,10), 504., 10e-5);

    BOOST_REQUIRE_CLOSE(result.getDouble(1,15), 21., 10e-5);
    BOOST_REQUIRE_CLOSE(result.getDouble(2,15), 147., 10e-5);
    BOOST_REQUIRE_CLOSE(result.getDouble(3,15), 640., 10e-5);
    BOOST_REQUIRE_CLOSE(result.getDouble(4,15), 2681., 10e-5);

    BOOST_REQUIRE_CLOSE(result.getDouble(1,20), 28., 10e-5);
    BOOST_REQUIRE_CLOSE(result.getDouble(2,20), 267., 10e-5);
    BOOST_REQUIRE_CLOSE(result.getDouble(3,20), 1610., 10e-5);
    BOOST_REQUIRE_CLOSE(result.getDouble(4,20), 8683., 10e-5);


}

BOOST_AUTO_TEST_CASE(test_equation13)
{
    std::cout << " test_equation13 " << std::endl;
    auto ctx = vle::utils::make_context(); vle::utils::Package pack(ctx, "vle.examples");
    std::unique_ptr<vpz::Vpz> file(
    		new vpz::Vpz(pack.getExpFile("equation13.vpz")));
    vpz::Output& o(file->project().experiment().views().outputs().get("view"));
    o.setStream("", "storage", "vle.output");


    manager::Error error;
    manager::Simulation sim(ctx, manager::LOG_NONE,
            manager::SIMULATION_NONE,
            std::chrono::milliseconds(0), &std::cout);
    std::unique_ptr<value::Map> out = sim.run(std::move(file),
            "vle.examples", &error);

    BOOST_REQUIRE_EQUAL(error.code, 0);
    BOOST_REQUIRE(out != NULL);
    BOOST_REQUIRE_EQUAL(out->size(), 1);

    value::Matrix& result = out->getMatrix("view");


    BOOST_REQUIRE_EQUAL(result.columns(),
                        6);
    BOOST_REQUIRE_EQUAL(result.rows(),
                        21);

    BOOST_REQUIRE_CLOSE(result.getDouble(1,5), 54., 10e-5);
    BOOST_REQUIRE_CLOSE(result.getDouble(2,5), 5., 10e-5);
    BOOST_REQUIRE_CLOSE(result.getDouble(3,5), 29., 10e-5);
    BOOST_REQUIRE_CLOSE(result.getDouble(4,5), 28., 10e-5);
    BOOST_REQUIRE_CLOSE(result.getDouble(5,5), 10., 10e-5);


}

BOOST_AUTO_TEST_CASE(test_equation14)
{
    std::cout << " test_equation14 " << std::endl;
    auto ctx = vle::utils::make_context(); vle::utils::Package pack(ctx, "vle.examples");
    std::unique_ptr<vpz::Vpz> file(
    		new vpz::Vpz(pack.getExpFile("equation14.vpz")));
    vpz::Output& o(file->project().experiment().views().outputs().get("view"));
    o.setStream("", "storage", "vle.output");


    manager::Error error;
    manager::Simulation sim(ctx, manager::LOG_NONE,
            manager::SIMULATION_NONE,
            std::chrono::milliseconds(0), &std::cout);
    std::unique_ptr<value::Map> out = sim.run(std::move(file),
            "vle.examples", &error);

    BOOST_REQUIRE_EQUAL(error.code, 0);
    BOOST_REQUIRE(out != NULL);
    BOOST_REQUIRE_EQUAL(out->size(), 1);

    value::Matrix& result = out->getMatrix("view");


    BOOST_REQUIRE_EQUAL(result.columns(),
                        6);
    BOOST_REQUIRE_EQUAL(result.rows(),
                        21);

    BOOST_REQUIRE_CLOSE(result.getDouble(1,5), 54., 10e-5);
    BOOST_REQUIRE_CLOSE(result.getDouble(2,5), 5., 10e-5);
    BOOST_REQUIRE_CLOSE(result.getDouble(3,5), 10., 10e-5);
    BOOST_REQUIRE_CLOSE(result.getDouble(4,5), 28., 10e-5);
    BOOST_REQUIRE_CLOSE(result.getDouble(5,5), 10., 10e-5);

    BOOST_REQUIRE_CLOSE(result.getDouble(1,10), 2470., 10e-5);
    BOOST_REQUIRE_CLOSE(result.getDouble(2,10), 10., 10e-5);
    BOOST_REQUIRE_CLOSE(result.getDouble(3,10), 0., 10e-5);
    BOOST_REQUIRE_CLOSE(result.getDouble(4,10), 1396., 10e-5);
    BOOST_REQUIRE_CLOSE(result.getDouble(5,10), 0., 10e-5);


}

BOOST_AUTO_TEST_CASE(test_equation15)
{
    std::cout << " test_equation15 " << std::endl;
    auto ctx = vle::utils::make_context(); vle::utils::Package pack(ctx, "vle.examples");
    std::unique_ptr<vpz::Vpz> file(
    		new vpz::Vpz(pack.getExpFile("equation15.vpz")));
    vpz::Output& o(file->project().experiment().views().outputs().get("view"));
    o.setStream("", "storage", "vle.output");


    manager::Error error;
    manager::Simulation sim(ctx, manager::LOG_NONE,
            manager::SIMULATION_NONE,
            std::chrono::milliseconds(0), &std::cout);
    std::unique_ptr<value::Map> out = sim.run(std::move(file),
            "vle.examples", &error);

    BOOST_REQUIRE_EQUAL(error.code, 0);
    BOOST_REQUIRE(out != NULL);
    BOOST_REQUIRE_EQUAL(out->size(), 1);

    value::Matrix& result = out->getMatrix("view");


    BOOST_REQUIRE_EQUAL(result.columns(), 6);
    //due to appoximation
    BOOST_REQUIRE(result.rows() >= 11);
    BOOST_REQUIRE(result.rows() <= 12);

    BOOST_REQUIRE_CLOSE(result.getDouble(1,10), 450., 10e-5);
    BOOST_REQUIRE_CLOSE(result.getDouble(2,10), 10., 10e-5);
    BOOST_REQUIRE_CLOSE(result.getDouble(3,10), 52., 10e-5);
    BOOST_REQUIRE_CLOSE(result.getDouble(4,10), 145., 10e-5);
    BOOST_REQUIRE_CLOSE(result.getDouble(5,10), 657., 10e-5);


}

BOOST_AUTO_TEST_CASE(test_asynchronous)
{
    auto ctx = vle::utils::make_context(); vle::utils::Package pack(ctx, "vle.examples");
    std::unique_ptr<vpz::Vpz> file(
    		new vpz::Vpz(pack.getExpFile("asynchronous.vpz")));
    vpz::Output& o(file->project().experiment().views().outputs().get("view"));
    o.setStream("", "storage", "vle.output");


    manager::Error error;
    manager::Simulation sim(ctx, manager::LOG_NONE,
            manager::SIMULATION_NONE,
            std::chrono::milliseconds(0), &std::cout);
    std::unique_ptr<value::Map> out = sim.run(std::move(file),
            "vle.examples", &error);

    BOOST_REQUIRE_EQUAL(error.code, 0);
    BOOST_REQUIRE(out != NULL);
    BOOST_REQUIRE_EQUAL(out->size(), 1);

    value::Matrix& result = out->getMatrix("view");


    BOOST_REQUIRE_EQUAL(result.columns(),
                        5);
    BOOST_REQUIRE_EQUAL(result.rows(),
                        6);

    BOOST_REQUIRE_CLOSE(result.getDouble(4,0), 0., 10e-5);
    BOOST_REQUIRE_CLOSE(result.getDouble(4,1), 5., 10e-5);
    BOOST_REQUIRE_CLOSE(result.getDouble(4,2), 6., 10e-5);
    BOOST_REQUIRE_CLOSE(result.getDouble(4,3), 6., 10e-5);
    BOOST_REQUIRE_CLOSE(result.getDouble(4,4), 7., 10e-5);
    BOOST_REQUIRE_CLOSE(result.getDouble(4,5), 6., 10e-5);


}

BOOST_AUTO_TEST_CASE(test_earlyPerturb)
{
    auto ctx = vle::utils::make_context(); vle::utils::Package pack(ctx, "vle.examples");
    std::unique_ptr<vpz::Vpz> file(
    		new vpz::Vpz(pack.getExpFile("earlyPerturb.vpz")));
    vpz::Output& o(file->project().experiment().views().outputs().get("view"));
    o.setStream("", "storage", "vle.output");


    manager::Error error;
    manager::Simulation sim(ctx, manager::LOG_NONE,
            manager::SIMULATION_NONE,
            std::chrono::milliseconds(0), &std::cout);
    std::unique_ptr<value::Map> out = sim.run(std::move(file),
            "vle.examples", &error);

    BOOST_REQUIRE_EQUAL(error.code, 0);
    BOOST_REQUIRE(out != NULL);
    BOOST_REQUIRE_EQUAL(out->size(), 1);

    value::Matrix& result = out->getMatrix("view");


    BOOST_REQUIRE_EQUAL(result.columns(),
                        4);
    BOOST_REQUIRE_EQUAL(result.rows(),
                        6);

    BOOST_REQUIRE_CLOSE(result.getDouble(3,0), 0., 10e-5);
    BOOST_REQUIRE_CLOSE(result.getDouble(3,1), 3., 10e-5);
    BOOST_REQUIRE_CLOSE(result.getDouble(3,2), 5., 10e-5);
    BOOST_REQUIRE_CLOSE(result.getDouble(3,3), 8., 10e-5);
    BOOST_REQUIRE_CLOSE(result.getDouble(3,4), 9., 10e-5);
    BOOST_REQUIRE_CLOSE(result.getDouble(3,5), 11., 10e-5);


}


BOOST_AUTO_TEST_CASE(test_confluentNosync)
{
	std::cout << " test_confluentNosync " << std::endl;

    auto ctx = vle::utils::make_context(); vle::utils::Package pack(ctx, "vle.examples");
    std::unique_ptr<vpz::Vpz> file(
            new vpz::Vpz(pack.getExpFile("confluentNosync.vpz")));


    manager::Error error;
    manager::Simulation sim(ctx, manager::LOG_NONE,
            manager::SIMULATION_NONE,
            std::chrono::milliseconds(0), &std::cout);
    std::unique_ptr<value::Map> out = sim.run(std::move(file),
            "vle.examples", &error);

    BOOST_REQUIRE_EQUAL(error.code, 0);
}
