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

    vv::Map init;
    init.addString("config_parallel_type","threads");
    init.addInt("config_parallel_nbslots",2);
    init.addString("id_package","vle.recursive_test");
    init.addString("id_vpz","ExBohachevsky.vpz");
    init.addString("id_input_x1", "cond/x1");
    init.addString("id_input_x2", "cond/x2");
    init.addString("id_output_y", "last[view/ExBohachevsky:ExBohachevsky.y]");
    init.addString("id_output_y_noise",
            "last[view/ExBohachevsky:ExBohachevsky.y_noise]");
    init.addString("id_replica_r","cond/seed");
    vv::Tuple x1(2);
    x1[0] = 3.0;
    x1[1] = 0.0;
    vv::Tuple x2(2);
    x2[0] = -10.0;
    x2[1] = 0.0;
    vv::Set r;
    r.addInt(1235);
    r.addInt(7234);
    r.addInt(9531);
    init.add("values_x1",x1);
    init.add("values_x2",x2);
    init.add("values_r",r);
    vr::MetaManager meta;
    meta.init(init);
    const vv::Matrix& res = meta.launchSimulations().toMatrix();
    BOOST_REQUIRE_CLOSE(res.getDouble(0/*col*/,0),209.6,10e-4);
    BOOST_REQUIRE_CLOSE(res.getDouble(1,0),209.6486,10e-4);
    BOOST_REQUIRE_CLOSE(res.getDouble(0,1),0.0,10e-4);
    BOOST_REQUIRE_CLOSE(res.getDouble(1,1),0.04861,10e-3);
}
