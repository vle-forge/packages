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
 *  Unit test based on deSolve R package
 *  see R/check.R function test_Euler_LotkaVolterra
 ******************/
void test_Euler_LotkaVolterra()
{
    auto ctx = vu::make_context();
    std::cout << "  test_Euler_LotkaVolterra " << std::endl;
    vle::utils::Package pack(ctx, "vle.ode_test");
    std::unique_ptr<vz::Vpz> vpz(new vz::Vpz(
            pack.getExpFile("LotkaVolterra.vpz", vle::utils::PKG_BINARY)));

    //set all output plugin to storage
    vz::Outputs::iterator itb =
        vpz->project().experiment().views().outputs().begin();
    vz::Outputs::iterator ite =
        vpz->project().experiment().views().outputs().end();
    for(;itb!=ite;itb++) {
        std::unique_ptr<va::Map> configOutput(new va::Map());
        configOutput->addInt("rows",10000);
        configOutput->addInt("inc_rows",10000);
        configOutput->addString("header","top");
        vz::Output& output = itb->second;
        output.setStream("", "storage", "vle.output");
        output.setData(std::move(configOutput));
    }


    //attach condition condEuler and condLV
    std::vector<std::string> conds;
    conds.push_back("condEuler");
    conds.push_back("condLV");
    vz::Model& vpz_mod = vpz->project().model();
    vz::BaseModel* mdl = vpz_mod.node()->findModelFromPath("LotkaVolterra");
    Ensures(mdl != 0);
    vz::AtomicModel* atomg = mdl->toAtomic();
    atomg->setConditions(conds);
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
    Ensures(view.rows() <= 15003);
    Ensures(view.rows() >= 15002);

    //gets X,Y
    int colX = ttgetColumnFromView(view, "Top model:LotkaVolterra", "X");

    int colY = ttgetColumnFromView(view, "Top model:LotkaVolterra", "Y");

    //check X,Y line 10
    EnsuresApproximatelyEqual(view.getDouble(colX,10), 9.677077, 10e-5);
    EnsuresApproximatelyEqual(view.getDouble(colY,10), 5.317209, 10e-5);

    //check X,Y line 30
    EnsuresApproximatelyEqual(view.getDouble(colX,30), 8.903716, 10e-5);
    EnsuresApproximatelyEqual(view.getDouble(colY,30), 6.030680, 10e-5);

    //check X,Y line 15000
    EnsuresApproximatelyEqual(view.getDouble(colX,15000), 0.5528446, 10e-5);
    EnsuresApproximatelyEqual(view.getDouble(colY,15000), 0.09330513, 10e-5);


}

/******************
 *  Unit test based on deSolve R package
 *  see R/check.R function test_Euler_LotkaVolterra
 *
 *  Note: in cas of Euler integration results are the same then for
 *  a single model containing all equations
 ******************/
