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

    class Im :
        public ved::DifferentialEquation
    {
    public:
        Im(const vle::devs::DynamicsInit& model,
           const vle::devs::InitEventList& events) :
            ved::DifferentialEquation(model,events)
        {
            gamma = events.getDouble("gamma"); //= 0.2
            sigma = events.getDouble("sigma"); // = 0.5

            I = createVar("I");
            E = createExt("E");
        }
        virtual ~Im(){}

        void compute(const vle::devs::Time& /*time*/)
        {
            grad(I) = sigma*E() - gamma * I();
        }

    private:
        //rate an infected recovers and moves into the resistant phase.
        double gamma; //= 0.2,
        //rate at which an exposed person becomes infective.
        double sigma; // = 0.5,

        Var I;
        Ext E;
    };

}}} // namespace differential_equation test dynamics

DECLARE_DYNAMICS(differential_equation::test::dynamics::Im)
    //DECLARE_DYNAMICS(differential_equation::test::dynamics::Im)

