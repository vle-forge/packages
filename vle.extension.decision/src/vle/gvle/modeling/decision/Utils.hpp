/*
 * @file vle/gvle/modeling/decision/AckFunctionDialog.hpp
 *
 * This file is part of VLE, a framework for multi-modeling, simulation
 * and analysis of complex dynamical systems
 * http://www.vle-project.org
 *
 * Copyright (c) 2003-2007 Gauthier Quesnel <quesnel@users.sourceforge.net>
 * Copyright (c) 2003-2013 ULCO http://www.univ-littoral.fr
 * Copyright (c) 2007-2013 INRA http://www.inra.fr
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

#ifndef VLE_GVLE_UTILS_HPP
#define VLE_GVLE_UTILS_HPP

#include <string>

namespace vle {
namespace gvle {
namespace modeling {
namespace decision {

class Utils
{
public:
    static bool isValidName(std::string name)
    {
        if (name.empty()) {
            return false;
        }

        if (isdigit(name[0])) {
            return false;
        }

        size_t i = 0;
        while (i < name.length()) {
            if (!isalnum(name[i])) {
                if (name[i] != '_') {
                    return false;
                }
            }
            i++;
        }
        return true;
    };
};

}
}
}
}    // namespace vle gvle modeling decision

#endif
