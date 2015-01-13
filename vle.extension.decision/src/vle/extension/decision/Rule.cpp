/*
 * @file vle/extension/decision/Rule.cpp
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


#include <vle/extension/decision/Rule.hpp>


namespace vle { namespace extension { namespace decision {

Rule::Rule()
{
    m_predicates.reserve(4u);
    m_predicates_function.reserve(4u);
}

void Rule::add(const Predicate *pred)
{
    if (not pred)
        throw vle::utils::ArgError(_("Add a null Predicate"));

    m_predicates.push_back(pred);
}

void Rule::add(const PredicateFunction& function)
{
    m_predicates_function.push_back(function);
}

bool Rule::isAvailable(const std::string& activity,
                       const std::string& rule) const
{
    for (size_t i = 0, e = m_predicates.size(); i != e; ++i)
        if (not m_predicates[i]->isAvailable(activity, rule))
            return false;

    if (not m_predicates_function.empty()) {
        PredicateParameters empty;

        for (size_t i = 0, e = m_predicates_function.size(); i != e; ++i)
            if (not m_predicates_function[i](activity, rule, empty))
                return false;
    }

    return true;
}

}}} // namespace vle model decision
