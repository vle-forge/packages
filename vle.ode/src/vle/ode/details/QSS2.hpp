/*
 * @file vle/ode/QSS2.hpp
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

#ifndef VLE_ODE_QSS2_HPP
#define VLE_ODE_QSS2_HPP 1

#include <map>
#include <set>
#include <iomanip>


#include <vle/vpz/AtomicModel.hpp>
#include <vle/utils/Exception.hpp>

#include "DifferentialEquationImpl.hpp"

namespace vle {
namespace ode {
namespace qss2 {

namespace vd = vle::devs;
namespace vv = vle::value;
namespace vu = vle::utils;
namespace vz = vle::vpz;
namespace de = vle::ode;

class QSS2;

/**
 * @brief DEVS state of the dynamic
 */
enum DEVS_State
{
    INIT,
    INTEGRATION_TIME
};

/**
 * @brief Types of DEVS transitions
 */
enum DEVS_TransitionType
{
    INTERNAL, EXTERNAL, CONFLUENT
};

/**
 * @brief Guards structure for transition between DEVS states
 */
struct DEVS_TransitionGuards
{
    DEVS_TransitionGuards()
    {
    }

    /**
     * @brief No guards are required
     */
};

/**
 * @brief Internal State
 */
struct DEVS_internal
{
    DEVS_internal(): minSigma(vd::infinity), LWUt(vd::negativeInfinity),
            quantizedVar(), externalVar()
    {
    }
    double minSigma;//next time to internal transition
    double LWUt;    //last wake up time
    de::Variables::iterator quantizedVar; //variable that is quantized
    de::Variables::iterator externalVar; //variable getting an external update
                                         //(reset or external update)

};

/**
 * @brief Options structure for
 * QSS2 methods
 */
struct DEVS_Options
{
    DEVS_Options() : quanta()
    {
    }
    /**
     * @brief quantum for quantization for each variable
     */
    std::map<std::string, double> quanta;
};

/**
 * @brief Improver of a state variable
 */
class VarImprover
{
public:

    VarImprover() :
            name(), DeltaQ(0), x0(0), x1(0), x2(0), q0(0),
            q1(0), y0(0), y1(0), y2(0), sig(0), f0(0), f1(1), z(0), mz(0)
    {
    }

    std::string name;
    double DeltaQ;
    double x0;     //corresponds to x;  paper QSS2, eq 18-19
    double x1;     //corresponds to u
    double x2;     //corresponds to m_u
    double q0;     //corresponds to q
    double q1;     //corresponds to m_q
    double y0;     //corresponds to x+u*sig+m_u*sig^2/2 (x output)
    double y1;     //corresponds to u+m_u*sig (grad(x) output)
    double y2;
    double sig;

    /**
     * @brief Container of static functions.
     * Gradient and gradient derivative computation for all states variables
     * paper QSS2, page 10 : (z_{1}, m_{z_1}, c_{z_1}) ...
     *                       (z_{q}, m_{z_q}, c_{z_q})
     *
     * note: derivatives are shared between all static functions since
     * compute of gradients is common
     */

    /**
     * @brief simulates output (lambda) of the static function
     * (paper QSS2, page 10, eq 20)
     * @return gradient and gradient derivative
     */
    double f0; //gradient for numerical computation: f_i(z')
    double f1; //gradient derivative: (f_i(z'+m'_z*e) - f_i(z'))/dt

    /**
     * @brief Partial derivatives for
     * one state variable * one state (or external) variable
     * Note: second derivatives are not expressed with a linear function
     * but are estimated numerically in order to match the most general case
     * of non linear gradients (as in powerdevs and different from QSS2 paper).
     * Coefficient c_j are then useless in eq 20 of paper.
     */
    double z; //z'_j
    double mz; //m'_{z_j}
};

inline std::ostream& operator<<(std::ostream& o, const VarImprover& vi)
{
    o << "(name=" << vi.name << ";"
            " DeltaQ=" << vi.DeltaQ << ";"
            " x=(" << vi.x0 << "," << vi.x1 << "," << vi.x2 << "); "
            " q=(" << vi.q0 << "," << vi.q1 << "); "
            " y=(" << vi.y0 << "," << vi.y1 << "," << vi.y2 << "); "
            " sig=" << vi.sig << ";"
            ")";
    return o;
}

