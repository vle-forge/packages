/*
 * This file is part of VLE, a framework for multi-modeling, simulation
 * and analysis of complex dynamical systems.
 * http://www.vle-project.org
 *
 * Copyright (c) 2003-2013 Gauthier Quesnel <quesnel@users.sourceforge.net>
 * Copyright (c) 2003-2013 ULCO http://www.univ-littoral.fr
 * Copyright (c) 2007-2013 INRA http://www.inra.fr
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


#define BOOST_TEST_MAIN
#define BOOST_AUTO_TEST_MAIN
#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE package_test
#include <boost/test/unit_test.hpp>
#include <boost/test/auto_unit_test.hpp>
#include <boost/test/floating_point_comparison.hpp>
#include <vle/value/Map.hpp>
#include <vle/utils/Package.hpp>
#include <vle/recursive/EmbeddedSimulatorSingle.hpp>


//rr::EmbeddedSimulatorPlan
BOOST_AUTO_TEST_CASE(test_simulator)
{
    namespace rr = vle::recursive;
    namespace vv = vle::value;

    vv::Map init;
    init.addString("package","vle.recursive_test");
    init.addString("vpz","ExBohachevsky.vpz");
    vv::Set& inputs = init.addSet("inputs");
    inputs.addString("cond/x1");
    inputs.addString("cond/x2");
    vv::Set& outputs = init.addSet("outputs");
    outputs.addString("view/ExBohachevsky:ExBohachevsky.y");
    rr::EmbeddedSimulatorSingle sim;
    sim.init(init);
    vv::Set point;
    point.addDouble(3);
    point.addDouble(-10);
    vv::Double res;
    sim.simulate(point);
    sim.fillWithLastValuesOfOutputs(res);
    double out = res.value();
    BOOST_REQUIRE_CLOSE(out,209.6,10e-4);
}

