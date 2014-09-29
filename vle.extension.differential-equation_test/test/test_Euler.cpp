/**
 * @file vle.extension.differential-equation_test/test/test_Euler.cpp
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

#define BOOST_TEST_MAIN
#define BOOST_AUTO_TEST_MAIN
#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE package_test
#include <boost/test/unit_test.hpp>
#include <boost/test/auto_unit_test.hpp>
#include <boost/test/floating_point_comparison.hpp>

#include <ostream>
#include <iostream>

#include <vle/vle.hpp>
#include <vle/value/Map.hpp>
#include <vle/value/Matrix.hpp>
#include <vle/vpz/Vpz.hpp>
#include <vle/vpz/Model.hpp>
#include <vle/vpz/AtomicModel.hpp>
#include <vle/utils/Package.hpp>
#include <vle/manager/Simulation.hpp>
#include <vle/manager/Manager.hpp>


namespace vz = vle::vpz;
namespace vu = vle::utils;
namespace vm = vle::manager;
namespace va = vle::value;

struct F
{
    F()
    {
    }

    vle::Init app;
};
BOOST_GLOBAL_FIXTURE(F)

va::ConstVectorView getColumnFromView(const va::Matrix& view, const std::string& model,
                                      const std::string& port)
{
    for(unsigned int j=1; j < view.columns(); j++){
        if(view.getString(j,0) == (model + std::string(".") + port)){
            return view.column(j);
        }
    }
    return view.column(0);
}


/******************
 *  Unit test based on deSolve R package
 *  see R/check.R function test_Euler_LotkaVolterra
 ******************/
BOOST_AUTO_TEST_CASE(test_Euler_LotkaVolterra)
{
    std::cout << "  test_Euler_LotkaVolterra " << std::endl;
    vle::utils::Package pack("vle.extension.differential-equation_test");
    vz::Vpz vpz(pack.getExpFile("LotkaVolterra.vpz", vle::utils::PKG_BINARY));

    //set all output plugin to storage
    vz::Outputs::iterator itb =
        vpz.project().experiment().views().outputs().begin();
    vz::Outputs::iterator ite =
        vpz.project().experiment().views().outputs().end();
    for(;itb!=ite;itb++) {
        va::Map* configOutput = new va::Map();
        configOutput->addInt("rows",10000);
        configOutput->addInt("inc_rows",10000);
        configOutput->addString("header","top");
        vz::Output& output = itb->second;
        output.setLocalStream("", "storage", "vle.output");
        output.setData(configOutput);
    }


    //attach condition condEuler and condLV
    std::vector<std::string> conds;
    conds.push_back("condEuler");
    conds.push_back("condLV");
    vz::Model& vpz_mod = vpz.project().model();
    vz::BaseModel* mdl = vpz_mod.model()->findModelFromPath("LotkaVolterra");
    BOOST_REQUIRE(mdl != 0);
    vz::AtomicModel* atomg = mdl->toAtomic();
    atomg->setConditions(conds);

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
    va::ConstVectorView colX = getColumnFromView(view,
                                                 "Top model:LotkaVolterra", "X");

    va::ConstVectorView colY = getColumnFromView(view,
                                                 "Top model:LotkaVolterra", "Y");

    //check X,Y line 10
    BOOST_REQUIRE_CLOSE(va::toDouble(colX[10]),
                        9.677077, 10e-5);
    BOOST_REQUIRE_CLOSE(va::toDouble(colY[10]),
                        5.317209, 10e-5);

    //check X,Y line 30
    BOOST_REQUIRE_CLOSE(va::toDouble(colX[30]),
                        8.903716, 10e-5);
    BOOST_REQUIRE_CLOSE(va::toDouble(colY[30]),
                        6.030680, 10e-5);

    //check X,Y line 15000
    BOOST_REQUIRE_CLOSE(va::toDouble(colX[15000]),
                        0.5528446, 10e-5);
    BOOST_REQUIRE_CLOSE(va::toDouble(colY[15000]),
                        0.09330513, 10e-5);

}

/******************
 *  Unit test based on deSolve R package
 *  see R/check.R function test_Euler_LotkaVolterra
 *
 *  Note: we test that the version of LotkaVolterra based on two
 *  differential_equation (LotkaVolterraXY.vpz) gets the same results
 *  in the case of Euler integration
 ******************/