class VarImprovers
{
public:

    typedef std::vector<std::pair<de::Variables::iterator, VarImprover> > cont;

    cont mcont;

    VarImprovers() :
            mcont()
    {
    }

    cont::iterator find(de::Variables::iterator it)
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

    cont::const_iterator find(de::Variables::const_iterator it) const
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

    const VarImprover& get(const std::string& name) const
    {
        cont::const_iterator itb = mcont.begin();
        cont::const_iterator ite = mcont.end();
        for (; itb != ite; itb++) {
            const VarImprover& vi = itb->second;
            if (vi.name == name) {
                return vi;
            }
        }
        throw vu::ModellingError(" Var not found ");
    }

    VarImprover& operator[](de::Variables::iterator it)
    {
        cont::iterator itf = find(it);
        if (itf == mcont.end()) {
            throw vu::ModellingError(" Var not in improvers ");
        }
        return itf->second;
    }

    VarImprover& add(de::Variables::iterator it)
    {
        cont::iterator itf = find(it);
        if (itf == mcont.end()) {
            mcont.push_back(std::make_pair(it, VarImprover()));
            ;
            itf = find(it);
        } else {
            throw vu::ModellingError(" Var only in improvers ");
        }
        return itf->second;
    }
};

inline std::ostream& operator<<(std::ostream& o, const VarImprovers& vis)
{
    VarImprovers::cont::const_iterator itb = vis.mcont.begin();
    VarImprovers::cont::const_iterator ite = vis.mcont.end();
    o << "[";
    for (; itb != ite; itb++) {
        o << itb->second << "\n ~~";
    }
    o << "]";
    return o;
}

inline std::ostream& operator<<(std::ostream& o, const DEVS_State& s)
{
    switch (s) {
    case INIT:
        o << "INIT ";
        break;
    case INTEGRATION_TIME:
        o << "INTEGRATION_TIME ";
        break;
    default:
        throw vu::InternalError("default operator<<");
    }
    return o;
}

inline std::ostream& operator<<(std::ostream& o, const DEVS_TransitionType& t)
{
    switch (t) {
    case INTERNAL:
        o << "INTERNAL ";
        break;
    case EXTERNAL:
        o << "EXTERNAL ";
        break;
    case CONFLUENT:
        o << "CONFLUENT ";
        break;
    default:
        throw vu::InternalError("default operator<<");
    }
    return o;
}

inline std::ostream& operator<<(std::ostream& o,
        const DEVS_TransitionGuards& /*g*/)
{
    o << " empty guards;";
    return o;
}

/**
 * @brief Differential equation implementation
 * of QSS2
 */
class QSS2 : public de::DifferentialEquationImpl
{
public:
    /**
     * @brief Partial derivatives for
     * one state variable * one state (or external) variable
     * Note: second derivatives are not expressed with a linear function
     * but are estimated numerically in order to match the most general case
     * of non linear gradients (as in powerdevs and different from QSS2 paper).
     * Coefficient c_j are then useless in eq 20 of paper.
     */
    struct derivative
    {
        derivative(de::Variables::iterator it) :
                z(0), mz(0), name()
        {
            Variable& v = *(it->second);
            z = v.getVal();
            name = it->first;
        }

        double z; //z'_j
        double mz; //m'_{z_j}
        std::string name;
    };


private:
    DEVS_State            devs_state;
    DEVS_Options          devs_options;
    DEVS_TransitionGuards devs_guards;
    DEVS_internal         devs_internal;
    VarImprovers varImprovers;

public:

    /**
     * @brief QSS2 constuctor
     * @param eq, reference on the corresponding differential equation
     * @param params, the map for initialization of QSS2
     */
    QSS2(de::DifferentialEquation& eq, const vd::InitEventList& events) :
            de::DifferentialEquationImpl(eq, events), devs_state(INIT),
            devs_options(), devs_guards(), devs_internal(), varImprovers()
    {
        vle::devs::InitEventList::const_iterator itb = events.begin();
        vle::devs::InitEventList::const_iterator ite = events.end();
        std::string prefix;
        std::string var_name;
        //2nd init (prior)
        for (; itb != ite; itb++) {
            const std::string& event_name = itb->first;
            if (!prefix.assign("quantum_").empty() and
                    !event_name.compare(0, prefix.size(), prefix)) {
                var_name.assign(event_name.substr(prefix.size(),
                        event_name.size()));
                double qt = itb->second->toDouble().value();
                devs_options.quanta.insert(std::make_pair(var_name, qt));
            }
        }
    }

