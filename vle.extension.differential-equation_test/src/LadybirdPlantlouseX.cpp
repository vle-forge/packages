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

namespace differential_equation { namespace test { namespace dynamics {

    namespace ved = vle::extension::differential_equation;

    class LadybirdPlantlouseX : public ved::DifferentialEquation
    {
    public:
        LadybirdPlantlouseX(const vle::devs::DynamicsInit& model,
                            const vle::devs::InitEventList& events) :
            ved::DifferentialEquation(model,events)
        {
            a = (events.exist("a"))
                ? events.getDouble("a") : 0.5;
            b = (events.exist("b"))
                ? events.getDouble("b") : 0.5;

            X = createVar("X");
            Y = createExt("Y");
        }
        virtual ~LadybirdPlantlouseX(){}

        void compute(const vle::devs::Time& /*time*/)
        {
            grad(X) = a * X() - b * X() * Y();

        }

    private:
        double a;
        double b;

        Var X;
        Ext Y;
    };

}}} // namespace differential_equation test dynamics

DECLARE_DYNAMICS(differential_equation::test::dynamics::LadybirdPlantlouseX)
    //DECLARE_DYNAMICS_DBG(differential_equation::test::dynamics::LadybirdPlantlouseX)

