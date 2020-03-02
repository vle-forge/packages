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

// @@tagtest@@

#include <vle/utils/unit-test.hpp>
#include <iostream>
#include <string>
#include <stdexcept>
#include <vle/manager/Manager.hpp>
#include <vle/manager/Simulation.hpp>
#include <vle/vpz/Vpz.hpp>
#include <vle/utils/Package.hpp>


#include <vle/value/Matrix.hpp>
#include <vle/vle.hpp>
#include <vle/version.hpp>

#include <iostream>
#include <iomanip>

struct F
{
    F() = default;

#if VLE_VERSION < 200100
    vle::Init app;
#endif
};

using namespace vle;

void test_petrinet_and()
{
    auto ctx = vle::utils::make_context(); vle::utils::Package pack(ctx, "vle.examples");
    std::unique_ptr<vpz::Vpz> file(
            new vpz::Vpz(pack.getExpFile("petrinet-and.vpz")));

    vpz::Output& o(file->project().experiment().views().outputs().get("view"));
    o.setStream("", "storage", "vle.output");


    manager::Error error;
    manager::Simulation sim(ctx, manager::SIMULATION_NONE,
            std::chrono::milliseconds(0));
    std::unique_ptr<value::Map> out = sim.run(std::move(file), &error);

    EnsuresEqual(error.code, 0);
    Ensures(out != NULL);
    EnsuresEqual(out->size(), 1);

    value::Matrix& result = out->getMatrix("view");


    EnsuresEqual(result.columns(),
                        5);
    //due to appoximation
    Ensures(result.rows() >= 11);
    Ensures(result.rows() <= 12);

    EnsuresApproximatelyEqual(result.getDouble(3,10), 0.75, 10e-5);
    EnsuresApproximatelyEqual(result.getDouble(4,10), 1.0, 10e-5);
}

void test_petrinet_or()
{
    auto ctx = vle::utils::make_context(); vle::utils::Package pack(ctx, "vle.examples");
    std::unique_ptr<vpz::Vpz> file(
            new vpz::Vpz(pack.getExpFile("petrinet-or.vpz")));

    vpz::Output& o(file->project().experiment().views().outputs().get("view"));
    o.setStream("", "storage", "vle.output");


    manager::Error error;
    manager::Simulation sim(ctx, manager::SIMULATION_NONE,
            std::chrono::milliseconds(0));
    std::unique_ptr<value::Map> out = sim.run(std::move(file), &error);

    EnsuresEqual(error.code, 0);
    Ensures(out != NULL);
    EnsuresEqual(out->size(), 1);

    value::Matrix& result = out->getMatrix("view");


    EnsuresEqual(result.columns(),
                        5);
    //due to appoximation
    Ensures(result.rows() >= 11);
    Ensures(result.rows() <= 12);


    EnsuresApproximatelyEqual(result.getDouble(3,10), 0.75, 10e-5);
    EnsuresApproximatelyEqual(result.getDouble(4,10), 2.0, 10e-5);
}

void test_petrinet_nand1()
{
    auto ctx = vle::utils::make_context(); vle::utils::Package pack(ctx, "vle.examples");
    std::unique_ptr<vpz::Vpz> file(
            new vpz::Vpz(pack.getExpFile("petrinet-nand1.vpz")));

    vpz::Output& o(file->project().experiment().views().outputs().get("view"));
    o.setStream("", "storage", "vle.output");


    manager::Error error;
    manager::Simulation sim(ctx, manager::SIMULATION_NONE,
            std::chrono::milliseconds(0));
    std::unique_ptr<value::Map> out = sim.run(std::move(file), &error);

    EnsuresEqual(error.code, 0);
    Ensures(out != NULL);
    EnsuresEqual(out->size(), 1);

    value::Matrix& result = out->getMatrix("view");


    EnsuresEqual(result.columns(),
                        5);
    //due to appoximation
    Ensures(result.rows() >= 11);
    Ensures(result.rows() <= 12);


    EnsuresApproximatelyEqual(result.getDouble(3,10), 0., 10e-5);
    EnsuresApproximatelyEqual(result.getDouble(4,10), 0., 10e-5);
}

