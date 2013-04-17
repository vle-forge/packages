/*
 * @file vle/extension/decision/Predicates.hpp
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


#ifndef VLE_EXT_DECISION_PREDICATES_HPP
#define VLE_EXT_DECISION_PREDICATES_HPP

#include <boost/function.hpp>
#include <boost/bind.hpp>
#include <vector>
#include <ostream>

namespace vle { namespace extension { namespace decision {

/**
 * @brief Defines a Predicate like a function which returns a boolean
 * without parameter.
 */
typedef boost::function < bool (void) > Predicate;

class Predicates
{
public:
    typedef std::vector < Predicate > predicates_t;
    typedef predicates_t::const_iterator const_iterator;
    typedef predicates_t::iterator iterator;
    typedef predicates_t::size_type size_type;

    void add(const Predicate& pred) { m_lst.push_back(pred); }

    bool isAvailable() const;

    iterator begin() { return m_lst.begin(); }
    const_iterator begin() const { return m_lst.begin(); }
    iterator end() { return m_lst.end(); }
    const_iterator end() const { return m_lst.end(); }
    size_type size() const { return m_lst.size(); }

private:
    predicates_t m_lst;
};

inline std::ostream& operator<<(std::ostream& s, const Predicates& o)
{
    s << "predicats: ";
    for (Predicates::const_iterator it = o.begin(); it != o.end(); ++it) {
        s << " [" << &(*it) << "]";
    }
    return s;
}

}}} // namespace vle model decision

#endif
