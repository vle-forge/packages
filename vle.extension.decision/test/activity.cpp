/*
 * @file test/activity.cpp
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



#include <vle/utils/unit-test.hpp>

#include <iostream>
#include <iterator>
#include <vle/value/Double.hpp>
#include <vle/utils/Tools.hpp>
#include <vle/utils/Context.hpp>
#include <vle/extension/decision/KnowledgeBase.hpp>
#include <vle/vle.hpp>

namespace vmd = vle::extension::decision;
namespace vd = vle::devs;

namespace vle { namespace extension { namespace decision { namespace ex {

class Maxstart: public vmd::KnowledgeBase
{
public:
    Maxstart(vle::utils::ContextPtr ctxp)
        : vmd::KnowledgeBase(ctxp)
    {
        vmd::Rule& r1 = addRule("Rule false");
        r1.add(std::bind(&vmd::ex::Maxstart::isAlwaysFalse, this));

        vmd::Activity& a = addActivity("A");
        a.initStartRangeFinishRange(0.0, 1.0, 2.0, 3.0);
        a.addRule("Rule false", r1);
    }

    virtual ~Maxstart() {}

    bool isAlwaysFalse() const
    {
        return false;
    }
};

class Minfinish: public vmd::KnowledgeBase
{
public:
    Minfinish(vle::utils::ContextPtr ctxp)
        : vmd::KnowledgeBase(ctxp)
    {
        vmd::Activity& a = addActivity("A");
        a.initStartRangeFinishRange(0.0, 1.0, 2.0, 3.0);
    }

    virtual ~Minfinish() {}
};

class KnowledgeBase : public vmd::KnowledgeBase
{
public:
    KnowledgeBase(vle::utils::ContextPtr ctxp)
        : vmd::KnowledgeBase(ctxp), today(0), yesterday(0)
    {
        addFact("today", std::bind(&vmd::ex::KnowledgeBase::date,
                                     this, std::placeholders::_1));

        vmd::Rule& r1 = addRule("Rule 1");
        r1.add(std::bind(&vmd::ex::KnowledgeBase::haveGoodTemp, this));
        r1.add(std::bind(&vmd::ex::KnowledgeBase::isAlwaysTrue, this));

        vmd::Rule& r2 = addRule("Rule 2");
        r2.add(std::bind(&vmd::ex::KnowledgeBase::haveGoodTemp, this));

        vmd::Activity& act1 = addActivity("act1");
        act1.addRule("Rule 1", r1);

        vmd::Activity& act2 = addActivity("act2");
        act2.addRule("Rule 2", r2);
        act2.initStartTimeFinishTime(1.5, 2.5);

        vmd::Activity& act3 = addActivity("act3");
        act3.addRule("Rule 2", r2);
        act3.initStartTimeFinishTime(3.5, 4.5);
    }

    virtual ~KnowledgeBase() {}

    /*
     * list of facts
     */

    void date(const vle::value::Value& val)
    {
        yesterday = today;
        today = val.toDouble().value();
    }

    /*
     * list of predicates
     */

    bool isAlwaysTrue() const
    {
        return true;
    }

    bool haveGoodTemp() const
    {
        return yesterday > 15.0 and today > 20.0;
    };

    double today, yesterday;
};

class KnowledgeBaseGraph : public vmd::KnowledgeBase
{
public:
    KnowledgeBaseGraph(vle::utils::ContextPtr ctxp)
        : vmd::KnowledgeBase(ctxp)
    {
        addActivity("act1");
        addActivity("act2");
        addActivity("act3");
        addActivity("act4");
        addActivity("act5");

        addFinishToStartConstraint("act1", "act2", 0.0, 1.0);
        addFinishToStartConstraint("act2", "act3", 0.0, 1.0);
        addFinishToStartConstraint("act2", "act4", 0.0, 1.0);
    }

    virtual ~KnowledgeBaseGraph() {}
};

