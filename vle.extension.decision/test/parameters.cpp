/*
 * @file test/parser.cpp
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
#define BOOST_TEST_MODULE test_parser
#include <boost/test/unit_test.hpp>
#include <boost/test/auto_unit_test.hpp>
#include <boost/test/floating_point_comparison.hpp>
#include <iostream>
#include <iterator>
#include <boost/assign/list_of.hpp>
#include <boost/assign.hpp>
#include <vle/version.hpp>
#include <vle/vle.hpp>
#include <vle/value/Double.hpp>
#include <vle/devs/Time.hpp>
#include <vle/utils/DateTime.hpp>
#include <vle/extension/Decision.hpp>

namespace vmd = vle::extension::decision;
namespace vd = vle::devs;
namespace vu = vle::utils;

using vle::fmt;
using namespace boost::assign;

namespace vle { namespace extension { namespace decision { namespace ex {

class KnowledgeBase : public vmd::KnowledgeBase
{
public:
    KnowledgeBase()
        : vmd::KnowledgeBase(), today(0), yesterday(0)
    {
        addRess(this) +=
            R("computeResource", &KnowledgeBase::resFunc);

        addPortFact("today", boost::bind(&vmd::ex::KnowledgeBase::date, this, _1, _2));

        addPredicates(this) +=
            P("predUsingPlanTimeStamp", &KnowledgeBase::predUsingPlanTimeStamp);

        addResources("Farmer", "Bob");
        addResources("Farmer", "Bill");
    }

    virtual ~KnowledgeBase() {}

    void date(const std::string& /*port*/,
              const vle::value::Value& val)
    {
        yesterday = today;
        today = val.toDouble().value();
    }

    bool predUsingPlanTimeStamp(const std::string&, const std::string&,
                                const PredicateParameters& params) const
    {
            double loadTime = params.getDouble("planTimeStamp");
            double dayThreshold = params.getDouble("dayThreshold");
            if (loadTime + dayThreshold <= today) {
                return true;
            } else {
                return false;
            }
    }

    std::string resFunc(const std::string& /*name*/,
                        const Activity& /*activity*/) const
    {
        return "Farmer";
    }

    double today, yesterday;

};

const char* Plan1 = \
"# This file is a part of the VLE environment # http://www.vle-project.org\n"
"# Copyright (C) 2016 INRA http://www.inra.fr\n"
"#\n"
"# This program is free software: you can redistribute it and/or modify\n"
"# it under the terms of the GNU General Public License as published by\n"
"# the Free Software Foundation, either version 3 of the License, or\n"
"# (at your option) any later version.\n"
"#\n"
"# This program is distributed in the hope that it will be useful,\n"
"# but WITHOUT ANY WARRANTY; without even the implied warranty of\n"
"# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the\n"
"# GNU General Public License for more details.\n"
"#\n"
"# You should have received a copy of the GNU General Public License\n"
"# along with this program.  If not, see <http://www.gnu.org/licenses/>.\n"
"\n"
"\n"
"predicates { #Predicates list. \n"
"	predicate {\n"
"		id = \"p_P\";\n"
"		type = \"predUsingPlanTimeStamp\";\n"
"		parameter {\n"
"			dayThreshold = 3.0;\n"
"			planTimeStamp = 0.0;\n"
"		}\n"
"	}\n"
"}\n"
"\n"
"rules { # listes des règles.\n"
"    rule { # définition de la rèle `rule 1'.\n"
"        id = \"rule 1\";\n"
"        predicates = \"p_P\"; # sa liste de prédicats.\n"
"    }\n"
"}\n"
"\n"
"activities {\n"
"    activity {\n"
"        id = \"activity1\";\n"
"        rules = \"rule 1\";\n"
"        temporal {\n"
"            start = 0;\n"
"            finish = 101;\n"
"        }\n"
"    }\n"
"}\n"
"\n"
"}\n";


const char* Plan2 = \
"# This file is a part of the VLE environment # http://www.vle-project.org\n"
"# Copyright (C) 2016 INRA http://www.inra.fr\n"
"#\n"
"# This program is free software: you can redistribute it and/or modify\n"
"# it under the terms of the GNU General Public License as published by\n"
"# the Free Software Foundation, either version 3 of the License, or\n"
"# (at your option) any later version.\n"
"#\n"
"# This program is distributed in the hope that it will be useful,\n"
"# but WITHOUT ANY WARRANTY; without even the implied warranty of\n"
"# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the\n"
"# GNU General Public License for more details.\n"
"#\n"
"# You should have received a copy of the GNU General Public License\n"
"# along with this program.  If not, see <http://www.gnu.org/licenses/>.\n"
"\n"
"\n"
"predicates { #Predicates list. \n"
"	predicate {\n"
"		id = \"p_P\";\n"
"		type = \"predUsingPlanTimeStamp\";\n"
"		parameter {\n"
"			dayThreshold = 10.0;\n"
"			planTimeStamp = 0.0;\n"
"		}\n"
"	}\n"
"}\n"
"\n"
"rules { # listes des règles.\n"
"    rule { # définition de la rèle `rule 1'.\n"
"        id = \"rule 1\";\n"
"        predicates = \"p_P\"; # sa liste de prédicats.\n"
"    }\n"
"}\n"
"\n"
"activities {\n"
"    activity {\n"
"        id = \"activity1\";\n"
"        rules = \"rule 1\";\n"
"        temporal {\n"
"            minstart = 0;\n"
"            maxstart = 5;\n"
"            finish = 100;\n"
"        }\n"
"	 parameter {\n"
"	      neverfail = 1.0;\n"
"	 }\n"
"    }\n"
"}\n"
"\n"
"}\n";

