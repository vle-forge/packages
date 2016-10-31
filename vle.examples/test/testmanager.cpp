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


#include <vle/utils/unit-test.hpp>
#include <string>
#include <stdexcept>
#include <iostream>
#include <vle/manager/Manager.hpp>
#include <vle/manager/Simulation.hpp>
#include <vle/value/Double.hpp>
#include <vle/value/Integer.hpp>
#include <vle/value/Matrix.hpp>

#include <vle/utils/Package.hpp>
#include <vle/vle.hpp>

struct F
{
    vle::Init app;

    F()
    {
    }
};


using namespace vle;

void build_experimental_frames()
{
    auto ctx = vle::utils::make_context(); vle::utils::Package pack(ctx, "vle.examples");
    std::unique_ptr<vpz::Vpz> file(
            new vpz::Vpz(pack.getExpFile("unittest.vpz")));
    vpz::Conditions& cnds(file->project().experiment().conditions());
    cnds.get("ca").addValueToPort("x", value::Double::create(1.0));
    cnds.get("ca").addValueToPort("x", value::Double::create(2.0));


    manager::Manager r(ctx, manager::LOG_NONE, manager::SIMULATION_NONE, NULL);
    manager::Error error;
    std::unique_ptr<value::Matrix> out = r.run(std::move(file),
            1, 0, 1, &error);

    Ensures(not error.code);
    Ensures(out);

    EnsuresEqual(out->size(), (value::Matrix::size_type)3);

    for (value::Matrix::size_type y = 0; y < out->size(); ++y) {
        Ensures(out->get(y, 0));
        value::Map &map = out->getMap(y, 0);

        EnsuresEqual(map.size(), (value::Matrix::size_type)2);

        Ensures(map.exist("view1"));
        Ensures(map.exist("view2"));
        const value::Matrix& matrix1 = map.getMatrix("view1");
        const value::Matrix& matrix2 = map.getMatrix("view2");
        EnsuresEqual(matrix1.rows(), 101);
        EnsuresEqual(matrix1.columns(), 5);
        EnsuresEqual(matrix2.rows(), 101);
        EnsuresEqual(matrix2.columns(), 4);

    }

}

void build_linear_combination_size()
{
    auto ctx = vle::utils::make_context(); vle::utils::Package pack(ctx, "vle.examples");
    std::unique_ptr<vpz::Vpz> file(
            new vpz::Vpz(pack.getExpFile("unittest.vpz")));

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


    manager::Manager r(ctx,manager::LOG_NONE, manager::SIMULATION_NONE, NULL);
    manager::Error error;
    std::unique_ptr<value::Matrix> out = 0;
    EnsuresThrow(out = r.run(std::move(file),1, 0, 1, &error),
            std::logic_error);

}


void build_linear_output_matrix_size()
{
    auto ctx = vle::utils::make_context(); vle::utils::Package pack(ctx, "vle.examples");
    std::unique_ptr<vpz::Vpz> file(
            new vpz::Vpz(pack.getExpFile("unittest.vpz")));

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


    manager::Manager r(ctx, manager::LOG_NONE, manager::SIMULATION_NONE, NULL);
    manager::Error error;
    std::unique_ptr<value::Matrix> out = r.run(std::move(file),
            1, 0, 1, &error);

    Ensures(not error.code);
    Ensures(out);

    EnsuresEqual(out->size(), (value::Matrix::size_type)5);

    for (value::Matrix::size_type y = 0; y < out->size(); ++y) {
        value::Map &map = out->getMap(y, 0);

        EnsuresEqual(map.size(), (value::Matrix::size_type)2);

        Ensures(map.exist("view1"));
        Ensures(map.exist("view2"));
        const value::Matrix& matrix1 = map.getMatrix("view1");
        const value::Matrix& matrix2 = map.getMatrix("view2");
        EnsuresEqual(matrix1.rows(), 101);
        EnsuresEqual(matrix1.columns(), 5);
        EnsuresEqual(matrix2.rows(), 101);
        EnsuresEqual(matrix2.columns(), 4);

        EnsuresEqual(100, matrix1.getDouble(0,100));
        EnsuresEqual(101, matrix1.getInt(1,100));
        EnsuresEqual(101, matrix1.getInt(2,100));
        EnsuresEqual(201, matrix1.getInt(3,100));

    }

}