class KnowledgeBaseGraph2 : public vmd::KnowledgeBase
{
public:
    KnowledgeBaseGraph2(vle::utils::ContextPtr ctxp)
        : vmd::KnowledgeBase(ctxp)
    {
        addActivity("A");
        addActivity("B");
        addActivity("C");
        addActivity("D");
        addActivity("E");
        addActivity("F");
        addActivity("G");

        addFinishToStartConstraint("A", "B", 0.0, 1.0);
        addFinishToStartConstraint("B", "C", 0.0, 1.0);
        addFinishToStartConstraint("B", "D", 0.0, 1.0);
        addFinishToStartConstraint("C", "E", 0.0, 1.0);
        addFinishToStartConstraint("D", "F", 0.0, 1.0);
        addFinishToStartConstraint("E", "G", 0.0, 1.0);
        addFinishToStartConstraint("F", "G", 0.0, 1.0);

        addStartToStartConstraint("C", "D", 0.0);

        addFinishToFinishConstraint("E", "F", 0.0);
    }

    virtual ~KnowledgeBaseGraph2() {}
};

class KnowledgeBaseGraph3 : public vmd::KnowledgeBase
{
public:
    KnowledgeBaseGraph3(vle::utils::ContextPtr ctxp)
        : vmd::KnowledgeBase(ctxp)
    {
        addActivity("A");
        addActivity("B");
        addActivity("C");
        addActivity("D");
        addActivity("E");
        addActivity("F");

        addFinishToStartConstraint("A", "B", 0.0, 1.0);
        addFinishToStartConstraint("A", "C", 0.0, 1.0);
        addFinishToStartConstraint("A", "D", 0.0, 1.0);
        addFinishToStartConstraint("B", "F", 0.0, 1.0);
        addFinishToStartConstraint("C", "E", 0.0, 1.0);
        addFinishToStartConstraint("D", "E", 0.0, 1.0);
        addFinishToStartConstraint("E", "F", 0.0, 1.0);
    }

    virtual ~KnowledgeBaseGraph3() {}
};

class KB4 : public vmd::KnowledgeBase
{
public:
    KB4(vle::utils::ContextPtr ctxp)
        : vmd::KnowledgeBase(ctxp)
    {
        addActivity("A", 0.0, vd::infinity);
        addActivity("B", 1.0, vd::infinity);
        addActivity("C", 1.0, vd::infinity);
        addActivity("D", 2.0, vd::infinity);
        addActivity("E", 3.0, vd::infinity);
        addActivity("F", 4.0, vd::infinity);
    }

    virtual ~KB4() {}
};

class KB5 : public vmd::KnowledgeBase
{
    int mNbUpdate, mNbAck, mNbOut;

public:
    KB5(vle::utils::ContextPtr ctxp) :
        vmd::KnowledgeBase(ctxp),
        mNbUpdate(0), mNbAck(0), mNbOut(0)
    {
        std::cout << "KB5 start\n";
        vmd::Activity& a = addActivity("A", 0.0, vd::infinity);
        vmd::Activity& b = addActivity("B", 1.0, vd::infinity);
        vmd::Activity& c = addActivity("C", 1.0, vd::infinity);
        vmd::Activity& d = addActivity("D", 2.0, vd::infinity);
        vmd::Activity& e = addActivity("E", 3.0, vd::infinity);
        vmd::Activity& f = addActivity("F", 4.0, vd::infinity);

        addOutputFunctions(this) += O("out", &KB5::out);
        addUpdateFunctions(this) += U("update", &KB5::update);

        a.addOutputFunction(outputFunctions()["out"]);
        b.addOutputFunction(outputFunctions()["out"]);
        c.addOutputFunction(outputFunctions()["out"]);
        d.addOutputFunction(outputFunctions()["out"]);
        e.addOutputFunction(outputFunctions()["out"]);
        f.addOutputFunction(outputFunctions()["out"]);

        a.addUpdateFunction(updateFunctions()["update"]);
        b.addUpdateFunction(updateFunctions()["update"]);
        c.addUpdateFunction(updateFunctions()["update"]);
        d.addUpdateFunction(updateFunctions()["update"]);
        e.addUpdateFunction(updateFunctions()["update"]);
        f.addUpdateFunction(updateFunctions()["update"]);
    }

    virtual ~KB5() {}

    void ack(const std::string&, const Activity&)
    {
        mNbAck++;
    }

