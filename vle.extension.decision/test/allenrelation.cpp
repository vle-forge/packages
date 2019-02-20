/*
 * @file test/allenrelation.cpp
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
#include <vle/extension/decision/KnowledgeBase.hpp>
#include <vle/vle.hpp>

#include "test.hpp"

namespace vmd = vle::extension::decision;

namespace vle { namespace extension { namespace decision { namespace ex {

    class Before: public vmd::KnowledgeBase
    {
    public:
        Before(vle::utils::ContextPtr ctxp)
            : vmd::KnowledgeBase(ctxp)
        {
            addActivity("A", 0.0, 2.0);
            addActivity("B", 3.0, 5.0);

            // B needs to start at time 3.0.
            addFinishToStartConstraint("A", "B", 0.0, 2.0);
        }

        virtual ~Before() {}
    };

    class Meets : public vmd::KnowledgeBase
    {
    public:
        Meets(vle::utils::ContextPtr ctxp)
            : vmd::KnowledgeBase(ctxp)
        {
            addActivity("A");
            addActivity("B");

            // B needs to start when A finishes.
            addFinishToStartConstraint("A", "B", 0.0, 0.0);
        }

        virtual ~Meets() {}
    };

    class Overlaps: public vmd::KnowledgeBase
    {
    public:
        Overlaps(vle::utils::ContextPtr ctxp)
            : vmd::KnowledgeBase(ctxp)
        {
            addActivity("A");
            addActivity("B");

            // B needs to start at begin time of A + 1.0.
            addStartToStartConstraint("A", "B", 1.0, 2.0);
        }

        virtual ~Overlaps() {}
    };

    class During: public vmd::KnowledgeBase
    {
    public:
        During(vle::utils::ContextPtr ctxp)
            : vmd::KnowledgeBase(ctxp)
        {
            addActivity("A");
            addActivity("B");

            // B needs to start at begin time of A + 1.0.
            // A needs to finished at end time of B + 1.0.
            addStartToStartConstraint("A", "B", 1.0, 1.0);
            addFinishToFinishConstraint("B", "A", 1.0);
        }

        virtual ~During() {}
    };

    class Starts: public vmd::KnowledgeBase
    {
    public:
        Starts(vle::utils::ContextPtr ctxp)
            : vmd::KnowledgeBase(ctxp)
        {
            addActivity("A");
            addActivity("B");

            // A and B need to start at the same time.
            addStartToStartConstraint("A", "B", 0.0, 0.0);
        }

        virtual ~Starts() {}
    };

    class StartsFailed: public vmd::KnowledgeBase
    {
    public:
        StartsFailed(vle::utils::ContextPtr ctxp)
            : vmd::KnowledgeBase(ctxp)
        {
            addActivity("A", 1.0, 2.0);
            addActivity("B");

            // A and B need to start at the same time.
            addStartToStartConstraint("A", "B", 0.0, 0.0);
        }

        virtual ~StartsFailed() {}
    };


    class Finishes: public vmd::KnowledgeBase
    {
    public:
        Finishes(vle::utils::ContextPtr ctxp)
            : vmd::KnowledgeBase(ctxp)
        {
            addActivity("A");
            addActivity("B");

            // A and B need to finish at the same time.
            addFinishToFinishConstraint("A", "B", 0.0);
        }

        virtual ~Finishes() {}
    };

    class Equal: public vmd::KnowledgeBase
    {
    public:
        Equal(vle::utils::ContextPtr ctxp)
            : vmd::KnowledgeBase(ctxp)
        {
            addActivity("A");
            addActivity("B");

            // A and B need to start and finish a the same time.
            addStartToStartConstraint("A", "B", 0.0, 0.0);
            addFinishToFinishConstraint("A", "B", 0.0, 0.0);
        }

        virtual ~Equal() {}
    };

}}}} // namespace vle ext decision ex

void Before1()
{
    vle::utils::ContextPtr ctxp =  vle::utils::make_context();
    vmd::ex::Before base(ctxp);
    vmd::Activities::result_t lst;

    base.processChanges(0.0);

    lst = base.startedActivities();
    EnsuresEqual(lst.size(), vmd::Activities::result_t::size_type(1));
    lst = base.waitedActivities();
    EnsuresEqual(lst.size(), vmd::Activities::result_t::size_type(1));

    base.processChanges(1.0);
    lst = base.startedActivities();
    EnsuresEqual(lst.size(), vmd::Activities::result_t::size_type(1));
    lst = base.waitedActivities();
    EnsuresEqual(lst.size(), vmd::Activities::result_t::size_type(1));

    base.processChanges(2.0);
    lst = base.startedActivities();
    EnsuresEqual(lst.size(), vmd::Activities::result_t::size_type(1));
    lst = base.waitedActivities();
    EnsuresEqual(lst.size(), vmd::Activities::result_t::size_type(1));

    base.processChanges(3.0);
    lst = base.failedActivities();
    EnsuresEqual(lst.size(), vmd::Activities::result_t::size_type(2));

    base.processChanges(4.0);
    lst = base.failedActivities();
    EnsuresEqual(lst.size(), vmd::Activities::result_t::size_type(2));
}

void Before2()
{
    vle::utils::ContextPtr ctxp =  vle::utils::make_context();
    vmd::ex::Before base(ctxp);
    vmd::Activities::result_t lst;

    base.processChanges(0.0);
    lst = base.startedActivities();
    EnsuresEqual(lst.size(), vmd::Activities::result_t::size_type(1));
    lst = base.waitedActivities();
    EnsuresEqual(lst.size(), vmd::Activities::result_t::size_type(1));

    base.processChanges(1.0);
    lst = base.startedActivities();
    EnsuresEqual(lst.size(), vmd::Activities::result_t::size_type(1));
    lst = base.waitedActivities();
    EnsuresEqual(lst.size(), vmd::Activities::result_t::size_type(1));

    base.setActivityDone("A", 1.0);

    base.processChanges(2.0);
    lst = base.endedActivities();
    EnsuresEqual(lst.size(), vmd::Activities::result_t::size_type(1));
    lst = base.waitedActivities();
    EnsuresEqual(lst.size(), vmd::Activities::result_t::size_type(1));

    base.processChanges(3.0);
    lst = base.startedActivities();
    EnsuresEqual(lst.size(), vmd::Activities::result_t::size_type(1));

    base.processChanges(4.0);
    lst = base.startedActivities();
    EnsuresEqual(lst.size(), vmd::Activities::result_t::size_type(1));

    base.processChanges(5.0);
    lst = base.startedActivities();
    EnsuresEqual(lst.size(), vmd::Activities::result_t::size_type(1));

    base.processChanges(5.1);
    lst = base.failedActivities();
    EnsuresEqual(lst.size(), vmd::Activities::result_t::size_type(1));

    base.processChanges(6.0);
    lst = base.failedActivities();
    EnsuresEqual(lst.size(), vmd::Activities::result_t::size_type(1));
}

void Meets1()
{
    vle::utils::ContextPtr ctxp =  vle::utils::make_context();
    vmd::ex::Meets base(ctxp);
    vmd::Activities::result_t lst;

    base.processChanges(0.0);

    lst = base.startedActivities();
    EnsuresEqual(lst.size(), vmd::Activities::result_t::size_type(1));
    lst = base.waitedActivities();
    EnsuresEqual(lst.size(), vmd::Activities::result_t::size_type(1));

    base.processChanges(1.0);
    lst = base.startedActivities();
    EnsuresEqual(lst.size(), vmd::Activities::result_t::size_type(1));
    lst = base.waitedActivities();
    EnsuresEqual(lst.size(), vmd::Activities::result_t::size_type(1));

    base.setActivityDone("A", 1.0);

    base.processChanges(2.0);
    lst = base.endedActivities();
    EnsuresEqual(lst.size(), vmd::Activities::result_t::size_type(1));
    lst = base.startedActivities();
    EnsuresEqual(lst.size(), vmd::Activities::result_t::size_type(1));

    base.processChanges(3.0);
    lst = base.startedActivities();
    EnsuresEqual(lst.size(), vmd::Activities::result_t::size_type(1));

    base.setActivityDone("B", 3.0);

    base.processChanges(4.0);
    lst = base.endedActivities();
    EnsuresEqual(lst.size(), vmd::Activities::result_t::size_type(2));
}

void Overlaps1()
{
    vle::utils::ContextPtr ctxp =  vle::utils::make_context();
    vmd::ex::Overlaps base(ctxp);
    vmd::Activities::result_t lst;

    base.processChanges(0.0);

    lst = base.startedActivities();
    EnsuresEqual(lst.size(), vmd::Activities::result_t::size_type(1));
    lst = base.waitedActivities();
    EnsuresEqual(lst.size(), vmd::Activities::result_t::size_type(1));

    base.processChanges(1.0);
    lst = base.startedActivities();
    EnsuresEqual(lst.size(), vmd::Activities::result_t::size_type(2));

    base.processChanges(2.0);
    lst = base.startedActivities();
    EnsuresEqual(lst.size(), vmd::Activities::result_t::size_type(2));

    base.processChanges(3.0);
    lst = base.startedActivities();
    EnsuresEqual(lst.size(), vmd::Activities::result_t::size_type(2));

    base.setActivityDone("A", 0.0);
    base.setActivityDone("B", 0.0);

    base.processChanges(4.0);
    lst = base.endedActivities();
    EnsuresEqual(lst.size(), vmd::Activities::result_t::size_type(2));
}

void Overlaps2()
{
    vle::utils::ContextPtr ctxp =  vle::utils::make_context();
    vmd::ex::Overlaps base(ctxp);
    vmd::Activities::result_t lst;

    base.processChanges(0.0);

    lst = base.startedActivities();
    EnsuresEqual(lst.size(), vmd::Activities::result_t::size_type(1));
    lst = base.waitedActivities();
    EnsuresEqual(lst.size(), vmd::Activities::result_t::size_type(1));

    base.processChanges(1.0);
    lst = base.startedActivities();
    EnsuresEqual(lst.size(), vmd::Activities::result_t::size_type(2));

    base.processChanges(2.0);
    lst = base.startedActivities();
    EnsuresEqual(lst.size(), vmd::Activities::result_t::size_type(2));

    base.processChanges(3.0);
    lst = base.startedActivities();
    EnsuresEqual(lst.size(), vmd::Activities::result_t::size_type(2));

    base.setActivityDone("A", 3.0);

    base.processChanges(4.0);
    lst = base.endedActivities();
    EnsuresEqual(lst.size(), vmd::Activities::result_t::size_type(1));
    lst = base.startedActivities();
    EnsuresEqual(lst.size(), vmd::Activities::result_t::size_type(1));

    base.setActivityDone("B", 4.0);

    base.processChanges(5.0);
    lst = base.endedActivities();
    EnsuresEqual(lst.size(), vmd::Activities::result_t::size_type(2));
}

void During1()
{
    vle::utils::ContextPtr ctxp =  vle::utils::make_context();
    vmd::ex::During base(ctxp);
    vmd::Activities::result_t lst;

    base.processChanges(0.0);
    lst = base.startedActivities();
    EnsuresEqual(lst.size(), vmd::Activities::result_t::size_type(1));
    lst = base.waitedActivities();
    EnsuresEqual(lst.size(), vmd::Activities::result_t::size_type(1));

    base.processChanges(1.0);
    lst = base.startedActivities();
    EnsuresEqual(lst.size(), vmd::Activities::result_t::size_type(2));

    base.processChanges(2.0);
    lst = base.startedActivities();
    EnsuresEqual(lst.size(), vmd::Activities::result_t::size_type(2));

    base.processChanges(3.0);
    lst = base.startedActivities();
    EnsuresEqual(lst.size(), vmd::Activities::result_t::size_type(2));

    base.setActivityDone("A", 3.0);

    base.processChanges(4.0);
    lst = base.endedActivities();
    EnsuresEqual(lst.size(), vmd::Activities::result_t::size_type(0));
    lst = base.startedActivities();
    EnsuresEqual(lst.size(), vmd::Activities::result_t::size_type(1));
    lst = base.failedActivities();
    EnsuresEqual(lst.size(), vmd::Activities::result_t::size_type(1));

    base.processChanges(5.0);
    lst = base.endedActivities();
    EnsuresEqual(lst.size(), vmd::Activities::result_t::size_type(0));
    lst = base.startedActivities();
    EnsuresEqual(lst.size(), vmd::Activities::result_t::size_type(1));
    lst = base.failedActivities();
    EnsuresEqual(lst.size(), vmd::Activities::result_t::size_type(1));
}

void During2()
{
    vle::utils::ContextPtr ctxp =  vle::utils::make_context();
    vmd::ex::During base(ctxp);
    vmd::Activities::result_t lst;

    base.processChanges(0.0);
    lst = base.startedActivities();
    EnsuresEqual(lst.size(), vmd::Activities::result_t::size_type(1));
    lst = base.waitedActivities();
    EnsuresEqual(lst.size(), vmd::Activities::result_t::size_type(1));

    base.processChanges(1.0);
    lst = base.startedActivities();
    EnsuresEqual(lst.size(), vmd::Activities::result_t::size_type(2));

    base.processChanges(2.0);
    lst = base.startedActivities();
    EnsuresEqual(lst.size(), vmd::Activities::result_t::size_type(2));

    base.processChanges(3.0);
    lst = base.startedActivities();
    EnsuresEqual(lst.size(), vmd::Activities::result_t::size_type(2));

    base.setActivityDone("B", 3.0);

    base.processChanges(3.5);
    lst = base.endedActivities();
    EnsuresEqual(lst.size(), vmd::Activities::result_t::size_type(1));
    lst = base.startedActivities();
    EnsuresEqual(lst.size(), vmd::Activities::result_t::size_type(1));

    base.setActivityDone("A", 3.9);

    base.processChanges(4.0);
    lst = base.endedActivities();
    EnsuresEqual(lst.size(), vmd::Activities::result_t::size_type(2));
}

void Starts1()
{
    vle::utils::ContextPtr ctxp =  vle::utils::make_context();
    vmd::ex::Starts base(ctxp);
    vmd::Activities::result_t lst;

    base.processChanges(0.0);
    lst = base.startedActivities();
    EnsuresEqual(lst.size(), vmd::Activities::result_t::size_type(2));

    base.processChanges(1.0);
    lst = base.startedActivities();
    EnsuresEqual(lst.size(), vmd::Activities::result_t::size_type(2));

    base.setActivityDone("B", 0.0);

    base.processChanges(2.0);
    lst = base.startedActivities();
    EnsuresEqual(lst.size(), vmd::Activities::result_t::size_type(1));
    lst = base.endedActivities();
    EnsuresEqual(lst.size(), vmd::Activities::result_t::size_type(1));

    base.setActivityDone("A", 0.0);

    base.processChanges(3.0);
    lst = base.endedActivities();
    EnsuresEqual(lst.size(), vmd::Activities::result_t::size_type(2));
}

void Starts2()
{
    vle::utils::ContextPtr ctxp =  vle::utils::make_context();
    vmd::ex::StartsFailed base(ctxp);
    vmd::Activities::result_t lst;

    base.processChanges(0.0);
    lst = base.waitedActivities();
    EnsuresEqual(lst.size(), vmd::Activities::result_t::size_type(2));

    base.processChanges(1.0);
    lst = base.startedActivities();
    EnsuresEqual(lst.size(), vmd::Activities::result_t::size_type(2));
}

void finishes()
{
    vle::utils::ContextPtr ctxp =  vle::utils::make_context();
    vmd::ex::Finishes base(ctxp);
    vmd::Activities::result_t lst;

    base.processChanges(0.0);
    lst = base.startedActivities();
    EnsuresEqual(lst.size(), vmd::Activities::result_t::size_type(2));

    base.processChanges(1.0);
    lst = base.startedActivities();
    EnsuresEqual(lst.size(), vmd::Activities::result_t::size_type(2));

    base.processChanges(2.0);
    base.setActivityDone("A", 2.0);

    lst = base.startedActivities();
    EnsuresEqual(lst.size(), vmd::Activities::result_t::size_type(1));

    base.processChanges(3.0);
    lst = base.endedActivities();
    EnsuresEqual(lst.size(), vmd::Activities::result_t::size_type(1));
    lst = base.failedActivities();
    EnsuresEqual(lst.size(), vmd::Activities::result_t::size_type(1));

    base.processChanges(4.0);
    lst = base.endedActivities();
    EnsuresEqual(lst.size(), vmd::Activities::result_t::size_type(1));
    lst = base.failedActivities();
    EnsuresEqual(lst.size(), vmd::Activities::result_t::size_type(1));
}

void Equal()
{
    vle::utils::ContextPtr ctxp =  vle::utils::make_context();
    vmd::ex::Equal base(ctxp);
    vmd::Activities::result_t lst;

    base.processChanges(0.0);
    lst = base.startedActivities();
    EnsuresEqual(lst.size(), vmd::Activities::result_t::size_type(2));

    base.processChanges(1.0);
    lst = base.startedActivities();
    EnsuresEqual(lst.size(), vmd::Activities::result_t::size_type(2));

    base.setActivityDone("A", 2.0);
    base.setActivityDone("B", 2.0);
    base.processChanges(2.0);

    lst = base.endedActivities();
    EnsuresEqual(lst.size(), vmd::Activities::result_t::size_type(2));
    lst = base.failedActivities();
    EnsuresEqual(lst.size(), vmd::Activities::result_t::size_type(0));
}

void EqualFail()
{
    vle::utils::ContextPtr ctxp =  vle::utils::make_context();
    vmd::ex::Equal base(ctxp);
    vmd::Activities::result_t lst;

    base.processChanges(0.0);
    lst = base.startedActivities();
    EnsuresEqual(lst.size(), vmd::Activities::result_t::size_type(2));

    base.processChanges(1.0);
    lst = base.startedActivities();
    EnsuresEqual(lst.size(), vmd::Activities::result_t::size_type(2));

    base.setActivityDone("A", 2.0);
    base.processChanges(2.0);

    lst = base.endedActivities();
    EnsuresEqual(lst.size(), vmd::Activities::result_t::size_type(1));

    base.processChanges(2.1);

    lst = base.endedActivities();
    EnsuresEqual(lst.size(), vmd::Activities::result_t::size_type(1));
    lst = base.failedActivities();
    EnsuresEqual(lst.size(), vmd::Activities::result_t::size_type(1));
}


int main()
{
    fixture f;

    Before1();
    Before2();
    Meets1();
    Overlaps1();
    Overlaps2();
    During1();
    During2();
    Starts1();
    Starts2();
    finishes();
    Equal();
    EqualFail();

    return unit_test::report_errors();
}
