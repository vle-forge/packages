/**
 * @file vle.extension.differential-equation_test/test/test_RK4.cpp
 * @author The VLE Development Team
 */

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


#include "test_common.hpp"

/******************
 *  Unit test based on deSolve R package
 *  see R/check.R function test_RK4_LotkaVolterra
 ******************/
BOOST_AUTO_TEST_CASE(test_RK4_LotkaVolterra)
{

    auto ctx = vu::make_context();
    std::cout << "  test_RK4_LotkaVolterra " << std::endl;
    vle::utils::Package pack(ctx, "vle.extension.differential-equation_test");
    std::unique_ptr<vz::Vpz> vpz(new vz::Vpz(
            pack.getExpFile("LotkaVolterra.vpz", vle::utils::PKG_BINARY)));


    ttconfOutputPlugins(*vpz);

    std::vector<std::string> conds;
    conds.push_back("condRK4");
    conds.push_back("condLV");
    ttattachConditions(*vpz,conds,"LotkaVolterra");

    //simulation
    vm::Error error;
    vm::Simulation sim(ctx, vm::LOG_NONE, vm::SIMULATION_NONE, NULL);
    std::unique_ptr<va::Map> out = sim.run(std::move(vpz), &error);


    //checks that simulation has succeeded
    BOOST_REQUIRE_EQUAL(error.code, 0);
    //checks the number of views
    BOOST_REQUIRE_EQUAL(out->size(),1);
    //checks the selected view
    const va::Matrix& view = out->getMatrix("view");
    BOOST_REQUIRE_EQUAL(view.columns(),3);
    //note: the number of rows depend on the averaging of sum of 0.01
    BOOST_REQUIRE(view.rows() <= 15003);
    BOOST_REQUIRE(view.rows() >= 15002);

    //gets X,Y
    int colX = ttgetColumnFromView(view, "Top model:LotkaVolterra", "X");
    int colY = ttgetColumnFromView(view, "Top model:LotkaVolterra", "Y");

    //check X,Y line 10
    BOOST_REQUIRE_CLOSE(view.getDouble(colX,10),
                        9.676110, 10e-5);
    BOOST_REQUIRE_CLOSE(view.getDouble(colY,10),
                        5.317448, 10e-5);

    //check X,Y line 30
    BOOST_REQUIRE_CLOSE(view.getDouble(colX,30),
                        8.901151, 10e-5);
    BOOST_REQUIRE_CLOSE(view.getDouble(colY,30),
                        6.030798, 10e-5);

    //check X,Y line 15000
    BOOST_REQUIRE_CLOSE(view.getDouble(colX,15000),
                        0.7127354, 10e-5);
    BOOST_REQUIRE_CLOSE(view.getDouble(colY,15000),
                        0.07558043, 10e-5);
}

/******************
 *  Unit test based on deSolve R package
 *  see R/check.R function test_RK4_Seir
 ******************/
BOOST_AUTO_TEST_CASE(test_RK4_Seir)
{

    auto ctx = vu::make_context();
    std::cout << "  test_RK4_Seir " << std::endl;
    vle::utils::Package pack(ctx, "vle.extension.differential-equation_test");
    std::unique_ptr<vz::Vpz> vpz(new vz::Vpz(
            pack.getExpFile("Seir.vpz", vle::utils::PKG_BINARY)));

    ttconfOutputPlugins(*vpz);

    std::vector<std::string> conds;
    conds.push_back("condRK4");
    conds.push_back("condSeir");
    ttattachConditions(*vpz,conds,"Seir");

    //simulation
    vm::Error error;
    vm::Simulation sim(ctx, vm::LOG_NONE, vm::SIMULATION_NONE, NULL);
    std::unique_ptr<va::Map> out = sim.run(std::move(vpz), &error);

    //checks that simulation has succeeded
    BOOST_REQUIRE_EQUAL(error.code, 0);
    //checks the number of views
    BOOST_REQUIRE_EQUAL(out->size(),1);
    //checks the selected view
    const va::Matrix& view = out->getMatrix("view");
    BOOST_REQUIRE_EQUAL(view.columns(),5);
    //note: the number of rows depend on the averaging of sum of 0.01
    BOOST_REQUIRE(view.rows() <= 1503);
    BOOST_REQUIRE(view.rows() >= 1502);


    //gets S,E,I,R
    int colS = ttgetColumnFromView(view, "Top model:Seir", "S");
    int colE = ttgetColumnFromView(view, "Top model:Seir", "E");
    int colI = ttgetColumnFromView(view, "Top model:Seir", "I");
    int colR = ttgetColumnFromView(view, "Top model:Seir", "R");

    //check S,E,I,R line 1500, TODO 10-3 diff with deSolve
    BOOST_REQUIRE_CLOSE(view.getDouble(colS,1501),
                        0.635933347641559, 10e-5);
    BOOST_REQUIRE_CLOSE(view.getDouble(colE,1501),
                        0.654326066542035, 10e-5);
    BOOST_REQUIRE_CLOSE(view.getDouble(colI,1501),
                        2.97469330853422, 10e-5);
    BOOST_REQUIRE_CLOSE(view.getDouble(colR,1501),
                        6.73504727728219, 10e-5);
}