    void out(const std::string&, const Activity&,
             vle::devs::ExternalEventList&)
    {
        mNbOut++;
    }

    void update(const std::string& name, const Activity& act)
    {
        std::cout << vle::utils::format("new state of %s is %i\n",
                name.c_str(), (int)act.state());
        mNbUpdate++;
    }

    bool isAlwaysTrue() const
    {
        return true;
    }

    int getNumberOfUpdate() const
    {
        return mNbUpdate;
    }

    int getNumberOfAck() const
    {
        return mNbAck;
    }

    int getNumberOfOut() const
    {
        return mNbOut;
    }
};

}}}} // namespace vle ext decision

void Maxstart()
{
    vle::utils::ContextPtr ctxp =  vle::utils::make_context();
    vmd::ex::Maxstart base(ctxp);
    vmd::Activities::result_t lst;

    base.processChanges(0.0);

    lst = base.waitedActivities();
    EnsuresEqual(lst.size(), vmd::Activities::result_t::size_type(1));

    base.processChanges(1.0);
    lst = base.waitedActivities();
    EnsuresEqual(lst.size(), vmd::Activities::result_t::size_type(1));

    base.processChanges(2.0);
    lst = base.failedActivities();
    EnsuresEqual(lst.size(), vmd::Activities::result_t::size_type(1));
}

void Minfinish()
{
    vle::utils::ContextPtr ctxp =  vle::utils::make_context();
    vmd::ex::Minfinish base(ctxp);
    vmd::Activities::result_t lst;

    base.processChanges(0.0);

    lst = base.startedActivities();
    EnsuresEqual(lst.size(), vmd::Activities::result_t::size_type(1));

    base.processChanges(1.0);
    lst = base.startedActivities();
    EnsuresEqual(lst.size(), vmd::Activities::result_t::size_type(1));

    base.setActivityDone("A", 1.5);

    base.processChanges(1.5);
    lst = base.failedActivities();
    EnsuresEqual(lst.size(), vmd::Activities::result_t::size_type(1));
}

void kb()
{
    vle::Init app;
    vle::utils::ContextPtr ctxp =  vle::utils::make_context();
    vmd::ex::KnowledgeBase base(ctxp);

    vmd::Activities::result_t lst;
    double date = 0.0;

    EnsuresEqual(lst.size(), vmd::Activities::result_t::size_type(0));
    ++date;

    base.applyFact("today", vle::value::Double(16));
    base.processChanges(date);
    lst = base.startedActivities();
    EnsuresEqual(lst.size(), vmd::Activities::result_t::size_type(0));
    ++date;

    base.applyFact("today", vle::value::Double(21));
    base.processChanges(date);
    lst = base.startedActivities();
    EnsuresEqual(lst.size(), vmd::Activities::result_t::size_type(2));
    ++date;

    base.applyFact("today", vle::value::Double(18));
    base.processChanges(date);
    lst = base.startedActivities();
    EnsuresEqual(lst.size(), vmd::Activities::result_t::size_type(1));
    lst = base.waitedActivities();
    EnsuresEqual(lst.size(), vmd::Activities::result_t::size_type(1));
    lst = base.failedActivities();
    EnsuresEqual(lst.size(), vmd::Activities::result_t::size_type(1));
    ++date;

    base.applyFact("today", vle::value::Double(22));
    base.processChanges(date);
    lst = base.startedActivities();
    EnsuresEqual(lst.size(), vmd::Activities::result_t::size_type(2));
}

