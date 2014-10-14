
/*
 * Copyright (C) 2014 INRA
 *
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

#ifndef VLE_RECURSIVE_OperatorOstream_HPP_
#define VLE_RECURSIVE_OperatorOstream_HPP_

#include <iostream>
#include "EmbeddedSimulatorInput.hpp"
#include "EmbeddedSimulatorOutput.hpp"

namespace rr = vle::recursive;


std::ostream& operator<< (std::ostream& out,
                         const rr::EmbeddedSimulatorInput& d)
{
    out << d.cond_name << " " << d.port_name;
    return out;
}

std::ostream& operator<< (std::ostream& out,
                         const rr::EmbeddedSimulatorOutput& d)
{
    out << d.view_name << " " << d.port_name;
    return out;
}


#endif