void test_petrinet_nand2()
{
    auto ctx = vle::utils::make_context(); vle::utils::Package pack(ctx, "vle.examples");
    std::unique_ptr<vpz::Vpz> file(
            new vpz::Vpz(pack.getExpFile("petrinet-nand2.vpz")));

    vpz::Output& o(file->project().experiment().views().outputs().get("view"));
    o.setStream("", "storage", "vle.output");


    manager::Error error;
    manager::Simulation sim(ctx, manager::SIMULATION_NONE,
            std::chrono::milliseconds(0));
    std::unique_ptr<value::Map> out = sim.run(std::move(file), &error);

    EnsuresEqual(error.code, 0);
    Ensures(out != NULL);
    EnsuresEqual(out->size(), 1);

    value::Matrix& result = out->getMatrix("view");


    EnsuresEqual(result.columns(),
                        5);
    //due to appoximation
    Ensures(result.rows() >= 11);
    Ensures(result.rows() <= 12);


    EnsuresApproximatelyEqual(result.getDouble(3,10), 0.5, 10e-5);
    EnsuresApproximatelyEqual(result.getDouble(4,10), 1., 10e-5);
}

void test_petrinet_and_timed()
{
    auto ctx = vle::utils::make_context(); vle::utils::Package pack(ctx, "vle.examples");
    std::unique_ptr<vpz::Vpz> file(
            new vpz::Vpz(pack.getExpFile("petrinet-and-timed.vpz")));

    vpz::Output& o(file->project().experiment().views().outputs().get("view"));
    o.setStream("", "storage", "vle.output");


    manager::Error error;
    manager::Simulation sim(ctx, manager::SIMULATION_NONE,
            std::chrono::milliseconds(0));
    std::unique_ptr<value::Map> out = sim.run(std::move(file), &error);

    EnsuresEqual(error.code, 0);
    Ensures(out != NULL);
    EnsuresEqual(out->size(), 1);

    value::Matrix& result = out->getMatrix("view");


    EnsuresEqual(result.columns(),
                        5);
    //due to appoximation
    Ensures(result.rows() >= 20);
    Ensures(result.rows() <= 21);


    EnsuresApproximatelyEqual(result.getDouble(3,19), 1.7, 10e-5);
    EnsuresApproximatelyEqual(result.getDouble(4,19), 3., 10e-5);
}

void test_petrinet_or_priority()
{
    auto ctx = vle::utils::make_context(); vle::utils::Package pack(ctx, "vle.examples");
    std::unique_ptr<vpz::Vpz> file(
            new vpz::Vpz(pack.getExpFile("petrinet-or-priority.vpz")));

    vpz::Output& o(file->project().experiment().views().outputs().get("view"));
    o.setStream("", "storage", "vle.output");


    manager::Error error;
    manager::Simulation sim(ctx, manager::SIMULATION_NONE,
            std::chrono::milliseconds(0));
    std::unique_ptr<value::Map> out = sim.run(std::move(file), &error);

    EnsuresEqual(error.code, 0);
    Ensures(out != NULL);
    EnsuresEqual(out->size(), 1);

    value::Matrix& result = out->getMatrix("view");


    EnsuresEqual(result.columns(),
                        5);
    //due to appoximation
    Ensures(result.rows() >= 11);
    Ensures(result.rows() <= 12);


    EnsuresApproximatelyEqual(result.getDouble(3,10), 1., 10e-5);
    EnsuresApproximatelyEqual(result.getDouble(4,10), 6., 10e-5);
}

