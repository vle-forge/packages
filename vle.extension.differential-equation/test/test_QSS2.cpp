/**
 * @file vle.extension.differential-equation/test/test_QSS2.cpp
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
 *  unit test of QSS2 on lotka_volterra model,
 *  based  on powerdevs results
 ******************/
BOOST_AUTO_TEST_CASE(test_QSS2_LotkaVolterra)
{
    std::cout << "  test_QSS2_LotkaVolterra " << std::endl;
    vle::utils::Package pack("vle.extension.differential-equation");
    vz::Vpz vpz(pack.getExpFile("LotkaVolterra.vpz", vle::utils::PKG_BINARY));

    ttconfOutputPlugins(vpz);

    std::vector<std::string> conds;
    conds.push_back("condQSS2");
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


    //check X,Y line 15000
    BOOST_REQUIRE_CLOSE(va::toDouble(colX[15001]),
                        0.69363324986147468, 10e-5);
    //previous 0.696088281553273

    BOOST_REQUIRE_CLOSE(va::toDouble(colY[15001]),
                        0.07761934820509947, 10e-5);
    //previous 0.0774536442779648
}

/******************
 *  unit test of QSS2 on lotka_volterra model,
 *  based  on powerdevs results
 *
 *  note: results have to be the same as test_QSS2_LotkaVolterra
 ******************/
BOOST_AUTO_TEST_CASE(test_QSS2_LotkaVolterraXY)
{
    std::cout << "  test_QSS2_LotkaVolterraXY " << std::endl;
    vle::utils::Package pack("vle.extension.differential-equation");
    vz::Vpz vpz(pack.getExpFile("LotkaVolterraXY.vpz", vle::utils::PKG_BINARY));
    ttconfOutputPlugins(vpz);


    std::vector<std::string> conds;
    conds.push_back("condQSS2_X");
    conds.push_back("condLV_X");
    ttattachConditions(vpz,conds,"LotkaVolterraX");

    conds.clear();
    conds.push_back("condQSS2_Y");
    conds.push_back("condLV_Y");
    ttattachConditions(vpz,conds,"LotkaVolterraY");

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
                                                   "Top model:LotkaVolterraX", "X");
    va::ConstVectorView colY = ttgetColumnFromView(view,
                                                   "Top model:LotkaVolterraY", "Y");

    //check X,Y line 15000
    BOOST_REQUIRE_CLOSE(va::toDouble(colX[15001]),
                        0.69363324986147468, 10e-5);
    //previous 0.696088281553273

    BOOST_REQUIRE_CLOSE(va::toDouble(colY[15001]),
                        0.07761934820509947, 10e-5);
    //previous 0.0774536442779648
}

/******************
 *  Unit test based on powerdevs
 ******************/
BOOST_AUTO_TEST_CASE(test_QSS2_Seir)
{
    std::cout << "  test_QSS2_Seir " << std::endl;
    vle::utils::Package pack("vle.extension.differential-equation");
    vz::Vpz vpz(pack.getExpFile("Seir.vpz", vle::utils::PKG_BINARY));

    ttconfOutputPlugins(vpz);

    std::vector<std::string> conds;
    conds.push_back("condQSS2");
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

    //check S,E,I,R line 1500
    BOOST_REQUIRE_CLOSE(va::toDouble(colS[1501]),
                        0.636836529382071, 10e-5);
    BOOST_REQUIRE_CLOSE(va::toDouble(colE[1501]),
                        0.636051925651117, 10e-5);
    BOOST_REQUIRE_CLOSE(va::toDouble(colI[1501]),
                        2.95405622447345, 10e-5);
    BOOST_REQUIRE_CLOSE(va::toDouble(colR[1501]),
                        6.77305532047774, 10e-5);

}

/******************
 *  Unit test based on powerdevs
 *
 *  Note: we test that the version of Seir based on four
 *  differential_equation (SeirXY.vpz) gets the same results
 *  in the case of QSS2 integration
 ******************/
BOOST_AUTO_TEST_CASE(test_QSS2_SeirXY)
{
    std::cout << "  test_QSS2_SeirXY " << std::endl;
    vle::utils::Package pack("vle.extension.differential-equation");
    vz::Vpz vpz(pack.getExpFile("SeirXY.vpz", vle::utils::PKG_BINARY));

    ttconfOutputPlugins(vpz);

    std::vector<std::string> conds;
    conds.push_back("condQSS2");
    conds.push_back("condSeir");
    conds.push_back("condSm");
    ttattachConditions(vpz,conds,"Sm");

    conds.clear();
    conds.push_back("condQSS2");
    conds.push_back("condSeir");
    conds.push_back("condEm");
    ttattachConditions(vpz,conds,"Em");

    conds.clear();
    conds.push_back("condQSS2");
    conds.push_back("condSeir");
    conds.push_back("condIm");
    ttattachConditions(vpz,conds,"Im");

    conds.clear();
    conds.push_back("condQSS2");
    conds.push_back("condSeir");
    conds.push_back("condRm");
    ttattachConditions(vpz,conds,"Rm");

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
                                                   "Top model:Sm", "S");
    va::ConstVectorView colE = ttgetColumnFromView(view,
                                                   "Top model:Em", "E");
    va::ConstVectorView colI = ttgetColumnFromView(view,
                                                   "Top model:Im", "I");
    va::ConstVectorView colR = ttgetColumnFromView(view,
                                                   "Top model:Rm", "R");

    //check S,E,I,R line 1500
    BOOST_REQUIRE_CLOSE(va::toDouble(colS[1501]),
                        0.636836529382071, 10e-5);
    BOOST_REQUIRE_CLOSE(va::toDouble(colE[1501]),
                        0.636051925651117, 10e-5);
    BOOST_REQUIRE_CLOSE(va::toDouble(colI[1501]),
                        2.95405622447345, 10e-5);
    BOOST_REQUIRE_CLOSE(va::toDouble(colR[1501]),
                        6.77305532047774, 10e-5);
}



