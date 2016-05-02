/**
 * @file vle.extension.differential-equation_test/test/test_QSS2_perturbation.cpp
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
 *  Test of perturbation
 *  with X set to 0 at time 4.3545, Y should converge to 0
 ******************/
BOOST_AUTO_TEST_CASE(test_QSS2_PerturbLotkaVolterra)
{
    auto ctx = vu::make_context();
    std::cout << "  test_QSS2_PerturbLotkaVolterra " << std::endl;
    vle::utils::Package pack(ctx, "vle.extension.differential-equation_test");
    std::unique_ptr<vz::Vpz> vpz(new vz::Vpz(pack.getExpFile("PerturbLotkaVolterra.vpz",
            vle::utils::PKG_BINARY)));

    ttconfOutputPlugins(*vpz);


    std::vector<std::string> conds;
    conds.push_back("condQSS2");
    conds.push_back("condLV");
    ttattachConditions(*vpz,conds,"LotkaVolterra");

    //simulation
    vm::Error error;
    vm::Simulation sim(ctx, vm::LOG_NONE, vm::SIMULATION_NONE, NULL);
    std::unique_ptr<va::Map> out =
              sim.run(std::move(vpz), &error);


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

    //check X,Y at 4.356
    BOOST_REQUIRE_CLOSE(view.getDouble(colX,4356) + 1, 1, 10e-5);
    BOOST_REQUIRE_CLOSE(view.getDouble(colY,4356),
                        6.9326790251826349, 10e-4);//not sure
    //previous 6.97265225316311

    //check X,Y line at 15
    BOOST_REQUIRE_CLOSE(view.getDouble(colX,15000) + 1, 1, 10e-5);
    BOOST_REQUIRE_CLOSE(view.getDouble(colY,15000) + 1,
                        1, 10e-2);//not precise


}

/******************
 *  Test of perturbation
 *  with X set to 0 at time 4.3545, Y should converge to 0.
 *  Results are expected to be the same as test_QSS2_PerturbLotkaVolterra
 ******************/
BOOST_AUTO_TEST_CASE(test_QSS2_PerturbLotkaVolterraXY)
{
    auto ctx = vu::make_context();
    std::cout << "  test_QSS2_PerturbLotkaVolterraXY " << std::endl;
    vle::utils::Package pack(ctx, "vle.extension.differential-equation_test");
    std::unique_ptr<vz::Vpz> vpz(new vz::Vpz(pack.getExpFile("PerturbLotkaVolterraXY.vpz",
            vle::utils::PKG_BINARY)));

    ttconfOutputPlugins(*vpz);


    std::vector<std::string> conds;
    conds.push_back("condQSS2_X");
    conds.push_back("condLV_X");
    ttattachConditions(*vpz,conds,"LotkaVolterraX");

    conds.clear();
    conds.push_back("condQSS2_Y");
    conds.push_back("condLV_Y");
    ttattachConditions(*vpz,conds,"LotkaVolterraY");

    //simulation
    vm::Error error;
    vm::Simulation sim(ctx, vm::LOG_NONE, vm::SIMULATION_NONE, NULL);
    std::unique_ptr<va::Map> out =
            sim.run(std::move(vpz), &error);

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
    int colX = ttgetColumnFromView(view, "Top model:LotkaVolterraX", "X");
    int colY = ttgetColumnFromView(view, "Top model:LotkaVolterraY", "Y");

    //check X,Y at 4.355
    BOOST_REQUIRE_CLOSE(view.getDouble(colX,4356) + 1, 1, 10e-5);
    BOOST_REQUIRE_CLOSE(view.getDouble(colY,4356),
                        6.9326790251826349, 10e-4);//not sure
    //previous : 6.97265225316311

    //check X,Y line at 14.999
    BOOST_REQUIRE_CLOSE(view.getDouble(colX,15000) + 1, 1, 10e-5);
    BOOST_REQUIRE_CLOSE(view.getDouble(colY,15000) + 1,
                        1, 10e-2);//not precise
}
