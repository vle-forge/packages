/*
 * @file vle/examples/smartgardener/Plantlouse.cpp
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
 * VLE Environment - the multimodeling and simulation environment
 * This file is a part of the VLE environment (http://vle.univ-littoral.fr)
 * Copyright (C) 2003 - 2008 The VLE Development Team
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



namespace vd = vle::devs;
namespace vv = vle::value;

namespace vle { namespace examples { namespace smartgardeners {


using namespace vle::discrete_time;

class Plantlouse : public DiscreteTimeDyn
{
public:
    Plantlouse(
       const vd::DynamicsInit& atom,
       const vd::InitEventList& evts)
        : DiscreteTimeDyn(atom, evts)
    {
        a = vv::toDouble(evts.get("a"));
        b = vv::toDouble(evts.get("b"));
        ts = evts.getDouble("time_step");
        x.init(this, "x", evts);
        y.init(this, "y", evts);
    }

    virtual ~Plantlouse()
    {}


    virtual void compute(const vd::Time& time) override
    {
        if (x.itVar->lastUpdateTime() <= time - ts) {
            //if an event occurred between the previous compute and this one
            //it is understood as a forcing event, thus no update is done
            x = x(-1) + ts * (a* x(-1)-b*y()*x(-1));
        }
    }


private:
    double a;
    double b;
    double ts;
    Var x;
    Var y;//sync
};

}}} // namespaces

DECLARE_DYNAMICS(vle::examples::smartgardeners::Plantlouse)