void manager_thread_result_access()
{
    using namespace manager;
    auto ctx = vle::utils::make_context(); vle::utils::Package pack(ctx, "vle.examples");
    std::unique_ptr<vpz::Vpz> file(new vpz::Vpz(
            pack.getExpFile("unittest.vpz")));
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
        view1.setStream("", "storage", "vle.output");
    }
    vle::vpz::Output& view2 = vle_views.outputs().get(vle_views.get("view2").output());
    if (view2.plugin() != "storage") {
        view2.setStream("", "storage", "vle.output");
    }


    manager::Manager r(ctx, manager::LOG_NONE, manager::SIMULATION_NONE, NULL);
    manager::Error error;
    std::unique_ptr<value::Matrix> out = r.run(std::move(file),
            2, 0, 1, &error);

    Ensures(out);
    EnsuresEqual(out->size(), (value::Matrix::size_type)2);

    for (value::Matrix::size_type y = 0; y < out->size(); ++y) {
        Ensures(out->get(y, 0));
        value::Map &map = out->getMap(y, 0);

        //check number of views
        EnsuresEqual(map.size(), (value::Map::size_type)2);

        Ensures(map.exist("view1"));
        Ensures(map.exist("view2"));
        const value::Matrix& matrix1 = map.getMatrix("view1");
        const value::Matrix& matrix2 = map.getMatrix("view2");
        EnsuresEqual(matrix1.rows(), 2);
        EnsuresEqual(matrix1.columns(), 5);
        EnsuresEqual(matrix2.rows(), 2);
        EnsuresEqual(matrix2.columns(), 4);
    }

}

void manager_thread_fast_producer()
{
    using namespace manager;
    auto ctx = vle::utils::make_context(); vle::utils::Package pack(ctx, "vle.examples");
    std::unique_ptr<vpz::Vpz> file(
            new vpz::Vpz(pack.getExpFile("unittest.vpz")));

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
        view1.setStream("", "storage", "vle.output");
    }
    vle::vpz::Output& view2 = vle_views.outputs().get(vle_views.get("view2").output());
    if (view2.plugin() != "storage") {
        view2.setStream("", "storage", "vle.output");
    }


    manager::Manager r(ctx, manager::LOG_NONE, manager::SIMULATION_NONE, NULL);
    manager::Error error;
    std::unique_ptr<value::Matrix> out = r.run(std::move(file),
            2, 0, 1, &error);

    Ensures(out);

    EnsuresEqual(out->size(), (value::Matrix::size_type)2);

}

void manager_thread_fast_consumer()
{
    /*
     * TODO parfois une errur au test
     * unknown location(0): fatal error in "manager_thread_fast_consumer":
     * signal: integer divide by zero; address of failing instruction: 0x7f8dbcd94a56
     * /pub/src/vleapi14/packages/vle.examples/test/testmanager.cpp(289): last checkpoint
     */

    using namespace manager;
    auto ctx = vle::utils::make_context(); vle::utils::Package pack(ctx, "vle.examples");
    std::unique_ptr<vpz::Vpz> file(
            new vpz::Vpz(pack.getExpFile("unittest.vpz")));

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
        view1.setStream("", "storage", "vle.output");
    }
    vle::vpz::Output& view2 = vle_views.outputs().get(vle_views.get("view2").output());
    if (view2.plugin() != "storage") {
        view2.setStream("", "storage", "vle.output");
    }


    manager::Manager r(ctx, manager::LOG_NONE, manager::SIMULATION_NONE, NULL);
    manager::Error error;
    std::unique_ptr<value::Matrix> out = r.run(std::move(file),
            2, 0, 1, &error);

    Ensures(out);

    EnsuresEqual(out->size(), (value::Matrix::size_type)2);

}


int main()
{
    F fixture;
    build_experimental_frames();
    build_linear_combination_size();
    build_linear_output_matrix_size();
    manager_thread_result_access();
    manager_thread_fast_producer();
    manager_thread_fast_consumer();

    return unit_test::report_errors();
}
