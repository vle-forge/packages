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

void test_api()
{
    namespace vv = vle::value;
    namespace vm = vle::manager;

    //define x1 values
    vv::Tuple x1(2);
    x1[0] = 3.0;
    x1[1] = 0.0;
    //define x2 values
    vv::Tuple x2(2);
    x2[0] = -10.0;
    x2[1] = 0.0;
    //define replicate values
    vv::Set r;
    r.addInt(1235);
    r.addInt(7234);
    r.addInt(9531);

    vv::Map config;
    config.addString("parallel_option","mono");
    config.addString("working_dir","/tmp/");
    config.addInt("nb_slots",1);

    vv::Map init;
    init.addString("package","test_manager");
    init.addString("vpz","ExBohachevsky.vpz");
    init.add("input_cond.x1", x1.clone());
    init.add("input_cond.x2", x2.clone());
    init.addString("output_y", "view/ExBohachevsky:ExBohachevsky.y");
    init.addString("output_ynoiseFinish",
            "view/ExBohachevsky:ExBohachevsky.y_noise");
    vv::Map& conf_ynoise = init.addMap("output_ynoise");
    conf_ynoise.addString("path",
            "viewNoise/ExBohachevsky:ExBohachevsky.y_noise");
    conf_ynoise.addString("integration","all");

    init.add("replicate_cond.seed",r.clone());

    auto ctx = vle::utils::make_context();
    vm::Manager manager(ctx, config);

    vle::manager::Error err;
    std::unique_ptr<vv::Map> res = manager.runPlan(init, err);

    if (err.code ==-1) {
        std::cout << " error: " << err.message << "\n";
    }

    std::cout << " res : " << *res << "\n";

    Ensures(res->getTable("ynoise").width() ==  2);//2 inputs
    Ensures(res->getTable("ynoise").height() ==  11);//duration+1
    Ensures(res->getTable("y").width() ==  2);
    Ensures(res->getTable("ynoiseFinish").height() ==  1);
    Ensures(res->getTable("ynoiseFinish").width() ==  2);
    Ensures(res->getTable("y").height() ==  1);
    EnsuresApproximatelyEqual(res->getTable("ynoise")(0/*col*/,0), 209.60005,10e-4);
    EnsuresApproximatelyEqual(res->getTable("ynoiseFinish")(0,0),209.60005,10e-4);
    EnsuresApproximatelyEqual(res->getTable("y")(0,0),209.6,10e-4);
    EnsuresApproximatelyEqual(res->getTable("ynoise")(1,0),5.43077761310471e-05,10e-4);
    EnsuresApproximatelyEqual(res->getTable("ynoiseFinish")(1,0),
            5.43077761310471e-05,10e-4);
    EnsuresApproximatelyEqual(res->getTable("y")(1,0),0.0,10e-4);
}

void test_complex_values()
{
    namespace vm = vle::manager;
    namespace vv = vle::value;

    //define x1 values
    vv::Tuple x1(2);
    x1[0] = 3.0;
    x1[1] = 0.0;
    //define x2 values
    vv::Tuple x2(2);
    x2[0] = -10.0;
    x2[1] = 0.0;

    vv::Map config;
    config.addString("parallel_option","threads");
    config.addString("working_dir","/tmp/");
    config.addInt("nb_slots",1);

    vv::Map init;
    init.addString("package","test_manager");
    init.addString("vpz","ExBohachevsky.vpz");
    init.add("input_cond.x1", x1.clone());
    init.add("input_cond.x2", x2.clone());
    vv::Map& conf_yall = init.addMap("output_yall");
    conf_yall.addString("path",
            "allinOne/ExBohachevsky:ExBohachevsky.y_all");
    conf_yall.addString("integration","last");
    conf_yall.addString("aggregation_input","all");

    init.addInt("propagate_cond.seed",1235);

    auto ctx = vle::utils::make_context();
    vm::Manager manager(ctx, config);

    vle::manager::Error err;
    std::unique_ptr<vv::Map> res = manager.runPlan(init, err);

    if (err.code ==-1) {
        std::cout << " error: " << err.message << "\n";
    }

    std::cout << " res : " << *res << "\n";

    Ensures(res->getMatrix("yall").rows() ==  1);//integration last
    Ensures(res->getMatrix("yall").columns() ==  2);//2 inputs
    double v = res->getMatrix("yall").getMap(0,0).getDouble("with_noise");
    EnsuresApproximatelyEqual(v, 209.643,10e-4);
    v = res->getMatrix("yall").getMap(0,0).getDouble("without_noise");
    EnsuresApproximatelyEqual(v, 209.6,10e-4);
    v = res->getMatrix("yall").getMap(1,0).getDouble("with_noise");
    EnsuresApproximatelyEqual(v, 0.0427962,10e-4);
    v = res->getMatrix("yall").getMap(1,0).getDouble("without_noise");
    EnsuresApproximatelyEqual(v, 0.0,10e-4);
}

