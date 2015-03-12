/*
 * @file vle/extension/mas/PropertyContainer.hpp
 *
 * This file is part of VLE, a framework for multi-modeling, simulation
 * and analysis of complex dynamical systems
 * http://www.vle-project.org
 *
 * Copyright (c) 2013-2015 INRA http://www.inra.fr
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
#ifndef PROPERTY_CONTAINER
#define PROPERTY_CONTAINER

#include <vle/value/Value.hpp>
#include <unordered_map>

namespace vle {
namespace extension {
namespace mas {

namespace vv = vle::value;

class PropertyContainer
{
/* Public types */
public:
    typedef std::shared_ptr<vv::Value> value_ptr;
    typedef std::unordered_map<std::string, value_ptr> property_map;

/* Public functions */
public:
    inline void add(const std::string &t, vv::Value * && v)
    {add(t, value_ptr(v)); }

    inline bool exists(const std::string &title) const
    {return (mInformations.find(title) != mInformations.end());}

    void add(const std::string &t, const value_ptr &v)
    {
        if (exists(t))
            mInformations.erase(t);
        mInformations.insert(std::make_pair(t, v));
    }

    value_ptr get(const std::string& p) const
    {
        try {
            return mInformations.at(p);
        } catch (std::exception e) {
            std::string txt = e.what();
            txt = "Runtime error(PropertyContainer): property operation failed";
            txt += " => key=" + p;
            throw std::logic_error(txt);
        }
    }

    inline const property_map& getInformations() const
    {return mInformations;}

/* Private members */
private:
    property_map mInformations;
};

}
}
}// namespace vle extension mas

#endif
