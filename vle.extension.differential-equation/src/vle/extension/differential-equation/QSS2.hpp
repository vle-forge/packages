/*
 * @file vle/extension/differential_equation/QSS2.hpp
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

#ifndef VLE_EXTENSION_DIFFERENTIALEQUATION_QSS2_HPP
#define VLE_EXTENSION_DIFFERENTIALEQUATION_QSS2_HPP 1

#include <map>
#include <set>

#include <vle/vpz/AtomicModel.hpp>
#include <vle/utils/Exception.hpp>

#include <vle/extension/differential-equation/DifferentialEquationImpl.hpp>
#include <vle/extension/differential-equation/PerturbationHandler.hpp>

namespace vle {
namespace extension {
namespace differential_equation {
namespace qss2 {

namespace vd = vle::devs;
namespace vv = vle::value;
namespace vu = vle::utils;
namespace vz = vle::vpz;
namespace de = vle::extension::differential_equation;

/**
 * @brief DEVS state of the dynamic
 */
enum QSS2_State
{
    INIT_SEND,
    INIT_BAG_EATER,
    INIT_SEND_1,
    INIT_BAG_EATER_1,
    INIT_STATIC_FUNCTIONS,
    INTEGRATION_TIME,
    PERTURBATION,
    NEW_DISC,
    HAS_DISC
};

/**
 * @brief Types of DEVS transitions
 */
enum TransitionType
{
    INTERNAL, EXTERNAL, CONFLUENT
};

/**
 * @brief Guards structure for transition between DEVS states
 */
struct QSS2_TransitionGuards
{
    QSS2_TransitionGuards() :
            all_ext_init(false), pert_in_evt(false), new_disc(false), has_disc(
                    false)
    {
    }

    /**
     * @brief True if all external variables are
     * initialized. This guard is computed and used
     * only on the two first states
     * INIT_SEND and INIT_BAG_EATER.
     */
    bool all_ext_init;

    bool pert_in_evt;

    bool new_disc;

    bool has_disc;
};

/**
 * @brief Options structure for
 * QSS2 methods
 */
struct QSS2_Options
{
    QSS2_Options() :
            expectGradients(false)
    {
    }

    /**
     * @brief if true, the dynamic expects that
     * external variables send gradients at inititialisation
     * and on external event update
     */
    bool expectGradients;
};

/**
 * @brief Container of static functions.
 * Gradient and gradient derivative computation for all states variables
 * paper QSS2, page 10 : (z_{1}, m_{z_1}, c_{z_1}) ... (z_{q}, m_{z_q}, c_{z_q})
 *
 * note: derivatives are shared between all static functions since
 * compute of gradients is common
 */
