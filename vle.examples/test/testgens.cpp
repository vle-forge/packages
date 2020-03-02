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

// @@tagtest@@

#include <vle/utils/unit-test.hpp>
#include <string>
#include <stdexcept>
#include <iostream>
#include <vle/manager/Manager.hpp>
#include <vle/manager/Simulation.hpp>
#include <vle/vpz/Vpz.hpp>
#include <vle/utils/Package.hpp>


#include <vle/vle.hpp>
#include <vle/value/Matrix.hpp>
#include <vle/version.hpp>

struct F
{
    F() = default;

#if VLE_VERSION < 200100
    vle::Init app;
#endif
};

using namespace vle;

void test_gens()
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
    manager::Simulation sim(ctx, manager::SIMULATION_NONE,
                std::chrono::milliseconds(0));
    std::unique_ptr<value::Map> out = sim.run(std::move(file), &error);

    /* begin check */
    EnsuresEqual(error.code, 0);
    Ensures(out != NULL);
    EnsuresEqual(out->size(), 2);

    /* get result of simulation */
    value::Matrix& result = out->getMatrix("view1");


    EnsuresEqual(result.columns(),
                        3);
    EnsuresEqual(result.rows(),
                        101);

    EnsuresEqual(result.getDouble(0,0), 0);
    EnsuresEqual(result.getDouble(1,0), 0);
    EnsuresEqual(result.getInt(2,0), 1);

    EnsuresEqual(result.getDouble(0,10), 10);
    EnsuresEqual(result.getDouble(1,10), 55);
    EnsuresEqual(result.getInt(2,10), 11);

    EnsuresEqual(result.getDouble(0,14), 14);
    Ensures(result.get(1,14) == 0);
    EnsuresEqual(result.getInt(2,14), 15);

    EnsuresEqual(result.getDouble(0,31), 31);
    Ensures(result.get(1,31) == 0);
    EnsuresEqual(result.getInt(2,31), 32);

    EnsuresEqual(result.getDouble(0,100), 100);
    EnsuresEqual(result.getDouble(1,100), 2550);
    EnsuresEqual(result.getInt(2,100), 1);


}

void test_gens_with_class()
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
    manager::Simulation sim(ctx, manager::SIMULATION_NONE,
                std::chrono::milliseconds(0));
    std::unique_ptr<value::Map> out = sim.run(std::move(file), &error);

    /* begin check */
    EnsuresEqual(error.code, 0);
    Ensures(out != NULL);
    EnsuresEqual(out->size(), 2);

    /* get result of simulation */
    value::Matrix& result = out->getMatrix("view1");


    EnsuresEqual(error.code, 0);
    Ensures(out != NULL);
    EnsuresEqual(out->size(), 2);

    EnsuresEqual(result.columns(),
                        3);
    EnsuresEqual(result.rows(),
                        101);

    EnsuresEqual(result.getDouble(0,0), 0);
    EnsuresEqual(result.getDouble(1,0), 0);
    EnsuresEqual(result.getInt(2,0), 2);

    EnsuresEqual(result.getDouble(0,5), 5);
    EnsuresEqual(result.getDouble(1,5), 63);
    EnsuresEqual(result.getInt(2,5), 7);

    EnsuresEqual(result.getDouble(0,7), 7);
    Ensures(result.get(1,7) == 0);
    EnsuresEqual(result.getInt(2,7), 9);

    EnsuresEqual(result.getDouble(0,15), 15);
    EnsuresEqual(result.getDouble(1,15), 528);
    EnsuresEqual(result.getInt(2,15), 17);

    EnsuresEqual(result.getDouble(0,100), 100);
    EnsuresEqual(result.getDouble(1,100), 11400);
    EnsuresEqual(result.getInt(2,100), 2);


}

void test_gens_with_graph()
{
    auto ctx = vle::utils::make_context(); vle::utils::Package pack(ctx, "vle.examples");
    std::unique_ptr<vpz::Vpz> file(new vpz::Vpz(
        pack.getExpFile("gensgraph.vpz")));

    /* change the output text to storage output */
    vpz::Output& o(file->project().experiment().views().outputs().get("view1"));
    o.setStream("", "storage", "vle.output");

    /* run the simulation */

    manager::Error error;
    manager::Simulation sim(ctx, manager::SIMULATION_NONE,
            std::chrono::milliseconds(0));
    std::unique_ptr<value::Map> out = sim.run(std::move(file), &error);

    /* begin check */
    EnsuresEqual(error.code, 0);
    Ensures(out != NULL);
    EnsuresEqual(out->size(), 1);

    /* get result of simulation */
    value::Matrix& result = out->getMatrix("view1");



    EnsuresEqual(result.columns(),
                        2);
    EnsuresEqual(result.rows(),
                        101);

    EnsuresEqual(result.getDouble(0,0), 0);
    EnsuresEqual(result.getDouble(1,0), 5);

    EnsuresEqual(result.getDouble(0,5), 5);
    EnsuresEqual(result.getDouble(1,5), 30);

    EnsuresEqual(result.getDouble(0,20), 20);
    Ensures(result.get(1,20) == 0);

    EnsuresEqual(result.getDouble(0,99), 99);
    EnsuresEqual(result.getDouble(1,99), 500);

    EnsuresEqual(result.getDouble(0,100), 100);
    EnsuresEqual(result.getDouble(1,100), 505);


}


void test_gens_delete_connection()
{
    auto ctx = vle::utils::make_context(); vle::utils::Package pack(ctx, "vle.examples");
    std::unique_ptr<vpz::Vpz> file(new vpz::Vpz(
        pack.getExpFile("gensdelete.vpz")));

    /* change the output text to storage output */
    vpz::Output& o(file->project().experiment().views().outputs().get("view1"));
    o.setStream("", "storage", "vle.output");

    /* run the simulation */

    manager::Error error;
    manager::Simulation sim(ctx, manager::SIMULATION_NONE,
                std::chrono::milliseconds(0));
    std::unique_ptr<value::Map> out = sim.run(std::move(file), &error);

    /* begin check */
    EnsuresEqual(error.code, 0);


}

int main()
{
    F fixture;
    test_gens();
    test_gens_with_class();
    test_gens_with_graph();
    test_gens_delete_connection();

    return unit_test::report_errors();
}
