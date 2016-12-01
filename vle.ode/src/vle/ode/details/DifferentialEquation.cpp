/*
 * @file vle/ode/DifferentialEquation.cpp
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


#include <vle/ode/DifferentialEquation.hpp>
#include "TimeSlicingMethod.hpp"
#include "DifferentialEquationImpl.hpp"
#include "QSS2.hpp"

namespace vle {
namespace ode {

namespace vd = vle::devs;
namespace vv = vle::value;
namespace vu = vle::utils;
namespace ts = vle::ode::timeSlicingMethod;

/***************************************************
 * Implementation of DifferentialEquation user API
 ***************************************************/

DifferentialEquation::DifferentialEquation(const vd::DynamicsInit& model,
        const vd::InitEventList& events) :
        vd::Dynamics(model, events), meqImpl(nullptr)
{
    if (not events.exist("method")) {
        meqImpl = new timeSlicingMethod::TimeSlicingMethod<timeSlicingMethod::Euler>(*this, events);
    } else {
        std::string method = events.getString("method");
        if (method == "euler") {
            meqImpl= new timeSlicingMethod::TimeSlicingMethod<timeSlicingMethod::Euler>(*this, events);
        } else if (method == "rk4") {
            meqImpl= new timeSlicingMethod::TimeSlicingMethod<timeSlicingMethod::RK4>(*this, events);
        } else if (method == "qss2") {
            meqImpl = new qss2::QSS2(*this, events);
        } else {
            throw vu::ArgError(vle::utils::format(
                    "[%s] wrong method initialization for ode (got '%s')",
                    getModelName().c_str(), method.c_str()));
        }
    }
}

DifferentialEquation::~DifferentialEquation()
{
}

vd::Time
DifferentialEquation::init(vd::Time time)
{
    return meqImpl->init(time);
}

void
DifferentialEquation::output(vd::Time time, vd::ExternalEventList& output) const
{
    meqImpl->output(time, output);
}

vd::Time
DifferentialEquation::timeAdvance() const
{
    return meqImpl->timeAdvance();
}

void
DifferentialEquation::internalTransition(vd::Time time)
{
    meqImpl->internalTransition(time);
}

void
DifferentialEquation::externalTransition(const vd::ExternalEventList& event,
        vd::Time time)
{
    meqImpl->externalTransition(event, time);
}

void
DifferentialEquation::confluentTransitions(vd::Time time,
        const vd::ExternalEventList& extEventlist)
{
    meqImpl->confluentTransitions(time, extEventlist);
}

std::unique_ptr<vv::Value>
DifferentialEquation::observation(const vd::ObservationEvent& event) const
{
    return meqImpl->observation(event);
}



/***************************************************
 * Implementation of Var user API
 ***************************************************/

void
DifferentialEquation::Var::init(DifferentialEquation* de,
        const std::string& varName, const vle::value::Map& initMap)
{
    vle::value::Map::const_iterator ite = initMap.end();
    vle::value::Map::const_iterator itf = ite;
    std::string port;

    name.assign(varName);
    meqImpl = de->meqImpl;
    std::pair<Variables::iterator, bool> resInsert =
            meqImpl->vars().addVar(name);
    if (not resInsert.second) {
        throw vle::utils::ModellingError(vu::format("[%s] Var '%s' "
                "already declared \n",
                de->getModelName().c_str(),  name.c_str()));
    }

    itVar = dynamic_cast<Variable*>(resInsert.first->second);

    port.assign("init_value_");
    port += name;
    itf = initMap.find(port);
    if (itf != ite) {
        itVar->setVal(itf->second->toDouble().value());
    } else {
        itVar->setVal(0);//Default
    }

    port.assign("init_grad_");
    port += name;
    itf = initMap.find(port);
    if (itf != ite) {
        itVar->setGrad(itf->second->toDouble().value());
    } else {
        itVar->setGrad(0);//Default
    }
}

double
DifferentialEquation::Var::operator()() const
{
    if (not itVar or not meqImpl) {
        throw vu::ModellingError(vle::utils::format(
                "[%s] Error when const accessing value of '%s'",
                meqImpl->getModelName().c_str(), name.c_str()));
    }
    return itVar->getVal();
}

void
DifferentialEquation::Var::operator=(double value)
{
    itVar->setVal(value);
}

double
DifferentialEquation::Var::operator()()
{
    return itVar->getVal();
}

double
DifferentialEquation::grad_intern::operator()()
{
    return var.itVar->getGrad();
}

void
DifferentialEquation::grad_intern::operator=(double g)
{
    var.itVar->setGrad(g);
}

}
}//namespaces
