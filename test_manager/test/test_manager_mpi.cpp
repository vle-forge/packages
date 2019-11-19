/*
 * This file is part of VLE, a framework for multi-modeling, simulation
 * and analysis of complex dynamical systems.
 * http://www.vle-project.org
 *
 * Copyright (c) 2014-2014 INRA http://www.inra.fr
 *
 * See the AUTHORS or Authors.txt file for copyright owners and
 * contributors
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or (at
 * your option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

//@@tagtest@@
//@@tagdepends: @@endtagdepends


#include <vle/value/Map.hpp>
#include <vle/value/Tuple.hpp>
#include <vle/value/Set.hpp>
#include <vle/value/Matrix.hpp>
#include <vle/value/Table.hpp>
#include <vle/utils/unit-test.hpp>
#include <vle/utils/Package.hpp>
#include <vle/manager/Manager.hpp>

#include <iostream>

void test_ExBohachevsky_mpi()
{
    namespace vv = vle::value;
    namespace vm = vle::manager;
    namespace vz = vle::vpz;
    namespace vu = vle::utils;

    //build model
    auto ctx = vle::utils::make_context();
    vu::Package pkg(ctx, "test_manager");
    std::string vpzFile = pkg.getExpFile(
            "ExBohachevsky.vpz", vle::utils::PKG_BINARY);
    std::unique_ptr<vz::Vpz> model(new vz::Vpz(vpzFile));

    //define x1 values
    vv::Tuple x1(2);
    x1[0] = 3.0;
    x1[1] = 0.0;

    //define x2 values
    vv::Tuple x2(2);
    x2[0] = -10;
    x2[1] = 0.0;

    //define seed values
    vv::Set seeds;
    seeds.addInt(1235);
    seeds.addInt(7234);
    seeds.addInt(9531);

    vv::Map config;
    config.addString("parallel_option","mpi");
    config.addString("working_dir","/tmp/");
    config.addInt("nb_slots",3);

    vv::Map init;
    init.addString("package","test_manager");
    init.add("input_cond.x1", x1.clone());
    init.add("input_cond.x2", x2.clone());
    init.add("replicate_cond.seed", seeds.clone());
    vv::Map& conf_y = init.addMap("output_y");
    conf_y.addString("path", "view/ExBohachevsky:ExBohachevsky.y");
    conf_y.addString("integration","max");
    conf_y.addString("aggregation_replicate","mean");

    vv::Map& conf_ynoise = init.addMap("output_ynoise");
    conf_ynoise.addString("path", "view/ExBohachevsky:ExBohachevsky.y_noise");
    conf_ynoise.addString("integration","max");
    //conf_ynoise.addString("aggregation_replicate","quantile");

    vm::Manager manager(ctx, config);

    vle::manager::Error err;
    std::unique_ptr<vv::Map> res = manager.runPlan(
            std::move(model), init, err);

    if (err.code ==-1) {
        std::cout << " error: " << err.message << "\n";
    }

    std::cout << " res : " << *res << "\n";

    Ensures(res->getTable("y").width() ==  2);//2 inputs
    Ensures(res->getTable("y").height() ==  1);//max value
    EnsuresApproximatelyEqual(res->getTable("y")(0/*col*/,0),
            209.6,10e-3);
    EnsuresApproximatelyEqual(res->getTable("y")(1/*col*/,0),
            0,10e-3);
    Ensures(res->getTable("ynoise").width() ==  2);//2 inputs
    Ensures(res->getTable("ynoise").height() ==  1);//max value
    EnsuresApproximatelyEqual(res->getTable("ynoise")(0/*col*/,0),
            209.60005,10e-3);
    EnsuresApproximatelyEqual(res->getTable("ynoise")(1/*col*/,0),
            5.43077761310471e-05,10e-3);
}


int main()
{
    test_ExBohachevsky_mpi();

    return unit_test::report_errors();
}
