/*
 * @file vle/extension/decision/Predicates.cpp
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


#include <vle/extension/decision/Predicates.hpp>
#include <vle/utils/Exception.hpp>

namespace vle { namespace extension { namespace decision {

bool PredicateParameters::exist(const std::string& name) const
{
    auto it = m_lst.find(name);
    if (it == m_lst.cend())
        return false;
    return true;
}

void PredicateParameters::addDouble(const std::string& name, double param)
{
     m_lst.insert(
         std::make_pair(name, bx::parameter(param)));
}

void PredicateParameters::addString(const std::string& name,
                                    const std::string& param)
{
    m_lst.insert(
        std::make_pair(name, bx::parameter(param)));
}

void PredicateParameters::resetDouble(const std::string& name, double param)
{
    m_lst[name] = param;
}

void PredicateParameters::resetString(const std::string& name,
                                      const std::string& param)
{
    m_lst[name] = param;
}

double PredicateParameters::getDouble(const std::string& name) const
{
    auto it = m_lst.find(name);
    if (it == m_lst.cend())
        return 0.;

    if (it->second.type == bx::parameter::tag::real)
        return it->second.d;

    return 0.;
}

std::string PredicateParameters::getString(const std::string& name) const
{
    auto it = m_lst.find(name);
    if (it == m_lst.cend())
        return {};

    if (it->second.type == bx::parameter::tag::string)
        return it->second.s;

    return {};

}

}}} // namespace vle model decision