class StaticFunctions
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
            Variable& v = it->second;
            z = v.getVal();
            name = v.getName();
        }

        derivative(de::ExternVariables::iterator it) :
                z(0), mz(0), name()
        {
            ExternVariable& ev = it->second;
            z = ev.getVal();
            name = ev.getName();
        }

        double z; //z'_j
        double mz; //m'_{z_j}
        std::string name;
    };

    typedef std::vector<std::pair<de::Variables::iterator, derivative> > contState;

    typedef std::vector<std::pair<de::ExternVariables::iterator, derivative> > contExt;

    typedef std::vector<
            std::pair<de::Variables::iterator, std::pair<double, double> > > contF;

    /**
     * @brief StaticFunctions constructor
     * @param eq, reference to the corresponding equation
     */
    StaticFunctions(DifferentialEquation& eq) :
            meq(eq), mcontState(), mcontExt(), mcontF()
    {
        de::Variables::iterator itb = meq.vars().begin();
        de::Variables::iterator ite = meq.vars().end();
        for (; itb != ite; itb++) {
            mcontState.push_back(std::make_pair(itb, derivative(itb)));
            mcontF.push_back(std::make_pair(itb, std::make_pair(0.0, 0.0)));
        }
        de::ExternVariables::iterator iteb = meq.extVars().begin();
        de::ExternVariables::iterator itee = meq.extVars().end();
        for (; iteb != itee; iteb++) {
            mcontExt.push_back(std::make_pair(iteb, derivative(iteb)));
        }
    }

    void initializeDerivatives(ExtUps& extups, double t)
    {

        contExt::iterator iteb = mcontExt.begin();
        contExt::iterator itee = mcontExt.end();
        for (; iteb != itee; iteb++) {
            de::ExternVariables::iterator itev = iteb->first;
            ExternVariable& var = itev->second;
            derivative& dj = iteb->second;
            ExtUps::cont::iterator itexu = extups.find(itev);
            ExtUp& eu = itexu->second;
            dj.z = eu.getVal();
            dj.mz = eu.getGrad();
            var.set(dj.z);
        }

        meq.compute(t);
        contState::iterator itb = mcontState.begin();
        contState::iterator ite = mcontState.end();
        for (; itb != ite; itb++) {
            de::Variables::iterator itv = itb->first;
            Variable& var = itv->second;
            derivative& dj = itb->second;
            dj.mz = var.getGrad();
        }
    }

    /**
     * @brief Finds the iterator of derivatives <f0,f1>
     * for state variable iv
     * @param iv, state variable iterator
     * @return iterator on the derivatives <f0,f1>
     */
    contF::iterator findF(de::Variables::iterator iv)
    {
        contF::iterator itb = mcontF.begin();
        contF::iterator ite = mcontF.end();
        for (; itb != ite; itb++) {
            if (itb->first == iv) {
                return itb;
            }
        }
        return ite;
    }

    /**
     * @brief Finds the derivative iterator
     * of a state variable
     * @param iv, state variable iterator
     * @return iterator on the derivative of iv
     */
    contState::iterator findState(de::Variables::iterator iv)
    {
        contState::iterator itb = mcontState.begin();
        contState::iterator ite = mcontState.end();
        for (; itb != ite; itb++) {
            if (itb->first == iv) {
                return itb;
            }
        }
        return ite;
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
            contState::iterator itb = mcontState.begin();
            contState::iterator ite = mcontState.end();
            for (; itb != ite; itb++) {
                de::Variables::iterator itv = itb->first;
                Variable& var = itv->second;
                derivative& dj = itb->second;
                var.setVal(dj.z);
                var.setGrad(dj.mz);
            }
            contExt::iterator iteb = mcontExt.begin();
            contExt::iterator itee = mcontExt.end();
            for (; iteb != itee; iteb++) {
                de::ExternVariables::iterator itev = iteb->first;
                ExternVariable& var = itev->second;
                derivative& dj = iteb->second;
                var.set(dj.z);
            }
        }
        //computation of f0 = f_i(z') and fill f0
        {
            meq.compute(t);
            de::Variables::iterator itb = meq.vars().begin();
            de::Variables::iterator ite = meq.vars().end();
            for (; itb != ite; itb++) {
                contF::iterator itf = findF(itb);
                std::pair<double, double>& f0f1 = itf->second;
                f0f1.first = itb->second.getGrad();
            }
        }

        //fill state and external variables
        //for computation of f1
        {
            contState::iterator itb = mcontState.begin();
            contState::iterator ite = mcontState.end();
            for (; itb != ite; itb++) {
                de::Variables::iterator itv = itb->first;
                Variable& var = itv->second;
                derivative& dj = itb->second;

                var.setVal(dj.z + dj.mz * dt);
                var.setGrad(dj.mz);
            }
            contExt::iterator iteb = mcontExt.begin();
            contExt::iterator itee = mcontExt.end();
            for (; iteb != itee; iteb++) {
                de::ExternVariables::iterator itev = iteb->first;
                ExternVariable& var = itev->second;
                derivative& dj = iteb->second;
                var.set(dj.z + dj.mz * dt);
            }
        }

        //computation of f1 = (f_i(z'+m'_z*e) - f_i(z'))/dt
        {
            meq.compute(t);
            de::Variables::iterator itb = meq.vars().begin();
            de::Variables::iterator ite = meq.vars().end();
            for (; itb != ite; itb++) {
                contF::iterator itf = findF(itb);
                std::pair<double, double>& f0f1 = itf->second;
                f0f1.second = (itb->second.getGrad() - f0f1.first) / dt;
            }
        }
    }

    /**
     * @brief simulates delta_ext of the static function
     * (paper QSS2, page 10, eq 20) in the case of state variable
     * quantization
     * @param itVar, variable concerned by a quantization
     * @param v, new quantized value of itVar
     * @param mv, new quantized derivative value of itVar
     * @param t, current time
     * @param e, time elapsed
     */
    void deltaExtFromStateVar(de::Variables::iterator itVar, double v,
            double mv, double t, double e)
    {
        //compute z'_j and m'_{z_j}
        contState::iterator itb = mcontState.begin();
        contState::iterator ite = mcontState.end();
        for (; itb != ite; itb++) {
            de::Variables::iterator itv = itb->first;
            derivative& dj = itb->second;
            if (itVar == itv) {
                dj.z = v;
                dj.mz = mv;
            } else {
                dj.z = dj.z + dj.mz * e;
            }
        }
        contExt::iterator iteb = mcontExt.begin();
        contExt::iterator itee = mcontExt.end();
        for (; iteb != itee; iteb++) {
            derivative& dj = iteb->second;
            dj.z = dj.z + dj.mz * e;
        }
        //compute gradients
        numericalGradientDerivative(t, e);
    }

    /**
     * @brief simulates delta_ext of the static function
     * (paper QSS2, page 10, eq 20) in the case of external
     * variables quantization
     * @param extups, external variables update structure
     * @param t, current time
     * @param e, time elapsed
     */
    void deltaExtFromExtVar(ExtUps& extups, double t, double e)
    {
        //compute z'_j and m'_{z_j}
        contState::iterator itb = mcontState.begin();
        contState::iterator ite = mcontState.end();
        for (; itb != ite; itb++) {
            derivative& dj = itb->second;
            dj.z = dj.z + dj.mz * e;
        }
        contExt::iterator iteb = mcontExt.begin();
        contExt::iterator itee = mcontExt.end();
        for (; iteb != itee; iteb++) {
            de::ExternVariables::iterator itev = iteb->first;
            derivative& dj = iteb->second;
            ExtUps::cont::iterator itexu = extups.find(itev);
            ExtUp& eu = itexu->second;
            if (eu.getTime() == t) {
                dj.z = eu.getVal();
                dj.mz = eu.getGrad();
            } else {
                dj.z = dj.z + dj.mz * e;
            }

        }
        //compute gradients
        numericalGradientDerivative(t, e);
    }

    /**
     * @brief simulates output (lambda) of the static function
     * (paper QSS2, page 10, eq 20)
     * @param iv, iterator of the state variable
     * @return gradient and gradient derivative
     */
    std::pair<double, double> output(de::Variables::iterator iv)
    {
        contF::iterator itf = findF(iv);
        std::pair<double, double>& f0f1 = itf->second;
        return std::make_pair(f0f1.first, f0f1.second);
    }

    DifferentialEquation& meq;
    contState mcontState;
    contExt mcontExt;
    //set of pairs {f_i(z') ; (f_i(z'+m'_z*e) - f_i(z'))/dt}
    //ie.  gradients and gradient derivative for all i
    contF mcontF;
};

