/*
 * @file vle/examples/equation/D3.cpp
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
 * @@tagdepends: vle.extension.difference-equation @@endtagdepends
 */


#include <vle/extension/difference-equation/Simple.hpp>

namespace vle { namespace examples { namespace equation {

    class D3 : public extension::DifferenceEquation::Simple
    {
        Var d;
        Sync c;

    public:
        D3(const devs::DynamicsInit& model,
           const devs::InitEventList& events) :
	    vle::extension::DifferenceEquation::Simple(model, events)
	    {
		d = createVar("d");
		c = createSync("c");
	    }

        virtual ~D3() { }

        virtual double compute(const devs::Time& /* time */)
	    {
		return d(-1) + c() + 1;
	    }

        virtual double initValue(const devs::Time& /* time */)
	    {
		return c() + 1;
	    }

    };

}}} // namespace vle examples equation

DECLARE_DYNAMICS(vle::examples::equation::D3)
