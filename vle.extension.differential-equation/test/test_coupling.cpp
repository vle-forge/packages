/**
 * @file vle.extension.differential-equation/test/test_coupling.cpp
 * @author The VLE Development Team
 */

/*
 * VLE Environment - the multimodeling and simulation environment
 * This file is a part of the VLE environment (http://vle.univ-littoral.fr)
 * Copyright (C) 2003 - 2009 The VLE Development Team
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

#include "test_common.hpp"

/******************
 *  Unit test for testing coupling of different integration methods and
 *  simultaneous perturbations, only the success of simulation is
 *  tested
 ******************/
BOOST_AUTO_TEST_CASE(test_coupling)
{
    std::cout << "  test_coupling " << std::endl;
    vle::utils::Package pack("vle.extension.differential-equation");
    vz::Vpz vpz(pack.getExpFile("PerturbSeirXY.vpz", vle::utils::PKG_BINARY));


    ttconfOutputPlugins(vpz);

    //simulation
    vu::ModuleManager man;
    vm::Error error;
    vm::Simulation sim(vm::LOG_NONE, vm::SIMULATION_NONE, NULL);
    va::Map *out = sim.run(new vz::Vpz(vpz), man, &error);

    //checks that simulation has succeeded
    BOOST_REQUIRE_EQUAL(error.code, 0);
    //checks the number of views
    BOOST_REQUIRE_EQUAL(out->size(),1);

    delete out;
}