inline std::ostream& operator<<(std::ostream& o, const StaticFunctions& sfs)
{
    StaticFunctions::contF::const_iterator itb = sfs.mcontF.begin();
    StaticFunctions::contF::const_iterator ite = sfs.mcontF.end();
    o << " contF:";
    for (; itb != ite; itb++) {
        const std::pair<double, double>& f0f1 = itb->second;
        const Variable& v = itb->first->second;
        o << " (" << v.getName() << " f0=" << f0f1.first << " f1=" << f0f1.first
                << "),";
    }
    return o;
}

/**
 * @brief Improver of a state variable
 */
class VarImprover
{
public:

    VarImprover() :
            name(), DeltaQ(0), x0(0), x1(0), x2(0), q0(0), q1(0), y0(0), y1(0), y2(
                    0), sig(0)
    {
    }

    std::string name;
    double DeltaQ;
    double x0;    //corresponds to x;  paper QSS2, eq 18-19
    double x1;    //corresponds to u
    double x2;    //corresponds to m_u
    double q0;    //corresponds to q
    double q1;    //corresponds to m_q
    double y0;    //corresponds to x+u*sig+m_u*sig^2/2 (x output)
    double y1;    //corresponds to u+m_u*sig (grad(x) output)
    double y2;
    double sig;

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

inline std::ostream& operator<<(std::ostream& o, const QSS2_State& s)
{
    switch (s) {
    case INIT_SEND:
        o << "INIT_SEND ";
        break;
    case INIT_BAG_EATER:
        o << "INIT_BAG_EATER ";
        break;
    case INIT_SEND_1:
        o << "INIT_SEND_1 ";
        break;
    case INIT_BAG_EATER_1:
        o << "INIT_BAG_EATER_1 ";
        break;
    case INIT_STATIC_FUNCTIONS:
        o << "INIT_STATIC_FUNCTIONS ";
        break;
    case INTEGRATION_TIME:
        o << "INTEGRATION_TIME ";
        break;
    case PERTURBATION:
        o << "PERTURBATION ";
        break;
    case NEW_DISC:
        o << "NEW_DISC ";
        break;
    case HAS_DISC:
        o << "HAS_DISC ";
        break;
    default:
        throw vu::InternalError("default operator<<");
    }
    return o;
}

inline std::ostream& operator<<(std::ostream& o, const QSS2_TransitionGuards& g)
{
    o << " all_ext_init=" << g.all_ext_init << ";"
            " pert_in_evt=" << g.pert_in_evt << ";"
            " new_disc=" << g.new_disc << ";"
            " has_disc=" << g.has_disc << ";";
    return o;
}

/**
 * @brief Differential equation implementation
 * of QSS2
 */
class QSS2 : public de::DifferentialEquationImpl
{
private:
    //definition of DEVS state variables
    QSS2_State state;
    vd::Time minSigma;
    vd::Time lastWakeUp;
    de::Variables::iterator quantizedVariable;
    VarImprovers varImprovers;
    StaticFunctions staticFunctions;
    ExtUps extUps;
    Discontinuities discontinuities;
    QSS2_TransitionGuards guards;
    QSS2_Options options;

public:

