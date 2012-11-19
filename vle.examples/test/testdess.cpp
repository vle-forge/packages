/*
 * @file examples/test/testdess.cpp
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
#define BOOST_TEST_MODULE test_dess_extension

#include <boost/test/unit_test.hpp>
#include <boost/test/auto_unit_test.hpp>
#include <boost/test/floating_point_comparison.hpp>
#include <boost/lexical_cast.hpp>
#include <stdexcept>
#include <vle/manager/Manager.hpp>
#include <vle/manager/Simulation.hpp>
#include <vle/vpz/Vpz.hpp>
#include <vle/utils/Package.hpp>
#include <vle/utils/Path.hpp>
#include <vle/utils/ModuleManager.hpp>
#include <vle/vle.hpp>

struct F
{
    F()
    {
        vle::utils::Package::package().select("vle.examples");
    }

    vle::Init app;
};

BOOST_GLOBAL_FIXTURE(F)


using namespace vle;

BOOST_AUTO_TEST_CASE(test_dess1)
{
    vpz::Vpz *file = new vpz::Vpz(utils::Path::path().getPackageExpFile("sir2.vpz"));

    vpz::Output& o(file->project().experiment().views().outputs().get("o3"));
    o.setLocalStream("", "storage", "vle.output");

    utils::ModuleManager man;
    manager::Error error;
    manager::Simulation sim(manager::LOG_NONE,
                            manager::SIMULATION_NONE,
                            NULL);
    value::Map *out = sim.run(file, man, &error);

    BOOST_REQUIRE_EQUAL(error.code, 0);
    BOOST_REQUIRE(out != NULL);
    BOOST_REQUIRE_EQUAL(out->size(), 1);

    value::Matrix &result = out->getMatrix("dessview");

    BOOST_REQUIRE_EQUAL(result.columns(), (value::MatrixView::size_type)6);
    BOOST_REQUIRE_EQUAL(result.rows(), (value::MatrixView::size_type)2000);

    BOOST_REQUIRE_CLOSE(value::toDouble(result.get(4, 319)), 3.185, 10e-5);
    BOOST_REQUIRE_CLOSE(value::toDouble(result.get(5, 319)), 1.0, 10e-5);

    BOOST_REQUIRE_CLOSE(value::toDouble(result.get(4, 483)), 4.82, 10e-5);
    BOOST_REQUIRE_CLOSE(value::toDouble(result.get(5, 483)), 2.0, 10e-5);

    delete out;
}

BOOST_AUTO_TEST_CASE(test_dess2)
{
    vpz::Vpz *file = new vpz::Vpz(utils::Path::path().getPackageExpFile("sir3.vpz"));

    vpz::Output& o(file->project().experiment().views().outputs().get("o5"));
    o.setLocalStream("", "storage", "vle.output");

    utils::ModuleManager man;
    manager::Error error;
    manager::Simulation sim(manager::LOG_NONE,
                            manager::SIMULATION_NONE,
                            NULL);
    value::Map *out = sim.run(file, man, &error);

    BOOST_REQUIRE_EQUAL(error.code, 0);
    BOOST_REQUIRE(out != NULL);
    BOOST_REQUIRE_EQUAL(out->size(), 1);

    value::Matrix &result = out->getMatrix("dessview");

    BOOST_REQUIRE_EQUAL(result.columns(), (value::MatrixView::size_type)6);
    BOOST_REQUIRE_EQUAL(result.rows(), (value::MatrixView::size_type)2000);

    BOOST_REQUIRE_CLOSE(value::toDouble(result.get(4, 321)), 3.2030279, 10e-2);
    BOOST_REQUIRE_CLOSE(value::toDouble(result.get(5, 321)), 1.0, 10e-2);

    BOOST_REQUIRE_CLOSE(value::toDouble(result.get(4, 483)), 4.8268177, 10e-2);
    BOOST_REQUIRE_CLOSE(value::toDouble(result.get(5, 483)), 2.0, 10e-2);
}
