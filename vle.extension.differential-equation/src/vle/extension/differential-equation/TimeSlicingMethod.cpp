/*
 * @file vle/extension/differential_equation/TimeSlicingMethod.cpp
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

#include <iostream>
#include <sstream>
#include <map>
#include <set>

#include <vle/vpz/AtomicModel.hpp>
#include <vle/utils/Exception.hpp>

#include <vle/extension/differential-equation/DllDefines.hpp>
#include <vle/extension/differential-equation/DifferentialEquationImpl.hpp>
#include <vle/extension/differential-equation/PerturbationHandler.hpp>
#include <vle/extension/differential-equation/TimeSlicingMethod.hpp>

namespace vle {
namespace extension {
namespace differential_equation {
namespace timeSlicingMethod {

namespace vd = vle::devs;
namespace vv = vle::value;
namespace vu = vle::utils;
namespace vz = vle::vpz;

VarImprover::VarImprover() :
        buVal(0), buGrad(0)
{
}

VarImprovers::VarImprovers() :
        mcont()
{
}

VarImprovers::cont::iterator VarImprovers::find(
        de::Variables::const_iterator it)
{
    cont::iterator itb = mcont.begin();
    cont::iterator ite = mcont.end();
    for (; itb != ite; itb++) {
        if (itb->first == it) {
            return itb;
        }
    }
    return ite;
}

VarImprovers::cont::const_iterator VarImprovers::find(
        de::Variables::const_iterator it) const
{
    cont::const_iterator itb = mcont.begin();
    cont::const_iterator ite = mcont.end();
    for (; itb != ite; itb++) {
        if (itb->first == it) {
            return itb;
        }
    }
    return ite;
}

VarImprover& VarImprovers::operator[](de::Variables::const_iterator it)
{
    cont::iterator itf = find(it);
    if (itf == mcont.end()) {
        mcont.push_back(std::make_pair(it, VarImprover()));
        ;
        itf = find(it);
    }
    return itf->second;
}

IntegrationMethod::IntegrationMethod(DifferentialEquationImpl& eq,
        const vv::Map& /*params*/) :
        eqImpl(eq)
{
}

IntegrationMethod::~IntegrationMethod()
{
}

Euler::Euler(DifferentialEquationImpl& eq, const vv::Map& params) :
        IntegrationMethod(eq, params), timestep(1)
{
    timestep = params.getDouble("timestep");
}

Euler::~Euler()
{
}

vd::Time Euler::integrationTime(const vd::Time& /*time*/)
{
    return timestep;
}

void Euler::predictValue(const vd::Time& tin, const vd::Time& tout,
        VarImprovers& /*predictions*/)
{
    if (tout == tin) {
        //no need for computation
        return;
    }

    /*
     * y_{n+1} = h * f(t,y_{n})
     * where t = tin
     * h = tout - tin
     *
     */
    eqImpl.compute(tin);
    Variables::iterator itb = eqImpl.vars().begin();
    Variables::iterator ite = eqImpl.vars().end();
    for (; itb != ite; itb++) {
        Variable& v = itb->second;
        v.setVal(v.getVal() + v.getGrad() * (tout - tin));
    }

}

RK4::RK4(DifferentialEquationImpl& eq, const vv::Map& params) :
        IntegrationMethod(eq, params), timestep(1)
{
    timestep = params.getDouble("timestep");
}

RK4::~RK4()
{
}

vd::Time RK4::integrationTime(const vd::Time& /*time*/)
{
    return timestep;
}

void RK4::predictValue(const vd::Time& tin, const vd::Time& tout,
        VarImprovers& /*predictions*/)
{
    if (tout == tin) {
        //no need for computation
        return;
    }

    /**
     * y_{n+1} = y_{n} + 1/6 * (k_1 + 2 * k_2 + 2 * k_3 + k_4)
     * t_{n+1} = t_{n} + h
     * with:  h  = tout - tin
     *        f such as y' = f(t,y)
     *        t_{n} is tin
     * k_1 = h * f(t_{n}, y_{n})
     * k_2 = h * f(t_{n} + 1/2 * h, y_n + 1/2 * k_1)
     * k_3 = h * f(t_{n} + 1/2 * h, y_n + 1/2 * k_2)
     * k_4 = h * f(t_{n} + h, y_{n} + k_3)
     *
     **/
    unsigned int nbVars = eqImpl.vars().size();
    unsigned int index = 0;
    double k_i = 0;
    double duration = (tout - tin);

    //sup represents the computation (incremental) of
    //  (k_1 + 2 * k_2 + 2 * k_3 + k_4)
    std::vector<double> sup(nbVars, 0);

    //y_n represents a backup of y_{n}
    std::vector<double> y_n(nbVars, 0);
    {
        Variables::iterator itb = eqImpl.vars().begin();
        Variables::iterator ite = eqImpl.vars().end();
        for (index = 0; itb != ite; itb++) {
            const Variable& v = itb->second;
            y_n[index] = v.getVal();
            index++;
        }
    }
    //compute k_1
    this->eqImpl.compute(tin);
    {
        Variables::iterator itb = eqImpl.vars().begin();
        Variables::iterator ite = eqImpl.vars().end();
        for (index = 0; itb != ite; itb++) {
            Variable& v = itb->second;
            k_i = duration * v.getGrad();
            sup[index] += k_i;
            //set value to y_{n} + 1/2 * k_1 (for computing k_2)
            v.setVal(y_n[index] + k_i / 2.0);
            index++;
        }
    }
    //compute k_2
    this->eqImpl.compute(tin + duration / 2.0);
    {
        Variables::iterator itb = eqImpl.vars().begin();
        Variables::iterator ite = eqImpl.vars().end();
        for (index = 0; itb != ite; itb++) {
            Variable& v = itb->second;
            k_i = duration * v.getGrad();
            sup[index] += 2 * k_i;
            //set value to y_{n} + 1/2 * k_2 (for computing k_3)
            v.setVal(y_n[index] + k_i / 2.0);
            index++;
        }
    }
    //compute k_3
    this->eqImpl.compute(tin + duration / 2.0);
    {
        Variables::iterator itb = eqImpl.vars().begin();
        Variables::iterator ite = eqImpl.vars().end();
        for (index = 0; itb != ite; itb++) {
            Variable& v = itb->second;
            k_i = duration * v.getGrad();
            sup[index] += 2 * k_i;
            //set value to y_{n} + k_3 (for computing k_4)
            v.setVal(y_n[index] + k_i);
            index++;
        }
    }
    //compute k_4 and resulting values
    this->eqImpl.compute(tin + duration);
    {
        Variables::iterator itb = eqImpl.vars().begin();
        Variables::iterator ite = eqImpl.vars().end();
        for (index = 0; itb != ite; itb++) {
            Variable& v = itb->second;
            k_i = duration * v.getGrad();
            sup[index] += k_i;
            //prediction = y_{n} + 1/6 * (k_1 + 2 * k_2 + 2 * k_3 + k_4)
            v.setVal(y_n[index] + sup[index] / 6.0);
            index++;
        }
    }
}

}
}
}
}// namespace vle extension differential_equation timeSlicingMethod
