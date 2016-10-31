/*
 * @file examples/test/testqss.cpp
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
    vle::Init app;

    F()
    {
    }
};


using namespace vle;

void test_qss4()
{
    auto ctx = vle::utils::make_context(); vle::utils::Package pack(ctx, "vle.examples");
    std::unique_ptr<vpz::Vpz> file(
            new vpz::Vpz(pack.getExpFile("smartgardener.vpz")));

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
    //due to approximation
    Ensures(result.rows() >= 101);
    Ensures(result.rows() <= 102);

    //at the maximum of population of ladybirds
    EnsuresApproximatelyEqual(result.getDouble(3,38), 146.5673122, 10e-5);
    EnsuresApproximatelyEqual(result.getDouble(4,38), 1748.3006172, 10e-5);

    //at the end
    EnsuresApproximatelyEqual(result.getDouble(3,100), 15.913027015, 10e-5);
    EnsuresApproximatelyEqual(result.getDouble(4,100), 122.1797197, 10e-5);

}

int main()
{
    F fixture;
    test_qss4();

    return unit_test::report_errors();
}
