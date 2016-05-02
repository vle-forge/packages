/*
 * @file vle/extension/differential_equation/TimeSlicingMethod.hpp
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

#ifndef VLE_EXTENSION_DIFFERENTIAL_EQUATION_HYBRIDSYSTEM_HPP
#define VLE_EXTENSION_DIFFERENTIAL_EQUATION_HYBRIDSYSTEM_HPP 1

#include <iostream>
#include <sstream>
#include <map>
#include <set>

#include <vle/utils/Exception.hpp>

#include <vle/extension/differential-equation/DifferentialEquationImpl.hpp>
#include <vle/extension/differential-equation/PerturbationHandler.hpp>

namespace vle {
namespace extension {
namespace differential_equation {
namespace timeSlicingMethod {

namespace vd = vle::devs;
namespace vv = vle::value;
namespace vu = vle::utils;
namespace vz = vle::vpz;
namespace de = vle::extension::differential_equation;

/**
 * @brief DEVS state of the dynamic
 */
enum TSM_State
{
    INIT_SEND,
    INIT_BAG_EATER,
    INTEGRATION_TIME,
    PERTURBATION,
    NEW_DISC,
    HAS_DISC,
    UPDATE_EXT
};

/**
 * @brief Types of DEVS transitions
 */
enum TransitionType
{
    INTERNAL, EXTERNAL, CONFLUENT
};

/**
 * @brief Guards structure for transition
 * between DEVS states
 */
struct TSM_TransitionGuards
{
    TSM_TransitionGuards() :
            all_ext_init(false), e_eq_0(false), pert_in_evt(false),
            new_disc(false), has_disc(false)
    {
    }
    /**
     * @brief True if all external variables are
     * initialized. This guard is computed and used
     * only on the two first states
     * INIT_SEND and INIT_BAG_EATER.
     */
    bool all_ext_init;
    /**
     * @brief True if elapsed time
     * equals 0
     */
    bool e_eq_0;
    bool pert_in_evt;
    bool new_disc;
    bool has_disc;

};

/**
 * @brief Options structure for
 * Time slicing methods
 */
struct TSM_Options
{
    TSM_Options() :
        synchronization(false), output_period(1)
    {
    }
    /**
     * @brief If true, then external variables
     * are udpated at the same time as integration,
     * the system takes into account this new
     * values and proceed to a new calculation
     * for the current integration step
     */
    bool synchronization;
    /**
     * @brief Period at which output of variable values are performed.
     * Output is performed each (output_period * timestep) time unit.
     */
    int output_period;
};

/**
 * @brief Improver of a state variable
 */
class VarImprover
{
public:
    VarImprover();

    inline double getBuGrad() const
    {
        return buGrad;
    }

    inline double getBuVal() const
    {
        return buVal;
    }

    inline void setBackUp(double v, double g)
    {
        buVal = v;
        buGrad = g;
    }

private:

    /**
     * @brief backup of the state variable value
     */
    double buVal;

    /**
     * @brief backup of the state variable gradient
     */
    double buGrad;
};

/**
 * @brief Container of state variables improvers
 */
class VarImprovers
{
public:
    typedef std::vector<std::pair<de::Variables::const_iterator, VarImprover> >
         cont;

    cont mcont;

    VarImprovers();

    cont::iterator find(de::Variables::const_iterator it);

    cont::const_iterator find(de::Variables::const_iterator it) const;

    VarImprover& operator[](de::Variables::const_iterator it);
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

    //definition of DEVS state variables
    TSM_State state;
    vd::Time nextIntegration;
    vd::Time lastWakeUp;
    int nbIntegration;//nb integration steps since last output
    //integration method
    INTEGRATE intMethod;
    //Variable improver
    VarImprovers varImprovers;
    //Perturbation handling
    ExtUps extUps;
    Discontinuities discontinuities;
    //Transition guards and options
    TSM_TransitionGuards guards;
    TSM_Options options;

public:

