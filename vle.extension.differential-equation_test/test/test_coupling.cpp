/**
 * @file vle.extension.differential-equation_test/test/test_coupling.cpp
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
    auto ctx = vu::make_context();
    std::cout << "  test_coupling " << std::endl;
    vle::utils::Package pack(ctx, "vle.extension.differential-equation_test");
    std::unique_ptr<vz::Vpz> vpz(new vz::Vpz(pack.getExpFile("PerturbSeirXY.vpz", vle::utils::PKG_BINARY)));


    ttconfOutputPlugins(*vpz);

    //simulation
    vm::Error error;
    vm::Simulation sim(ctx, vm::LOG_NONE, vm::SIMULATION_NONE,
            std::chrono::milliseconds(0), &std::cout);
    std::unique_ptr<va::Map> out = sim.run(std::move(vpz), &error);

    //checks that simulation has succeeded
    BOOST_REQUIRE_EQUAL(error.code, 0);
    //checks the number of views
    BOOST_REQUIRE_EQUAL(out->size(),1);

}
