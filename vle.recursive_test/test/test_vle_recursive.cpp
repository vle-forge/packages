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
//@@tagdepends: vle.recursive @@endtagdepends

#include <iostream>
#define BOOST_TEST_MAIN
#define BOOST_AUTO_TEST_MAIN
#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE package_test
#include <boost/test/unit_test.hpp>
#include <boost/test/auto_unit_test.hpp>
#include <boost/test/floating_point_comparison.hpp>
#include <vle/value/Map.hpp>
#include <vle/utils/Package.hpp>
#include <vle/recursive/MetaManager.hpp>


BOOST_AUTO_TEST_CASE(test_api)
{
    namespace vr = vle::recursive;
    namespace vv = vle::value;

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

    vv::Map init;
    init.addString("config_parallel_type","threads");
    init.addInt("config_parallel_nbslots",2);
    init.addString("package","vle.recursive_test");
    init.addString("vpz","ExBohachevsky.vpz");
    init.add("input_cond.x1", x1);
    init.add("input_cond.x2", x2);
    init.addString("output_y", "view/ExBohachevsky:ExBohachevsky.y");
    init.addString("output_ynoise",
            "view/ExBohachevsky:ExBohachevsky.y_noise");
    init.add("replicate_cond.seed",r);
    vr::MetaManager meta;
    vv::Matrix* res = meta.run(init);
    BOOST_REQUIRE_CLOSE(res->getDouble(0/*col*/,1),209.6,10e-4);
    BOOST_REQUIRE_CLOSE(res->getDouble(1,1),209.6486,10e-4);
    BOOST_REQUIRE_CLOSE(res->getDouble(0,2),0.0,10e-4);
    BOOST_REQUIRE_CLOSE(res->getDouble(1,2),0.04861,10e-3);
    delete res;
}
