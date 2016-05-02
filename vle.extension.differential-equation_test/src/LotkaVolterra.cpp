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


namespace differential_equation { namespace test { namespace dynamics {

    namespace ved = vle::extension::differential_equation;

    class LotkaVolterra :
        public ved::DifferentialEquation
    {
    public:
        LotkaVolterra(const vle::devs::DynamicsInit& model,
                      const vle::devs::InitEventList& events) :
            ved::DifferentialEquation(model,events)
        {

            alpha = (events.exist("alpha"))
                ? events.getDouble("alpha") : 0.5;
            beta = (events.exist("beta"))
                ? events.getDouble("beta") : 0.5;
            gamma = (events.exist("gamma"))
                ? events.getDouble("gamma") : 0.5;
            delta = (events.exist("delta"))
                ? events.getDouble("delta") : 0.5;

            X = createVar("X");
            Y = createVar("Y");
        }
        virtual ~LotkaVolterra(){}

        void compute(const vle::devs::Time& /*time*/)
        {

            grad(X) = X() * (alpha - beta * Y());
            grad(Y) = Y() * (gamma * X() - delta);

        }

    private:
        double alpha;
        double beta;
        double gamma;
        double delta;

        Var X;
        Var Y;
    };

}}} // namespace differential_equation test dynamics

DECLARE_DYNAMICS(differential_equation::test::dynamics::LotkaVolterra)
    //DECLARE_DYNAMICS(differential_equation::test::dynamics::LotkaVolterra)