void kb2()
{
    vle::Init app;
    vle::utils::ContextPtr ctxp =  vle::utils::make_context();
    vmd::ex::KnowledgeBaseGraph base(ctxp);

    vmd::Activities::result_t lst;

    base.processChanges(0.0);
    lst = base.startedActivities();
    EnsuresEqual(lst.size(), vmd::Activities::result_t::size_type(2));

    base.processChanges(0.0);
    lst = base.waitedActivities();
    EnsuresEqual(lst.size(), vmd::Activities::result_t::size_type(3));

    lst = base.startedActivities();
    EnsuresEqual(lst.size(), vmd::Activities::result_t::size_type(2));

    for (vmd::Activities::result_t::iterator it = lst.begin(); it !=
         lst.end(); ++it) {
        (*it)->second.end(0.0);
    }

    base.processChanges(0.0);
    lst = base.waitedActivities();
    EnsuresEqual(lst.size(), vmd::Activities::result_t::size_type(2));
    lst = base.startedActivities();
    EnsuresEqual(lst.size(), vmd::Activities::result_t::size_type(1));

    for (vmd::Activities::result_t::iterator it = lst.begin(); it !=
         lst.end(); ++it) {
        (*it)->second.end(0.0);
    }

    base.processChanges(0.0);
    lst = base.startedActivities();
    EnsuresEqual(lst.size(), vmd::Activities::result_t::size_type(2));

    for (vmd::Activities::result_t::iterator it = lst.begin(); it !=
         lst.end(); ++it) {
        (*it)->second.end(0.0);
    }

    lst = base.startedActivities();
    EnsuresEqual(lst.size(), vmd::Activities::result_t::size_type(2));
}

void kb3()
{
    vle::Init app;
    vle::utils::ContextPtr ctxp =  vle::utils::make_context();
    vmd::ex::KnowledgeBaseGraph2 base(ctxp);

    vmd::Activities::result_t lst;

    base.processChanges(0.0);
    lst = base.startedActivities();
    EnsuresEqual(lst.size(), vmd::Activities::result_t::size_type(1));
    base.setActivityDone("A", 0.0);

    base.processChanges(0.0);
    lst = base.startedActivities();
    EnsuresEqual(lst.size(), vmd::Activities::result_t::size_type(1));

    base.processChanges(0.0);
    lst = base.startedActivities();
    EnsuresEqual(lst.size(), vmd::Activities::result_t::size_type(1));
    base.setActivityDone("B", 0.0);

    base.processChanges(0.0);
    lst = base.startedActivities();
    EnsuresEqual(lst.size(), vmd::Activities::result_t::size_type(2));

    base.processChanges(0.0);
    lst = base.startedActivities();
    EnsuresEqual(lst.size(), vmd::Activities::result_t::size_type(2));
    base.setActivityDone("C", 0.0);

    base.processChanges(0.0);
    lst = base.startedActivities();
    EnsuresEqual(lst.size(), vmd::Activities::result_t::size_type(2));

    base.processChanges(0.0);
    lst = base.startedActivities();
    EnsuresEqual(lst.size(), vmd::Activities::result_t::size_type(2));
    base.setActivityDone("D", 0.0);

    base.processChanges(0.0);
    lst = base.startedActivities();
    EnsuresEqual(lst.size(), vmd::Activities::result_t::size_type(2));
    base.setActivityDone("E", 0.0);

    base.processChanges(0.0);
    lst = base.startedActivities();
    EnsuresEqual(lst.size(), vmd::Activities::result_t::size_type(1));
    base.setActivityDone("F", 0.0);

    base.processChanges(0.0);
    lst = base.startedActivities();
    EnsuresEqual(lst.size(), vmd::Activities::result_t::size_type(1));
}

void Activity4()
{
    vle::Init app;
    vle::utils::ContextPtr ctxp =  vle::utils::make_context();
    vmd::ex::KnowledgeBaseGraph3 base(ctxp);

    vmd::Activities::result_t lst;

    base.processChanges(0.0);
    lst = base.startedActivities();
    EnsuresEqual(lst.size(), vmd::Activities::result_t::size_type(1));
    base.setActivityDone("A", 0.0);

    base.processChanges(0.0);
    lst = base.startedActivities();
    EnsuresEqual(lst.size(), vmd::Activities::result_t::size_type(3));
    base.setActivityDone("B", 0.0);
}

