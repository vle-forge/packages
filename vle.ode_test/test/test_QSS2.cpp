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
 *  unit test of QSS2 on lotka_volterra model,
 *  based  on powerdevs results
 ******************/
void test_QSS2_LotkaVolterra()
{
    auto ctx = vu::make_context();
    std::cout << "  test_QSS2_LotkaVolterra " << std::endl;
    vle::utils::Package pack(ctx, "vle.ode_test");
    std::unique_ptr<vz::Vpz> vpz(new vz::Vpz(
            pack.getExpFile("LotkaVolterra.vpz", vle::utils::PKG_BINARY)));

    ttconfOutputPlugins(*vpz);

    std::vector<std::string> conds;
    conds.push_back("condQSS2");
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
    int colX = ttgetColumnFromView(view,"Top model:LotkaVolterra", "X");
    int colY = ttgetColumnFromView(view,"Top model:LotkaVolterra", "Y");


    //check X,Y line 15000
    EnsuresApproximatelyEqual(view.getDouble(colX,15001), 0.69363324986147468, 10e-5);
    //previous 0.696088281553273

    EnsuresApproximatelyEqual(view.getDouble(colY,15001), 0.07761934820509947, 10e-5);
    //previous 0.0774536442779648
}

/******************
 *  unit test of QSS2 on lotka_volterra model,
 *  based  on powerdevs results
 *
 *  note: results have to be the same as test_QSS2_LotkaVolterra
 ******************/
void test_QSS2_LotkaVolterraXY()
{
    auto ctx = vu::make_context();
    std::cout << "  test_QSS2_LotkaVolterraXY " << std::endl;
    vle::utils::Package pack(ctx, "vle.ode_test");
    std::unique_ptr<vz::Vpz> vpz(new vz::Vpz(pack.getExpFile("LotkaVolterraXY.vpz", vle::utils::PKG_BINARY)));
    ttconfOutputPlugins(*vpz);

    {
        std::vector<std::string> conds;
        conds.push_back("condLV");
        conds.push_back("condQSS2");
        conds.push_back("condQSS2_X");
        ttattachConditions(*vpz,conds,"LotkaVolterraX");
    }
    {
        std::vector<std::string> conds;
        conds.push_back("condLV");
        conds.push_back("condQSS2");
        conds.push_back("condQSS2_Y");
        ttattachConditions(*vpz,conds,"LotkaVolterraY");
    }

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

    //check X,Y line 15000
    EnsuresApproximatelyEqual(view.getDouble(colX,15001), 0.69363324986147468, 10e-5);
    //previous 0.696088281553273

    EnsuresApproximatelyEqual(view.getDouble(colY,15001), 0.07761934820509947, 10e-5);
    //previous 0.0774536442779648
}

/******************
 *  Unit test based on powerdevs
 ******************/
void test_QSS2_Seir()
{
    auto ctx = vu::make_context();
    std::cout << "  test_QSS2_Seir " << std::endl;
    vle::utils::Package pack(ctx, "vle.ode_test");
    std::unique_ptr<vz::Vpz> vpz(new vz::Vpz(pack.getExpFile("Seir.vpz", vle::utils::PKG_BINARY)));

    ttconfOutputPlugins(*vpz);

    std::vector<std::string> conds;
    conds.push_back("condQSS2");
    conds.push_back("condSeir");
    ttattachConditions(*vpz,conds,"Seir");

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
    EnsuresEqual(view.columns(),5);
    //note: the number of rows depend on the averaging of sum of 0.01
    Ensures(view.rows() <= 1503);
    Ensures(view.rows() >= 1502);


    //gets S,E,I,R
    int colS = ttgetColumnFromView(view, "Top model:Seir", "S");
    int colE = ttgetColumnFromView(view, "Top model:Seir", "E");
    int colI = ttgetColumnFromView(view, "Top model:Seir", "I");
    int colR = ttgetColumnFromView(view, "Top model:Seir", "R");

    //check S,E,I,R line 1500
    EnsuresApproximatelyEqual(view.getDouble(colS,1501), 0.636836529382071, 10e-5);
    EnsuresApproximatelyEqual(view.getDouble(colE,1501), 0.636051925651117, 10e-5);
    EnsuresApproximatelyEqual(view.getDouble(colI,1501), 2.95405622447345, 10e-5);
    EnsuresApproximatelyEqual(view.getDouble(colR,1501), 6.77305532047774, 10e-5);

}

/******************
 *  Unit test based on powerdevs
 *
 *  Note: we test that the version of Seir based on four
 *  differential_equation (SeirXY.vpz) gets the same results
 *  in the case of QSS2 integration
 ******************/
void test_QSS2_SeirXY()
{
    auto ctx = vu::make_context();
    std::cout << "  test_QSS2_SeirXY " << std::endl;
    vle::utils::Package pack(ctx, "vle.ode_test");
    std::unique_ptr<vz::Vpz> vpz(new vz::Vpz(pack.getExpFile("SeirXY.vpz", vle::utils::PKG_BINARY)));

    ttconfOutputPlugins(*vpz);

    {
        std::vector<std::string> conds;
        conds.push_back("condSeir");
        conds.push_back("condQSS2");
        conds.push_back("condQSS2_S");
        ttattachConditions(*vpz,conds,"Sm");
    }
    {
        std::vector<std::string> conds;
        conds.push_back("condSeir");
        conds.push_back("condQSS2");
        conds.push_back("condQSS2_E");
        ttattachConditions(*vpz,conds,"Em");
    }
    {
        std::vector<std::string> conds;
        conds.push_back("condSeir");
        conds.push_back("condQSS2");
        conds.push_back("condQSS2_I");
        ttattachConditions(*vpz,conds,"Im");
    }
    {
        std::vector<std::string> conds;
        conds.push_back("condSeir");
        conds.push_back("condQSS2");
        conds.push_back("condQSS2_R");
        ttattachConditions(*vpz,conds,"Rm");
    }

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
    EnsuresEqual(view.columns(),5);
    //note: the number of rows depend on the averaging of sum of 0.01
    //The two last obs theoretically should be 14.99 and 15.0
    //but one can find eg. : 14.999999999 and 15.0 with one more obs
    Ensures(view.rows() <= 1503);
    Ensures(view.rows() >= 1502);

    //gets S,E,I,R
    int colS = ttgetColumnFromView(view, "Top model:Sm", "S");
    int colE = ttgetColumnFromView(view, "Top model:Em", "E");
    int colI = ttgetColumnFromView(view, "Top model:Im", "I");
    int colR = ttgetColumnFromView(view, "Top model:Rm", "R");

    //check S,E,I,R line 1500
    EnsuresApproximatelyEqual(view.getDouble(colS,1501), 0.636836529382071, 10e-5);
    EnsuresApproximatelyEqual(view.getDouble(colE,1501), 0.636051925651117, 10e-5);
    EnsuresApproximatelyEqual(view.getDouble(colI,1501), 2.95405622447345, 10e-5);
    EnsuresApproximatelyEqual(view.getDouble(colR,1501), 6.77305532047774, 10e-5);
}

int main()
{
    F fixture;
    test_QSS2_LotkaVolterra();
    test_QSS2_LotkaVolterraXY();
    test_QSS2_Seir();
    test_QSS2_SeirXY();

    return unit_test::report_errors();
}


