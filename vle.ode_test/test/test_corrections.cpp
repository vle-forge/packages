/*
 * VLE Environment - the multimodeling and simulation environment
 * This file is a part of the VLE environment (http://vle.univ-littoral.fr)
 * Copyright (C) 2003 - 2009 The VLE Development Team
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

#include "test_common.hpp"


/******************
 * Bug correction, update of external variables were not taken into
 * account if not synchronized with a compute
 ******************/
void test_ExtUpLV()
{
    auto ctx = vu::make_context();
    std::cout << "  test_ExtUpLV " << std::endl;
    vle::utils::Package pack(ctx, "vle.ode_test");
    std::unique_ptr<vz::Vpz> vpz(new vz::Vpz(pack.getExpFile("ExtUpLV.vpz", vle::utils::PKG_BINARY)));


    ttconfOutputPlugins(*vpz);

    //simulation
    vm::Error error;
    vm::Simulation sim(ctx, vm::LOG_NONE, vm::SIMULATION_NONE,
            std::chrono::milliseconds(0), &std::cout);
    std::unique_ptr<va::Map> out = sim.run(std::move(vpz), &error);


    //checks that simulation has succeeded
    EnsuresEqual(error.code, 0);
    //checks the number of views
    EnsuresEqual(out->size(),1);
    //checks the selected view
    const va::Matrix& view = out->getMatrix("view");
    EnsuresEqual(view.columns(),3);
    //note: the number of rows depend on the averaging of sum of 0.01
    Ensures(view.rows() <= 52);
    Ensures(view.rows() >= 51);

    //gets X,Y
    int colX = ttgetColumnFromView(view, "Top model:LotkaVolterraY", "X");

    //check X at = 0.400 and t=0.41
    EnsuresApproximatelyEqual(view.getDouble(colX,41), 10, 10e-5);
    EnsuresApproximatelyEqual(view.getDouble(colX,42), 1.0, 10e-5);
}


/******************
 * test output_period parameter
 ******************/
void test_OutputPeriod()
{
    auto ctx = vu::make_context();
    vle::utils::Package pack(ctx, "vle.ode_test");
    std::unique_ptr<vz::Vpz> vpz(new vz::Vpz(pack.getExpFile("LotkaVolterraOutputPeriod.vpz")));

    ttconfOutputPlugins(*vpz);

    //simulation
    vm::Error error;
    vm::Simulation sim(ctx, vm::LOG_NONE, vm::SIMULATION_NONE,
            std::chrono::milliseconds(0), &std::cout);
    std::unique_ptr<va::Map> out = sim.run(std::move(vpz), &error);


    //checks that simulation has succeeded
    EnsuresEqual(error.code, 0);
    //checks the number of views
    EnsuresEqual(out->size(),1);
    //checks the selected view
    const va::Matrix& view = out->getMatrix("view");
    EnsuresEqual(view.columns(),4);
    EnsuresEqual(view.rows(),2);

    //gets nbExtEvents
    int col = ttgetColumnFromView(view, "Top model:Counter", "nbExtEvents");
    //check X at = 0.400 and t=0.41
    EnsuresApproximatelyEqual(view.getDouble(col,1), 150, 10e-5);

}

int main()
{
    F fixture;
    test_ExtUpLV();
    test_OutputPeriod();

    return unit_test::report_errors();
}
