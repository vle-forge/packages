/*
 * @file examples/test/testfsa.cpp
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


#include <vle/utils/unit-test.hpp>
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

using namespace vle;

void test_moore1()
{
    auto ctx = vle::utils::make_context(); vle::utils::Package pack(ctx, "vle.examples");
    std::unique_ptr<vpz::Vpz> file(new vpz::Vpz(pack.getExpFile("moore.vpz")));

    vpz::Output& o(file->project().experiment().views().outputs().get("view"));
    o.setStream("", "storage", "vle.output");


    manager::Error error;
    manager::Simulation sim(ctx, manager::LOG_NONE,
            manager::SIMULATION_NONE,
            std::chrono::milliseconds(0), &std::cout);
    std::unique_ptr<value::Map> out = sim.run(std::move(file), &error);

    EnsuresEqual(error.code, 0);
    Ensures(out != NULL);
    EnsuresEqual(out->size(), 1);

    value::Matrix& result = out->getMatrix("view");


    EnsuresEqual(result.columns(),
                       4);
    EnsuresEqual(result.rows(),
                        101);

    EnsuresEqual(result.getInt(1,0), 1);
    EnsuresEqual(result.getInt(2,0), 1);
    EnsuresEqual(result.getInt(3,0), 1);

    EnsuresEqual(result.getInt(1,14), 3);
    EnsuresEqual(result.getInt(2,14), 3);
    EnsuresEqual(result.getInt(3,14), 3);

    EnsuresEqual(result.getInt(1,22), 3);
    EnsuresEqual(result.getInt(2,22), 2);
    EnsuresEqual(result.getInt(3,22), 2);

    EnsuresEqual(result.getInt(1,100), 3);
    EnsuresEqual(result.getInt(2,100), 2);
    EnsuresEqual(result.getInt(3,100), 2);
}

void test_moore2()
{
    auto ctx = vle::utils::make_context(); vle::utils::Package pack(ctx, "vle.examples");
    std::unique_ptr<vpz::Vpz> file(new vpz::Vpz(pack.getExpFile("moore2.vpz")));

    vpz::Output& o(file->project().experiment().views().outputs().get("view"));
    o.setStream("", "storage", "vle.output");


    manager::Error error;
    manager::Simulation sim(ctx, manager::LOG_NONE,
            manager::SIMULATION_NONE,
            std::chrono::milliseconds(0), &std::cout);
    std::unique_ptr<value::Map> out = sim.run(std::move(file), &error);

    EnsuresEqual(error.code, 0);
    Ensures(out != NULL);
    EnsuresEqual(out->size(), 1);

    value::Matrix& result = out->getMatrix("view");


    EnsuresEqual(result.columns(),
                       4);
    EnsuresEqual(result.rows(),
                        101);

    EnsuresEqual(result.getInt(1,0), 1);
    EnsuresEqual(result.getInt(2,0), 0);
    EnsuresEqual(result.getInt(3,0), 1);

    EnsuresEqual(result.getInt(1,100), 1);
    EnsuresEqual(result.getInt(2,100), 9);
    EnsuresEqual(result.getInt(3,100), 2);
}

void test_mealy1()
{
    auto ctx = vle::utils::make_context(); vle::utils::Package pack(ctx, "vle.examples");
    std::unique_ptr<vpz::Vpz> file(new vpz::Vpz(pack.getExpFile("mealy.vpz")));

    vpz::Output& o(file->project().experiment().views().outputs().get("view"));
    o.setStream("", "storage", "vle.output");


    manager::Error error;
    manager::Simulation sim(ctx, manager::LOG_NONE,
            manager::SIMULATION_NONE,
            std::chrono::milliseconds(0), &std::cout);
    std::unique_ptr<value::Map> out = sim.run(std::move(file), &error);

    EnsuresEqual(error.code, 0);
    Ensures(out != NULL);
    EnsuresEqual(out->size(), 1);

    value::Matrix& result = out->getMatrix("view");


    EnsuresEqual(result.columns(),
                       4);
    EnsuresEqual(result.rows(),
                        101);

    EnsuresEqual(result.getInt(1,0), 1);
    EnsuresEqual(result.getInt(2,0), 1);
    EnsuresEqual(result.getInt(3,0), 1);

    EnsuresEqual(result.getInt(1,14), 1);
    EnsuresEqual(result.getInt(2,14), 1);
    EnsuresEqual(result.getInt(3,14), 2);

    EnsuresEqual(result.getInt(1,22), 1);
    EnsuresEqual(result.getInt(2,22), 1);
    EnsuresEqual(result.getInt(3,22), 1);

    EnsuresEqual(result.getInt(1,61), 3);
    EnsuresEqual(result.getInt(2,61), 2);
    EnsuresEqual(result.getInt(3,61), 3);

    EnsuresEqual(result.getInt(1,100), 1);
    EnsuresEqual(result.getInt(2,100), 1);
    EnsuresEqual(result.getInt(3,100), 1);
}

void test_mealy2()
{
    auto ctx = vle::utils::make_context(); vle::utils::Package pack(ctx, "vle.examples");
    std::unique_ptr<vpz::Vpz> file(new vpz::Vpz(pack.getExpFile("mealy2.vpz")));

    vpz::Output& o(file->project().experiment().views().outputs().get("view"));
    o.setStream("", "storage", "vle.output");


    manager::Error error;
    manager::Simulation sim(ctx, manager::LOG_NONE,
            manager::SIMULATION_NONE,
            std::chrono::milliseconds(0), &std::cout);
    std::unique_ptr<value::Map> out = sim.run(std::move(file), &error);

    EnsuresEqual(error.code, 0);
    Ensures(out != NULL);
    EnsuresEqual(out->size(), 1);

    value::Matrix& result = out->getMatrix("view");


    EnsuresEqual(result.columns(),
                       4);
    EnsuresEqual(result.rows(),
                        101);

    EnsuresEqual(result.getInt(1,0), 1);
    EnsuresEqual(result.getInt(2,0), 0);
    EnsuresEqual(result.getInt(3,0), 1);

    EnsuresEqual(result.getInt(1,100), 1);
    EnsuresEqual(result.getInt(2,100), 9);
    EnsuresEqual(result.getInt(3,100), 2);
}

void test_statechart1()
{
    auto ctx = vle::utils::make_context(); vle::utils::Package pack(ctx, "vle.examples");
    std::unique_ptr<vpz::Vpz> file(new vpz::Vpz(pack.getExpFile("statechart.vpz")));

    vpz::Output& o(file->project().experiment().views().outputs().get("view"));
    o.setStream("", "storage", "vle.output");


    manager::Error error;
    manager::Simulation sim(ctx, manager::LOG_NONE,
            manager::SIMULATION_NONE,
            std::chrono::milliseconds(0), &std::cout);
    std::unique_ptr<value::Map> out = sim.run(std::move(file), &error);

    EnsuresEqual(error.code, 0);
    Ensures(out != NULL);
    EnsuresEqual(out->size(), 1);

    value::Matrix& result = out->getMatrix("view");


    EnsuresEqual(result.columns(),
                       4);
    EnsuresEqual(result.rows(),
                        101);

    EnsuresEqual(result.getInt(1,0), 1);
    EnsuresEqual(result.getInt(2,0), 1);
    EnsuresEqual(result.getInt(3,0), 1);

    EnsuresEqual(result.getInt(1,100), 1);
    EnsuresEqual(result.getInt(2,100), 1);
    EnsuresEqual(result.getInt(3,100), 1);
}

//// TODO test that uses rng (logic of model should be understood to rewrite the test)
//void test_statechart2)
//{
//    auto ctx = vle::utils::make_context(); vle::utils::Package pack(ctx, "vle.examples");
//    std::unique_ptr<vpz::Vpz> file(new vpz::Vpz(pack.getExpFile("statechart2.vpz")));
//
//    vpz::Output& o(file->project().experiment().views().outputs().get("view"));
//    o.setStream("", "storage", "vle.output");
//
//
//    manager::Error error;
//    manager::Simulation sim(ctx, manager::LOG_NONE,
//                            manager::SIMULATION_NONE,
//                            NULL);
//    std::unique_ptr<value::Map> out = sim.run(std::move(file), &error);
//
//    EnsuresEqual(error.code, 0);
//    Ensures(out != NULL);
//    EnsuresEqual(out->size(), 1);
//
//    value::Matrix& result = out->getMatrix("view");
//
//
//
//    EnsuresEqual(result.columns(),
//                       5);
//    //due to approximation
//    Ensures(result.rows() >= 1001);
//    Ensures(result.rows() <= 1002);
//
//    EnsuresEqual(result.getInt(1,0), 0);
//    EnsuresApproximatelyEqual(result.getDouble(2,0), 2.45601, 10e-2);
//    EnsuresApproximatelyEqual(result.getDouble(3,0), 3.0058, 10e-2);
//    EnsuresEqual(result.getInt(4,0), 2);
//
//    EnsuresEqual(result.getInt(1,25), 0);
//    EnsuresApproximatelyEqual(result.getDouble(2,25), 6.91764, 10e-2);
//    EnsuresApproximatelyEqual(result.getDouble(3,25), 3.0058, 10e-2);
//    EnsuresEqual(result.getInt(4,25), 3);
//
//    EnsuresEqual(result.getInt(1,1000), 0);
//    EnsuresApproximatelyEqual(result.getDouble(2,1000), 102.532, 10e-2);
//    EnsuresApproximatelyEqual(result.getDouble(3,1000), 100.913, 10e-2);
//    EnsuresEqual(result.getInt(4,1000), 4);
//}

//// TODO test that uses rng (logic of model should be understood to rewrite the test)
//void test_statechart3)
//{
//    auto ctx = vle::utils::make_context(); vle::utils::Package pack(ctx, "vle.examples");
//    std::unique_ptr<vpz::Vpz> file(new vpz::Vpz(pack.getExpFile("statechart3.vpz")));
//
//    vpz::Output& o(file->project().experiment().views().outputs().get("view"));
//    o.setStream("", "storage", "vle.output");
//
//
//    manager::Error error;
//    manager::Simulation sim(ctx, manager::LOG_NONE,
//                            manager::SIMULATION_NONE,
//                            NULL);
//    std::unique_ptr<value::Map> out = sim.run(std::move(file), &error);
//
//    EnsuresEqual(error.code, 0);
//    Ensures(out != NULL);
//    EnsuresEqual(out->size(), 1);
//
//    value::Matrix& result = out->getMatrix("view");
//
//
//    EnsuresEqual(result.columns(),
//                       5);
//    //due to approximation
//    Ensures(result.rows() >= 1001);
//    Ensures(result.rows() <= 1002);
//
//    EnsuresEqual(result.getInt(1,0), 0);
//    EnsuresApproximatelyEqual(result.getDouble(2,0), 2.45601, 10e-2);
//    EnsuresApproximatelyEqual(result.getDouble(3,0), 3.0058, 10e-2);
//    EnsuresEqual(result.getInt(4,0), 2);
//
//    EnsuresEqual(result.getInt(1,25), 1);
//    EnsuresApproximatelyEqual(result.getDouble(2,25), 6.91764, 10e-2);
//    EnsuresApproximatelyEqual(result.getDouble(3,25), 3.0058, 10e-2);
//    EnsuresEqual(result.getInt(4,25), 3);
//
//    EnsuresEqual(result.getInt(1,1000), 29);
//    EnsuresApproximatelyEqual(result.getDouble(2,1000), 102.532, 10e-2);
//    EnsuresApproximatelyEqual(result.getDouble(3,1000), 100.913, 10e-2);
//    EnsuresEqual(result.getInt(4,1000), 4);
//}

//// TODO test that uses rng (logic of model should be understood to rewrite the test)
//void test_statechart4)
//{
//    auto ctx = vle::utils::make_context(); vle::utils::Package pack(ctx, "vle.examples");
//    std::unique_ptr<vpz::Vpz> file(new vpz::Vpz(pack.getExpFile("statechart4.vpz")));
//
//    vpz::Output& o(file->project().experiment().views().outputs().get("view"));
//    o.setStream("", "storage", "vle.output");
//
//
//    manager::Error error;
//    manager::Simulation sim(ctx, manager::LOG_NONE,
//                            manager::SIMULATION_NONE,
//                            NULL);
//    std::unique_ptr<value::Map> out = sim.run(std::move(file), &error);
//
//    EnsuresEqual(error.code, 0);
//    Ensures(out != NULL);
//    EnsuresEqual(out->size(), 1);
//
//    value::Matrix& result = out->getMatrix("view");
//
//
//    EnsuresEqual(result.columns(),
//                       7);
//    //due to approximation
//    Ensures(result.rows() >= 1001);
//    Ensures(result.rows() <= 1002);
//
//    EnsuresEqual(result.getInt(1,0), 0);
//    EnsuresApproximatelyEqual(result.getDouble(2,0), 2.45601, 10e-2);
//    EnsuresApproximatelyEqual(result.getDouble(3,0), 3.0058, 10e-2);
//    EnsuresApproximatelyEqual(result.getDouble(4,0), 4.46163, 10e-2);
//    EnsuresApproximatelyEqual(result.getDouble(5,0), 0., 10e-2);
//    EnsuresEqual(result.getInt(6,0), 2);
//
//    EnsuresEqual(result.getInt(1,25), 1);
//    EnsuresApproximatelyEqual(result.getDouble(2,25), 5.53795, 10e-2);
//    EnsuresApproximatelyEqual(result.getDouble(3,25), 3.0058, 10e-2);
//    EnsuresApproximatelyEqual(result.getDouble(4,25), 4.46163, 10e-2);
//    EnsuresApproximatelyEqual(result.getDouble(5,25), 0., 10e-2);
//    EnsuresEqual(result.getInt(6,25), 3);
//
//    EnsuresEqual(result.getInt(1,523), 16);
//    EnsuresApproximatelyEqual(result.getDouble(2,523), 55.9634, 10e-2);
//    EnsuresApproximatelyEqual(result.getDouble(3,523), 52.9811, 10e-2);
//    EnsuresApproximatelyEqual(result.getDouble(4,523), 53.5509, 10e-2);
//    EnsuresApproximatelyEqual(result.getDouble(5,523), 29., 10e-2);
//    EnsuresEqual(result.getInt(6,523), 4);
//
//    EnsuresEqual(result.getInt(1,1000), 29);
//    EnsuresApproximatelyEqual(result.getDouble(2,1000), 100.174, 10e-2);
//    EnsuresApproximatelyEqual(result.getDouble(3,1000), 100.985, 10e-2);
//    EnsuresApproximatelyEqual(result.getDouble(4,1000), 101.733, 10e-2);
//    EnsuresApproximatelyEqual(result.getDouble(5,1000), 57., 10e-2);
//    EnsuresEqual(result.getInt(6,1000), 2);
//}

//// TODO test that uses rng (logic of model should be understood to rewrite the test)
//void test_statechart5)
//{
//    auto ctx = vle::utils::make_context(); vle::utils::Package pack(ctx, "vle.examples");
//    std::unique_ptr<vpz::Vpz> file(new vpz::Vpz(pack.getExpFile("statechart5.vpz")));
//
//    vpz::Output& o(file->project().experiment().views().outputs().get("view"));
//    o.setStream("", "storage", "vle.output");
//
//
//    manager::Error error;
//    manager::Simulation sim(ctx, manager::LOG_NONE,
//                            manager::SIMULATION_NONE,
//                            NULL);
//    std::unique_ptr<value::Map> out = sim.run(std::move(file), &error);
//
//    EnsuresEqual(error.code, 0);
//    Ensures(out != NULL);
//    EnsuresEqual(out->size(), 1);
//
//    value::Matrix& result = out->getMatrix("view");
//
//
//    EnsuresEqual(result.columns(),
//                       5);
//    //due to approximation
//    Ensures(result.rows() >= 1001);
//    Ensures(result.rows() <= 1002);
//
//    EnsuresEqual(result.getInt(1,0), 0);
//    EnsuresApproximatelyEqual(result.getDouble(2,0), 2.45601, 10e-2);
//    EnsuresApproximatelyEqual(result.getDouble(3,0), 3.0058, 10e-2);
//    EnsuresEqual(result.getInt(4,0), 2);
//
//    EnsuresEqual(result.getInt(1,25), 0);
//    EnsuresApproximatelyEqual(result.getDouble(2,25), 6.91764, 10e-2);
//    EnsuresApproximatelyEqual(result.getDouble(3,25), 3.0058, 10e-2);
//    EnsuresEqual(result.getInt(4,25), 3);
//
//    EnsuresEqual(result.getInt(1,295), 0);
//    EnsuresApproximatelyEqual(result.getDouble(2,295), 30.8661, 10e-2);
//    EnsuresApproximatelyEqual(result.getDouble(3,295), 31.4728, 10e-2);
//    EnsuresEqual(result.getInt(4,295), 5);
//
//    EnsuresEqual(result.getInt(1,1000), 0);
//    EnsuresApproximatelyEqual(result.getDouble(2,1000), 102.532, 10e-2);
//    EnsuresApproximatelyEqual(result.getDouble(3,1000), 100.913, 10e-2);
//    EnsuresEqual(result.getInt(4,1000), 3);
//}

//// TODO test that uses rng (logic of model should be understood to rewrite the test)
//void test_statechart6)
//{
//    auto ctx = vle::utils::make_context(); vle::utils::Package pack(ctx, "vle.examples");
//    std::unique_ptr<vpz::Vpz> file(new vpz::Vpz(pack.getExpFile("statechart6.vpz")));
//
//    vpz::Output& o(file->project().experiment().views().outputs().get("view"));
//    o.setStream("", "storage", "vle.output");
//
//
//    manager::Error error;
//    manager::Simulation sim(ctx, manager::LOG_NONE,
//                            manager::SIMULATION_NONE,
//                            NULL);
//    std::unique_ptr<value::Map> out = sim.run(std::move(file), &error);
//
//    EnsuresEqual(error.code, 0);
//    Ensures(out != NULL);
//    EnsuresEqual(out->size(), 1);
//
//    value::Matrix& result = out->getMatrix("view");
//
//
//    EnsuresEqual(result.columns(),
//                       5);
//    //due to approximation
//    Ensures(result.rows() >= 1001);
//    Ensures(result.rows() <= 1002);
//
//    EnsuresEqual(result.getInt(1,0), 0);
//    EnsuresApproximatelyEqual(result.getDouble(2,0), 2.45601, 10e-2);
//    EnsuresApproximatelyEqual(result.getDouble(3,0), 3.0058, 10e-2);
//    EnsuresEqual(result.getInt(4,0), 2);
//
//    EnsuresEqual(result.getInt(1,25), 0);
//    EnsuresApproximatelyEqual(result.getDouble(2,25), 6.91764, 10e-2);
//    EnsuresApproximatelyEqual(result.getDouble(3,25), 3.0058, 10e-2);
//    EnsuresEqual(result.getInt(4,25), 3);
//
//    EnsuresEqual(result.getInt(1,690), 0);
//    EnsuresApproximatelyEqual(result.getDouble(2,690), 69.8031, 10e-2);
//    EnsuresApproximatelyEqual(result.getDouble(3,690), 70.1079, 10e-2);
//    EnsuresEqual(result.getInt(4,690), 5);
//
//    EnsuresEqual(result.getInt(1,1000), 0);
//    EnsuresApproximatelyEqual(result.getDouble(2,1000), 102.532, 10e-2);
//    EnsuresApproximatelyEqual(result.getDouble(3,1000), 100.913, 10e-2);
//    EnsuresEqual(result.getInt(4,1000), 4);
//}

//// TODO test that uses rng (logic of model should be understood to rewrite the test)
//void test_stage)
//{
//    auto ctx = vle::utils::make_context(); vle::utils::Package pack(ctx, "vle.examples");
//    std::unique_ptr<vpz::Vpz> file(new vpz::Vpz(pack.getExpFile("stage.vpz")));
//
//    vpz::Output& o(file->project().experiment().views().outputs().get("view"));
//    o.setStream("", "storage", "vle.output");
//
//
//    manager::Error error;
//    manager::Simulation sim(ctx, manager::LOG_NONE,
//                            manager::SIMULATION_NONE,
//                            NULL);
//    std::unique_ptr<value::Map> out = sim.run(std::move(file), &error);
//
//    EnsuresEqual(error.code, 0);
//    Ensures(out != NULL);
//    EnsuresEqual(out->size(), 1);
//
//    value::Matrix& result = out->getMatrix("view");
//
//
//    EnsuresEqual(result.columns(),
//                       3);
//    EnsuresEqual(result.rows(),
//                        331);
//
//    EnsuresEqual(result.getInt(2,0), 1);
//    EnsuresEqual(result.getInt(2,186), 2);
//    EnsuresEqual(result.getInt(2,193), 3);
//    EnsuresEqual(result.getInt(2,202), 4);
//    EnsuresEqual(result.getInt(2,282), 5);
//    EnsuresEqual(result.getInt(2,295), 6);
//    EnsuresEqual(result.getInt(2,304), 7);
//    EnsuresEqual(result.getInt(2,323), 8);
//    EnsuresEqual(result.getInt(2,330), 9);
//}

void test_statechart7()
{
    auto ctx = vle::utils::make_context(); vle::utils::Package pack(ctx, "vle.examples");
    std::unique_ptr<vpz::Vpz> file(new vpz::Vpz(pack.getExpFile("statechart7.vpz")));

    vpz::Output& o(file->project().experiment().views().outputs().get("view"));
    o.setStream("", "storage", "vle.output");


    manager::Error error;
    manager::Simulation sim(ctx, manager::LOG_NONE,
            manager::SIMULATION_NONE,
            std::chrono::milliseconds(0), &std::cout);
    std::unique_ptr<value::Map> out = sim.run(std::move(file), &error);

    EnsuresEqual(error.code, 0);
    Ensures(out != NULL);
    EnsuresEqual(out->size(), 1);

    value::Matrix& result = out->getMatrix("view");


    EnsuresEqual(result.columns(),
                       2);
    //due to approximation
    Ensures(result.rows() >= 101);
    Ensures(result.rows() <= 102);

    EnsuresEqual(result.getInt(1,0), 2);
    EnsuresEqual(result.getInt(1,40), 3);
}

//// TODO test that uses rng (logic of model should be understood to rewrite the test)
//void test_statechart8)
//{
//    auto ctx = vle::utils::make_context(); vle::utils::Package pack(ctx, "vle.examples");
//    std::unique_ptr<vpz::Vpz> file(new vpz::Vpz(pack.getExpFile("statechart8.vpz")));
//
//    vpz::Output& o(file->project().experiment().views().outputs().get("view"));
//    o.setStream("", "storage", "vle.output");
//
//
//    manager::Error error;
//    manager::Simulation sim(ctx, manager::LOG_NONE,
//                            manager::SIMULATION_NONE,
//                            NULL);
//    std::unique_ptr<value::Map> out = sim.run(std::move(file), &error);
//
//    EnsuresEqual(error.code, 0);
//    Ensures(out != NULL);
//    EnsuresEqual(out->size(), 1);
//
//    value::Matrix& result = out->getMatrix("view");
//
//
//    EnsuresEqual(result.columns(),
//                       3);
//    EnsuresEqual(result.rows(),
//                        101);
//
//    EnsuresEqual(result.getInt(1,3), 2);
//    EnsuresEqual(result.getInt(2,3), 2);
//    EnsuresEqual(result.getInt(1,6), 3);
//    EnsuresEqual(result.getInt(1,30), 10);
//    EnsuresEqual(result.getInt(1,100), 32);
//}

//// TODO test that uses rng (logic of model should be understood to rewrite the test)
//void test_statechart9)
//{
//    auto ctx = vle::utils::make_context(); vle::utils::Package pack(ctx, "vle.examples");
//    std::unique_ptr<vpz::Vpz> file(new vpz::Vpz(pack.getExpFile("statechart9.vpz")));
//
//    vpz::Output& o(file->project().experiment().views().outputs().get("view"));
//    o.setStream("", "storage", "vle.output");
//
//
//    manager::Error error;
//    manager::Simulation sim(ctx, manager::LOG_NONE,
//                            manager::SIMULATION_NONE,
//                            NULL);
//    std::unique_ptr<value::Map> out = sim.run(std::move(file), &error);
//
//    EnsuresEqual(error.code, 0);
//    Ensures(out != NULL);
//    EnsuresEqual(out->size(), 1);
//
//    value::Matrix& result = out->getMatrix("view");
//
//
//    EnsuresEqual(result.columns(),
//                       4);
//    EnsuresEqual(result.rows(),
//                        101);
//
//    EnsuresEqual(result.getInt(1,0), 0);
//    EnsuresEqual(result.getInt(2,0), 0);
//    EnsuresEqual(result.getInt(3,0), 2);
//
//    EnsuresEqual(result.getInt(1,15), 1);
//    EnsuresEqual(result.getInt(2,15), 4);
//    EnsuresEqual(result.getInt(3,15), 2);
//
//    EnsuresEqual(result.getInt(1,100), 8);
//    EnsuresEqual(result.getInt(2,100), 31);
//    EnsuresEqual(result.getInt(3,100), 2);
//}

void test_statechart10()
{
    auto ctx = vle::utils::make_context(); vle::utils::Package pack(ctx, "vle.examples");
    std::unique_ptr<vpz::Vpz> file(new vpz::Vpz(pack.getExpFile("statechart10.vpz")));

    vpz::Output& o(file->project().experiment().views().outputs().get("view"));
    o.setStream("", "storage", "vle.output");


    manager::Error error;
    manager::Simulation sim(ctx, manager::LOG_NONE,
            manager::SIMULATION_NONE,
            std::chrono::milliseconds(0), &std::cout);
    std::unique_ptr<value::Map> out = sim.run(std::move(file), &error);

    EnsuresEqual(error.code, 0);
    Ensures(out != NULL);
    EnsuresEqual(out->size(), 1);

    value::Matrix& result = out->getMatrix("view");


    EnsuresEqual(result.columns(),
                       3);
    //due to approximation
    Ensures(result.rows() >= 1001);
    Ensures(result.rows() <= 1002);
}

void test_statechart11()
{
    auto ctx = vle::utils::make_context(); vle::utils::Package pack(ctx, "vle.examples");
    std::unique_ptr<vpz::Vpz> file(new vpz::Vpz(pack.getExpFile("statechart11.vpz")));

    vpz::Output& o(file->project().experiment().views().outputs().get("view"));
    o.setStream("", "storage", "vle.output");


    manager::Error error;
    manager::Simulation sim(ctx, manager::LOG_NONE,
            manager::SIMULATION_NONE,
            std::chrono::milliseconds(0), &std::cout);
    std::unique_ptr<value::Map> out = sim.run(std::move(file), &error);

    EnsuresEqual(error.code, 0);
    Ensures(out != NULL);
    EnsuresEqual(out->size(), 1);

    value::Matrix& result = out->getMatrix("view");


    EnsuresEqual(result.columns(),
                       5);
    EnsuresEqual(result.rows(),
                        101);

    EnsuresEqual(result.getDouble(1,0), 0);
    EnsuresEqual(result.getDouble(2,0), 0);
    EnsuresEqual(result.getDouble(3,0), 0);
    EnsuresEqual(result.getDouble(4,0), 0);

    EnsuresEqual(result.getDouble(1,25), 1);
    EnsuresEqual(result.getDouble(2,25), -25);
    EnsuresEqual(result.getDouble(3,25), 0);
    EnsuresEqual(result.getDouble(4,25), 25);

    EnsuresEqual(result.getDouble(1,55), 0);
    EnsuresEqual(result.getDouble(2,55), 30);
    EnsuresEqual(result.getDouble(3,55), 30);
    EnsuresEqual(result.getDouble(4,55), 55);

    EnsuresEqual(result.getDouble(1,64), -1.5);
    EnsuresEqual(result.getDouble(2,64), 126);
    EnsuresEqual(result.getDouble(3,64), 30);
    EnsuresEqual(result.getDouble(4,64), 64);

    EnsuresEqual(result.getDouble(1,100), -1.5);
    EnsuresEqual(result.getDouble(2,100), 126);
    EnsuresEqual(result.getDouble(3,100), -24);
    EnsuresEqual(result.getDouble(4,100), 100);
}

void test_statechartMultipleSend()
{
    auto ctx = vle::utils::make_context(); vle::utils::Package pack(ctx, "vle.examples");
    std::unique_ptr<vpz::Vpz> file(new vpz::Vpz(pack.getExpFile(
        "statechartMultipleSend.vpz")));

    vpz::Output& o(file->project().experiment().views().outputs().get("view"));
    o.setStream("", "storage", "vle.output");


    manager::Error error;
    manager::Simulation sim(ctx, manager::LOG_NONE,
            manager::SIMULATION_NONE,
            std::chrono::milliseconds(0), &std::cout);
    std::unique_ptr<value::Map> out = sim.run(std::move(file), &error);

    EnsuresEqual(error.code, 0);
    Ensures(out != NULL);
    EnsuresEqual(out->size(), 1);
}

int main()
{
    F fixture;
    test_moore1();
    test_moore2();
    test_mealy1();
    test_mealy2();
    test_statechart1();
    test_statechart7();
    test_statechart10();
    test_statechart11();
    test_statechartMultipleSend();

    return unit_test::report_errors();
}