const char* Plan3 = \
"# This file is a part of the VLE environment # http://www.vle-project.org\n"
"# Copyright (C) 2016 INRA http://www.inra.fr\n"
"#\n"
"# This program is free software: you can redistribute it and/or modify\n"
"# it under the terms of the GNU General Public License as published by\n"
"# the Free Software Foundation, either version 3 of the License, or\n"
"# (at your option) any later version.\n"
"#\n"
"# This program is distributed in the hope that it will be useful,\n"
"# but WITHOUT ANY WARRANTY; without even the implied warranty of\n"
"# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the\n"
"# GNU General Public License for more details.\n"
"#\n"
"# You should have received a copy of the GNU General Public License\n"
"# along with this program.  If not, see <http://www.gnu.org/licenses/>.\n"
"\n"
"\n"
"predicates { #Predicates list. \n"
"	predicate {\n"
"		id = \"p_P\";\n"
"		type = \"predUsingPlanTimeStamp\";\n"
"		parameter {\n"
"			dayThreshold = 3.0;\n"
"			planTimeStamp = 0.0;\n"
"		}\n"
"	}\n"
"}\n"
"\n"
"rules { # listes des règles.\n"
"    rule { # définition de la rèle `rule 1'.\n"
"        id = \"rule 1\";\n"
"        predicates = \"p_P\"; # sa liste de prédicats.\n"
"    }\n"
"}\n"
"\n"
"activities {\n"
"    activity {\n"
"        id = \"activity1\";\n"
"        rules = \"rule 1\";\n"
"        temporal {\n"
"            start = 0;\n"
"            finish = 101;\n"
"        }\n"
"        parameter {\n"
"            resourceFunc = \"computeResource\";\n"
"        }\n"
"    }\n"
"}\n"
"\n"
"}\n";

}}}} // namespace vle ext decision ex

BOOST_AUTO_TEST_CASE(test_planTimeStamp)
{
    vle::Init app;
    {
        vmd::ex::KnowledgeBase b;
        b.plan().fill(std::string(vmd::ex::Plan1), 7);

        const vmd::Activity& act1 = b.activities().get("activity1")->second;
        b.applyFact("today", vle::value::Double(7));
        b.processChanges(0.0);
        BOOST_REQUIRE(not act1.isInStartedState());
        b.applyFact("today", vle::value::Double(8));
        b.processChanges(1.0);
        BOOST_REQUIRE(not act1.isInStartedState());
        b.applyFact("today", vle::value::Double(9));
        b.processChanges(2.0);
        BOOST_REQUIRE(not act1.isInStartedState());
        b.applyFact("today", vle::value::Double(10));
        b.processChanges(3.0);
        BOOST_REQUIRE(act1.isInStartedState());
    }
}

BOOST_AUTO_TEST_CASE(test_neverFail)
{
    vle::Init app;
    {
        vmd::ex::KnowledgeBase b;
        b.plan().fill(std::string(vmd::ex::Plan2), 0);

        const vmd::Activity& act1 = b.activities().get("activity1")->second;
        b.applyFact("today", vle::value::Double(0));
        b.processChanges(0.0);
        BOOST_REQUIRE(not act1.isInStartedState());
        b.applyFact("today", vle::value::Double(1));
        b.processChanges(1.0);
        BOOST_REQUIRE(not act1.isInStartedState());
        b.applyFact("today", vle::value::Double(2));
        b.processChanges(2.0);
        BOOST_REQUIRE(not act1.isInStartedState());
        b.applyFact("today", vle::value::Double(3));
        b.processChanges(3.0);
        BOOST_REQUIRE(not act1.isInStartedState());

        b.applyFact("today", vle::value::Double(4));
        b.processChanges(4.0);
        BOOST_REQUIRE(not act1.isInStartedState());

        b.applyFact("today", vle::value::Double(5));
        b.processChanges(5.0);
        BOOST_REQUIRE(not act1.isInStartedState());

        b.applyFact("today", vle::value::Double(5.1));
        b.processChanges(5.1);
        BOOST_REQUIRE(act1.isInStartedState());
    }
}

BOOST_AUTO_TEST_CASE(test_resourceFunc)
{
    vle::Init app;
    {
        vmd::ex::KnowledgeBase b;
        b.plan().fill(std::string(vmd::ex::Plan3), 7);

        const vmd::Activity& act1 = b.activities().get("activity1")->second;
        b.applyFact("today", vle::value::Double(7));
        b.processChanges(0.0);
        BOOST_REQUIRE(not act1.isInStartedState());
        b.applyFact("today", vle::value::Double(8));
        b.processChanges(1.0);
        BOOST_REQUIRE(not act1.isInStartedState());
        b.applyFact("today", vle::value::Double(9));
        b.processChanges(2.0);
        BOOST_REQUIRE(not act1.isInStartedState());
        b.applyFact("today", vle::value::Double(10));
        b.processChanges(3.0);
        BOOST_REQUIRE(act1.isInStartedState());
    }
}