    /**
     * @brief QSS2 constuctor
     * @param eq, reference on the corresponding differential equation
     * @param params, the map for initialization of QSS2
     */
    QSS2(de::DifferentialEquation& eq, const vv::Map& params) :
            de::DifferentialEquationImpl(eq, params), state(INIT_SEND), minSigma(
                    vd::infinity), lastWakeUp(vd::negativeInfinity), quantizedVariable(), varImprovers(), staticFunctions(
                    eq), extUps(), discontinuities(eq.getModelName()), guards(), options()
    {
        if (!params.exist("DeltaQ") || !params.get("DeltaQ")->isMap()) {
            throw vu::ModellingError(
                    vle::fmt("[%1%] QSS2 expects a Map for 'DeltaQ' "
                            "parameters") % getModelName());
        }
        const vv::Map& deltaQs = params.getMap("DeltaQ");
        //initialisation of variable indexes and real values
        de::Variables::iterator itb = vars().begin();
        de::Variables::iterator ite = vars().end();
        for (; itb != ite; itb++) {
            const std::string& varName = itb->first;
            const de::Variable& v = itb->second;
            VarImprover& vi = varImprovers.add(itb);
            vi.x0 = v.getVal();
            vi.q0 = v.getVal();
            vi.y0 = v.getVal();
            vi.DeltaQ = deltaQs.getDouble(varName);
            vi.name = varName;
        }
        quantizedVariable = ite;
        //initialization of options
        if (params.exist("expect-gradients")) {
            options.expectGradients = params.getBoolean("expect-gradients");
        } else {
            //compute the default parameter
            options.expectGradients = false;

        }
    }

