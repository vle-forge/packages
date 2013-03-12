/*
 * @file vle/examples/smartgardener/Ladybird.cpp
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

#include <vle/extension/DifferentialEquationDbg.hpp>

namespace vd = vle::devs;
namespace ve = vle::extension::differential_equation;
namespace vv = vle::value;

namespace vle { namespace examples { namespace smartgardeners {

class Ladybird : public ve::DifferentialEquation
{
public:
    Ladybird(
       const vd::DynamicsInit& init,
       const vd::InitEventList& events)
    : ve::DifferentialEquation(init, events)
    {
        b = events.getDouble("b");
        d = events.getDouble("d");
        e = events.getDouble("e");
        y = createVar("y");
        x = createExt("x");
    }

    virtual void compute(const vd::Time& /*t*/)
    {
        grad(y) = b * d * x() * y() - e * y();
    }

    virtual ~Ladybird()
    { }

private:
    double b;
    double d;
    double e;
    Var y;
    Ext x;
};

}}} // namespaces

DECLARE_DIFFERENTIAL_EQUATION_DBG(vle::examples::smartgardeners::Ladybird)

