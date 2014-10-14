/*
 * @file test/dynamics/SimpleEquation.cpp
 *
 * This file is part of VLE, a framework for multi-modeling, simulation
 * and analysis of complex dynamical systems
 * http://www.vle-project.org
 *
 * Copyright (c) 2013-2013 INRA http://www.inra.fr
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

#include <vle/extension/RecurrenceRelationDbg.hpp>
#include <iostream>
#include <vle/devs/DynamicsDbg.hpp>

namespace recurrenceRelationTest { namespace dynamics  {

    namespace ver = vle::extension::recurrenceRelation;

    class SimpleEquation :
        public ver::RecurrenceRelation
    {
    public:
        SimpleEquation(const vle::devs::DynamicsInit& model,
           const vle::devs::InitEventList& events) :
               ver::RecurrenceRelation(model,events)
        {
            A = createVar("A");
            B = createVect("B",5);
        }
        virtual ~SimpleEquation(){}

        void compute(const vle::devs::Time& /*time*/)
        {
            std::cout << " --- compute " << std::endl;
            A = A(-2) + 1;
            for (unsigned int i =0; i < 5; i++){
                B[i] = B[i](-1) + i;
            }
        }

    private:

        Var A;
        Vect B;

    };

}} // namespace recurrenceRelationTest dynamics

//DECLARE_DYNAMICS(recurrence_relation::test::dynamics::SimpleEquation)
//DECLARE_RECURRENCE_RELATION_DBG(recurrence_relation::test::dynamics::SimpleEquation)
DECLARE_DYNAMICS_DBG(recurrenceRelationTest::dynamics::SimpleEquation)