void test_SIR()
{
    std::string conf_simu = "single"; int nb_slots=1;
    //std::string conf_simu = "threads"; int nb_slots=2;
    //std::string conf_simu = "mpi"; int nb_slots=4;

    namespace vm = vle::manager;
    namespace vv = vle::value;

    {//multiple simulation on init_value_S
        vv::Map config;
        config.addString("parallel_option",conf_simu);
        config.addString("working_dir","/tmp/");
        config.addInt("nb_slots",nb_slots);

        vv::Map init;
        init.addString("package","test_manager");
        init.addString("vpz","SIR.vpz");
        init.addString("output_Sfinal", "view/top:SIR.S");
        vv::Map& conf_out = init.addMap("output_Sfinal");
        conf_out.addString("path", "view/top:SIR.S");
        conf_out.addString("integration", "last");
        vv::Tuple& Svalues = init.addTuple(
                "input_condSIR.init_value_S", 5, 0.0);

        Svalues[0] = 150;
        Svalues[1] = 120;
        Svalues[2] = 99;
        Svalues[3] = 75;
        Svalues[4] = 50;

        auto ctx = vle::utils::make_context();
        vm::Manager manager(ctx, config);

        vle::manager::Error err;
        std::unique_ptr<vv::Map> res = manager.runPlan(init, err);


        if (err.code ==-1) {
            std::cout << " error: " << err.message << "\n";
        }

        std::cout << " Results1 : " << *res << "\n";

        Ensures(res->getTable("Sfinal").width() ==  5);
        Ensures(res->getTable("Sfinal").height() ==  1);
        EnsuresApproximatelyEqual(res->getTable("Sfinal")(0,0), 9.2498,10e-4);
        EnsuresApproximatelyEqual(res->getTable("Sfinal")(1,0), 18.4088,10e-4);
        EnsuresApproximatelyEqual(res->getTable("Sfinal")(2,0), 31.4078,10e-4);
        EnsuresApproximatelyEqual(res->getTable("Sfinal")(3,0), 49.1268,10e-4);
        EnsuresApproximatelyEqual(res->getTable("Sfinal")(4,0), 44.8678,10e-4);
    }

    {//multiple replicate on seed and multiple inputs on init_value_S
        vv::Map config;
        config.addString("parallel_option",conf_simu);
        config.addInt("nb_slots",nb_slots);
        config.addString("working_dir","/tmp/");

        vv::Map init;
        init.addString("package","test_manager");
        init.addString("vpz","SIRnoise.vpz");
        //config output, mean on replicate, all on inputs
        vv::Map& conf_out = init.addMap("output_Sfinal");
        conf_out.addString("path", "view/top:SIRnoise.S");
        conf_out.addString("integration", "last");
        conf_out.addString("aggregation_replicate", "mean");
        conf_out.addString("aggregation_input", "all");
        //set 5 input values
        vv::Tuple& Svalues = init.addTuple(
                "input_condSIRnoise.init_value_S", 5, 0.0);
        Svalues[0] = 150;
        Svalues[1] = 120;
        Svalues[2] = 99;
        Svalues[3] = 75;
        Svalues[4] = 50;
        //set 6 seeds
        vv::Tuple& seeds = init.addTuple(
                "replicate_condSIRnoise.init_value_seed", 6, 0.0);
        seeds[0] = 45694;
        seeds[1] = 55695;
        seeds[2] = 65696;
        seeds[3] = 85698;
        seeds[4] = 95699;

        auto ctx = vle::utils::make_context();
        vm::Manager manager(ctx, config);

        vle::manager::Error err;
        std::unique_ptr<vv::Map> res = manager.runPlan(init, err);

        if (err.code ==-1) {
            std::cout << " error: " << err.message << "\n";
        }
        std::cout << " Results2 (close to Results1): " << *res << "\n";

        Ensures(res->getTable("Sfinal").width() ==  5);
        Ensures(res->getTable("Sfinal").height() ==  1);
        //results below should be close to the first test
        EnsuresApproximatelyEqual(res->getTable("Sfinal")(0,0), 11.5526,10e-4);
        EnsuresApproximatelyEqual(res->getTable("Sfinal")(1,0), 22.2598,10e-4);
        EnsuresApproximatelyEqual(res->getTable("Sfinal")(2,0), 36.1712,10e-4);
        EnsuresApproximatelyEqual(res->getTable("Sfinal")(3,0), 51.3126,10e-4);
        EnsuresApproximatelyEqual(res->getTable("Sfinal")(4,0), 45.0968,10e-4);
    }

    {//compute mse on multiple beta parameters
        vv::Map config;
        config.addString("parallel_option",conf_simu);
        config.addInt("nb_slots",nb_slots);
        config.addString("working_dir","/tmp/");

        vv::Map init;
        init.addString("package","test_manager");
        init.addString("vpz","SIR.vpz");
        //config output, mse on I
        vv::Map& conf_out = init.addMap("output_mseI");
        conf_out.addString("path", "view/top:SIR.I");
        conf_out.addString("integration", "mse");
        vv::Tuple& mseTimes = conf_out.addTuple("mse_times", 3,0.0);
        mseTimes[0] = 20;
        mseTimes[1] = 30;
        mseTimes[2] = 40;
        vv::Tuple& mseObs = conf_out.addTuple("mse_observations", 3,0.0);
        mseObs[0] = 6;
        mseObs[1] = 10;
        mseObs[2] = 15;
        conf_out.addString("aggregation_input", "all");

        //config output, I values
        vv::Map& conf_I = init.addMap("output_I");
        conf_I.addString("path", "view/top:SIR.I");
        conf_I.addString("integration", "all");
        conf_I.addString("aggregation_input", "all");

        //set 3 input values for beta parameter
        vv::Tuple& beta = init.addTuple(
                "input_condSIR.init_value_beta", 3, 0.0);
        beta[0] = 0.001;
        beta[1] = 0.002;
        beta[2] = 0.003;

        auto ctx = vle::utils::make_context();
        vm::Manager manager(ctx, config);

        vle::manager::Error err;
        std::unique_ptr<vv::Map> res = manager.runPlan(init, err);
        if (err.code ==-1) {
            std::cout << " error: " << err.message << "\n";
        }

        std::cout << " Mse for beta in (0.001,0.002,0.003): " << res->getTable("mseI") << "\n";

        Ensures(res->getTable("mseI").width() ==  3);
        Ensures(res->getTable("mseI").height() ==  1);
        EnsuresApproximatelyEqual(res->getTable("mseI")(0,0), 102.227, 10e-4);
        EnsuresApproximatelyEqual(res->getTable("mseI")(1,0), 0.420443,10e-4);
        EnsuresApproximatelyEqual(res->getTable("mseI")(2,0), 238.222,10e-4);

    }

}


int main()
{
    test_api();
    test_complex_values();
    test_SIR();

    return unit_test::report_errors();
}
