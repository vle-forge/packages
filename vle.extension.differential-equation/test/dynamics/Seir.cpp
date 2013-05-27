/*
 * @file test/dynamics/Seir.cpp
 *
 * This file is part of VLE, a framework for multi-modeling, simulation
 * and analysis of complex dynamical systems
 * http://www.vle-project.org
 *
 * Copyright (c) 2011 INRA http://www.inra.fr
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

#include <vle/extension/DifferentialEquation.hpp>
#include <vle/devs/DynamicsDbg.hpp>

namespace differential_equation { namespace test { namespace dynamics {

    namespace ved = vle::extension::differential_equation;

    class Seir :
        public ved::DifferentialEquation
    {
    public:
        Seir(const vle::devs::DynamicsInit& model,
             const vle::devs::InitEventList& events) :
            ved::DifferentialEquation(model,events)
        {

            beta = events.getDouble("beta");// = 0.9
            gamma = events.getDouble("gamma"); //= 0.2
            sigma = events.getDouble("sigma"); // = 0.5
            nu  = events.getDouble("nu");// = 0.0

            S = createVar("S");
            E = createVar("E");
            I = createVar("I");
            R = createVar("R");

        }
        virtual ~Seir(){}

        void compute(const vle::devs::Time& /*time*/)
        {
            n = S() + E() + I() + R();
            grad(S) =  - beta*S()*I()/n - nu*S();
            grad(E) =  beta*S()*I()/n - sigma*E();
            grad(I) = sigma*E() - gamma * I();
            grad(R) = gamma*I()  + nu*S();

        }

    private:
        //total population, should be constant
        double n;// = 11;
        //parameter controlling how often a susceptible-infected contact
        //results in a new exposure.
        double beta;// = 0.9;
        //rate an infected recovers and moves into the resistant phase.
        double gamma; //= 0.2,
        //rate at which an exposed person becomes infective.
        double sigma; // = 0.5,
        //vaccination rate
        double nu;// = 0.0)

        Var S;//susceptible
        Var E;//exposed
        Var I;//infectious
        Var R;//recovered
    };

}}} // namespace differential_equation test dynamics

DECLARE_DYNAMICS(differential_equation::test::dynamics::Seir)
    //DECLARE_DYNAMICS_DBG(differential_equation::test::dynamics::Seir)