    /**
     * @brief QSS2 destructor
     */
    virtual ~QSS2()
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
//        DTraceExtension(vle::fmt("[%1%] QSS2 ::processIn (in) state = '%2%',"
//                " trans= '%3%'" ) % getModelName() % state % trans)
        switch (state) {
        case INIT_SEND:
        case INIT_BAG_EATER:
            lastWakeUp = t;
            break;
        case INIT_SEND_1:
            staticFunctions.initializeDerivatives(extUps, t);
            storeDerivatives();
            extUps.resetAllInitialized();
            lastWakeUp = t;
            break;
        case INIT_BAG_EATER_1:
        case INIT_STATIC_FUNCTIONS:
            lastWakeUp = t;
            break;
        case INTEGRATION_TIME:
            switch (trans) {
            case INTERNAL:
                min_sigma(t);
                lastWakeUp = t;
                break;
            case EXTERNAL:
            case CONFLUENT:
                quantizedVariable = vars().end();
                staticFunctionsDeltaExt(false, t);
                quantizerStateVarDeltaExt(t);
                min_sigma(t);
                lastWakeUp = t;
                break;
            }
            break;
        case PERTURBATION:
            quantizedVariable = vars().end();
            staticFunctionsDeltaExt(false, t);
            quantizerStateVarDeltaExt(t);
            meq.reinit(discontinuities.getPerturbsForReinit(), true, t);
            applyPerturbations();
            staticFunctions.numericalGradientDerivative(t, 0);
            quantizerStateVarDeltaExt(t);
            lastWakeUp = t;
            break;
        case NEW_DISC:
        case HAS_DISC:
            quantizedVariable = vars().end();
            staticFunctionsDeltaExt(false, t);
            quantizerStateVarDeltaExt(t);
            meq.reinit(discontinuities.getPerturbsForReinit(), false, t);
            applyPerturbations();
            staticFunctions.numericalGradientDerivative(t, 0);
            quantizerStateVarDeltaExt(t);
            lastWakeUp = t;
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
    void processOut(const vd::Time& t, TransitionType trans)
    {
//        DTraceExtension(vle::fmt("[%1%] QSS2 ::processOut (out) state = '%2%',"
//                " trans= '%3%'" ) % getModelName() % state % trans)
        switch (state) {
        case INIT_SEND:
        case INIT_BAG_EATER:
        case INIT_SEND_1:
        case INIT_BAG_EATER_1:
            break;
        case INIT_STATIC_FUNCTIONS:
            staticFunctions.initializeDerivatives(extUps, t);
            staticFunctions.numericalGradientDerivative(t, 0);
            quantizerStateVarDeltaExt(t);
            break;
        case INTEGRATION_TIME:
            switch (trans) {
            case INTERNAL:
                quantizerStateVarDeltaInt();
                staticFunctionsDeltaExt(true, t);
                quantizerStateVarDeltaExt(t);
                break;
            case EXTERNAL:
                break;
            case CONFLUENT:
                quantizerStateVarDeltaInt();
                staticFunctionsDeltaExt(true, t);
                quantizerStateVarDeltaExt(t);
                break;
            }
            break;

            break;
        case PERTURBATION:

            discontinuities.resetDiscontinuities();
            break;
        case NEW_DISC:
        case HAS_DISC:

            discontinuities.resetDiscontinuities();
            break;
        default:
        {
            throw vu::InternalError("default");
        }
        }
    }

    void storeDerivatives()
    {
        de::Variables::iterator itb = vars().begin();
        de::Variables::iterator ite = vars().end();
        for (; itb != ite; itb++) {
            VarImprover& vi = varImprovers[itb];
            StaticFunctions::contState::iterator itf =
                    staticFunctions.findState(itb);
            StaticFunctions::derivative& der = itf->second;
            vi.y1 = der.mz;
        }
    }

    void applyPerturbations()
    {
        de::Variables::iterator itb = vars().begin();
        de::Variables::iterator ite = vars().end();
        for (; itb != ite; itb++) {
            const de::Variable& v = itb->second;
            VarImprover& vi = varImprovers[itb];
            vi.x0 = v.getVal();
            vi.q0 = v.getVal();
            vi.y0 = v.getVal();
            vi.x1 = v.getGrad();
            vi.q1 = v.getGrad();
            vi.y1 = v.getGrad();
            vi.x2 = 0;
            vi.y2 = 0;
            //simulates staticFunctionsDeltaExt from state var
            //without time advance
            StaticFunctions::contState::iterator itf =
                    staticFunctions.findState(itb);
            StaticFunctions::derivative& der = itf->second;
            der.z = vi.x0;
            der.mz = vi.x1;
        }
    }

    /**
     * @brief Update guards for internal transition
     * @param t, the current time
     * @param trans, the type of the transition
     */
    void updateGuards(const vd::Time& /*t*/, TransitionType trans)
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
                //note : expect gradients is an option
                break;
            }
            break;
        case INIT_SEND_1:
            break;
        case INIT_BAG_EATER_1:
            switch (trans) {
            case INTERNAL:
            case EXTERNAL:
                break;
            case CONFLUENT:
                guards.all_ext_init = extUps.allInitialized(extVars());
                break;
            }
            break;
        case INIT_STATIC_FUNCTIONS:
            break;
        case INTEGRATION_TIME:
            switch (trans) {
            case INTERNAL:
                break;
            case EXTERNAL:
                guards.pert_in_evt = discontinuities.hasPerturb();
                guards.new_disc = discontinuities.hasNewDiscontinuity();
                break;
            case CONFLUENT:
                guards.pert_in_evt = discontinuities.hasPerturb();
                guards.new_disc = discontinuities.hasNewDiscontinuity();
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
            const std::string& portName = (*itb)->getPortName();

//            DTraceExtension(vle::fmt("[%1%] QSS2::handleExtEvt "
//              "state = '%2%', port= '%3%', attributes='%4%'" )
//              % getModelName() % state % portName % (*itb)->attributes())

            //update targeted variable
            if (portName == "perturb") {
                discontinuities.registerPerturb(t, (*itb)->getAttributes());
            } else if (isExtVar(portName)) {
                //get event informations and set default values
                double varValue = (*itb)->getDoubleAttributeValue("value");
                double varGrad = 0;
                if ((*itb)->existAttributeValue("gradient")) {
                    varGrad = (*itb)->getDoubleAttributeValue("gradient");
                } else {
                    if (options.expectGradients) {
                        throw vu::ModellingError(
                                vle::fmt(
                                        "[%1%] External variable update of '%2%' is expected "
                                                "to carry gradient")
                                        % getModelName() % portName);
                    }
                }
                ExternVariables::iterator itf = extVars().find(portName);
                bool hasDiscontinuity = (*itb)->existAttributeValue(
                        "discontinuities");
                if (hasDiscontinuity) {
                    discontinuities.registerExtDisc(t, (*itb)->getAttributes());
                }
                extUps.registerExtUp(itf, varValue, varGrad, t);
            } else {
                throw vu::ModellingError(
                        vle::fmt("[%1%] Unrecognised port '%2%'")
                                % getModelName() % portName);
            }
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

        if (quantizedVariable != vars().end()) {
            const Variable& v = quantizedVariable->second;
            if (getModel().existOutputPort(v.getName())) {
                VarImprovers::cont::const_iterator itfvi = varImprovers.find(
                        quantizedVariable);
                const VarImprover& vi = itfvi->second;

                vd::ExternalEvent* ee = new vd::ExternalEvent(v.getName());
                ee->putAttribute("name", new vv::String(v.getName()));
                ee->putAttribute("value", new vv::Double(vi.y0));
                ee->putAttribute("gradient", new vv::Double(vi.y1));

                extEvtList.push_back(ee);
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
    void outputAllVar(const vd::Time& time, vd::ExternalEventList& extEvtList,
            bool discontinuity) const
    {
        Variables::const_iterator itb = vars().begin();
        Variables::const_iterator ite = vars().end();
        for (; itb != ite; itb++) {
            const Variable& v = itb->second;
            if (getModel().existOutputPort(v.getName())) {
                const VarImprover& vi = varImprovers.get(v.getName());
                vd::ExternalEvent* ee = new vd::ExternalEvent(v.getName());
                ee->putAttribute("name", new vv::String(v.getName()));
                ee->putAttribute("value", new vv::Double(vi.y0));
                ee->putAttribute("gradient", new vv::Double(vi.y1));
                if (discontinuity) {
                    ee->putAttribute("discontinuities",
                            discontinuities.buildDiscsToPropagate(time));
                }
                extEvtList.push_back(ee);
            }
        }
    }

    /************** DEVS functions *****************/
    vd::Time init(const vd::Time& /*time*/)
    {
        state = INIT_SEND;
        return timeAdvance();
    }

    void output(const vd::Time& time, vd::ExternalEventList& ext) const
    {
        switch (state) {
        case INIT_SEND:
            outputAllVar(time, ext, false);
            break;
        case INIT_BAG_EATER:
            break;
        case INIT_SEND_1:
            outputAllVar(time, ext, false);
            break;
        case INIT_BAG_EATER_1:
            break;
        case INIT_STATIC_FUNCTIONS:
            break;
        case INTEGRATION_TIME:
            outputVar(time, ext);
            break;
        case PERTURBATION:
        case NEW_DISC:
            outputAllVar(time, ext, true);
            break;
        case HAS_DISC:
            break;
        default:
            throw vu::InternalError("TODO");
        }
    }

    vd::Time timeAdvance() const
    {
        switch (state) {
        case INIT_SEND:
        case INIT_BAG_EATER:
        case INIT_SEND_1:
        case INIT_BAG_EATER_1:
        case INIT_STATIC_FUNCTIONS:
            return 0;
            break;
        case INTEGRATION_TIME:
            return minSigma;
            break;
        case PERTURBATION:
        case NEW_DISC:
        case HAS_DISC:
            return 0;
            break;
        default:
            throw vu::InternalError("TODO 2");
        }
    }

    void internalTransition(const vd::Time& t)
    {
        processOut(t, INTERNAL);

        updateGuards(t, INTERNAL);

        switch (state) {
        case INIT_SEND:
            if (guards.all_ext_init) {
                state = INIT_STATIC_FUNCTIONS;
            } else {
                state = INIT_BAG_EATER;
            }
            break;
        case INIT_BAG_EATER:
            state = INIT_BAG_EATER;
            break;
        case INIT_SEND_1:
            state = INIT_BAG_EATER_1;
            break;
        case INIT_BAG_EATER_1:
            state = INIT_BAG_EATER_1;
            break;
        case INIT_STATIC_FUNCTIONS:
        case INTEGRATION_TIME:
        case PERTURBATION:
        case NEW_DISC:
        case HAS_DISC:
            state = INTEGRATION_TIME;
            break;
        default:
            throw vu::InternalError("default intTr");
        }
        processIn(t, INTERNAL);
    }

    void externalTransition(const vd::ExternalEventList& event,
            const vd::Time& t)
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
        case INIT_SEND_1:
            throw vu::InternalError("Error 4.5");
            break;
        case INIT_BAG_EATER_1:
            throw vu::InternalError("Error 4.7");
            break;
        case INIT_STATIC_FUNCTIONS:
            throw vu::InternalError("Error 5");
            break;
        case INTEGRATION_TIME:
            if (guards.pert_in_evt) {
                state = PERTURBATION;
            } else if (guards.new_disc) {
                state = NEW_DISC;
            } else {
                state = INTEGRATION_TIME;
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
        default:
            throw vu::InternalError("default extTr");
        }
        processIn(t, EXTERNAL);
    }

    void confluentTransitions(const vd::Time& t,
            const vd::ExternalEventList& ext)
    {
        processOut(t, CONFLUENT);
        handleExtEvt(t, ext);
        updateGuards(t, CONFLUENT);

        switch (state) {
        case INIT_SEND:
            throw vu::InternalError("UNKNOWN 1");
            break;
        case INIT_BAG_EATER:
            if (guards.all_ext_init && !options.expectGradients) {
                state = INIT_STATIC_FUNCTIONS;
            } else if (guards.all_ext_init && options.expectGradients) {
                state = INIT_SEND_1;
            } else {
                state = INIT_BAG_EATER;
            }
            break;
        case INIT_SEND_1:
            state = INIT_BAG_EATER_1;
            break;
        case INIT_BAG_EATER_1:
            if (guards.all_ext_init) {
                state = INIT_STATIC_FUNCTIONS;
            } else {
                state = INIT_BAG_EATER_1;
            }
            break;
        case INIT_STATIC_FUNCTIONS:
            throw vu::InternalError("UNKNOWN 2");
            break;
        case INTEGRATION_TIME:
            if (guards.pert_in_evt) {
                state = PERTURBATION;
            } else if (guards.new_disc) {
                state = NEW_DISC;
            } else {
                state = INTEGRATION_TIME;
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
        case HAS_DISC:
            if (guards.pert_in_evt) {
                state = PERTURBATION;
            } else if (guards.new_disc) {
                state = NEW_DISC;
            } else {
                state = HAS_DISC;
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
    vv::Value* observation(const vd::ObservationEvent& event) const
    {
        const std::string& port = event.getPortName();
        {
            de::Variables::iterator itf = meq.vars().find(port);
            if (itf != meq.vars().end()) {
                VarImprovers::cont::const_iterator itfvi = varImprovers.find(
                        itf);
                if (itfvi != varImprovers.mcont.end()) {
                    double e = event.getTime() - lastWakeUp;
                    const VarImprover& vi = itfvi->second;
                    return new vv::Double(
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
        de::Variables::iterator itb = meq.vars().begin();
        de::Variables::iterator ite = meq.vars().end();

        minSigma = vd::infinity;
        bool found = false;
        for (; itb != ite; itb++) {
            VarImprover& vi = varImprovers[itb];
            if (vi.sig < minSigma) {
                minSigma = vi.sig;
                quantizedVariable = itb;
                found = true;
            }
        }
        if (!found) {
            quantizedVariable = meq.vars().begin();
        }

        //prepare output (lambda)
        VarImprover& vq = varImprovers[quantizedVariable];
        vq.y0 = vq.x0 + vq.x1 * minSigma + vq.x2 * minSigma * minSigma / 2;
        vq.y1 = vq.x1 + vq.x2 * minSigma;
    }

    /**
     * @brief Simulates delta_ext of all state variables
     * quantizers, which is always done after static function updates.
     * (QSS2 paper, page 9)
     * @param t, current time
     */
    void quantizerStateVarDeltaExt(double t)
    {
        double e = t - lastWakeUp;

        de::Variables::iterator itb = vars().begin();
        de::Variables::iterator ite = vars().end();
        for (; itb != ite; itb++) {
            VarImprover& vi = varImprovers[itb];
            std::pair<double, double> out = staticFunctions.output(itb);
            if (itb != quantizedVariable) {
                //note: update of quantizedVariable already done
                //in quantizerStateVarDeltaInt. To match the
                //algorithm, this update should actually be performed
                //with e=0 for the quantized variable, which
                //leads to the same results.
                vi.x0 = vi.x0 + vi.x1 * e + vi.x2 * e * e / 2;
                vi.q0 = vi.q0 + vi.q1 * e;
            }
            vi.x1 = out.first;
            vi.x2 = out.second;
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
    void quantizerStateVarDeltaInt()
    {
        VarImprover& vi = varImprovers[quantizedVariable];
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
     * (QSS2 paper, page 9)
     * @param fromStateVar, if true, the static function
     * delta_ext is computed after a state variable quantization.
     * Otherwise, it is computed after external variables updates.
     * @param t, current time
     */
    void staticFunctionsDeltaExt(bool fromStateVar, double t)
    {
        double e = t - lastWakeUp;

        if (fromStateVar) {
            VarImprover& vi = varImprovers[quantizedVariable];
            staticFunctions.deltaExtFromStateVar(quantizedVariable, vi.y0,
                    vi.y1, t, e);
        } else {
            staticFunctions.deltaExtFromExtVar(extUps, t, e);
        }

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
}
}// namespace vle extension differential_equation qss2

#endif
