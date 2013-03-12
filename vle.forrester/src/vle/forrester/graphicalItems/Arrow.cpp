/*
 * @file vle/forrester/graphicalItems/Arrow.cpp
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


#include <vle/forrester/graphicalItems/Arrow.hpp>

namespace vle {
namespace gvle {
namespace modeling {
namespace forrester {

bool operator== (const Arrow &a1, const Arrow &a2)
{
    return (a1.mOrigin->getOwner() == a2.mOrigin->getOwner()) &&
            (a1.mDestination->getOwner() == a2.mDestination->getOwner());
}

}
}
}
} // namespace vle gvle modeling forrester