void test_petrinet_meteo()
{
    auto ctx = vle::utils::make_context(); vle::utils::Package pack(ctx, "vle.examples");
    std::unique_ptr<vpz::Vpz> file(
            new vpz::Vpz(pack.getExpFile("petrinet-meteo.vpz")));

    vpz::Output& o(file->project().experiment().views().outputs().get("view"));
    o.setStream("", "storage", "vle.output");


    manager::Error error;
    manager::Simulation sim(ctx, manager::SIMULATION_NONE,
            std::chrono::milliseconds(0));
    std::unique_ptr<value::Map> out = sim.run(std::move(file), &error);

    EnsuresEqual(error.code, 0);
    Ensures(out != NULL);
    EnsuresEqual(out->size(), 1);

    value::Matrix& result = out->getMatrix("view");


    EnsuresEqual(result.columns(),
                        6);
    EnsuresEqual(result.rows(),
                        31);

    EnsuresApproximatelyEqual(result.getDouble(4,30), 29., 10e-5);
    EnsuresApproximatelyEqual(result.getDouble(5,30), 4., 10e-5);
}

void test_petrinet_inout()
{
    auto ctx = vle::utils::make_context(); vle::utils::Package pack(ctx, "vle.examples");
    std::unique_ptr<vpz::Vpz> file(
            new vpz::Vpz(pack.getExpFile("petrinet-inout.vpz")));

    vpz::Output& o(file->project().experiment().views().outputs().get("view"));
    o.setStream("", "storage", "vle.output");


    manager::Error error;
    manager::Simulation sim(ctx, manager::SIMULATION_NONE,
            std::chrono::milliseconds(0));
    std::unique_ptr<value::Map> out = sim.run(std::move(file), &error);

    EnsuresEqual(error.code, 0);
    Ensures(out != NULL);
    EnsuresEqual(out->size(), 1);

    value::Matrix& result = out->getMatrix("view");


    EnsuresEqual(result.columns(),
                        5);
    //due to appoximation
    Ensures(result.rows() >= 11);
    Ensures(result.rows() <= 12);

    EnsuresApproximatelyEqual(result.getDouble(0,10), 1., 1.);

    Ensures(result.getInt(2,10) == 6);
    EnsuresApproximatelyEqual(result.getDouble(3,10), 1., 10e-5);
    EnsuresApproximatelyEqual(result.getDouble(4,10), 6., 10e-5);
}

void test_petrinet_conflict()
{
    auto ctx = vle::utils::make_context(); vle::utils::Package pack(ctx, "vle.examples");
    std::unique_ptr<vpz::Vpz> file(
            new vpz::Vpz(pack.getExpFile("petrinet-conflict.vpz")));

    vpz::Output& o(file->project().experiment().views().outputs().get("view"));
    o.setStream("", "storage", "vle.output");


    manager::Error error;
    manager::Simulation sim(ctx, manager::SIMULATION_NONE,
                std::chrono::milliseconds(0));
    std::unique_ptr<value::Map> out = sim.run(std::move(file), &error);

    EnsuresEqual(error.code, 0);
    Ensures(out != NULL);
    EnsuresEqual(out->size(), 1);

    value::Matrix& result = out->getMatrix("view");


    EnsuresEqual(result.columns(),
                        5);
    //due to appoximation
    Ensures(result.rows() >= 11);
    Ensures(result.rows() <= 12);


    Ensures(result.getInt(2,10) == 1);
    EnsuresApproximatelyEqual(result.getDouble(3,10), 1., 10e-5);
    EnsuresApproximatelyEqual(result.getDouble(4,10), 5., 10e-5);
}

int main()
{
    F fixture;

    test_petrinet_and();
    test_petrinet_or();
    test_petrinet_nand1();
    test_petrinet_nand2();
    test_petrinet_and_timed();
    test_petrinet_or_priority();
    test_petrinet_meteo();
    test_petrinet_inout();
    test_petrinet_conflict();

    return unit_test::report_errors();
}