void test_Euler_LotkaVolterraXY()
{
    auto ctx = vu::make_context();
    std::cout << "  test_Euler_LotkaVolterraXY " << std::endl;
    vle::utils::Package pack(ctx, "vle.ode_test");
    std::unique_ptr<vz::Vpz> vpz(new vz::Vpz(pack.getExpFile("LotkaVolterraXY.vpz", vle::utils::PKG_BINARY)));

    //set all output plugin to storage
    vz::Outputs::iterator itb =
        vpz->project().experiment().views().outputs().begin();
    vz::Outputs::iterator ite =
        vpz->project().experiment().views().outputs().end();
    for(;itb!=ite;itb++) {
        std::unique_ptr<va::Map> configOutput(new va::Map());
        configOutput->addInt("rows",10000);
        configOutput->addInt("inc_rows",10000);
        configOutput->addString("header","top");
        vz::Output& output = itb->second;
        output.setStream("", "storage", "vle.output");
        output.setData(std::move(configOutput));
    }

    //attach condition condEuler and condLV to atomics models
    std::vector<std::string> conds;

    {
        conds.clear();
        conds.push_back("condEuler");
        conds.push_back("condLV");
        vz::Model& vpz_mod = vpz->project().model();
        vz::BaseModel* mdl = vpz_mod.node()->findModelFromPath("LotkaVolterraX");
        Ensures(mdl != 0);
        vz::AtomicModel* atomg = mdl->toAtomic();
        atomg->setConditions(conds);
    }
    {
        conds.clear();
        conds.push_back("condEuler");
        conds.push_back("condLV");
        vz::Model& vpz_mod = vpz->project().model();
        vz::BaseModel* mdl = vpz_mod.node()->findModelFromPath("LotkaVolterraY");
        Ensures(mdl != 0);
        vz::AtomicModel* atomg = mdl->toAtomic();
        atomg->setConditions(conds);
    }
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
    Ensures(view.rows() <= 15003);
    Ensures(view.rows() >= 15002);

    //gets X,Y
    int colX = ttgetColumnFromView(view, "Top model:LotkaVolterraX", "X");
    int colY = ttgetColumnFromView(view, "Top model:LotkaVolterraY", "Y");

    //check X,Y line 10
    EnsuresApproximatelyEqual(view.getDouble(colX,10), 9.677077, 10e-5);
    EnsuresApproximatelyEqual(view.getDouble(colY,10), 5.317209, 10e-5);

    //check X,Y line 30
    EnsuresApproximatelyEqual(view.getDouble(colX,30), 8.903716, 10e-5);
    EnsuresApproximatelyEqual(view.getDouble(colY,30), 6.030680, 10e-5);

    //check X,Y line 15000
    EnsuresApproximatelyEqual(view.getDouble(colX,15000), 0.5528446, 10e-5);
    EnsuresApproximatelyEqual(view.getDouble(colY,15000), 0.09330513, 10e-5);
}

/******************
 *  Unit test based on deSolve R package
 *  see R/check.R function test_Euler_Seir
 ******************/
void test_Euler_Seir()
{
    auto ctx = vu::make_context();
    std::cout << "  test_Euler_Seir " << std::endl;
    vle::utils::Package pack(ctx, "vle.ode_test");
    std::unique_ptr<vz::Vpz> vpz(new vz::Vpz(pack.getExpFile("Seir.vpz",
            vle::utils::PKG_BINARY)));

    //set all output plugin to storage
    vz::Outputs::iterator itb =
        vpz->project().experiment().views().outputs().begin();
    vz::Outputs::iterator ite =
        vpz->project().experiment().views().outputs().end();
    for(;itb!=ite;itb++) {
        std::unique_ptr<va::Map> configOutput(new va::Map());
        configOutput->addInt("rows",10000);
        configOutput->addInt("inc_rows",10000);
        configOutput->addString("header","top");
        vz::Output& output = itb->second;
        output.setStream("", "storage", "vle.output");
        output.setData(std::move(configOutput));
    }

    //attach condition condEuler and condLV
    std::vector<std::string> conds;

    conds.push_back("condEuler");
    conds.push_back("condSeir");
    vz::Model& vpz_mod = vpz->project().model();
    vz::BaseModel* mdl = vpz_mod.node()->findModelFromPath("Seir");
    Ensures(mdl != 0);
    vz::AtomicModel* atomg = mdl->toAtomic();
    atomg->setConditions(conds);

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
    EnsuresEqual(view.columns(),5);
    //note: the number of rows depend on the averaging of sum of 0.01
    Ensures(view.rows() <= 1503);
    Ensures(view.rows() >= 1502);

    //gets S,E,I,R
    int colS = ttgetColumnFromView(view, "Top model:Seir", "S");
    int colE = ttgetColumnFromView(view, "Top model:Seir", "E");
    int colI = ttgetColumnFromView(view, "Top model:Seir", "I");
    int colR = ttgetColumnFromView(view, "Top model:Seir", "R");

    //check S,E,I,R line 1501
    EnsuresApproximatelyEqual(view.getDouble(colS,1501), 0.634334231496758, 10e-5);
    EnsuresApproximatelyEqual(view.getDouble(colE,1501), 0.65430775343564, 10e-5);
    EnsuresApproximatelyEqual(view.getDouble(colI,1501), 2.97798653683738, 10e-5);
    EnsuresApproximatelyEqual(view.getDouble(colR,1501), 6.73337147823022, 10e-5);
//    E I R S
//      0.65430775343564;2.97798653683738;6.73337147823022;0.634334231496758

}

