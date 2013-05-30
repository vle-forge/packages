#include <vle/extension/differential-equation/DifferentialEquation.hpp>
#include <vle/extension/differential-equation/TimeSlicingMethod.hpp>
#include <vle/extension/differential-equation/QSS2.hpp>

namespace vle {
namespace extension {
namespace differential_equation {

namespace vd = vle::devs;
namespace vv = vle::value;
namespace vu = vle::utils;
namespace ts = vle::extension::differential_equation::timeSlicingMethod;

DifferentialEquation::DifferentialEquation(const vd::DynamicsInit& model,
        const vd::InitEventList& events) :
        vd::Dynamics(model, events), mvars(), mextVars(), meqImpl(0),
            mdeclarationOn(true), minitVariables(0), mmethParams(0), mmethod()
{
    if (!events.exist("method") || !events.get("method")->isString()) {
        throw vu::ModellingError("error method");
    }
    if (!events.exist("method-parameters")
            || !events.get("method-parameters")->isMap()) {
        throw vu::ModellingError("error method-parameters");
    }
    minitVariables = new vv::Map(events.getMap("variables"));
    mmethParams = (vv::Map*) events.get("method-parameters")->clone();
    mmethod = events.getString("method");
}

DifferentialEquation::~DifferentialEquation()
{
    if (minitVariables) {
        delete minitVariables;
    }
    if (mmethParams) {
        delete mmethParams;
    }
    if (meqImpl) {
        delete meqImpl;
    }
}

DifferentialEquation::Var DifferentialEquation::createVar(
        const std::string& name)
{
    if (not mdeclarationOn) {
        throw vu::InternalError(" TODO mdeclarationOn Var");
    }
    mvars.add(name);
    return Var(name, this);
}

DifferentialEquation::Var DifferentialEquation::createVar(
        const std::string& name, double value)
{
    if (not mdeclarationOn) {
        throw vu::InternalError(" TODO mdeclarationOn Var");
    }
    mvars.add(name);
    return Var(name, value, this);
}

DifferentialEquation::Ext DifferentialEquation::createExt(
        const std::string& name)
{
    if (not mdeclarationOn) {
        throw vu::InternalError(" TODO mdeclarationOn Ext");
    }
    mextVars.add(name);
    return Ext(name, this);
}

void DifferentialEquation::reinit(const vv::Set& evt, bool perturb,
        const vd::Time& t)
{
    if (perturb) {
        vv::Set::const_iterator itb = evt.begin();
        vv::Set::const_iterator ite = evt.end();
        for (; itb != ite; itb++) {
            const vv::Map& evtMap = (*itb)->toMap();
            const std::string& varName = evtMap.getString("name");
            double varVal = evtMap.getDouble("value");
            Variables::iterator itf = vars().find(varName);
            if (itf == vars().end()) {
                throw utils::ModellingError(
                        vle::fmt("[%1%] State variable '%2%' not found "
                                "on perturbation") % getModelName() % varName);
            }
            itf->second.setVal(varVal);
        }
    }
    compute(t);
}

/************** DEVS functions *****************/
vd::Time DifferentialEquation::init(const vd::Time& time)
{
    mdeclarationOn = false;

    //initialisation of variables
    if (minitVariables) {
        Variables::iterator itb = mvars.begin();
        Variables::iterator ite = mvars.end();
        for (; itb != ite; itb++) {
            Variable& v = itb->second;
            if (minitVariables->exist(v.getName())) {
                v.setVal(minitVariables->getDouble(v.getName()));
            } else {
                v.setVal(0);
            }
        }
    }

    //check that external variables ports are present
    //(required for initialization)
    ExternVariables::const_iterator ieb = mextVars.begin();
    ExternVariables::const_iterator iee = mextVars.end();
    for (; ieb != iee; ieb++) {
        if (!getModel().existInputPort(ieb->first)) {
            throw vu::ModellingError(
                    vle::fmt("[%1%] Input port corresponding to extern variable"
                            " '%2%' should be present") % getModelName()
                            % ieb->first);
        }
    }

    //initialisation of method
    if (mmethod == "euler") {
        meqImpl = new ts::TimeSlicingMethod<ts::Euler>(*this, *mmethParams);
    } else if (mmethod == "rk4") {
        meqImpl = new ts::TimeSlicingMethod<ts::RK4>(*this, *mmethParams);
    } else if (mmethod == "qss2") {
        meqImpl = new qss2::QSS2(*this, *mmethParams);
    } else {
        throw vu::InternalError(vle::fmt("[%1%] numerical integration method"
                " not recognized '%1%'") % mmethod);
    }
    return meqImpl->init(time);
}

void DifferentialEquation::output(const vd::Time& time,
        vd::ExternalEventList& output) const
{
    meqImpl->output(time, output);
}

vd::Time DifferentialEquation::timeAdvance() const
{
    return meqImpl->timeAdvance();
}

void DifferentialEquation::confluentTransitions(const vd::Time& time,
        const vd::ExternalEventList& extEventlist)
{
    meqImpl->confluentTransitions(time, extEventlist);
}

void DifferentialEquation::internalTransition(const vd::Time& event)
{
    meqImpl->internalTransition(event);
}

void DifferentialEquation::externalTransition(
        const vd::ExternalEventList& event, const vd::Time& time)
{
    meqImpl->externalTransition(event, time);
}

vv::Value* DifferentialEquation::observation(
        const vd::ObservationEvent& event) const
{
    return meqImpl->observation(event);
}

}
}
}
