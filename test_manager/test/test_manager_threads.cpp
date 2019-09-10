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

void test_logEx_threads()
{
    namespace vv = vle::value;
    namespace vm = vle::manager;

    //define x values
    vv::Tuple x(2);
    x[0] = 3.0;
    x[1] = 0.0;

    //define y observations
    vv::Tuple y_obs(3);
    y_obs[0] = 2.7;
    y_obs[1] = 3.42;
    y_obs[2] = 3.75;

    //define y observation times
    vv::Tuple y_times(3);
    y_times[0] = 4;
    y_times[1] = 10;
    y_times[2] = 16;

    vv::Map init;
    init.addString("paprallel_option","mono");
    init.addInt("nb_slots",1);
    init.addString("package","test_manager");
    init.addString("vpz","LogEx.vpz");
    init.add("input_cond.init_value_x", x.clone());
    vv::Map& conf_y = init.addMap("output_msey");
    conf_y.addString("path", "view/LogEx:LogEx.y");
    conf_y.addString("integration","mse");
    conf_y.add("mse_observations", y_obs.clone());
    conf_y.add("mse_times", y_times.clone());

    auto ctx = vle::utils::make_context();
    vm::Manager manager(ctx);

    vle::manager::Error err;
    std::unique_ptr<vv::Map> res = manager.runPlan(init, err);

    if (err.code ==-1) {
        std::cout << " error: " << err.message << "\n";
    }

    std::cout << " res : " << *res << "\n";

    Ensures(res->getTable("msey").width() ==  2);//2 inputs
    Ensures(res->getTable("msey").height() ==  1);//mse
    EnsuresApproximatelyEqual(res->getTable("msey")(0/*col*/,0),
            3.96593356281757,10e-3);
    EnsuresApproximatelyEqual(res->getTable("msey")(1/*col*/,0),
            1.0248405042402,10e-3);
}

void test_ExBohachevsky_threads()
{
    namespace vv = vle::value;
    namespace vm = vle::manager;

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

    vv::Map init;
    init.addString("paprallel_option","threads");
    init.addInt("nb_slots",3);
    init.addString("package","test_manager");
    init.addString("vpz","ExBohachevsky.vpz");
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

    auto ctx = vle::utils::make_context();
    vm::Manager manager(ctx);

    vle::manager::Error err;
    std::unique_ptr<vv::Map> res = manager.runPlan(init, err);

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
    test_logEx_threads();
    test_ExBohachevsky_threads();

    return unit_test::report_errors();
}
