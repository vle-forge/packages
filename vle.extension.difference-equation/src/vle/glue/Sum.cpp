/*
 * @file vle/glue/Sum.cpp
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
 * Copyright (C) 2009-2011 INRA
 * Copyright (C) 2009-2011 ULCO http://www.univ-littoral.fr
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

#include <vle/extension/DifferenceEquation.hpp>
#include <vle/extension/DifferenceEquationDbg.hpp>
#include <vle/devs/DynamicsDbg.hpp>

namespace glue {

class Sum : public vle::extension::DifferenceEquation::Generic
{
public:
    Sum(const vle::devs::DynamicsInit& init,
	const vle::devs::InitEventList& events) :
	vle::extension::DifferenceEquation::Generic(init, events)
    { allSync(); }

    virtual ~Sum() { }

    virtual double compute(const vle::devs::Time& /* time */)
    {
        double s = 0;

        beginExt();
        while (not endExt()) {
            s += valueExt(0);
            nextExt();
        }
        return s;
    }

    virtual double initValue(const vle::devs::Time& time)
    { return compute(time); }
};

} // namespace glue

DECLARE_DYNAMICS(Sum, glue::Sum)
DECLARE_DYNAMICS_DBG(SumDynamicsDbg, glue::Sum)
DECLARE_DIFFERENCE_EQUATION_GENERIC_DBG(SumDbg, glue::Sum)
