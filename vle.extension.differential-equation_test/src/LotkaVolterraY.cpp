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
#include <vle/devs/DynamicsDbg.hpp>

namespace differential_equation { namespace test { namespace dynamics  {

    namespace ved = vle::extension::differential_equation;

    class LotkaVolterraY :
        public ved::DifferentialEquation
    {
    public:
        LotkaVolterraY(const vle::devs::DynamicsInit& model,
                       const vle::devs::InitEventList& events) :
            ved::DifferentialEquation(model,events)
        {
            gamma = (events.exist("gamma"))
                ? events.getDouble("gamma") : 0.5;
            delta = (events.exist("delta"))
                ? events.getDouble("delta") : 0.5;


            Y = createVar("Y");
            X = createExt("X");
        }
        virtual ~LotkaVolterraY(){}

        void compute(const vle::devs::Time& /*time*/)
        {
            grad(Y) = Y() * (gamma * X() - delta);
        }

    private:
        double gamma;
        double delta;

        Ext X;
        Var Y;
    };

}}} // namespace differential_equation test dynamics

DECLARE_DYNAMICS(differential_equation::test::dynamics::LotkaVolterraY)
    //DECLARE_DYNAMICS_DBG(differential_equation::test::dynamics::LotkaVolterraY)
