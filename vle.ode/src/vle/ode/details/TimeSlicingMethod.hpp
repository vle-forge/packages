/*
 * @file vle/ode/TimeSlicingMethod.hpp
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

#ifndef VLE_ODE_TIME_SLICING_METHOD_HPP
#define VLE_ODE_TIME_SLICING_METHOD_HPP 1

#include <iostream>
#include <sstream>
#include <map>
#include <set>

#include <vle/utils/Exception.hpp>

#include "DifferentialEquationImpl.hpp"

namespace vle {
namespace ode {
namespace timeSlicingMethod {

namespace vd = vle::devs;
namespace vv = vle::value;
namespace vu = vle::utils;
namespace vz = vle::vpz;
namespace de = vle::ode;

/**
 * @brief DEVS state of the dynamic
 */
enum DEVS_State
{
    INIT,
    COMPUTE,
    OUTPUT,
};

/**
 * @brief Types of DEVS transitions
 */
enum DEVS_TransitionType
{
    INTERNAL, EXTERNAL, CONFLUENT
};

/**
 * @brief Guards structure for transition
 * between DEVS states
 */
struct DEVS_TransitionGuards
{
    DEVS_TransitionGuards()
    {
    }

};

/**
 * @brief Options structure for
 * Time slicing methods
 */
struct DEVS_Options
{
    DEVS_Options() : output_period(1), dt(1.0)
    {
    }
    /**
     * @brief Period at which output of variable values are performed.
     * Output is performed each (output_period * timestep) time unit.
     */
    int output_period;
    /**
     * @brief time step of numerical integration
     */
    double dt;
};

/**
 * @brief Internal State
 */
struct  DEVS_Internal
{
    double NCt; //next compute time
    double LWUt; //last wake up time
    int nbIntegration; //nbIntegration since last output
    double declarationOn; //if true variables can be created via createVar
    DEVS_Internal() : NCt(0), LWUt(0), nbIntegration(0), declarationOn(true)
    {
    }
};

/**
 * @brief Time slicing methods for integration.
 * Template INTEGRATE is the specified integration method :
 * Runge Kutta 4 or Euler.
 */
template<class INTEGRATE>
class TimeSlicingMethod : public DifferentialEquationImpl
{

private:
    DEVS_State            devs_state;
    DEVS_Options          devs_options;
    DEVS_TransitionGuards devs_guards;
    DEVS_Internal         devs_internal;
    //integration method
    INTEGRATE             int_method;

public:

    TimeSlicingMethod(DifferentialEquation& eq,
            const vd::InitEventList& events) :
            DifferentialEquationImpl(eq, events), devs_state(INIT),
            devs_options(), devs_guards(), devs_internal(),
            int_method(*this, events)
    {
        if (events.exist("output_period")) {
            const value::Value& v = *(events.get("output_period"));
            devs_options.output_period = 0;
            if (v.isInteger()) {
                devs_options.output_period = v.toInteger().value();
            }
            if (devs_options.output_period < 1) {
                throw utils::ModellingError(vle::utils::format(
                        "[%s] Parameter 'output_period' should be an int > 0",
                        getModelName().c_str()));
            }
        }
        if (events.exist("time_step")) {
            devs_options.dt = events.getDouble("time_step");
            if (devs_options.dt <= 0) {
                throw utils::ModellingError(vle::utils::format(
                        "[%s] Parameter 'time_step' should be > 0",
                        getModelName().c_str()));
            }
        }

    }

    virtual ~TimeSlicingMethod()
    {
    }

private:

    /**
     * @brief Process method used for DEVS state entrance
     * @param t, the current time
     * @param trans, the type of the transition
     */
    void processIn(const vd::Time& /*t*/, DEVS_TransitionType /*trans*/)
    {
        switch (devs_state) {
        case INIT:
            devs_internal.declarationOn = false;
            break;
        case COMPUTE:
            break;
        case OUTPUT:
            break;
        default:
            throw vu::InternalError("default");
        }
    }

    /**
     * @brief Process method used for DEVS state exit
     * @param t, the current time
     * @param trans, the type of the transition
     */
    void processOut(const vd::Time& time, DEVS_TransitionType /*trans*/)
    {
        switch (devs_state) {
        case INIT:
            devs_internal.NCt = time + devs_options.dt;
            devs_internal.LWUt = time;
            break;
        case COMPUTE:
            int_method.updateVars(devs_internal.LWUt, time);
            devs_internal.nbIntegration = (devs_internal.nbIntegration+1)
                    % devs_options.output_period;
            devs_internal.NCt = time + devs_options.dt;
            devs_internal.LWUt = time;
            break;
        case OUTPUT:
            break;
        default:
            throw vu::InternalError("default");
        }
    }

    /**
     * @brief Update guards for internal transition
     * @param t, the current time
     * @param trans, the type of the transition
     */
    void updateGuards(const vd::Time& /*t*/, DEVS_TransitionType /*trans*/)
    {
        switch (devs_state) {
        case INIT:
            break;
        case COMPUTE:
            break;
        case OUTPUT:
            break;
        default:
            throw vu::InternalError("default guards");
        }
    }

    /**
     * @brief Handles external event and registers if necesserary
     * updates of external events (ExtUp), perturabations and discontinuities.
     * @param t, the current time
     * @param ext, the list of external event
     */
    void handleExtEvt(const vd::Time& /*t*/, const vd::ExternalEventList& ext)
    {

        vd::ExternalEventList::const_iterator itb = ext.begin();
        vd::ExternalEventList::const_iterator ite = ext.end();
        for (; itb != ite; itb++) {
            //get port name
            const std::string& portName = itb->getPortName();
            //find internal variable
            Variables::iterator itf = vars().find(portName);
            if (itf == vars().end()) {
                throw utils::InternalError(vle::utils::format(
                        "[%s] Unrecognised variable '%s'",
                        getModelName().c_str(), portName.c_str()));
            }
            if (itb->attributes()->isMap() and
                    itb->attributes()->toMap().exist("value")) {
                itf->second->setVal(
                        itb->attributes()->toMap().getDouble("value"));
            } else {
                itf->second->setVal(itb->attributes()->toDouble().value());
            }
        }
    }