    /**
     * @brief QSS2 destructor
     */
    virtual ~QSS2()
    {
    }

private:

    void initializeDerivatives(double t)
    {
        meq.compute(t);
        de::Variables::iterator itb = vars().begin();
        de::Variables::iterator ite = vars().end();
        for (; itb != ite; itb++) {
            VarImprover& vi = varImprovers[itb];
            vi.z = itb->second->getVal();
            vi.mz = itb->second->getGrad();
            //store derivatives
            vi.y1 = vi.mz;
        }
    }

    /**
     * @brief Compute numerical gradient and gradient derivative
     * @param t, current time
     * @param e, time elapsed
     */
    void numericalGradientDerivative(double t, double e)
    {
        double dt = 1e-8;
        if (e > 1e-15) {
            dt = e / 100;
        } //powerdevs trick

        //fill state and external variables
        //for computation of f0
        {
            de::Variables::iterator itb = vars().begin();
            de::Variables::iterator ite = vars().end();
            for (; itb != ite; itb++) {
                VarImprover& vi = varImprovers[itb];
                itb->second->setVal(vi.z);
                itb->second->setGrad(vi.mz);
            }
        }
        //computation of f0 = f_i(z') and fill f0
        {
            meq.compute(t);
            de::Variables::iterator itb = vars().begin();
            de::Variables::iterator ite = vars().end();
            for (; itb != ite; itb++) {
                VarImprover& vi = varImprovers[itb];
                vi.f0 = itb->second->getGrad();
            }
        }

        //fill state and external variables
        //for computation of f1
        {
            de::Variables::iterator itb = vars().begin();
            de::Variables::iterator ite = vars().end();
            for (; itb != ite; itb++) {
                VarImprover& vi = varImprovers[itb];
                itb->second->setVal(vi.z + vi.mz * dt);
                itb->second->setGrad(vi.mz);
            }
        }

        //computation of f1 = (f_i(z'+m'_z*e) - f_i(z'))/dt
        {
            meq.compute(t);
            de::Variables::iterator itb = vars().begin();
            de::Variables::iterator ite = vars().end();
            for (; itb != ite; itb++) {
                VarImprover& vi = varImprovers[itb];
                vi.f1 = (itb->second->getGrad() - vi.f0) / dt;
            }
        }
    }


    void finishInitializations(vd::Time /*time*/)
    {
        //initialisation of variable indexes and real values
        std::string port;
        de::Variables::iterator itb = vars().begin();
        de::Variables::iterator ite = vars().end();
        for (; itb != ite; itb++) {
            const std::string& varName = itb->first;
            const de::Variable& v = *(itb->second);
            VarImprover& vi = varImprovers.add(itb);
            vi.x0 = v.getVal();
            vi.q0 = v.getVal();
            vi.y0 = v.getVal();
            port.assign("quantum_");
            port += varName;
            if (devs_options.quanta.find(varName) == devs_options.quanta.end()){
                vi.DeltaQ = 0;
            } else {
                vi.DeltaQ = devs_options.quanta[varName];
            }
            vi.name = varName;
            //initalize derivative
            vi.z = vi.x0;
        }
        devs_internal.quantizedVar = ite;
        devs_internal.externalVar = ite;
    }
    /**
     * @brief Process method used for DEVS state entrance
     * @param t, the current time
     * @param trans, the type of the transition
     */
    void processIn(const vd::Time& t, DEVS_TransitionType trans)
    {
        switch (devs_state) {
        case INIT:
            devs_internal.LWUt = t;
            initializeDerivatives(t);
            break;
        case INTEGRATION_TIME:
            switch (trans) {
            case INTERNAL:
                min_sigma(t);
                devs_internal.LWUt = t;
                break;
            case EXTERNAL:
            case CONFLUENT:
                devs_internal.quantizedVar = vars().end();
                staticFunctionsDeltaExt(t);
                quantizerDeltaExt(t);
                min_sigma(t);
                devs_internal.externalVar = vars().end();
                devs_internal.LWUt = t;

                break;
            }
            break;
        default:
        {
            throw vu::InternalError("default processIn");
        }
        }
    }