/******************
 *  Unit test based on deSolve R package
 *  see R/check.R function test_Euler_Seir
 *
 *  Note: we test that the version of Seir based on four
 *  differential_equation (SeirXY.vpz) gets the same results
 *  in the case of Euler integration
 ******************/
void test_Euler_SeirXY()
{
    auto ctx = vu::make_context();
    std::cout << "  test_Euler_SeirXY " << std::endl;
    vle::utils::Package pack(ctx, "vle.ode_test");
    std::unique_ptr<vz::Vpz> vpz(new vz::Vpz(pack.getExpFile("SeirXY.vpz",
            vle::utils::PKG_BINARY)));

    //set all output plugin to storage
    vz::Outputs::iterator itb =
        vpz->project().experiment().views().outputs().begin();
    vz::Outputs::iterator ite =
        vpz->project().experiment().views().outputs().end();
    for(;itb!=ite;itb++) {
        std::unique_ptr<va::Map> configOutput(new va::Map());
        configOutput->addInt("rows",10000);
        configOutput->addInt("inc_rows",10000);
        configOutput->addString("header","top");
        vz::Output& output = itb->second;
        output.setStream("", "storage", "vle.output");
        output.setData(std::move(configOutput));
    }

    //attach condition condEuler and condLV to atomics models
    std::vector<std::string> conds;

    {
        conds.clear();
        conds.push_back("condEuler");
        conds.push_back("condSeir");
        vz::Model& vpz_mod = vpz->project().model();
        vz::BaseModel* mdl = vpz_mod.node()->findModelFromPath("Sm");
        Ensures(mdl != 0);
        vz::AtomicModel* atomg = mdl->toAtomic();
        atomg->setConditions(conds);
    }
    {
        conds.clear();
        conds.push_back("condEuler");
        conds.push_back("condSeir");
        vz::Model& vpz_mod = vpz->project().model();
        vz::BaseModel* mdl = vpz_mod.node()->findModelFromPath("Em");
        Ensures(mdl != 0);
        vz::AtomicModel* atomg = mdl->toAtomic();
        atomg->setConditions(conds);
    }
    {
        conds.clear();
        conds.push_back("condEuler");
        conds.push_back("condSeir");
        vz::Model& vpz_mod = vpz->project().model();
        vz::BaseModel* mdl = vpz_mod.node()->findModelFromPath("Im");
        Ensures(mdl != 0);
        vz::AtomicModel* atomg = mdl->toAtomic();
        atomg->setConditions(conds);
    }
    {
        conds.clear();
        conds.push_back("condEuler");
        conds.push_back("condSeir");
        vz::Model& vpz_mod = vpz->project().model();
        vz::BaseModel* mdl = vpz_mod.node()->findModelFromPath("Rm");
        Ensures(mdl != 0);
        vz::AtomicModel* atomg = mdl->toAtomic();
        atomg->setConditions(conds);
    }

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
    //note: the number of rows depend on the averaging of sum of 0.01
    Ensures(view.rows() <= 1503);
    Ensures(view.rows() >= 1502);

    //gets S,E,I,R
    int colS = ttgetColumnFromView(view, "Top model:Sm", "S");
    int colE = ttgetColumnFromView(view, "Top model:Em", "E");
    int colI = ttgetColumnFromView(view, "Top model:Im", "I");
    int colR = ttgetColumnFromView(view, "Top model:Rm", "R");
    //check S,E,I,R line 1501
    EnsuresApproximatelyEqual(view.getDouble(colS,1501), 0.634334231496758, 10e-5);
    EnsuresApproximatelyEqual(view.getDouble(colE,1501), 0.65430775343564, 10e-5);
    EnsuresApproximatelyEqual(view.getDouble(colI,1501), 2.97798653683738, 10e-5);
    EnsuresApproximatelyEqual(view.getDouble(colR,1501), 6.73337147823022, 10e-5);
}


int main()
{
    F fixture;
    test_Euler_LotkaVolterra();
    test_Euler_LotkaVolterraXY();
    test_Euler_Seir();
    test_Euler_SeirXY();

    return unit_test::report_errors();
}
