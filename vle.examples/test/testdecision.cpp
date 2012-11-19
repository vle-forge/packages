/*
 * @file examples/test/testdecision.cpp
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
#define BOOST_TEST_MODULE test_decision_extension

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
#include <iostream>

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

BOOST_AUTO_TEST_CASE(test_agentonly)
{
    vpz::Vpz *file = new vpz::Vpz(utils::Path::path().getPackageExpFile("agentonly.vpz"));

    utils::ModuleManager man;
    manager::Error error;
    manager::Simulation sim(manager::LOG_NONE,
                            manager::SIMULATION_NONE,
                            NULL);
    value::Map *out = sim.run(file, man, &error);

    BOOST_REQUIRE_EQUAL(error.code, 0);
    BOOST_REQUIRE(out != NULL);
    BOOST_REQUIRE_EQUAL(out->size(), 1);

    value::Matrix &result = out->getMatrix("storage");

    BOOST_REQUIRE_EQUAL(result.columns(), (value::MatrixView::size_type)2);
    BOOST_REQUIRE_EQUAL(result.rows(), (value::MatrixView::size_type)11);

    BOOST_REQUIRE_EQUAL(result.getInt(1, 0), 1);
    BOOST_REQUIRE_EQUAL(result.getInt(1, 1), 4);
    BOOST_REQUIRE_EQUAL(result.getInt(1, 2), 7);
    BOOST_REQUIRE_EQUAL(result.getInt(1, 3), 8);
    BOOST_REQUIRE_EQUAL(result.getInt(1, 4), 8);
    BOOST_REQUIRE_EQUAL(result.getInt(1, 5), 8);
    BOOST_REQUIRE_EQUAL(result.getInt(1, 6), 8);
    BOOST_REQUIRE_EQUAL(result.getInt(1, 7), 8);
    BOOST_REQUIRE_EQUAL(result.getInt(1, 8), 8);
    BOOST_REQUIRE_EQUAL(result.getInt(1, 9), 8);
    BOOST_REQUIRE_EQUAL(result.getInt(1, 10), 9);

    delete out;
}

BOOST_AUTO_TEST_CASE(test_agentonlyprecedenceconstraint)
{
    vpz::Vpz *file = new vpz::Vpz(utils::Path::path().getPackageExpFile("agentonlyc.vpz"));

    utils::ModuleManager man;
    manager::Error error;
    manager::Simulation sim(manager::LOG_NONE,
                            manager::SIMULATION_NONE,
                            NULL);
    value::Map *out = sim.run(file, man, &error);

    BOOST_REQUIRE_EQUAL(error.code, 0);
    BOOST_REQUIRE(out != NULL);
    BOOST_REQUIRE_EQUAL(out->size(), 1);

    value::Matrix &result = out->getMatrix("storage");

    BOOST_REQUIRE_EQUAL(result.getInt(1, 0), 1);
    BOOST_REQUIRE_EQUAL(result.getInt(1, 1), 2);
    BOOST_REQUIRE_EQUAL(result.getInt(1, 2), 3);
    BOOST_REQUIRE_EQUAL(result.getInt(1, 3), 3);
    BOOST_REQUIRE_EQUAL(result.getInt(1, 4), 3);
    BOOST_REQUIRE_EQUAL(result.getInt(1, 5), 3);
    BOOST_REQUIRE_EQUAL(result.getInt(1, 6), 3);
    BOOST_REQUIRE_EQUAL(result.getInt(1, 7), 3);
    BOOST_REQUIRE_EQUAL(result.getInt(1, 8), 3);
    BOOST_REQUIRE_EQUAL(result.getInt(1, 9), 3);
    BOOST_REQUIRE_EQUAL(result.getInt(1, 10), 4);

    delete out;
}

BOOST_AUTO_TEST_CASE(test_agentonlywakeup)
{
    vpz::Vpz *file = new vpz::Vpz(utils::Path::path().getPackageExpFile("agentonlywakeup.vpz"));

    utils::ModuleManager man;
    manager::Error error;
    manager::Simulation sim(manager::LOG_NONE,
                            manager::SIMULATION_NONE,
                            NULL);
    value::Map *out = sim.run(file, man, &error);

    BOOST_REQUIRE_EQUAL(error.code, 0);
    BOOST_REQUIRE(out != NULL);
    BOOST_REQUIRE_EQUAL(out->size(), 1);

    value::Matrix &result = out->getMatrix("storage");

    BOOST_REQUIRE_EQUAL(result.getInt(1, 0), 0);
    BOOST_REQUIRE_EQUAL(result.getInt(1, 1), 1);
    BOOST_REQUIRE_EQUAL(result.getInt(1, 2), 1);
    BOOST_REQUIRE_EQUAL(result.getInt(1, 3), 1);
    BOOST_REQUIRE_EQUAL(result.getInt(1, 4), 1);
    BOOST_REQUIRE_EQUAL(result.getInt(1, 5), 1);
    BOOST_REQUIRE_EQUAL(result.getInt(1, 6), 2);
    BOOST_REQUIRE_EQUAL(result.getInt(1, 7), 2);
    BOOST_REQUIRE_EQUAL(result.getInt(1, 8), 2);
    BOOST_REQUIRE_EQUAL(result.getInt(1, 9), 2);
    BOOST_REQUIRE_EQUAL(result.getInt(1, 10), 2);

    delete out;
}