    TimeSlicingMethod(DifferentialEquation& eq, const vv::Map& params) :
            DifferentialEquationImpl(eq, params), state(INIT_SEND),
            nextIntegration(vd::negativeInfinity),
            lastWakeUp(vd::negativeInfinity), nbIntegration(0),
            intMethod(*this, params), varImprovers(), extUps(),
            discontinuities(eq.getModelName()), guards(), options()
    {
        if (params.exist("synchronization")) {
            options.synchronization = params.getBoolean("synchronization");
        } else {
            //compute the default parameter
            if (extVars().size() == 0) {
                options.synchronization = false;
            } else {
                options.synchronization = true;
            }
        }
        if (params.exist("output_period")) {
            const value::Value& v = *(params.get("output_period"));
            options.output_period = 0;
            if (v.isInteger()) {
                options.output_period = (double) v.toInteger().value();
            }
            if (options.output_period < 1) {
                throw utils::ModellingError(
                        (boost::format("[%1%] Parameter 'output_period' should "
                                "be an Integer > 0")).str());
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
    void processIn(const vd::Time& t, TransitionType trans)
    {
        switch (state) {
        case INIT_SEND:
            lastWakeUp = t;
            break;
        case INIT_BAG_EATER:
            lastWakeUp = t;
            break;
        case INTEGRATION_TIME:
            switch (trans) {
            case INTERNAL:
                nextIntegration = t + intMethod.integrationTime(t);
                backUpVars(t);
                intMethod.predictValue(t, nextIntegration, varImprovers);
                lastWakeUp = t;
                nbIntegration = (nbIntegration + 1) % options.output_period;
                break;
            case EXTERNAL:
                restoreBackup(t);
                applyExtUp(t);
                nextIntegration = t + intMethod.integrationTime(t);
                backUpVars(t);
                intMethod.predictValue(t, nextIntegration, varImprovers);
                lastWakeUp = t;
                nbIntegration = (nbIntegration + 1) % options.output_period;
                break;
            case CONFLUENT:
                applyExtUp(t);
                nextIntegration = t + intMethod.integrationTime(t);
                backUpVars(t);
                intMethod.predictValue(t, nextIntegration, varImprovers);
                lastWakeUp = t;
                nbIntegration = (nbIntegration + 1) % options.output_period;
                break;
            }
            break;
        case PERTURBATION:
            switch (trans) {
            case INTERNAL:
                lastWakeUp = t;
                break;
            case EXTERNAL:
                restoreBackup(t);
                intMethod.predictValue(lastWakeUp, t, varImprovers);
                applyExtUp(t);
                meq.reinit(discontinuities.getPerturbsForReinit(), true, t);
                lastWakeUp = t;
                break;
            case CONFLUENT:
                applyExtUp(t);
                meq.reinit(discontinuities.getPerturbsForReinit(), true, t);
                lastWakeUp = t;
                break;
            }
            break;
        case NEW_DISC:
        case HAS_DISC:
            switch (trans) {
            case INTERNAL:
                lastWakeUp = t;
                break;
            case EXTERNAL:
                restoreBackup(t);
                intMethod.predictValue(lastWakeUp, t, varImprovers);
                applyExtUp(t);
                meq.reinit(discontinuities.getPerturbsForReinit(), false, t);
                lastWakeUp = t;
                break;
            case CONFLUENT:
                applyExtUp(t);
                meq.reinit(discontinuities.getPerturbsForReinit(), false, t);
                lastWakeUp = t;
                break;
            }
            break;
        case UPDATE_EXT:
            lastWakeUp = t;
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
    void processOut(const vd::Time& t, TransitionType trans)
    {
        switch (state) {
        case INIT_SEND:
            break;
        case INIT_BAG_EATER:
            applyExtUp(t); //TODO not in accordance with graph
            break;
        case INTEGRATION_TIME:
            break;
        case PERTURBATION:
        case NEW_DISC:
        case HAS_DISC:
            discontinuities.resetDiscontinuities();
            break;
        case UPDATE_EXT:
            switch (trans) {
            case INTERNAL:
                applyExtUp(t);
                break;
            case EXTERNAL:
            case CONFLUENT:
                break;
            }
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
    void updateGuards(const vd::Time& t, TransitionType trans)
    {
        switch (state) {
        case INIT_SEND:
            switch (trans) {
            case INTERNAL:
                guards.all_ext_init = extUps.allInitialized(extVars());
                break;
            case EXTERNAL:
            case CONFLUENT:
                break;
            }
            break;
        case INIT_BAG_EATER:
            switch (trans) {
            case INTERNAL:
            case EXTERNAL:
                break;
            case CONFLUENT:
                guards.all_ext_init = extUps.allInitialized(extVars());
                break;
            }
            break;
        case INTEGRATION_TIME:
            switch (trans) {
            case INTERNAL:
                break;
            case EXTERNAL:
                guards.pert_in_evt = discontinuities.hasPerturb();
                guards.new_disc = discontinuities.hasNewDiscontinuity();
                guards.e_eq_0 = (t == lastWakeUp);
                guards.has_disc = discontinuities.hasDiscontinuity();
                ;
                //Note: synchro is an option
                break;
            case CONFLUENT:
                guards.pert_in_evt = discontinuities.hasPerturb();
                guards.new_disc = discontinuities.hasNewDiscontinuity();
                guards.has_disc = discontinuities.hasDiscontinuity();
                break;
            }
            break;
        case PERTURBATION:
            break;
        case NEW_DISC:
            switch (trans) {
            case INTERNAL:
            case EXTERNAL:
                break;
            case CONFLUENT:
                guards.pert_in_evt = discontinuities.hasPerturb();
                break;
            }
            break;
        case HAS_DISC:
            switch (trans) {
            case INTERNAL:
            case EXTERNAL:
                break;
            case CONFLUENT:
                guards.pert_in_evt = discontinuities.hasPerturb();
                guards.new_disc = discontinuities.hasNewDiscontinuity();
                break;
            }
            break;
            break;
        case UPDATE_EXT:
            switch (trans) {
            case INTERNAL:
                break;
            case EXTERNAL:
            case CONFLUENT:
                guards.pert_in_evt = discontinuities.hasPerturb();
                guards.new_disc = discontinuities.hasNewDiscontinuity();
                guards.has_disc = discontinuities.hasDiscontinuity();
                break;
            }
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
    void handleExtEvt(const vd::Time& t, const vd::ExternalEventList& ext)
    {

        vd::ExternalEventList::const_iterator itb = ext.begin();
        vd::ExternalEventList::const_iterator ite = ext.end();
        for (; itb != ite; itb++) {
            //get port name
            const std::string& portName = itb->getPortName();
            const value::Map& attrs = itb->attributes()->toMap();
            //update targeted variable
            if (portName == "perturb") {
                discontinuities.registerPerturb(t, attrs);
            } else if (isExtVar(portName)) {
                //get event informations and set default values
                const std::string& varName = attrs.getString("name");
                double varValue = attrs.getDouble("value");
                ExternVariables::iterator itf = extVars().find(portName);
                bool hasDiscontinuity = attrs.exist("discontinuities");
                //for dbg
                if (varName != portName) {
                    throw utils::InternalError("Unhandled update mode...");
                }

                if (itf == extVars().end()) {
                    throw utils::InternalError("TODO 22");
                }
                if (hasDiscontinuity) {
                    discontinuities.registerExtDisc(t, attrs);

                }

                extUps.registerExtUp(itf, varValue, t);

            } else {
                throw utils::InternalError(
                        (boost::format("[%1%] Unrecognised port '%2%' on mode PORT")
                                % getModelName() % portName).str());
            }
        }
    }

    /**
     * @brief Output function that fills the value and gradient
     * of state variables, and fills the discontinuities if
     * there are some.
     * @param time, time of the output
     * @param extEvtList, the list of external event
     * @param discontinuity, if true the set of current perturbations ids
     * are given
     */
    void outputVar(const vd::Time& time, vd::ExternalEventList& extEvtList,
            bool discontinuity) const
    {
        Variables::const_iterator itb = vars().begin();
        Variables::const_iterator ite = vars().end();
        for (; itb != ite; itb++) {
            const Variable& v = itb->second;
            if (getModel().existOutputPort(v.getName())) {
                extEvtList.emplace_back(v.getName());
                value::Map& m = extEvtList.back().addMap();
                m.addString("name", v.getName());
                m.addDouble("value", v.getVal());
                if (discontinuity) {
                    m.add("discontinuities",
                            discontinuities.buildDiscsToPropagate(time));
                }
            }
        }
    }

    /**
     * @brief Apply external updates, based on the container of external
     * updates, the values of external variables are updated
     * @param time, the current time
     */
    void applyExtUp(const vd::Time& /*time*/)
    {
        ExtUps::cont::iterator itb = extUps.mcont.begin();
        ExtUps::cont::iterator ite = extUps.mcont.end();
        for (; itb != ite; itb++) {
            ExtUp& vu = itb->second;
            ExternVariable& v = (itb->first)->second;
            v.set(vu.getVal());
        }
    }

    /**
     * @brief Restore variables backup
     * @param time, the current time
     */
    void restoreBackup(const vd::Time& /*time*/)
    {

        Variables::const_iterator itb = vars().begin();
        Variables::const_iterator ite = vars().end();
        for (; itb != ite; itb++) {
            VarImprover& vi = varImprovers[itb];
            Variable& v = const_cast<Variable&>(itb->second);
            v.setVal(vi.getBuVal());
            v.setGrad(vi.getBuGrad());
        }
    }

    /**
     * @brief Performs a backup of the variables
     * @param time, the current time
     */
    void backUpVars(const vd::Time& /*time*/)
    {
        Variables::const_iterator itb = vars().begin();
        Variables::const_iterator ite = vars().end();
        for (; itb != ite; itb++) {
            VarImprover& vi = varImprovers[itb];
            const Variable& v = itb->second;
            vi.setBackUp(v.getVal(), v.getGrad());
        }
    }

    /************** DEVS functions *****************/
    vd::Time init(vd::Time /*time*/) override
    {
        state = INIT_SEND;
        return timeAdvance();
    }

    void output(vd::Time time, vd::ExternalEventList& ext) const override
    {
        switch (state) {
        case INIT_SEND:
            outputVar(time, ext, false);
            break;
        case INIT_BAG_EATER:
            break;
        case INTEGRATION_TIME:
            if (nbIntegration == 0) {
                outputVar(time, ext, false);
            }
            break;
        case PERTURBATION:
            outputVar(time, ext, true);
            break;
        case NEW_DISC:
            outputVar(time, ext, true);
            break;
        case HAS_DISC:
            break;
        case UPDATE_EXT:
            outputVar(time, ext, false);
            break;
        default:
            throw vu::InternalError("TODO");
        }
    }

    vd::Time timeAdvance() const override
    {
        switch (state) {
        case INIT_SEND:
        case INIT_BAG_EATER:
            return 0;
            break;
        case INTEGRATION_TIME:
            return nextIntegration - lastWakeUp;
            break;
        case PERTURBATION:
        case NEW_DISC:
        case HAS_DISC:
            return 0;
            break;
        case UPDATE_EXT:
            return nextIntegration - lastWakeUp;
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

        switch (state) {
        case INIT_SEND:
            if (guards.all_ext_init) {
                state = INTEGRATION_TIME;
            } else {
                state = INIT_BAG_EATER;
            }
            break;
        case INIT_BAG_EATER:
            state = INIT_BAG_EATER;
            break;
        case INTEGRATION_TIME:
        case PERTURBATION:
        case NEW_DISC:
        case HAS_DISC:
        case UPDATE_EXT:
            state = INTEGRATION_TIME;
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

        switch (state) {
        case INIT_SEND:
            throw vu::InternalError("Error 3");
            break;
        case INIT_BAG_EATER:
            throw vu::InternalError("Error 4");
            break;
        case INTEGRATION_TIME:
            if (guards.pert_in_evt) {
                state = PERTURBATION;
            } else if (guards.new_disc) {
                state = NEW_DISC;
            } else if (guards.has_disc) {
                state = HAS_DISC;
            } else if (guards.e_eq_0 and options.synchronization) {
                state = INTEGRATION_TIME;
            } else {
                state = UPDATE_EXT;
            }
            break;
        case PERTURBATION:
            throw vu::InternalError("Error 6");
            break;
        case NEW_DISC:
            throw vu::InternalError("Error 7");
            break;
        case HAS_DISC:
            throw vu::InternalError("Error 8");
            break;
        case UPDATE_EXT:
            if (guards.pert_in_evt) {
                state = PERTURBATION;
            } else if (guards.new_disc) {
                state = NEW_DISC;
            } else if (guards.has_disc) {
                state = HAS_DISC;
            } else {
                state = UPDATE_EXT;
            }
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

        switch (state) {
        case INIT_SEND:
            throw vu::InternalError("UNKNOWN 1");
            break;
        case INIT_BAG_EATER:
            if (guards.all_ext_init) {
                state = INTEGRATION_TIME;
            } else {
                state = INIT_BAG_EATER;
            }
            break;
        case INTEGRATION_TIME:
            if (guards.pert_in_evt) {
                state = PERTURBATION;
            } else if (guards.new_disc) {
                state = NEW_DISC;
            } else {
                state = UPDATE_EXT;
            }
            break;
        case PERTURBATION:
            state = PERTURBATION;
            break;
        case NEW_DISC:
            if (guards.pert_in_evt) {
                state = PERTURBATION;
            } else {
                state = NEW_DISC;
            }
            break;
        case UPDATE_EXT:
            if (guards.pert_in_evt) {
                state = PERTURBATION;
            } else if (guards.new_disc) {
                state = NEW_DISC;
            } else if (guards.has_disc) {
                state = HAS_DISC;
            } else {
                state = INTEGRATION_TIME;
            }
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
                VarImprovers::cont::const_iterator itfBU = varImprovers.find(
                        itf);
                if (itfBU != varImprovers.mcont.end()) {
                    const VarImprover& vi = itfBU->second;
                    return vv::Double::create(vi.getBuVal());
                }
            }
            ExternVariables::const_iterator ief = extVars().find(port);
            if (ief != extVars().end()) {
                const ExternVariable& extVar = ief->second;
                return vv::Double::create(extVar.getVal());
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
    IntegrationMethod(DifferentialEquationImpl& eq, const vv::Map& /*params*/);
    /**
     * @brief Destructor of IntegrationMethod
     */
    virtual ~IntegrationMethod();

    virtual vd::Time integrationTime(const vd::Time& /*time*/) = 0;
    virtual void predictValue(const vd::Time& tin, const vd::Time& tout,
            VarImprovers& predictions) = 0;
};

class Euler : public IntegrationMethod
{
private:

    double timestep;

public:

    Euler(DifferentialEquationImpl& eq, const vv::Map& params);

    virtual ~Euler();

    vd::Time integrationTime(const vd::Time& /*time*/);

    void predictValue(const vd::Time& tin, const vd::Time& tout,
            VarImprovers& predictions);
};

class RK4 : public IntegrationMethod
{

private:

    double timestep;

public:

    RK4(DifferentialEquationImpl& eq, const vv::Map& params);

    virtual ~RK4();

    vd::Time integrationTime(const vd::Time& /*time*/);

    void predictValue(const vd::Time& tin, const vd::Time& tout,
            VarImprovers& predictions);
};

inline std::ostream& operator<<(std::ostream& o, const VarImprover& vi)
{
    o << "(buVal=" << vi.getBuVal() << "; buGrad=" << vi.getBuGrad() << ")";
    return o;
}

inline std::ostream& operator<<(std::ostream& o, const TSM_State& s)
{
    switch (s) {
    case INIT_SEND:
    {
        o << "INIT_SEND ";
        break;
    }
    case INIT_BAG_EATER:
    {
        o << "INIT_BAG_EATER ";
        break;
    }
    case INTEGRATION_TIME:
    {
        o << "INTEGRATION_TIME ";
        break;
    }
    case PERTURBATION:
    {
        o << "PERTURBATION ";
        break;
    }
    case NEW_DISC:
    {
        o << "NEW_DISC ";
        break;
    }
    case HAS_DISC:
    {
        o << "HAS_DISC ";
        break;
    }
    case UPDATE_EXT:
    {
        o << "UPDATE_EXT ";
        break;
    }
    default:
    {
        throw vu::InternalError("default operator<<");
    }
    }
    return o;
}

inline std::ostream& operator<<(std::ostream& o, const TSM_TransitionGuards& g)
{
    o << " all_ext_init=" << g.all_ext_init << ";"
            " pert_in_evt=" << g.pert_in_evt << ";"
            " e_eq_0=" << g.e_eq_0 << ";"
            " new_disc=" << g.new_disc << ";";
    return o;
}

inline std::ostream& operator<<(std::ostream& o, const VarImprovers& vis)
{
    VarImprovers::cont::const_iterator itb = vis.mcont.begin();
    VarImprovers::cont::const_iterator ite = vis.mcont.end();
    o << "[";
    for (; itb != ite; itb++) {
        o << itb->second << ",";
    }
    o << "]";
    return o;
}

}
}
}
} // namespace vle extension differential_equation timeSlicingMethod

#endif
