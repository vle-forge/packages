/*
 * @file vle/extension/mas/Plugin.hpp
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
#ifndef EFFECT_HPP
#define EFFECT_HPP
#include <boost/function.hpp>
#include <vle/devs/Time.hpp>
#include <vle/value/Value.hpp>
#include <unordered_map>
#include <vle/extension/mas/PropertyContainer.hpp>

namespace vd = vle::devs;

namespace vle {
namespace extension {
namespace mas {

/** @class Effect
 *  @brief Describes an effect on agents
 *
 *  This class is used to apply effect on agents.
 */
class Effect : public PropertyContainer
{
public:
    /**  Function prototype which will be use to apply effect */
    typedef boost::function<void (const Effect&)> EffectFunction;

public:
    Effect(const vd::Time& t,const std::string& name,const std::string& origin)
    :mDate(t),mName(name),mOrigin(origin)
    {}

    inline vd::Time getDate() const
    {return mDate;}

    inline void setDate(vd::Time d)
    {mDate = d;}

    inline bool atDate(vd::Time d)
    {return (mDate == d);}

    inline const std::string& getName() const
    {return mName;}

    inline std::string getOrigin() const
    {return mOrigin;}

    /* Operator overload */
    friend bool operator==(const Effect& a,const Effect& b)
    {
        return (a.mName == b.mName)
               &&(a.mOrigin == b.mOrigin);
    }
    friend bool operator< (const Effect& a,const Effect& b)
    {return a.mDate < b.mDate;}
    friend bool operator!=(const Effect& a, const Effect& b)
    {return !operator==(a,b);}
    friend bool operator> (const Effect& a, const Effect& b)
    {return  operator< (b,a);}
    friend bool operator<=(const Effect& a, const Effect& b)
    {return !operator> (a,b);}
    friend bool operator>=(const Effect& a, const Effect& b)
    {return !operator< (a,b);}
private:
    Effect();
private:
    vd::Time     mDate; /**< Date when effect must be applied */
    std::string  mName; /**< Name of effect */
    std::string  mOrigin; /**< Origin(model name) of effect */
};

}}} //namespace vle extension mas
#endif