BOOST_AUTO_TEST_CASE(test_Euler_LotkaVolterraXY)
{
    std::cout << "  test_Euler_LotkaVolterraXY " << std::endl;
    vle::utils::Package pack("vle.extension.differential-equation_test");
    vz::Vpz vpz(pack.getExpFile("LotkaVolterraXY.vpz", vle::utils::PKG_BINARY));

    //set all output plugin to storage
    vz::Outputs::iterator itb =
        vpz.project().experiment().views().outputs().begin();
    vz::Outputs::iterator ite =
        vpz.project().experiment().views().outputs().end();
    for(;itb!=ite;itb++) {
        va::Map* configOutput = new va::Map();
        configOutput->addInt("rows",10000);
        configOutput->addInt("inc_rows",10000);
        configOutput->addString("header","top");
        vz::Output& output = itb->second;
        output.setLocalStream("", "storage", "vle.output");
        output.setData(configOutput);
    }

    //attach condition condEuler and condLV to atomics models
    std::vector<std::string> conds;

    {
        conds.clear();
        conds.push_back("condEuler");
        conds.push_back("condLV_X");
        vz::Model& vpz_mod = vpz.project().model();
        vz::BaseModel* mdl = vpz_mod.model()->findModelFromPath("LotkaVolterraX");
        BOOST_REQUIRE(mdl != 0);
        vz::AtomicModel* atomg = mdl->toAtomic();
        atomg->setConditions(conds);
    }
    {
        conds.clear();
        conds.push_back("condEuler");
        conds.push_back("condLV_Y");
        vz::Model& vpz_mod = vpz.project().model();
        vz::BaseModel* mdl = vpz_mod.model()->findModelFromPath("LotkaVolterraY");
        BOOST_REQUIRE(mdl != 0);
        vz::AtomicModel* atomg = mdl->toAtomic();
        atomg->setConditions(conds);
    }
    //simulation
    vu::ModuleManager m;

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
    va::ConstVectorView colX = getColumnFromView(view,
                                                 "Top model:LotkaVolterraX", "X");
    va::ConstVectorView colY = getColumnFromView(view,
                                                 "Top model:LotkaVolterraY", "Y");

    //check X,Y line 10
    BOOST_REQUIRE_CLOSE(va::toDouble(colX[10]),
                        9.677077, 10e-5);
    BOOST_REQUIRE_CLOSE(va::toDouble(colY[10]),
                        5.317209, 10e-5);

    //check X,Y line 30
    BOOST_REQUIRE_CLOSE(va::toDouble(colX[30]),
                        8.903716, 10e-5);
    BOOST_REQUIRE_CLOSE(va::toDouble(colY[30]),
                        6.030680, 10e-5);

    //check X,Y line 15000
    BOOST_REQUIRE_CLOSE(va::toDouble(colX[15000]),
                        0.5528446, 10e-5);
    BOOST_REQUIRE_CLOSE(va::toDouble(colY[15000]),
                        0.09330513, 10e-5);
}

/******************
 *  Unit test based on deSolve R package
 *  see R/check.R function test_Euler_Seir
 ******************/
BOOST_AUTO_TEST_CASE(test_Euler_Seir)
{
    std::cout << "  test_Euler_Seir " << std::endl;
    vle::utils::Package pack("vle.extension.differential-equation_test");
    vz::Vpz vpz(pack.getExpFile("Seir.vpz", vle::utils::PKG_BINARY));

    //set all output plugin to storage
    vz::Outputs::iterator itb =
        vpz.project().experiment().views().outputs().begin();
    vz::Outputs::iterator ite =
        vpz.project().experiment().views().outputs().end();
    for(;itb!=ite;itb++) {
        va::Map* configOutput = new va::Map();
        configOutput->addInt("rows",10000);
        configOutput->addInt("inc_rows",10000);
        configOutput->addString("header","top");
        vz::Output& output = itb->second;
        output.setLocalStream("", "storage", "vle.output");
        output.setData(configOutput);
    }

    //attach condition condEuler and condLV
    std::vector<std::string> conds;

    conds.push_back("condEuler");
    conds.push_back("condSeir");
    vz::Model& vpz_mod = vpz.project().model();
    vz::BaseModel* mdl = vpz_mod.model()->findModelFromPath("Seir");
    BOOST_REQUIRE(mdl != 0);
    vz::AtomicModel* atomg = mdl->toAtomic();
    atomg->setConditions(conds);

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
    va::ConstVectorView colS = getColumnFromView(view,
                                                 "Top model:Seir", "S");
    va::ConstVectorView colE = getColumnFromView(view,
                                                 "Top model:Seir", "E");
    va::ConstVectorView colI = getColumnFromView(view,
                                                 "Top model:Seir", "I");
    va::ConstVectorView colR = getColumnFromView(view,
                                                 "Top model:Seir", "R");

    //check S,E,I,R line 1501
    BOOST_REQUIRE_CLOSE(va::toDouble(colS[1501]),
                        0.634334231496758, 10e-5);
    BOOST_REQUIRE_CLOSE(va::toDouble(colE[1501]),
                        0.65430775343564, 10e-5);
    BOOST_REQUIRE_CLOSE(va::toDouble(colI[1501]),
                        2.97798653683738, 10e-5);
    BOOST_REQUIRE_CLOSE(va::toDouble(colR[1501]),
                        6.73337147823022, 10e-5);
    //E I R S
    //  0.65430775343564;2.97798653683738;6.73337147823022;0.634334231496758

}