    /**
     * @brief Output function that fills the value of state variables
     * @param time, time of the output
     * @param extEvtList, the list of external event
     */
    void outputVar(const vd::Time& /*time*/,
            vd::ExternalEventList& extEvtList) const
    {
        Variables::const_iterator itb = vars().begin();
        Variables::const_iterator ite = vars().end();
        for (; itb != ite; itb++) {
            const Variable& v = *itb->second;
            if (getModel().existOutputPort(itb->first)) {
                extEvtList.emplace_back(itb->first);
                value::Double& val = extEvtList.back().addDouble();
                val = v.getVal();
            }
        }
    }

    /************** DEVS functions *****************/
    vd::Time init(vd::Time time) override
    {
        devs_state = INIT;
        processIn(time, INTERNAL);
        return timeAdvance();
    }

    void output(vd::Time time, vd::ExternalEventList& ext) const override
    {
        switch (devs_state) {
        case INIT:
            break;
        case COMPUTE:
            break;
        case OUTPUT:
            if (devs_internal.nbIntegration == 0) {
                outputVar(time, ext);
            }
            break;
        default:
            throw vu::InternalError("TODO");
        }
    }

    vd::Time timeAdvance() const override
    {
        switch (devs_state) {
        case INIT:
            return 0;
            break;
        case COMPUTE:
            return devs_internal.NCt - devs_internal.LWUt;
            break;
        case OUTPUT:
            return 0;
            break;
        default:
            throw vu::InternalError("TODO 2");
        }
    }

    /**
     * @brief Implementation of internal transition DEVS
     * function
     */
    void internalTransition(vd::Time t) override
    {
        processOut(t, INTERNAL);
        updateGuards(t, INTERNAL);

        switch (devs_state) {
        case INIT:
            devs_state = COMPUTE;
            break;
        case COMPUTE:
            devs_state = OUTPUT;
            break;
        case OUTPUT:
            devs_state = COMPUTE;
            break;
        default:
            throw vu::InternalError("default 2");
        }
        processIn(t, INTERNAL);
    }

    void externalTransition(const vd::ExternalEventList& event,
            vd::Time t) override
    {
        processOut(t, EXTERNAL);
        handleExtEvt(t, event);
        updateGuards(t, EXTERNAL);

        switch (devs_state) {
        case INIT:
            throw vu::InternalError("Error 3");
            break;
        case COMPUTE:
            devs_state = COMPUTE;
            break;
        case OUTPUT:
            throw vu::InternalError("Error 4");
            break;
        default:
            throw vu::InternalError("default");
        }
        processIn(t, EXTERNAL);
    }

    void confluentTransitions(vd::Time t,
            const vd::ExternalEventList& ext) override
    {
        processOut(t, CONFLUENT);
        handleExtEvt(t, ext);
        updateGuards(t, CONFLUENT);

        switch (devs_state) {
        case INIT:
            devs_state = COMPUTE;
            break;
        case COMPUTE:
            devs_state=OUTPUT;
            break;
        case OUTPUT:
            devs_state = COMPUTE;
            break;
        default:
            throw vu::InternalError("default confluent");
        }
        processIn(t, CONFLUENT);
    }

    /***
     * @brief Implementation of observation DEVS function
     */
    std::unique_ptr<vv::Value> observation(
            const vd::ObservationEvent& event) const override
    {
        const std::string& port = event.getPortName();
        {
            Variables::const_iterator itf = vars().find(port);
            if (itf != vars().end()) {
                return value::Double::create(itf->second->getVal());
            }
        }
        return 0;
    }
};

/**
 * @brief Base class of time-slicing methods
 */
struct IntegrationMethod
{
    /**
     * @brief Differential Equation reference
     */
    DifferentialEquationImpl& eqImpl;
    /**
     * @brief Constructor of IntegrationMethod
     */
    IntegrationMethod(DifferentialEquationImpl& eq,
            const vd::InitEventList& /*params*/);
    /**
     * @brief Destructor of IntegrationMethod
     */
    virtual ~IntegrationMethod();

    virtual void updateVars(const vd::Time& tin, const vd::Time& tout) = 0;
};

struct Euler : public IntegrationMethod
{
    Euler(DifferentialEquationImpl& eq, const vd::InitEventList& events);

    virtual ~Euler();

    void updateVars(const vd::Time& tin, const vd::Time& tout);
};

struct RK4 : public IntegrationMethod
{

    RK4(DifferentialEquationImpl& eq, const vd::InitEventList& events);

    virtual ~RK4();

    void updateVars(const vd::Time& tin, const vd::Time& tout);
};

inline std::ostream& operator<<(std::ostream& o, const DEVS_State& s)
{
    switch (s) {
    case INIT:
    {
        o << "INIT ";
        break;
    }
    case COMPUTE:
    {
        o << "COMPUTE ";
        break;
    }
    case OUTPUT:
    {
        o << "OUTPUT ";
        break;
    }
    default:
    {
        throw vu::InternalError("default operator<<");
    }
    }
    return o;
}

inline std::ostream& operator<<(std::ostream& o,
        const DEVS_TransitionGuards& /*g*/)
{
    o << " guards:NULL " << ";";
    return o;
}

}
}
} // namespace vle ode timeSlicingMethod

#endif
