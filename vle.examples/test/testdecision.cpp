/*
 * @file examples/test/testdecision.cpp
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
#include <string>
#include <iostream>
#include <stdexcept>
#include <vle/manager/Manager.hpp>
#include <vle/manager/Simulation.hpp>
#include <vle/vpz/Vpz.hpp>
#include <vle/utils/Package.hpp>


#include <vle/value/Matrix.hpp>
#include <vle/vle.hpp>

struct F
{
    F()
    {

    }

    vle::Init app;
};


using namespace vle;

void test_agentonly()
{
    auto ctx = vle::utils::make_context(); vle::utils::Package pack(ctx, "vle.examples");
    std::unique_ptr<vpz::Vpz> file(new vpz::Vpz(
    		pack.getExpFile("agentonly.vpz")));


    manager::Error error;
    manager::Simulation sim(ctx, manager::LOG_NONE,
            manager::SIMULATION_NONE,
            std::chrono::milliseconds(0), &std::cout);
    std::unique_ptr<value::Map> out = sim.run(std::move(file), &error);

    EnsuresEqual(error.code, 0);
    Ensures(out != NULL);
    EnsuresEqual(out->size(), 1);

    value::Matrix& result = out->getMatrix("storage");

    EnsuresEqual(result.columns(), 2);
    EnsuresEqual(result.rows(), 12);

    EnsuresEqual(result.getInt(1, 1), 1);
    EnsuresEqual(result.getInt(1, 2), 4);
    EnsuresEqual(result.getInt(1, 3), 7);
    EnsuresEqual(result.getInt(1, 4), 8);
    EnsuresEqual(result.getInt(1, 5), 8);
    EnsuresEqual(result.getInt(1, 6), 8);
    EnsuresEqual(result.getInt(1, 7), 8);
    EnsuresEqual(result.getInt(1, 8), 8);
    EnsuresEqual(result.getInt(1, 9), 8);
    EnsuresEqual(result.getInt(1, 10), 8);
    EnsuresEqual(result.getInt(1, 11), 9);

}

void test_agentonlyprecedenceconstraint()
{
    auto ctx = vle::utils::make_context(); vle::utils::Package pack(ctx, "vle.examples");
    std::unique_ptr<vpz::Vpz> file(new vpz::Vpz(
    		pack.getExpFile("agentonlyc.vpz")));


    manager::Error error;
    manager::Simulation sim(ctx, manager::LOG_NONE,
            manager::SIMULATION_NONE,
            std::chrono::milliseconds(0), &std::cout);
    std::unique_ptr<value::Map> out = sim.run(std::move(file), &error);

    EnsuresEqual(error.code, 0);
    Ensures(out != NULL);
    EnsuresEqual(out->size(), 1);

    value::Matrix &result = out->getMatrix("storage");

    EnsuresEqual(result.getInt(1, 1), 1);
    EnsuresEqual(result.getInt(1, 2), 2);
    EnsuresEqual(result.getInt(1, 3), 3);
    EnsuresEqual(result.getInt(1, 4), 3);
    EnsuresEqual(result.getInt(1, 5), 3);
    EnsuresEqual(result.getInt(1, 6), 3);
    EnsuresEqual(result.getInt(1, 7), 3);
    EnsuresEqual(result.getInt(1, 8), 3);
    EnsuresEqual(result.getInt(1, 9), 3);
    EnsuresEqual(result.getInt(1, 10), 3);
    EnsuresEqual(result.getInt(1, 11), 4);

}

void test_agentonlywakeup()
{
    auto ctx = vle::utils::make_context(); vle::utils::Package pack(ctx, "vle.examples");
    std::unique_ptr<vpz::Vpz> file(
    		new vpz::Vpz(pack.getExpFile("agentonlywakeup.vpz")));


    manager::Error error;
    manager::Simulation sim(ctx, manager::LOG_NONE,
            manager::SIMULATION_NONE,
            std::chrono::milliseconds(0), &std::cout);
    std::unique_ptr<value::Map> out = sim.run(std::move(file), &error);

    EnsuresEqual(error.code, 0);
    Ensures(out != NULL);
    EnsuresEqual(out->size(), 1);

    value::Matrix &result = out->getMatrix("storage");

    EnsuresEqual(result.getInt(1, 1), 0);
    EnsuresEqual(result.getInt(1, 2), 1);
    EnsuresEqual(result.getInt(1, 3), 1);
    EnsuresEqual(result.getInt(1, 4), 1);
    EnsuresEqual(result.getInt(1, 5), 1);
    EnsuresEqual(result.getInt(1, 6), 1);
    EnsuresEqual(result.getInt(1, 7), 2);
    EnsuresEqual(result.getInt(1, 8), 2);
    EnsuresEqual(result.getInt(1, 9), 2);
    EnsuresEqual(result.getInt(1, 10), 2);
    EnsuresEqual(result.getInt(1, 11), 2);

}

int main()
{
    F fixture;
    test_agentonly();
    test_agentonlyprecedenceconstraint();
    test_agentonlywakeup();

    return unit_test::report_errors();
}

