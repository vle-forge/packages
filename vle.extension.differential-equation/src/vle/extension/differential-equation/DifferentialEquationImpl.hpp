/*
 * @file vle/extension/differential_equation/DifferentialEquationImpl.hpp
 *
 * This file is part of VLE, a framework for multi-modeling, simulation
 * and analysis of complex dynamical systems
 * http://www.vle-project.org
 *
 * Copyright (c) 2011 INRA http://www.inra.fr
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

#ifndef VLE_EXTENSION_DIFFERENTIAL_EQUATION_DIFFERENTIAL_EQUATION_IMPL_HPP
#define VLE_EXTENSION_DIFFERENTIAL_EQUATION_DIFFERENTIAL_EQUATION_IMPL_HPP 1

#include <iostream>
#include <sstream>
#include <map>
#include <set>

#include <vle/vpz/AtomicModel.hpp>
#include <vle/devs/Time.hpp>
#include <vle/utils/Exception.hpp>

#include <vle/extension/differential-equation/DifferentialEquation.hpp>

namespace vle {
namespace extension {
namespace differential_equation {

namespace vd = vle::devs;
namespace vv = vle::value;
namespace vu = vle::utils;
namespace vz = vle::vpz;

class DifferentialEquation;

class DifferentialEquationImpl
{
public:

    DifferentialEquationImpl(DifferentialEquation& eq,
            const vv::Map& /*parameters*/) :
            meq(eq)
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
        return meq.vars();
    }

    inline Variables& vars()
    {
        return meq.vars();
    }

    inline const ExternVariables& extVars() const
    {
        return meq.extVars();
    }

    inline ExternVariables& extVars()
    {
        return meq.extVars();
    }

    inline bool isVar(const std::string& v) const
    {
        return vars().find(v) != vars().end();
    }

    inline bool isExtVar(const std::string& v) const
    {
        return extVars().find(v) != extVars().end();
    }

    virtual vd::Time init(const vd::Time& time) = 0;

    virtual void output(const vd::Time& time,
            vd::ExternalEventList& output) const = 0;

    virtual vd::Time timeAdvance() const = 0;

    virtual void confluentTransitions(const vd::Time& time,
            const vd::ExternalEventList& extEventlist) = 0;

    virtual void internalTransition(const vd::Time& event) = 0;

    virtual void externalTransition(const vd::ExternalEventList& event,
            const vd::Time& time) = 0;

    virtual vv::Value* observation(const vd::ObservationEvent& event) const = 0;

protected:

    DifferentialEquation& meq;
};

}
}
} // namespace vle extension differential_equation

#endif
