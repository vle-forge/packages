/*
 * @file vle/manager/test/testmanager.cpp
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


#define BOOST_TEST_MAIN
#define BOOST_AUTO_TEST_MAIN
#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE manager_test

#include <boost/test/unit_test.hpp>
#include <boost/test/auto_unit_test.hpp>
#include <boost/test/floating_point_comparison.hpp>
#include <boost/lexical_cast.hpp>
#include <stdexcept>
#include <iostream>
#include <vle/manager/Manager.hpp>
#include <vle/manager/Simulation.hpp>
#include <vle/value/Double.hpp>
#include <vle/value/Integer.hpp>
#include <vle/utils/Path.hpp>
#include <vle/utils/Package.hpp>
#include <vle/vle.hpp>

struct F
{
    vle::Init app;

    F()
    {
        vle::utils::Package::package().select("vle.examples");
    }
};

BOOST_GLOBAL_FIXTURE(F)

using namespace vle;

BOOST_AUTO_TEST_CASE(build_experimental_frames)
{
    vpz::Vpz *file = new vpz::Vpz(utils::Path::path().getPackageExpFile("unittest.vpz"));
    vpz::Conditions& cnds(file->project().experiment().conditions());
    cnds.get("ca").addValueToPort("x", value::Double::create(1.0));
    cnds.get("ca").addValueToPort("x", value::Double::create(2.0));

    utils::ModuleManager man;
    manager::Manager r(manager::LOG_NONE, manager::SIMULATION_NONE, NULL);
    manager::Error error;
    value::Matrix *out = r.run(file, man, 1, 0, 1, &error);

    BOOST_REQUIRE(not error.code);
    BOOST_REQUIRE(out);

    BOOST_REQUIRE_EQUAL(out->size(), (value::Matrix::size_type)3);

    for (value::Matrix::size_type y = 0; y < out->size(); ++y) {
        BOOST_REQUIRE(out->get(y, 0));
        value::Map &map = out->getMap(y, 0);

        BOOST_REQUIRE_EQUAL(map.size(), (value::Matrix::size_type)2);

        value::Map::iterator it = map.begin();

        value::Matrix *matrix1 = value::toMatrixValue(it->second);
        BOOST_REQUIRE_EQUAL(matrix1->rows(), 101);
        BOOST_REQUIRE_EQUAL(matrix1->columns(), 5);

        ++it;
        BOOST_REQUIRE(it != map.end());

        value::Matrix *matrix2 = value::toMatrixValue(it->second);

        BOOST_REQUIRE(matrix2);
        BOOST_REQUIRE_EQUAL(matrix2->rows(), 101);
        BOOST_REQUIRE_EQUAL(matrix2->columns(), 4);
    }

    delete out;
}

BOOST_AUTO_TEST_CASE(build_linear_combination_size)
{
    vpz::Vpz *file = new vpz::Vpz(utils::Path::path().getPackageExpFile("unittest.vpz"));

    file->project().experiment().setCombination("linear");

    vpz::Conditions& cnds(file->project().experiment().conditions());
    cnds.get("ca").addValueToPort("x", value::Double::create(1.0));
    cnds.get("ca").addValueToPort("x", value::Double::create(2.0));
    cnds.get("ca").addValueToPort("x", value::Double::create(3.0));
    cnds.get("ca").addValueToPort("x", value::Double::create(4.0));

    cnds.get("cb").addValueToPort("x", value::Double::create(5.0));
    cnds.get("cb").addValueToPort("x", value::Double::create(6.0));
    cnds.get("cb").addValueToPort("x", value::Double::create(7.0));
    cnds.get("cb").addValueToPort("x", value::Double::create(8.0));

    cnds.get("cd").addValueToPort("x", value::Double::create(9.0));
    cnds.get("cd").addValueToPort("x", value::Double::create(10.0));

    utils::ModuleManager man;
    manager::Manager r(manager::LOG_NONE, manager::SIMULATION_NONE, NULL);
    manager::Error error;
    value::Matrix *out = 0;
    BOOST_REQUIRE_THROW(out = r.run(file, man, 1, 0, 1, &error), std::runtime_error);

    delete out;
}


BOOST_AUTO_TEST_CASE(build_linear_output_matrix_size)
{
    vpz::Vpz *file = new vpz::Vpz(utils::Path::path().getPackageExpFile("unittest.vpz"));

    file->project().experiment().setCombination("linear");

    vpz::Conditions& cnds(file->project().experiment().conditions());
    cnds.get("ca").addValueToPort("x", value::Double::create(1.0));
    cnds.get("ca").addValueToPort("x", value::Double::create(2.0));
    cnds.get("ca").addValueToPort("x", value::Double::create(3.0));
    cnds.get("ca").addValueToPort("x", value::Double::create(4.0));

    cnds.get("cb").addValueToPort("x", value::Double::create(6.0));
    cnds.get("cb").addValueToPort("x", value::Double::create(7.0));
    cnds.get("cb").addValueToPort("x", value::Double::create(8.0));
    cnds.get("cb").addValueToPort("x", value::Double::create(9.0));

    utils::ModuleManager man;
    manager::Manager r(manager::LOG_NONE, manager::SIMULATION_NONE, NULL);
    manager::Error error;
    value::Matrix *out = r.run(file, man, 1, 0, 1, &error);

    BOOST_REQUIRE(not error.code);
    BOOST_REQUIRE(out);

    BOOST_REQUIRE_EQUAL(out->size(), (value::Matrix::size_type)5);

    for (value::Matrix::size_type y = 0; y < out->size(); ++y) {
        value::Map &map = out->getMap(y, 0);

        value::Map::iterator it = map.begin();

        value::Matrix *matrix1 = value::toMatrixValue(it->second);
        BOOST_REQUIRE_EQUAL(matrix1->columns(), 5);
        BOOST_REQUIRE_EQUAL(matrix1->rows(), 101);

        ++it;
        value::Matrix *matrix2 = value::toMatrixValue(it->second);
        BOOST_REQUIRE_EQUAL(matrix2->columns(), 4);
        BOOST_REQUIRE_EQUAL(matrix2->rows(), 101);


        BOOST_REQUIRE_EQUAL(100, value::toDouble(
                                (matrix1->value()[0][100])));
        BOOST_REQUIRE_EQUAL(101, value::toInteger(
                                (matrix1->value()[1][100])));
        BOOST_REQUIRE_EQUAL(101, value::toInteger(
                                (matrix1->value()[2][100])));
        BOOST_REQUIRE_EQUAL(201, value::toInteger(
                                (matrix1->value()[3][100])));
    }

    delete out;
}

//BOOST_AUTO_TEST_CASE(combination_storage_test){
    //using namespace manager;

    //vpz::Vpz *file = new vpz::Vpz(utils::Path::path().getPackageExpFile("unittest.vpz"));

    //file->project().experiment().setCombination("linear");

    //vpz::Conditions& cnds(file->project().experiment().conditions());
    //cnds.get("ca").clearValueOfPort("x");
    //cnds.get("cb").clearValueOfPort("x");
    //cnds.get("cc").clearValueOfPort("x");
    //cnds.get("cd").clearValueOfPort("x");

    //cnds.get("ca").addValueToPort("x", value::Double::create(1.0));
    //cnds.get("ca").addValueToPort("x", value::Double::create(2.0));
    //cnds.get("cb").addValueToPort("x", value::Double::create(3.0));
    //cnds.get("cc").addValueToPort("x", value::Double::create(4.0));
    //cnds.get("cc").addValueToPort("x", value::Double::create(5.0));
    //cnds.get("cd").addValueToPort("x", value::Double::create(6.0));

    //bool writefile = false;
    //bool storecomb = true;
    //bool commonseed = true;


    //ExperimentGenerator expgen(file, 0, 1);

    //OutputSimulationList& out = *pout;
    //BOOST_REQUIRE_EQUAL(out.size(), (OutputSimulationList::size_type)2);

    //BOOST_REQUIRE_EQUAL(r.getCombinations().size(), 2);

    //BOOST_REQUIRE_EQUAL(value::toDouble(r.getInputFromCombination(0, "ca", "x")), 1.0);
    //BOOST_REQUIRE_EQUAL(value::toDouble(r.getInputFromCombination(0, "cb", "x")), 3.0);
    //BOOST_REQUIRE_EQUAL(value::toDouble(r.getInputFromCombination(0, "cc", "x")), 4.0);
    //BOOST_REQUIRE_EQUAL(value::toDouble(r.getInputFromCombination(0, "cd", "x")), 6.0);
    //BOOST_REQUIRE_EQUAL(value::toDouble(r.getInputFromCombination(1, "ca", "x")), 2.0);
    //BOOST_REQUIRE_EQUAL(value::toDouble(r.getInputFromCombination(1, "cb", "x")), 3.0);
    //BOOST_REQUIRE_EQUAL(value::toDouble(r.getInputFromCombination(1, "cc", "x")), 5.0);
    //BOOST_REQUIRE_EQUAL(value::toDouble(r.getInputFromCombination(1, "cd", "x")), 6.0);

    //file->project().experiment().setCombination("total");
    //ManagerRunMono r2(std::cout, writefile, storecomb);
    //r2.start(file);

    //const OutputSimulationList& out2(r2.outputSimulationList());
    //BOOST_REQUIRE_EQUAL(out2.size(), (OutputSimulationList::size_type)4);

    //BOOST_REQUIRE_EQUAL(r2.getCombinations().size(), 4);

    //BOOST_REQUIRE_EQUAL(value::toDouble(r2.getInputFromCombination(0, "ca", "x")), 1.0);
    //BOOST_REQUIRE_EQUAL(value::toDouble(r2.getInputFromCombination(0, "cb", "x")), 3.0);
    //BOOST_REQUIRE_EQUAL(value::toDouble(r2.getInputFromCombination(0, "cc", "x")), 4.0);
    //BOOST_REQUIRE_EQUAL(value::toDouble(r2.getInputFromCombination(0, "cd", "x")), 6.0);
    //BOOST_REQUIRE_EQUAL(value::toDouble(r2.getInputFromCombination(1, "ca", "x")), 1.0);
    //BOOST_REQUIRE_EQUAL(value::toDouble(r2.getInputFromCombination(1, "cb", "x")), 3.0);
    //BOOST_REQUIRE_EQUAL(value::toDouble(r2.getInputFromCombination(1, "cc", "x")), 5.0);
    //BOOST_REQUIRE_EQUAL(value::toDouble(r2.getInputFromCombination(1, "cd", "x")), 6.0);
    //BOOST_REQUIRE_EQUAL(value::toDouble(r2.getInputFromCombination(2, "ca", "x")), 2.0);
    //BOOST_REQUIRE_EQUAL(value::toDouble(r2.getInputFromCombination(2, "cb", "x")), 3.0);
    //BOOST_REQUIRE_EQUAL(value::toDouble(r2.getInputFromCombination(2, "cc", "x")), 4.0);
    //BOOST_REQUIRE_EQUAL(value::toDouble(r2.getInputFromCombination(2, "cd", "x")), 6.0);
    //BOOST_REQUIRE_EQUAL(value::toDouble(r2.getInputFromCombination(3, "ca", "x")), 2.0);
    //BOOST_REQUIRE_EQUAL(value::toDouble(r2.getInputFromCombination(3, "cb", "x")), 3.0);
    //BOOST_REQUIRE_EQUAL(value::toDouble(r2.getInputFromCombination(3, "cc", "x")), 5.0);
    //BOOST_REQUIRE_EQUAL(value::toDouble(r2.getInputFromCombination(3, "cd", "x")), 6.0);

    //delete file->project().model().model();
//}

//BOOST_AUTO_TEST_CASE(common_seed_test)
//{
    //using namespace manager;

    //vpz::Vpz *file = new vpz::Vpz(utils::Path::path().getPackageExpFile("petrinet-meteo.vpz"));

    //file->project().experiment().setCombination("linear");
    //vle::vpz::Views& vle_views = file->project().experiment().views();
    //vle::vpz::Output& view =
        //vle_views.outputs().get(vle_views.get("view").output());
    //if (view.plugin() != "storage") {
        //view.setLocalStream("", "storage", "vle.output");
    //}

    //vpz::Conditions& cnds(file->project().experiment().conditions());
    //cnds.get("petri").clearValueOfPort("day");
    //cnds.get("petri").addValueToPort("day", value::Integer::create(3));
    //cnds.get("petri").addValueToPort("day", value::Integer::create(3));

    //typedef value::MatrixView::size_type tmp_type;

    ////option common_seed activated
    //bool writefile = false;
    //bool storecomb = true;
    //bool commonseed = true;
    //ManagerRunMono r(std::cout, writefile, storecomb);
    //r.start(file);
    //const OutputSimulationList& out(r.outputSimulationList());
    //BOOST_REQUIRE_EQUAL(out.size(), (OutputSimulationList::size_type)2);

        ////combination 0 : common seed
        //const oov::OutputMatrixViewList& outView0 =  out[x][0];
        //BOOST_REQUIRE_EQUAL(outView0.size(),
                            //(oov::OutputMatrixViewList::size_type)1);
        //const oov::OutputMatrix& outMatrix0 = outView0.find("view")->second;
        //const value::ConstVectorView& vec0 =
            //outMatrix0.getValue("top:PetriNet", "token");
        //BOOST_REQUIRE_EQUAL(vec0.size(), 31);
        ////combination 1 : common seed
        //const oov::OutputMatrixViewList& outView1 =  out[x][1];
        //BOOST_REQUIRE_EQUAL(outView1.size(),
                            //(oov::OutputMatrixViewList::size_type)1);
        //const oov::OutputMatrix& outMatrix1 = outView1.find("view")->second;
        //const value::ConstVectorView& vec1 =
            //outMatrix1.getValue("top:PetriNet", "token");
        //BOOST_REQUIRE_EQUAL(vec1.size(), 31);
        ////comparison
        //for (unsigned int v = 0; v < vec1.size(); v++) {
            //vle::value::Value* val0 = vec0[v];
            //vle::value::Value* val1 = vec1[v];
            //BOOST_REQUIRE_EQUAL(val0->toInteger().value(),
                                //val1->toInteger().value());
        //}
    //}
    ////option common_seed not activated
    //writefile = false;
    //storecomb = true;
    //commonseed = false;
    //ManagerRunMono rb(std::cout, writefile, storecomb);
    //rb.start(file);
    //const OutputSimulationList& outb(rb.OutputSimulationList());
    //BOOST_REQUIRE_EQUAL(outb.shape()[0],
                        //(OutputSimulationList::size_type)3);
    //BOOST_REQUIRE_EQUAL(outb.shape()[1],
                        //(OutputSimulationList::size_type)2);

    //for (OutputSimulationList::size_type x = 0; x < outb.shape()[0]; ++x) {
        ////combination 0 : common seed
        //const oov::OutputMatrixViewList& outView0 =  outb[x][0];
        //BOOST_REQUIRE_EQUAL(outView0.size(),
                            //(oov::OutputMatrixViewList::size_type)1);
        //const oov::OutputMatrix& outMatrix0 = outView0.find("view")->second;
        //const value::ConstVectorView& vec0 =
            //outMatrix0.getValue("top:PetriNet", "token");
        //BOOST_REQUIRE_EQUAL(vec0.size(), 31);
        ////combination 1 : common seed
        //const oov::OutputMatrixViewList& outView1 =  outb[x][1];
        //BOOST_REQUIRE_EQUAL(outView1.size(),
                            //(oov::OutputMatrixViewList::size_type)1);
        //const oov::OutputMatrix& outMatrix1 = outView1.find("view")->second;
        //const value::ConstVectorView& vec1 =
            //outMatrix1.getValue("top:PetriNet", "token");
        //BOOST_REQUIRE_EQUAL(vec1.size(), 31);
        ////comparison
        //bool equality = true;
        //for (unsigned int v = 0; v < vec1.size(); v++) {
            //vle::value::Value* val0 = vec0[v];
            //vle::value::Value* val1 = vec1[v];
            //equality = equality &&
                       //(val0->toInteger().value() == val1->toInteger().value());
        //}
        //BOOST_REQUIRE_EQUAL(equality, false);
    //}

    //delete file->project().model().model();
//}

BOOST_AUTO_TEST_CASE(manager_thread_result_access)
{
    using namespace manager;

    vpz::Vpz *file = new vpz::Vpz(utils::Path::path().getPackageExpFile("unittest.vpz"));
    file->project().experiment().setCombination("linear");
    file->project().experiment().setDuration(1.0);

    vpz::Conditions& cnds(file->project().experiment().conditions());
    cnds.get("ca").clearValueOfPort("x");
    cnds.get("cb").clearValueOfPort("x");
    cnds.get("cc").clearValueOfPort("x");
    cnds.get("cd").clearValueOfPort("x");

    cnds.get("ca").addValueToPort("x", value::Double::create(1.0));
    cnds.get("ca").addValueToPort("x", value::Double::create(2.0));
    cnds.get("cb").addValueToPort("x", value::Double::create(3.0));
    cnds.get("cb").addValueToPort("x", value::Double::create(4.0));
    cnds.get("cc").addValueToPort("x", value::Double::create(5.0));
    cnds.get("cc").addValueToPort("x", value::Double::create(6.0));
    cnds.get("cd").addValueToPort("x", value::Double::create(7.0));
    cnds.get("cd").addValueToPort("x", value::Double::create(8.0));

    //set output
    vle::vpz::Views& vle_views = file->project().experiment().views();
    vle::vpz::Output& view1 = vle_views.outputs().get(vle_views.get("view1").output());
    if (view1.plugin() != "storage") {
        view1.setLocalStream("", "storage", "vle.output");
    }
    vle::vpz::Output& view2 = vle_views.outputs().get(vle_views.get("view2").output());
    if (view2.plugin() != "storage") {
        view2.setLocalStream("", "storage", "vle.output");
    }

    utils::ModuleManager man;
    manager::Manager r(manager::LOG_NONE, manager::SIMULATION_NONE, NULL);
    manager::Error error;
    value::Matrix *out = r.run(file, man, 2, 0, 1, &error);

    BOOST_REQUIRE(out);
    BOOST_REQUIRE_EQUAL(out->size(), (value::Matrix::size_type)2);

    for (value::Matrix::size_type y = 0; y < out->size(); ++y) {
        BOOST_REQUIRE(out->get(y, 0));
        value::Map &map = out->getMap(y, 0);

        //check number of views
        BOOST_REQUIRE_EQUAL(map.size(), (value::Map::size_type)2);

        //check view1
        value::Map::iterator it(map.begin());
        value::Matrix *matrix1 = value::toMatrixValue(it->second);
        BOOST_REQUIRE_EQUAL(matrix1->rows(), 2);
        BOOST_REQUIRE_EQUAL(matrix1->columns(), 5);

        //check view 2
        ++it;
        BOOST_REQUIRE(it != map.end());

        value::Matrix *matrix2 = value::toMatrixValue(it->second);

        BOOST_REQUIRE_EQUAL(matrix2->rows(), 2);
        BOOST_REQUIRE_EQUAL(matrix2->columns(), 4);

    }

    delete out;
}

BOOST_AUTO_TEST_CASE(manager_thread_fast_producer)
{
    using namespace manager;

    vpz::Vpz *file = new vpz::Vpz(utils::Path::path().getPackageExpFile("unittest.vpz"));

    file->project().experiment().setCombination("total");
    file->project().experiment().setDuration(100.0);

    vpz::Conditions& cnds(file->project().experiment().conditions());
    cnds.get("ca").clearValueOfPort("x");
    cnds.get("cb").clearValueOfPort("x");
    cnds.get("cc").clearValueOfPort("x");
    cnds.get("cd").clearValueOfPort("x");

    cnds.get("ca").addValueToPort("x", value::Double::create(1.0));
    cnds.get("ca").addValueToPort("x", value::Double::create(2.0));
    cnds.get("cb").addValueToPort("x", value::Double::create(3.0));
    cnds.get("cb").addValueToPort("x", value::Double::create(4.0));
    cnds.get("cc").addValueToPort("x", value::Double::create(5.0));
    cnds.get("cc").addValueToPort("x", value::Double::create(6.0));
    cnds.get("cd").addValueToPort("x", value::Double::create(7.0));
    cnds.get("cd").addValueToPort("x", value::Double::create(8.0));

    //set output
    vle::vpz::Views& vle_views = file->project().experiment().views();
    vle::vpz::Output& view1 = vle_views.outputs().get(vle_views.get("view1").output());
    if (view1.plugin() != "storage") {
        view1.setLocalStream("", "storage", "vle.output");
    }
    vle::vpz::Output& view2 = vle_views.outputs().get(vle_views.get("view2").output());
    if (view2.plugin() != "storage") {
        view2.setLocalStream("", "storage", "vle.output");
    }

    utils::ModuleManager man;
    manager::Manager r(manager::LOG_NONE, manager::SIMULATION_NONE, NULL);
    manager::Error error;
    value::Matrix *out = r.run(file, man, 2, 0, 1, &error);

    BOOST_REQUIRE(out);

    BOOST_REQUIRE_EQUAL(out->size(), (value::Matrix::size_type)2);

    delete out;
}

BOOST_AUTO_TEST_CASE(manager_thread_fast_consumer)
{
    using namespace manager;

    vpz::Vpz *file = new vpz::Vpz(utils::Path::path().getPackageExpFile("unittest.vpz"));

    file->project().experiment().setCombination("linear");
    file->project().experiment().setDuration(1.0);

    vpz::Conditions& cnds(file->project().experiment().conditions());
    cnds.get("ca").clearValueOfPort("x");
    cnds.get("cb").clearValueOfPort("x");
    cnds.get("cc").clearValueOfPort("x");
    cnds.get("cd").clearValueOfPort("x");

    cnds.get("ca").addValueToPort("x", value::Double::create(1.0));
    cnds.get("ca").addValueToPort("x", value::Double::create(2.0));
    cnds.get("cb").addValueToPort("x", value::Double::create(3.0));
    cnds.get("cb").addValueToPort("x", value::Double::create(4.0));
    cnds.get("cc").addValueToPort("x", value::Double::create(5.0));
    cnds.get("cc").addValueToPort("x", value::Double::create(6.0));
    cnds.get("cd").addValueToPort("x", value::Double::create(7.0));
    cnds.get("cd").addValueToPort("x", value::Double::create(8.0));

    //set output
    vle::vpz::Views& vle_views = file->project().experiment().views();
    vle::vpz::Output& view1 = vle_views.outputs().get(vle_views.get("view1").output());
    if (view1.plugin() != "storage") {
        view1.setLocalStream("", "storage", "vle.output");
    }
    vle::vpz::Output& view2 = vle_views.outputs().get(vle_views.get("view2").output());
    if (view2.plugin() != "storage") {
        view2.setLocalStream("", "storage", "vle.output");
    }

    utils::ModuleManager man;
    manager::Manager r(manager::LOG_NONE, manager::SIMULATION_NONE, NULL);
    manager::Error error;
    value::Matrix *out = r.run(file, man, 2, 0, 1, &error);

    BOOST_REQUIRE(out);

    BOOST_REQUIRE_EQUAL(out->size(), (value::Matrix::size_type)2);

    delete out;
}

