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
 * @@tagdepends: vle.ode @@endtagdepends
 */

#include <vle/DifferentialEquation.hpp>


namespace vle_ode { namespace test { namespace dynamics {

    using namespace vle::ode;

    class LadybirdPlantlouse :
        public DifferentialEquation
    {
    public:
        LadybirdPlantlouse(const vle::devs::DynamicsInit& model,
                           const vle::devs::InitEventList& events) :
            DifferentialEquation(model,events)
        {

            a = (events.exist("a"))
                ? events.getDouble("a") : 0.5;
            b = (events.exist("b"))
                ? events.getDouble("b") : 0.5;
            d = (events.exist("d"))
                ? events.getDouble("d") : 0.5;
            e = (events.exist("e"))
                ? events.getDouble("e") : 0.5;

            X.init(this, "X", events);
            Y.init(this, "Y", events);
        }
        virtual ~LadybirdPlantlouse(){}

        void compute(const vle::devs::Time& /*time*/)
        {

            grad(X) = a * X() - b * X() * Y();
            grad(Y) = b * d * X() * Y() - e * Y();


        }

    private:
        double a;
        double b;
        double d;
        double e;

        Var X;
        Var Y;
    };

}}} // namespace vle_ode test dynamics

DECLARE_DYNAMICS(vle_ode::test::dynamics::LadybirdPlantlouse)

