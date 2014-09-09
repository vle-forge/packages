/**
 * @file vle.extension.differential-equation/test/test_corrections.cpp
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
 * Bug correction, update of external variables were not taken into
 * account if not synchronized with a compute
 ******************/
BOOST_AUTO_TEST_CASE(test_ExtUpLV)
{
    std::cout << "  test_ExtUpLV " << std::endl;
    vle::utils::Package pack("vle.extension.differential-equation");
    vz::Vpz vpz(pack.getExpFile("ExtUpLV.vpz", vle::utils::PKG_BINARY));


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
    //checks the selected view
    const va::Matrix& view = out->getMatrix("view");
    BOOST_REQUIRE_EQUAL(view.columns(),3);
    BOOST_REQUIRE_EQUAL(view.column(0).size(),51);

    //gets X,Y
    va::ConstVectorView colX = ttgetColumnFromView(view,
                                                   "Top model:LotkaVolterraY", "X");

    //check X at = 0.400 and t=0.41
    BOOST_REQUIRE_CLOSE(va::toDouble(colX[41]),
                        10, 10e-5);

    BOOST_REQUIRE_CLOSE(va::toDouble(colX[42]),
                        1.0, 10e-5);
}


/******************
 * test output_period parameter
 ******************/
BOOST_AUTO_TEST_CASE(test_OutputPeriod)
{
    vle::utils::Package pack("vle.extension.differential-equation");
    vz::Vpz vpz(pack.getExpFile("LotkaVolterraOutputPeriod.vpz"));

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
    //checks the selected view
    const va::Matrix& view = out->getMatrix("view");
    BOOST_REQUIRE_EQUAL(view.columns(),4);
    BOOST_REQUIRE_EQUAL(view.column(0).size(),2);

    //gets nbExtEvents
    va::ConstVectorView col = ttgetColumnFromView(view,
                        "Top model:Counter", "nbExtEvents");

    //check X at = 0.400 and t=0.41
    BOOST_REQUIRE_CLOSE(va::toDouble(col[1]),
                        151, 10e-5);

}
