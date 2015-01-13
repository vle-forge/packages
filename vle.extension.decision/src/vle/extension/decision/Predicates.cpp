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
#include <boost/variant/get.hpp>

namespace vle { namespace extension { namespace decision {

struct PredicateParametersFind
{
    bool operator()(const PredicateParameters::name_parameter_type& p,
                    const std::string& str) const
    {
        return p.first < str;
    }

    bool operator()(const std::string& str,
                    const PredicateParameters::name_parameter_type& p) const
    {
        return str < p.first;
    }
};

struct PredicateParametersCompare
{
    bool operator()(const PredicateParameters::name_parameter_type& a,
                    const PredicateParameters::name_parameter_type& b) const
    {
        return a.first < b.first;
    }
};

template <typename T>
void addParam(PredicateParameters::container_type& p,
              const std::string& name,
              const T& param)
{
    p.push_back(
        std::make_pair <std::string, PredicateParameterType> (
            name, param));
}

template <typename T>
T getParam(const PredicateParameters::container_type& p,
           const std::string& name)
{
    PredicateParameters::const_iterator it =
        std::lower_bound(p.begin(),
                         p.end(),
                         name,
                         PredicateParametersFind());

    if (it == p.end() or it->first != name)
        throw vle::utils::ModellingError(
            vle::fmt("Decision fails to get parameter %1%") % name);

    const T* ret = boost::get <T>(&it->second);
    if (ret)
        return *ret;

    throw vle::utils::ModellingError(
        vle::fmt("Decision fails to convert parameter %1%") % name);
}

void PredicateParameters::addDouble(const std::string& name, double param)
{
    addParam <double>(m_lst, name, param);
}

void PredicateParameters::addString(const std::string& name, const std::string& param)
{
    addParam <std::string>(m_lst, name, param);
}

void PredicateParameters::sort()
{
    std::sort(m_lst.begin(), m_lst.end(), PredicateParametersCompare());
}

double PredicateParameters::getDouble(const std::string& name) const
{
    return getParam <double>(m_lst, name);
}

std::string PredicateParameters::getString(const std::string& name) const
{
    return getParam <std::string>(m_lst, name);
}

}}} // namespace vle model decision