    /**
     * @brief Process method used for DEVS state exit
     * @param t, the current time
     * @param trans, the type of the transition
     */
    void processOut(const vd::Time& t, DEVS_TransitionType trans)
    {
        switch (devs_state) {
        case INIT:
            switch (trans) {
            case INTERNAL:
                numericalGradientDerivative(t, 0);
                quantizerDeltaExt(t);
                devs_internal.LWUt = t;
                break;
            case EXTERNAL:
            case CONFLUENT:
                devs_internal.LWUt = t;
                break;
            }
            break;
        case INTEGRATION_TIME:
            switch (trans) {
            case INTERNAL:
                quantizerDeltaInt();
                staticFunctionsDeltaExt(t);
                quantizerDeltaExt(t);
                break;
            case EXTERNAL:
                break;
            case CONFLUENT:
                quantizerDeltaInt();
                staticFunctionsDeltaExt(t);
                quantizerDeltaExt(t);
                break;
            }
            break;
        default:
        {
            throw vu::InternalError("default");
        }
        }
    }

    /**
     * @brief Update guards for internal transition
     * @param t, the current time
     * @param trans, the type of the transition
     */
    void updateGuards(const vd::Time& /*t*/, DEVS_TransitionType /*trans*/)
    {
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
            VarImprovers::cont::iterator itfvi = varImprovers.find(itf);
            VarImprover& vi = itfvi->second;
            if (itb->attributes()->isMap()) {
                const vv::Map& attrs= itb->attributes()->toMap();
                vi.y0 = attrs.getDouble("value");
                if (attrs.exist("gradient")) {
                    vi.y1 = attrs.getDouble("gradient");
                } else {
                    vi.y1 = 0;
                }
                vi.y2 = 0;
            } else {
                vi.y0 = itb->attributes()->toDouble().value();
                vi.y1 = 0;
                vi.y2 = 0;
            }
            vi.x0 = vi.y0;
            vi.x1 = vi.y1;
            vi.x2 = 0;
            vi.q0 = vi.y0;
            vi.q1 = vi.y1;
            vi.z = vi.y0;
            vi.mz = vi.y1;
            itf->second->setVal(vi.y0);
            itf->second->setGrad(vi.y1);
            devs_internal.quantizedVar = vars().end();
            devs_internal.externalVar = itf;
        }
    }

    /**
     * @brief Output function that fills the value and gradient
     * of state variables, and fills the discontinuities if
     * there are some.
     * @param time, time of the output
     * @param extEvtList, the list of external event
     */
    void outputVar(const vd::Time& /*time*/,
            vd::ExternalEventList& extEvtList) const
    {

        if (devs_internal.quantizedVar != vars().end()) {
            //const Variable& v = *(quantizedVariable->second);
            if (getModel().existOutputPort(devs_internal.quantizedVar->first)) {
                VarImprovers::cont::const_iterator itfvi = varImprovers.find(
                        devs_internal.quantizedVar);
                const VarImprover& vi = itfvi->second;

                extEvtList.emplace_back(devs_internal.quantizedVar->first);
                vv::Map& m = extEvtList.back().addMap();
                m.addString("name", devs_internal.quantizedVar->first);
                m.addDouble("value", vi.y0);
                m.addDouble("gradient", vi.y1);

            }
        }
    }

    /**
     * @brief Output function that fills the value and gradient
     * of state variables
     * @param time, time of the output
     * @param extEvtList, the list of external event
     * @param discontinuity, if true the set of current perturbations ids
     * are given
     */
    void outputAllVar(const vd::Time& /*time*/,
            vd::ExternalEventList& extEvtList) const
    {
        Variables::const_iterator itb = vars().begin();
        Variables::const_iterator ite = vars().end();
        for (; itb != ite; itb++) {
            if (getModel().existOutputPort(itb->first)) {
                const VarImprover& vi = varImprovers.get(itb->first);
                extEvtList.emplace_back(itb->first);
                vv::Map& m = extEvtList.back().addMap();
                m.addDouble("value", vi.y0);
                m.addDouble("gradient", vi.y1);
            }
        }
    }

    /************** DEVS functions *****************/
    vd::Time init(vd::Time time)
    {
        finishInitializations(time);
        devs_state = INIT;
        processIn(time,INTERNAL);
        return timeAdvance();
    }

    void output(vd::Time time, vd::ExternalEventList& ext) const override
    {
        switch (devs_state) {
        case INIT:
            outputAllVar(time, ext);
            break;
        case INTEGRATION_TIME:
            outputVar(time, ext);
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
        case INTEGRATION_TIME:
            return devs_internal.minSigma;
            break;
        default:
            throw vu::InternalError("TODO 2");
        }
    }

    void internalTransition(vd::Time t) override
    {
        processOut(t, INTERNAL);
        updateGuards(t, INTERNAL);

        switch (devs_state) {
        case INIT:
            devs_state = INTEGRATION_TIME;
            break;
        case INTEGRATION_TIME:
            devs_state = INTEGRATION_TIME;
            break;
        default:
            throw vu::InternalError("default intTr");
        }
        processIn(t, INTERNAL);

    }

    void externalTransition(const vd::ExternalEventList& event,
            vd::Time t) override
    {
        processOut(t, EXTERNAL);
        handleExtEvt(t, event);
        updateGuards(t, EXTERNAL);

        devs_state = INTEGRATION_TIME;

        processIn(t, EXTERNAL);
    }

    void confluentTransitions(vd::Time t,
            const vd::ExternalEventList& ext) override
    {
        processOut(t, CONFLUENT);
        handleExtEvt(t, ext);
        updateGuards(t, CONFLUENT);

        devs_state = INTEGRATION_TIME;

        processIn(t, CONFLUENT);
    }

    /***
     * @brief Implementation of observation DEVS function
     */
    std::unique_ptr<value::Value> observation(
            const vd::ObservationEvent& event) const override
    {
        const std::string& port = event.getPortName();
        {
            de::Variables::iterator itf = meq.meqImpl->vars().find(port);
            if (itf != meq.meqImpl->vars().end()) {
                VarImprovers::cont::const_iterator itfvi = varImprovers.find(
                        itf);
                if (itfvi != varImprovers.mcont.end()) {
                    double e = event.getTime() - devs_internal.LWUt;
                    const VarImprover& vi = itfvi->second;
                    return vv::Double::create(
                            vi.x0 + vi.x1 * e + (vi.x2 / 2) * e * e);
                }
            }
        }
        return 0;
    }

    /**
     * @brief Computes the next state variable quantization
     * and predict output for the quantized state variable
     * @param t, current time
     */
    void min_sigma(double /*t*/)
    {
        //compute minSigma, quantizedVariable
        de::Variables::iterator itb = meq.meqImpl->vars().begin();
        de::Variables::iterator ite = meq.meqImpl->vars().end();

        devs_internal.minSigma = vd::infinity;
        bool found = false;
        for (; itb != ite; itb++) {
            VarImprover& vi = varImprovers[itb];
            if ((vi.DeltaQ != 0) and (vi.sig < devs_internal.minSigma)) {
                devs_internal.minSigma = vi.sig;
                devs_internal.quantizedVar = itb;
                found = true;
            }
        }
        if (!found) {
            devs_internal.quantizedVar = meq.meqImpl->vars().begin();
        }

        //prepare output (lambda)
        VarImprover& vq = varImprovers[devs_internal.quantizedVar];
        vq.y0 = vq.x0 + vq.x1 * devs_internal.minSigma + vq.x2
                * devs_internal.minSigma * devs_internal.minSigma / 2;
        vq.y1 = vq.x1 + vq.x2 * devs_internal.minSigma;
    }

    /**
     * @brief Simulates delta_ext of all quantizer state variables
     * quantizers, which is always done after static function updates.
     * (QSS2 paper, page 9)
     * @param t, current time
     */
    void quantizerDeltaExt(double t)
    {
        double e = t - devs_internal.LWUt;

        de::Variables::iterator itb = vars().begin();
        de::Variables::iterator ite = vars().end();
        for (; itb != ite; itb++) {
            VarImprover& vi = varImprovers[itb];
            //std::pair<double, double> out = output(itb);
            if (itb != devs_internal.quantizedVar) {
                //note: update of quantizedVariable already done
                //in quantizerStateVarDeltaInt. To match the
                //algorithm, this update should actually be performed
                //with e=0 for the quantized variable, which
                //leads to the same results.

                vi.x0 = vi.x0 + vi.x1 * e + vi.x2 * e * e / 2;
                vi.q0 = vi.q0 + vi.q1 * e;
            }
            vi.x1 = vi.f0;
            vi.x2 = vi.f1;
            double a = -vi.x2 / 2;
            double b = vi.q1 - vi.x1;

            double c = vi.q0 - vi.x0 - vi.DeltaQ;
            double mpr1 = min_pos_root(a, b, c);

            c += 2 * vi.DeltaQ;
            double mpr2 = min_pos_root(a, b, c);

            if (mpr1 >= 0) {
                if (mpr2 >= 0) {
                    vi.sig = std::min(mpr1, mpr2);
                } else {
                    vi.sig = mpr1;
                }
            } else {
                if (mpr2 >= 0) {
                    vi.sig = mpr2;
                } else {
                    vi.sig = vd::infinity;
                }
            }
        }
    }

    /**
     * @brief Simulates delta_int of quantized state variable
     * (QSS2 paper, page 9)
     */
    void quantizerDeltaInt()
    {
        VarImprover& vi = varImprovers[devs_internal.quantizedVar];
        double sigma = vi.sig;
        double x0 = vi.x0;
        double x1 = vi.x1;
        double x2 = vi.x2;

        vi.x1 = x1 + x2 * sigma;
        vi.x2 = x2;
        vi.x0 = x0 + x1 * sigma + (x2 / 2) * sigma * sigma;
        vi.q0 = vi.x0;
        vi.q1 = vi.x1;
        if (x2 != 0) {
            vi.sig = sqrt(fabs(2.0 * vi.DeltaQ / x2));
        } else {
            vi.sig = vd::infinity;
        }
    }

    /**
     * @brief Simulates delta_ext of all static functions
     * (paper QSS2, page 10)
     * @param t, current time
     *
     */
    void staticFunctionsDeltaExt(double t)
    {
       double e = t - devs_internal.LWUt;

       double y0 = 0;
       double y1 = 0;

       VarImprovers::cont::iterator itf =
               varImprovers.find(devs_internal.quantizedVar);
       if (itf != varImprovers.mcont.end()) {
           VarImprover& vi = itf->second;
           y0 = vi.y0;
           y1 = vi.y1;
       }

       //compute z'_j and m'_{z_j}
       de::Variables::iterator itb = vars().begin();
       de::Variables::iterator ite = vars().end();
       for (; itb != ite; itb++) {
           VarImprover& vi = varImprovers[itb];
           if (devs_internal.quantizedVar == itb) {
               vi.z = y0;
               vi.mz = y1;
           } else if (devs_internal.externalVar != itb){
               vi.z = vi.z + vi.mz * e;
           }
       }

       //compute gradients
       numericalGradientDerivative(t, e);
    }

    /**
     *@brief Computes the minimal positive root of a second
     * order polynom of form a*x^2+b*x+c =0
     */
    double min_pos_root(double a, double b, double c)
    {

        double mpr = -1;
        if (a == 0) {
            if (b == 0) {
                mpr = -1;
            } else {
                mpr = -c / b;
            };
            if (mpr < 0)
                mpr = -1;
        } else {
            double disc;
            disc = b * b - 4 * a * c;
            if (disc < 0) {
                //no real roots
                mpr = -1;
            } else {
                double sd, r1;
                sd = sqrt(disc);
                r1 = (-b + sd) / 2 / a;
                if (r1 > 0) {
                    mpr = r1;
                } else {
                    mpr = -1;
                };
                r1 = (-b - sd) / 2 / a;
                if ((r1 > 0) && (r1 < mpr || mpr == -1)) {
                    mpr = r1;
                }
            };
        };
        return mpr;
    }

};

}
}
} // namespace vle ode qss2

#endif
