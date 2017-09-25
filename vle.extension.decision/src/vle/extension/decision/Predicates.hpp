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

#include <functional>
#include <vector>
#include <vle/utils/Tools.hpp>
#include <vle/utils/Exception.hpp>
#include <map>
#include <ostream>
#include <unordered_set>
#include <unordered_map>
#include "Parameter.hpp"

namespace bx = baryonyx;

namespace vle { namespace extension { namespace decision {

/**
 * @brief Defines parameter type for predicates.
 */

class PredicateParameters
{
public:
    typedef std::pair <std::string, bx::parameter> name_parameter_type;
    typedef std::unordered_map<std::string, bx::parameter> container_type;
    typedef container_type::const_iterator const_iterator;
    typedef container_type::iterator iterator;
    typedef container_type::size_type size_type;

    void addDouble(const std::string& name, double param);
    void addString(const std::string& name, const std::string& param);

    /**
     * To check if the parameter does exist.
     *
     */
    bool exist(const std::string& name) const;

    /**
     * Reset a double from container.
     *
     * if already exist.
     */
    void resetDouble(const std::string& name, double param);

    /**
     * Reset a string from the container.
     *
     * if already exist.
     */
    void resetString(const std::string& name, const std::string& param);

    /**
     * Get a double from container.
     *
     * @attention O(log(n)) operation, but container must be sorted with
     * the @e sort function.
     * end().
     */
    double getDouble(const std::string& name) const;

    /**
     * Get a string from the container.
     *
     * @attention O(log(n)) operation, but container must be sorted with
     * the @e sort function.
     */
    std::string getString(const std::string& name) const;

    iterator begin() { return m_lst.begin(); }
    const_iterator begin() const { return m_lst.begin(); }
    iterator end() { return m_lst.end(); }
    const_iterator end() const { return m_lst.end(); }
    size_type size() const { return m_lst.size(); }
    bool empty() const { return m_lst.empty(); }

private:
    container_type m_lst;
};

/**
 * @brief Defines a Predicate like a function which returns a boolean
 * without parameter.
 */

typedef std::function <bool (const std::string& activity,
                             const std::string& rule,
                             const PredicateParameters& params)> PredicateFunction;

class Predicate
{
public:
    Predicate(const std::string& name,
              const PredicateFunction& function,
              const PredicateParameters& params)
        : m_name(name)
        , m_function(function)
        , m_parameters(params)
    {}

    Predicate(const std::string& name,
              const PredicateFunction& function)
        : m_name(name)
        , m_function(function)
    {}

    Predicate(const std::string& name)
        : m_name(name)
    {}

    bool isAvailable(const std::string& activity,
                     const std::string& rule) const
    {
        return m_function(activity, rule, m_parameters);
    }

    const std::string& name() const { return m_name; }
    const PredicateFunction& function() const { return m_function; }
    const PredicateParameters& params() const { return m_parameters; }

private:
    std::string         m_name;
    PredicateFunction   m_function;
    PredicateParameters m_parameters;
};

struct PredicateEqual
{
    bool operator()(const Predicate& a, const Predicate& b) const
    {
        return a.name() == b.name();
    }

    bool operator()(const Predicate& a, const std::string& b) const
    {
        return a.name() == b;
    }

    bool operator()(const std::string& a, const Predicate& b) const
    {
        return a == b.name();
    }
};

struct PredicateHash
{
    std::size_t operator()(const Predicate& predicate) const
    {
        std::hash <std::string> hasher;
        return hasher(predicate.name());
    }

    std::size_t operator()(const std::string& predicate) const
    {
        std::hash <std::string> hasher;
        return hasher(predicate);
    }
};

class Predicates
{
public:
    typedef std::unordered_set <Predicate, PredicateHash,
                                PredicateEqual> container_type;
    typedef container_type::const_iterator const_iterator;
    typedef container_type::iterator iterator;
    typedef container_type::size_type size_type;

    bool add(const Predicate& pred)
    {
        return m_lst.insert(pred).second;
    }

    bool add(const std::string& name, const PredicateFunction& function)
    {
        return m_lst.insert(Predicate(name, function)).second;
    }

    bool add(const std::string& name, const PredicateFunction& function,
             const PredicateParameters& params)
    {
        return m_lst.insert(Predicate(name, function, params)).second;
    }

    bool exist(const std::string& name) const
    {
        return find(name) != end();
    }

    const_iterator find(const std::string& name) const
    {
        return m_lst.find(Predicate(name));
    }

    const Predicate& get(const std::string& name) const
    {
        const_iterator it = find(name);
        if (it == m_lst.end())
        if (it == m_lst.end())
            throw utils::ArgError(vle::utils::format("Unknown predicates '%s'", name.c_str()));

        return *it;
    }

    bool isAvailable(const std::string& activity, const std::string& rule) const;

    iterator begin() { return m_lst.begin(); }
    const_iterator begin() const { return m_lst.begin(); }
    iterator end() { return m_lst.end(); }
    const_iterator end() const { return m_lst.end(); }
    size_type size() const { return m_lst.size(); }

private:
    container_type m_lst;
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
