/**
 * @file vle.extension.differential-equation_test/test/test_common.hpp
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

#ifndef VLE_EXTENSION_DIFFERENTIAL_EQUATION_TEST_COMMON_HPP
#define VLE_EXTENSION_DIFFERENTIAL_EQUATION_TEST_COMMON_HPP 1

#define BOOST_TEST_MAIN
#define BOOST_AUTO_TEST_MAIN
#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE package_test
#include <boost/test/unit_test.hpp>
#include <boost/test/auto_unit_test.hpp>
#include <boost/test/floating_point_comparison.hpp>

#include <ostream>
#include <iostream>

#include <vle/vle.hpp>
#include <vle/value/Map.hpp>
#include <vle/value/Matrix.hpp>
#include <vle/vpz/Vpz.hpp>
#include <vle/vpz/Model.hpp>
#include <vle/vpz/AtomicModel.hpp>
#include <vle/utils/Context.hpp>
#include <vle/utils/Package.hpp>
#include <vle/manager/Simulation.hpp>

namespace vz = vle::vpz;
namespace vu = vle::utils;
namespace vm = vle::manager;
namespace va = vle::value;

struct F
{
    F()
    {
    }

    vle::Init app;
};
BOOST_GLOBAL_FIXTURE(F);


int
ttgetColumnFromView(const va::Matrix& view, const std::string& model,
                    const std::string& port)
{
    for(unsigned int j=1; j < view.columns(); j++){
        if(view.getString(j,0) == (model + std::string(".") + port)){
            return j;
        }
    }
    return -1;
}


void
ttconfOutputPlugins(vz::Vpz& vpz)
{
    //set all output plugin to storage
    vz::Outputs::iterator itb =
        vpz.project().experiment().views().outputs().begin();
    vz::Outputs::iterator ite =
        vpz.project().experiment().views().outputs().end();
    for(;itb!=ite;itb++) {
        std::unique_ptr<va::Map> configOutput(new va::Map());
        configOutput->addInt("rows",10000);
        configOutput->addInt("inc_rows",10000);
        configOutput->addString("header","top");
        vz::Output& output = itb->second;
        output.setStream("", "storage", "vle.output");
        output.setData(std::move(configOutput));
    }
}


void
ttattachConditions(vz::Vpz& vpz, const std::vector<std::string>& conds,
                   const std::string& atomModel)
{
    vz::Model& vpz_mod = vpz.project().model();
    vz::BaseModel* mdl = vpz_mod.model()->findModelFromPath(atomModel);
    BOOST_REQUIRE(mdl != 0);
    vz::AtomicModel* atomg = mdl->toAtomic();
    atomg->setConditions(conds);
}


#endif
