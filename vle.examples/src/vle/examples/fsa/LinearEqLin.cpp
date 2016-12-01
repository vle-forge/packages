/*
 * @file vle/examples/fsa/LinearEqLin.cpp
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

/*
 * @@tagdynamic@@
 * @@tagdepends: vle.discrete-time @@endtagdepends
 */

#include <vle/DiscreteTime.hpp>
#include <iomanip>

namespace vd = vle::devs;
namespace vv = vle::value;

namespace vle { namespace examples { namespace fsa {

using namespace vle::discrete_time;

class LinearEqLin :public DiscreteTimeDyn
{
public:
    LinearEqLin(const vd::DynamicsInit& init, const vd::InitEventList& evts)
        : DiscreteTimeDyn(init, evts)
    {
        Y.init(this, "Y", evts);
        A.init(this, "A", evts);
        B.init(this, "B", evts);
        X.init(this, "X", evts);
        Y = A() * X() + B();
    }

    virtual ~LinearEqLin()
    {}

    virtual void compute(const vd::Time& time) override
    {
        if (A.itVar->lastUpdateTime() < time) {
            //first perturbation from fsa occurs before this compute thus,
            //we should not update A since we need allow_update option for the
            //second perturbation, which occurs after the compute
            //TODO provide API lastUpdateTime in discrete_time
            A = A(-1);
        }
        if (B.itVar->lastUpdateTime() < time) {
            B = B(-1);
        }
        Y = A() * X() + B();
    }
private:
    Var Y;
    Var A;
    Var B;
    Var X;//sync
    //Sync X;
};

DECLARE_DYNAMICS(LinearEqLin)

}}} // namespace vle examples fsa