/******************
 *  Unit test based on deSolve R package
 *  see R/check.R function test_Euler_Seir
 *
 *  Note: we test that the version of Seir based on four
 *  differential_equation (SeirXY.vpz) gets the same results
 *  in the case of Euler integration
 ******************/
BOOST_AUTO_TEST_CASE(test_Euler_SeirXY)
{
    std::cout << "  test_Euler_SeirXY " << std::endl;
    vle::utils::Package pack("vle.extension.differential-equation_test");
    vz::Vpz vpz(pack.getExpFile("SeirXY.vpz", vle::utils::PKG_BINARY));

    //set all output plugin to storage
    vz::Outputs::iterator itb =
        vpz.project().experiment().views().outputs().begin();
    vz::Outputs::iterator ite =
        vpz.project().experiment().views().outputs().end();
    for(;itb!=ite;itb++) {
        va::Map* configOutput = new va::Map();
        configOutput->addInt("rows",10000);
        configOutput->addInt("inc_rows",10000);
        configOutput->addString("header","top");
        vz::Output& output = itb->second;
        output.setLocalStream("", "storage", "vle.output");
        output.setData(configOutput);
    }

    //attach condition condEuler and condLV to atomics models
    std::vector<std::string> conds;

    {
        conds.clear();
        conds.push_back("condEuler");
        conds.push_back("condSeir");
        conds.push_back("condSm");
        vz::Model& vpz_mod = vpz.project().model();
        vz::BaseModel* mdl = vpz_mod.model()->findModelFromPath("Sm");
        BOOST_REQUIRE(mdl != 0);
        vz::AtomicModel* atomg = mdl->toAtomic();
        atomg->setConditions(conds);
    }
    {
        conds.clear();
        conds.push_back("condEuler");
        conds.push_back("condSeir");
        conds.push_back("condEm");
        vz::Model& vpz_mod = vpz.project().model();
        vz::BaseModel* mdl = vpz_mod.model()->findModelFromPath("Em");
        BOOST_REQUIRE(mdl != 0);
        vz::AtomicModel* atomg = mdl->toAtomic();
        atomg->setConditions(conds);
    }
    {
        conds.clear();
        conds.push_back("condEuler");
        conds.push_back("condSeir");
        conds.push_back("condIm");
        vz::Model& vpz_mod = vpz.project().model();
        vz::BaseModel* mdl = vpz_mod.model()->findModelFromPath("Im");
        BOOST_REQUIRE(mdl != 0);
        vz::AtomicModel* atomg = mdl->toAtomic();
        atomg->setConditions(conds);
    }
    {
        conds.clear();
        conds.push_back("condEuler");
        conds.push_back("condSeir");
        conds.push_back("condRm");
        vz::Model& vpz_mod = vpz.project().model();
        vz::BaseModel* mdl = vpz_mod.model()->findModelFromPath("Rm");
        BOOST_REQUIRE(mdl != 0);
        vz::AtomicModel* atomg = mdl->toAtomic();
        atomg->setConditions(conds);
    }

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
    va::ConstVectorView colS = getColumnFromView(view,
                                                 "Top model:Sm", "S");
    va::ConstVectorView colE = getColumnFromView(view,
                                                 "Top model:Em", "E");
    va::ConstVectorView colI = getColumnFromView(view,
                                                 "Top model:Im", "I");
    va::ConstVectorView colR = getColumnFromView(view,
                                                 "Top model:Rm", "R");
    //check S,E,I,R line 1501
    BOOST_REQUIRE_CLOSE(va::toDouble(colS[1501]),
                        0.634334231496758, 10e-5);
    BOOST_REQUIRE_CLOSE(va::toDouble(colE[1501]),
                        0.65430775343564, 10e-5);
    BOOST_REQUIRE_CLOSE(va::toDouble(colI[1501]),
                        2.97798653683738, 10e-5);
    BOOST_REQUIRE_CLOSE(va::toDouble(colR[1501]),
                        6.73337147823022, 10e-5);
}