void ActivitiesnextDate1()
{
    vle::Init app;
    vle::utils::ContextPtr ctxp =  vle::utils::make_context();
    vmd::ex::KB4 base(ctxp);

    vmd::Activities::result_t lst;

    EnsuresApproximatelyEqual((double)base.nextDate(0.), 0., 1.);
    base.processChanges(0.);
    base.setActivityDone("A", 0.5);
    EnsuresApproximatelyEqual((double)base.nextDate(1.), 1., 1.);
    base.processChanges(1.);
    base.setActivityDone("B", 1.5);
    EnsuresApproximatelyEqual((double)base.nextDate(1.), 1., 1.);
    base.processChanges(1.);
    base.setActivityDone("C", 1.5);
    EnsuresApproximatelyEqual((double)base.nextDate(2.), 2., 1.);
    base.processChanges(2.);
    base.setActivityDone("D", 2.5);
    EnsuresApproximatelyEqual((double)base.nextDate(3.), 3., 1.);
    base.processChanges(3.);
    base.setActivityDone("E", 3.5);
    EnsuresApproximatelyEqual((double)base.nextDate(4.), 4., 1.);
    base.processChanges(4.);
    base.setActivityDone("F", 4.5);
}

void ActivitiesnextDate2()
{
    vle::Init app;
    vle::utils::ContextPtr ctxp =  vle::utils::make_context();
    vmd::ex::KB4 base(ctxp);

    vmd::Activities::result_t lst;

    EnsuresApproximatelyEqual((double)base.duration(0.), 0., 1.);
    base.processChanges(0.);
    base.setActivityDone("A", 0.5);
    EnsuresApproximatelyEqual((double)base.duration(.5), .5, 1.);
    base.processChanges(1.);
    base.setActivityDone("B", 1.);
    EnsuresApproximatelyEqual((double)base.duration(1.), 0., 1.);
    base.processChanges(1.);
    base.setActivityDone("C", 1.5);
    EnsuresApproximatelyEqual((double)base.duration(1.5), .5, 1.);
    base.processChanges(2.);
    base.setActivityDone("D", 2.5);
    EnsuresApproximatelyEqual((double)base.duration(2.5), .5, 1.);
    base.processChanges(3.);
    base.setActivityDone("E", 3.5);
    EnsuresApproximatelyEqual((double)base.duration(3.5), .5, 1.);
    base.processChanges(4.);
    base.setActivityDone("F", 4.5);
}

void activitiesExist()
{
    vle::Init app;
    vle::utils::ContextPtr ctxp =  vle::utils::make_context();
    vmd::ex::KB4 base(ctxp);

    bool b = base.activities().exist("A");
    EnsuresEqual(b, true);
    b = base.activities().exist("Z");
    EnsuresEqual(b, false);
}

void Activities_test_slot_function()
{
    vle::Init app;
    vle::utils::ContextPtr ctxp =  vle::utils::make_context();
    vmd::ex::KB5 base(ctxp);

    vmd::Activities::result_t lst;

    EnsuresApproximatelyEqual((double)base.duration(0.), 0., 1.);
    base.processChanges(0.);
    base.setActivityDone("A", 0.5);
    EnsuresApproximatelyEqual((double)base.duration(.5), .5, 1.);
    base.processChanges(1.);
    base.setActivityDone("B", 1.);
    EnsuresApproximatelyEqual((double)base.duration(1.), 0., 1.);
    base.processChanges(1.);
    base.setActivityDone("C", 1.5);
    EnsuresApproximatelyEqual((double)base.duration(1.5), .5, 1.);
    base.processChanges(2.);
    base.setActivityDone("D", 2.5);
    EnsuresApproximatelyEqual((double)base.duration(2.5), .5, 1.);
    base.processChanges(3.);
    base.setActivityDone("E", 3.5);
    EnsuresApproximatelyEqual((double)base.duration(3.5), .5, 1.);
    base.processChanges(4.);
    base.setActivityDone("F", 4.5);
    base.processChanges(5.);

    // All activity switch from wait to start and from start to end.
    EnsuresEqual(base.getNumberOfUpdate(), 12);
    std::cout << vle::utils::format("%i %i\n", base.getNumberOfOut(),
        base.getNumberOfUpdate());
}

int main()
{
    Maxstart();
    Minfinish();
    kb();
    kb2();
    kb3();
    Activity4();
    ActivitiesnextDate1();
    ActivitiesnextDate2();
    activitiesExist();
    Activities_test_slot_function();

    return unit_test::report_errors();
}
