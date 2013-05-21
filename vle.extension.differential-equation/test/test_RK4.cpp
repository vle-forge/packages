/**
 * @file vle.extension.differential-equation/test/test_RK4.cpp
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
    std::cout << "  test_RK4_LotkaVolterra " << std::endl;
    vle::utils::Package pack("vle.extension.differential-equation");
    vz::Vpz vpz(pack.getExpFile("LotkaVolterra.vpz", vle::utils::PKG_BINARY));


    ttconfOutputPlugins(vpz);

    std::vector<std::string> conds;
    conds.push_back("condRK4");
    conds.push_back("condLV");
    ttattachConditions(vpz,conds,"LotkaVolterra");

    //simulation
    vu::ModuleManager man;
    vm::Error error;
    vm::Simulation sim(vm::LOG_NONE, vm::SIMULATION_NONE, NULL);
    va::Map *out = sim.run(new vz::Vpz(vpz), man, &error);


    //checks that simulation has succeeded
    BOOST_REQUIRE_EQUAL(error.code, 0);
    //checks the number of views
    BOOST_REQUIRE_EQUAL(out->size(),1);
    //checks the selected view
    const va::Matrix& view = out->getMatrix("view");
    BOOST_REQUIRE_EQUAL(view.columns(),3);
    BOOST_REQUIRE_EQUAL(view.column(0).size(),15002);

    //gets X,Y
    va::ConstVectorView colX = ttgetColumnFromView(view,
                                                   "Top model:LotkaVolterra", "X");
    va::ConstVectorView colY = ttgetColumnFromView(view,
                                                   "Top model:LotkaVolterra", "Y");

    //check X,Y line 10
    BOOST_REQUIRE_CLOSE(va::toDouble(colX[10]),
                        9.676110, 10e-5);
    BOOST_REQUIRE_CLOSE(va::toDouble(colY[10]),
                        5.317448, 10e-5);

    //check X,Y line 30
    BOOST_REQUIRE_CLOSE(va::toDouble(colX[30]),
                        8.901151, 10e-5);
    BOOST_REQUIRE_CLOSE(va::toDouble(colY[30]),
                        6.030798, 10e-5);

    //check X,Y line 15000
    BOOST_REQUIRE_CLOSE(va::toDouble(colX[15000]),
                        0.7127354, 10e-5);
    BOOST_REQUIRE_CLOSE(va::toDouble(colY[15000]),
                        0.07558043, 10e-5);
}

/******************
 *  Unit test based on deSolve R package
 *  see R/check.R function test_RK4_Seir
 ******************/
BOOST_AUTO_TEST_CASE(test_RK4_Seir)
{
    std::cout << "  test_RK4_Seir " << std::endl;
    vle::utils::Package pack("vle.extension.differential-equation");
    vz::Vpz vpz(pack.getExpFile("Seir.vpz", vle::utils::PKG_BINARY));

    ttconfOutputPlugins(vpz);

    std::vector<std::string> conds;
    conds.push_back("condRK4");
    conds.push_back("condSeir");
    ttattachConditions(vpz,conds,"Seir");

    //simulation
    vu::ModuleManager man;
    vm::Error error;
    vm::Simulation sim(vm::LOG_NONE, vm::SIMULATION_NONE, NULL);
    va::Map *out = sim.run(new vz::Vpz(vpz), man, &error);

    //checks that simulation has succeeded
    BOOST_REQUIRE_EQUAL(error.code, 0);
    //checks the number of views
    BOOST_REQUIRE_EQUAL(out->size(),1);
    //checks the selected view
    const va::Matrix& view = out->getMatrix("view");
    BOOST_REQUIRE_EQUAL(view.columns(),5);
    BOOST_REQUIRE_EQUAL(view.column(0).size(),1502);


    //gets S,E,I,R
    va::ConstVectorView colS = ttgetColumnFromView(view,
                                                   "Top model:Seir", "S");
    va::ConstVectorView colE = ttgetColumnFromView(view,
                                                   "Top model:Seir", "E");
    va::ConstVectorView colI = ttgetColumnFromView(view,
                                                   "Top model:Seir", "I");
    va::ConstVectorView colR = ttgetColumnFromView(view,
                                                   "Top model:Seir", "R");

    //check S,E,I,R line 1500, TODO 10-3 diff with deSolve
    BOOST_REQUIRE_CLOSE(va::toDouble(colS[1501]),
                        0.635933347641559, 10e-5);
    BOOST_REQUIRE_CLOSE(va::toDouble(colE[1501]),
                        0.654326066542035, 10e-5);
    BOOST_REQUIRE_CLOSE(va::toDouble(colI[1501]),
                        2.97469330853422, 10e-5);
    BOOST_REQUIRE_CLOSE(va::toDouble(colR[1501]),
                        6.73504727728219, 10e-5);
}
