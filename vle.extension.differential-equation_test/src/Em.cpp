/*
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

/**
 * @@tagdynamic@@
 * @@tagdepends: vle.extension.differential-equation @@endtagdepends
 */

#include <vle/extension/DifferentialEquation.hpp>


namespace differential_equation { namespace test { namespace dynamics  {

    namespace ved = vle::extension::differential_equation;

    class Sm :
        public ved::DifferentialEquation
    {
    public:
        Sm(const vle::devs::DynamicsInit& model,
           const vle::devs::InitEventList& events) :
            ved::DifferentialEquation(model,events)
        {
            beta = events.getDouble("beta");// = 0.9
            sigma = events.getDouble("sigma"); // = 0.5

            E = createVar("E");
            S = createExt("S");
            I = createExt("I");
            R = createExt("R");
        }
        virtual ~Sm(){}

        void compute(const vle::devs::Time& /*time*/)
        {
            n = S() + E() + I() + R();
            grad(E) =  beta*S()*I()/n - sigma*E();
        }

    private:
        //total population, should be constant
        double n;// = 11;
        //parameter controlling how often a susceptible-infected contact
        //results in a new exposure.
        double beta;// = 0.9;
        //rate at which an exposed person becomes infective.
        double sigma; // = 0.5,

        Var E;
        Ext S;
        Ext I;
        Ext R;

    };

}}} // namespace differential_equation test dynamics

DECLARE_DYNAMICS(differential_equation::test::dynamics::Sm)
    //DECLARE_DYNAMICS(differential_equation::test::dynamics::Sm)

