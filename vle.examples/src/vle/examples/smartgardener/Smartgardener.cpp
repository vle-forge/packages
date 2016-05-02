/*
 * @file vle/examples/smartgardener/Smartgardener.cpp
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

/*
 * VLE Environment - the multimodeling and simulation environment
 * This file is a part of the VLE environment (http://vle.univ-littoral.fr)
 * Copyright (C) 2003 - 2008 The VLE Development Team
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

/*
 * @@tagdynamic@@
 * @@tagdepends: vle.extension.decision @@endtagdepends
 */

#include <vle/extension/decision/Agent.hpp>
#include <vle/extension/decision/Activity.hpp>
#include <vle/extension/decision/KnowledgeBase.hpp>
#include <vle/utils/Package.hpp>

#include <sstream>
#include <numeric>
#include <fstream>

namespace vd = vle::devs;
namespace vv = vle::value;
namespace ved = vle::extension::decision;
namespace vu = vle::utils;
namespace vmd = vle::extension::decision;

namespace vle { namespace examples { namespace smartgardeners {

class Smartgardener : public vmd::Agent
{
public:
    Smartgardener(
        const vd::DynamicsInit& init,
        const vd::InitEventList& evts)
        : vmd::Agent(init, evts),
          plantlouse_population(0),
          ladybird_population(0),
          treatment_effect_on_plantlouse(0),
          treatment_effect_on_ladybird(0),
          plantlouse_max_population(1000),
          ladybird_min_population(-1000)
    {
        treatment_effect_on_plantlouse =
                evts.getDouble("treatment_effect_on_plantlouse");
        treatment_effect_on_ladybird =
                evts.getDouble("treatment_effect_on_ladybird");
        plantlouse_max_population =
                evts.getDouble("plantlouse_max_population");
        ladybird_min_population =
                evts.getDouble("ladybird_min_population");

        addPredicates(this) +=
                P("TwoManyPlantlouse", &Smartgardener::TwoManyPlantlouse),
                P("NotToSmallLadybirdPopulation",
                        &Smartgardener::NotToSmallLadybirdPopulation);

        addFacts(this) +=
                F("x", &Smartgardener::x),
                F("y", &Smartgardener::y);

        addOutputFunctions(this) +=
                O("treat", &Smartgardener::treat);

        vle::utils::Package pack(context(), "vle.examples");
        std::string filePath = pack.getDataFile("Smartgardener.txt");
        std::ifstream fileStream(filePath.c_str());
        KnowledgeBase::plan().fill(fileStream);

    }

    bool TwoManyPlantlouse() const
    {
        return plantlouse_population > plantlouse_max_population;
    }

    bool NotToSmallLadybirdPopulation() const
    {

        return ladybird_population > ladybird_min_population;
    }


    void x(const vv::Value& value)
    {
        plantlouse_population = value.toDouble().value();
    }

    void y(const vv::Value& value)
    {
        ladybird_population = value.toDouble().value();
    }

    void treat(const std::string& /*name*/, const ved::Activity& /*activity*/,
            vd::ExternalEventList& output)
    {
        output.emplace_back("x");
        value::Map& m = output.back().addMap();
        m.addString("name","x");
        m.addDouble("value",
                plantlouse_population * (1-treatment_effect_on_plantlouse));

        output.emplace_back("y");
        value::Map& m2 = output.back().addMap();
        m2.addString("name","y");
        m2.addDouble("value",
                ladybird_population * (1-treatment_effect_on_ladybird));

    }

    virtual ~Smartgardener()
    {}


private:
    //state
    double plantlouse_population;
    double ladybird_population;
    //parameters
    double treatment_effect_on_plantlouse;
    double treatment_effect_on_ladybird;
    double plantlouse_max_population;
    double ladybird_min_population;
};

}}}// namespaces

DECLARE_DYNAMICS(vle::examples::smartgardeners::Smartgardener)
