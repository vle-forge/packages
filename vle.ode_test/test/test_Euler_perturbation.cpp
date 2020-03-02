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
 *  Test of perturbation
 *  with X set to 0 at time 4.3545, Y should converge to 0
 ******************/
void test_Euler_PerturbLotkaVolterra()
{
    auto ctx = vu::make_context();
    std::cout << "  test_Euler_PerturbLotkaVolterra " << std::endl;
    vle::utils::Package pack(ctx, "vle.ode_test");
    std::unique_ptr<vz::Vpz> vpz(new vz::Vpz(pack.getExpFile("PerturbLotkaVolterra.vpz",
            vle::utils::PKG_BINARY)));

    ttconfOutputPlugins(*vpz);

    std::vector<std::string> conds;
    conds.push_back("condEuler");
    conds.push_back("condLV");
    ttattachConditions(*vpz,conds,"LotkaVolterra");

    //simulation
    vm::Error error;
    vm::Simulation sim(ctx, vm::SIMULATION_NONE, std::chrono::milliseconds(0));
    std::unique_ptr<va::Map> out = sim.run(std::move(vpz), &error);

    //checks that simulation has succeeded
    EnsuresEqual(error.code, 0);
    //checks the number of views
    EnsuresEqual(out->size(),1);
    //checks the selected view
    const va::Matrix& view = out->getMatrix("view");
    EnsuresEqual(view.columns(),3);
    //note: the number of rows depend on the averaging of sum of 0.01
    Ensures(view.rows() <= 15003);
    Ensures(view.rows() >= 15002);

    //gets X,Y
    int colX = ttgetColumnFromView(view, "Top model:LotkaVolterra", "X");

    int colY = ttgetColumnFromView(view, "Top model:LotkaVolterra", "Y");

    //check X,Y at 4.357
    EnsuresApproximatelyEqual(view.getDouble(colX,4357) + 1, 1, 10e-5);
    EnsuresApproximatelyEqual(view.getDouble(colY,4357),
                        6.62948339477899, 10e-4);//not sure

    //check X,Y line at 15
    EnsuresApproximatelyEqual(view.getDouble(colX,15000) + 1, 1, 10e-5);
    EnsuresApproximatelyEqual(view.getDouble(colY, 15000) + 1, 1, 10e-5);

}


/******************
 *  Test of perturbation
 *  with X set to 0 at time 4.3545, Y should converge to 0
 *  same results as test_Euler_PerturbLotkaVolterra are expected
 ******************/
void test_Euler_PerturbLotkaVolterraXY()
{
    auto ctx = vu::make_context();
    std::cout << "  test_Euler_PerturbLotkaVolterraXY " << std::endl;
    vle::utils::Package pack(ctx, "vle.ode_test");
    std::unique_ptr<vz::Vpz> vpz(new vz::Vpz(pack.getExpFile("PerturbLotkaVolterraXY.vpz",
            vle::utils::PKG_BINARY)));

    ttconfOutputPlugins(*vpz);

    std::vector<std::string> conds;
    conds.push_back("condEuler");
    conds.push_back("condLV");
    ttattachConditions(*vpz,conds,"LotkaVolterraX");


    conds.clear();
    conds.push_back("condEuler");
    conds.push_back("condLV");
    ttattachConditions(*vpz,conds,"LotkaVolterraY");

    //simulation
    vm::Error error;
    vm::Simulation sim(ctx, vm::SIMULATION_NONE, std::chrono::milliseconds(0));
    std::unique_ptr<va::Map> out = sim.run(std::move(vpz), &error);


    //checks that simulation has succeeded
    EnsuresEqual(error.code, 0);
    //checks the number of views
    EnsuresEqual(out->size(),1);
    //checks the selected view
    const va::Matrix& view = out->getMatrix("view");
    EnsuresEqual(view.columns(),3);
    //note: the number of rows depend on the averaging of sum of 0.01
    Ensures(view.rows() <= 15003);
    Ensures(view.rows() >= 15002);

    //gets X,Y
    int colX = ttgetColumnFromView(view, "Top model:LotkaVolterraX", "X");

    int colY = ttgetColumnFromView(view, "Top model:LotkaVolterraY", "Y");

    //check X,Y at 4.357
    EnsuresApproximatelyEqual(view.getDouble(colX,4357) + 1, 1, 10e-5);
    EnsuresApproximatelyEqual(view.getDouble(colY, 4357),
                        6.62948339477899, 10e-4);//not sure

    //check X,Y line at 15
    EnsuresApproximatelyEqual(view.getDouble(colX, 15000) + 1, 1, 10e-5);
    EnsuresApproximatelyEqual(view.getDouble(colY, 15000) + 1, 1, 10e-5);
}

int main()
{
    F fixture;
    test_Euler_PerturbLotkaVolterra();
    test_Euler_PerturbLotkaVolterraXY();

    return unit_test::report_errors();
}
