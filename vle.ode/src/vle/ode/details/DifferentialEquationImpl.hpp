/*
 * @file vle/ode/DifferentialEquationImpl.hpp
 *
 * This file is part of VLE, a framework for multi-modeling, simulation
 * and analysis of complex dynamical systems
 * http://www.vle-project.org
 *
 * Copyright (c) 2011-2016 INRA http://www.inra.fr
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

#ifndef VLE_ODE_DIFFERENTIAL_EQUATION_IMPL_HPP
#define VLE_ODE_DIFFERENTIAL_EQUATION_IMPL_HPP 1

#include <iostream>
#include <sstream>
#include <map>
#include <set>

#include <vle/vpz/AtomicModel.hpp>
#include <vle/devs/ExternalEventList.hpp>
#include <vle/devs/Time.hpp>
#include <vle/vpz/AtomicModel.hpp>
#include <vle/value/Map.hpp>
#include <vle/utils/Exception.hpp>

#include <vle/ode/DifferentialEquation.hpp>

namespace vle {
namespace ode {

namespace vv = vle::value;
namespace vd = vle::devs;
namespace vz = vle::vpz;

class DifferentialEquation;
class DifferentialEquationImpl;


class Variable
{
public:

    Variable() :
        value(0), gradient(0)
    {
    }


    inline void setGrad(double g)
    {
        gradient = g;
    }

    inline double getVal() const
    {
        return value;
    }

    inline void setVal(double v)
    {
        value = v;
    }

    inline double getGrad() const
    {
        return gradient;
    }


private:
    double value;
    double gradient;
};

class Variables
{
private:
    typedef typename std::map<std::string, Variable*> Container;
    Container                 cont;
    DifferentialEquationImpl& meqImpl;

public:
    typedef typename Container::const_iterator const_iterator;
    typedef typename Container::iterator iterator;

    Variables(DifferentialEquationImpl& eqImpl) : cont(), meqImpl(eqImpl)
    {
    }

    const_iterator begin() const
    {
        return cont.begin();
    }

    const_iterator end() const
    {
        return cont.end();
    }

    iterator begin()
    {
        return cont.begin();
    }

    iterator end()
    {
        return cont.end();
    }

    iterator find(const std::string& n)
    {
        return cont.find(n);
    }

    const_iterator find(const std::string& n) const
    {
        return cont.find(n);
    }

    unsigned int size() const
    {
        return cont.size();
    }

    std::pair<iterator, bool> addVar(const std::string& name)
    {
        Variable* v= new Variable();
        std::pair<iterator, bool> ret = cont.insert(std::make_pair(name,v));
        if (not ret.second) {
            delete v;
        }
        return ret;
    }
};



class DifferentialEquationImpl
{
public:

    DifferentialEquationImpl(DifferentialEquation& eq,
            const vd::InitEventList& /*events*/) : mvars(*this), meq(eq)
    {

    }

    virtual ~DifferentialEquationImpl()
    {
    }

    inline void compute(double time)
    {
        meq.compute(time);
    }

    inline const vz::AtomicModel& getModel() const
    {
        return meq.getModel();
    }

    inline const std::string& getModelName() const
    {
        return meq.getModelName();
    }

    inline const Variables& vars() const
    {
        return mvars;
    }

    inline Variables& vars()
    {
        return mvars;
    }

    inline bool isVar(const std::string& v) const
    {
        return vars().find(v) != vars().end();
    }

    /*  - - - - - - - - - - - - - --ooOoo-- - - - - - - - - - - -  */
    ////////////////////////
    //simili DEVS interface
    ////////////////////////

    virtual vd::Time init(vd::Time time) =0 ;
    virtual void output(vd::Time time, vd::ExternalEventList& output) const =0;
    virtual vd::Time timeAdvance() const = 0;
    virtual void internalTransition(vd::Time time) = 0;
    virtual void externalTransition(const vd::ExternalEventList& event,
            vd::Time time) = 0;
    virtual void confluentTransitions(vd::Time time,
            const vd::ExternalEventList& extEventlist) = 0;
    virtual std::unique_ptr<vv::Value> observation(
            const vd::ObservationEvent& event) const = 0;


    Variables             mvars;
    DifferentialEquation& meq;
};

}
} // namespace vle ode

#endif
