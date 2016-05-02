/*
 * @file vle/extension/differential_equation/PerturbationHanlder.hpp
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
#include <vle/extension/differential-equation/PerturbationHandler.hpp>

namespace vle {
namespace extension {
namespace differential_equation {

namespace vd = vle::devs;
namespace vv = vle::value;
namespace vu = vle::utils;
namespace vz = vle::vpz;

Discontinuities::Discontinuities(const std::string& modId) :
        registeredDiscs(), discsToPropagate(), pertsForReinit(),
        registeredTime(vd::negativeInfinity), modelId(modId),
        registeredPerturb(false), registeredDisc(false),
        registeredNewDisc(false)
{

}

void Discontinuities::registerExtDisc(const vd::Time& time,
        const vv::Map& dataCarried)
{
    if (registeredTime != time) {
        registeredDiscs.clear();
        discsToPropagate.clear();
        pertsForReinit.clear();
        registeredTime = time;
        registeredPerturb = false;
        registeredDisc = false;
        registeredNewDisc = false;
    }

    //should propagate ?
    const vv::Map& perturbs = dataCarried.getMap("discontinuities");
    vv::Map::const_iterator itb = perturbs.begin();
    vv::Map::const_iterator ite = perturbs.end();

    for (; itb != ite; itb++) {
        const std::string& idPert = itb->first;
        const vv::Set& idModels = itb->second->toSet();
        std::pair<RegisteredDiscs::iterator, bool> ri = registeredDiscs.insert(
                std::make_pair(idPert, std::set<std::string>()));
        std::set<std::string>& idModelsHere = ri.first->second;
        idModelsHere.insert(modelId);
        if (ri.second) { //new perturbation
            vv::Set::const_iterator itbm = idModels.begin();
            vv::Set::const_iterator item = idModels.end();
            for (; itbm != item; itbm++) {
                const std::string& idMod = (*itbm)->toString().value();
                idModelsHere.insert(idMod);
            }
            registeredNewDisc = true;
        } else {
            registeredNewDisc = false;
            vv::Set::const_iterator itbm = idModels.begin();
            vv::Set::const_iterator item = idModels.end();
            for (; itbm != item; itbm++) {
                const std::string& idMod = (*itbm)->toString().value();
                registeredNewDisc = registeredNewDisc
                        || idModelsHere.insert(idMod).second; //new model id?
            }
        }
        //declare pert passes through current model
        idModelsHere.insert(modelId);
        //build discToPropagate if necessary
        if (registeredNewDisc) {
            vv::Set& idModelsAll = discsToPropagate.addSet(idPert);
            std::set<std::string>::const_iterator itbm = idModelsHere.begin();
            std::set<std::string>::const_iterator item = idModelsHere.end();
            for (; itbm != item; itbm++) {
                const std::string& idM = *itbm;
                idModelsAll.addString(idM);
            }
        } else {
            discsToPropagate.clear();
        }
    }
    registeredDisc = true;
}

void Discontinuities::registerPerturb(const vd::Time& time,
        const vv::Map& dataCarried)
{
    if (registeredTime != time) {
        registeredDiscs.clear();
        discsToPropagate.clear();
        pertsForReinit.clear();
        registeredTime = time;
        registeredPerturb = false;
        registeredDisc = false;
        registeredNewDisc = false;
    }

    std::string idPert;
    std::stringstream ss;
    ss << time << "_" << modelId;
    idPert.assign(ss.str());
    std::set<std::string>& idModels = registeredDiscs[idPert];
    idModels.insert(modelId);
    pertsForReinit.add(std::unique_ptr<vv::Value>(new vv::Map(dataCarried)));
    vv::Set& idModelsAll = discsToPropagate.addSet(idPert);
    idModelsAll.addString(modelId);
    registeredPerturb = true;
    registeredDisc = true;
    registeredNewDisc = false;
}

void Discontinuities::resetDiscontinuities()
{
    discsToPropagate.clear();
    pertsForReinit.clear();
    registeredPerturb = false;
    registeredDisc = false;
    registeredNewDisc = false;
}

std::unique_ptr<vv::Map> Discontinuities::buildDiscsToPropagate(
        const vd::Time& time) const
{
    //for dbg
    if ((time != registeredTime) || discsToPropagate.empty()) {
        throw vu::InternalError("INT error : nothing to propagate ");
    }
    return std::unique_ptr<vv::Map>(new vv::Map(discsToPropagate));
}

const vv::Set& Discontinuities::getPerturbsForReinit() const
{
    return pertsForReinit;
}

bool Discontinuities::hasPerturb() const
{
    return registeredPerturb;
}

bool Discontinuities::hasNewDiscontinuity() const
{
    return registeredNewDisc;
}

bool Discontinuities::hasDiscontinuity() const
{
    return registeredDisc;
}

ExtUp::ExtUp() :
        value(0), gradient(0), upTime(vd::negativeInfinity), hasgradient(false)
{
}

ExtUps::ExtUps() :
        mcont()
{
}

ExtUps::cont::iterator ExtUps::find(ExternVariables::iterator it)
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

void ExtUps::registerExtUp(ExternVariables::iterator it, double value,
        const vd::Time& time)
{
    cont::iterator itf = find(it);
    if (itf == mcont.end()) {
        mcont.push_back(std::make_pair(it, ExtUp()));
        itf = find(it);
        ExtUp& u = itf->second;
        u.set(value);
        u.setTime(time);
    } else {
        ExtUp& u = itf->second;
        u.set(value);
        u.setTime(time);
    }
}

void ExtUps::registerExtUp(ExternVariables::iterator it, double value,
        double gradient, const vd::Time& time)
{
    cont::iterator itf = find(it);
    if (itf == mcont.end()) {
        mcont.push_back(std::make_pair(it, ExtUp()));
        itf = find(it);
        ExtUp& u = itf->second;
        u.set(value, gradient);
        u.setTime(time);
    } else {
        ExtUp& u = itf->second;
        u.set(value, gradient);
        u.setTime(time);
    }
}

bool ExtUps::allInitialized(ExternVariables& ext)
{
    ExternVariables::iterator itb = ext.begin();
    ExternVariables::iterator ite = ext.end();
    for (; itb != ite; itb++) {
        if (find(itb) == mcont.end()) {
            return false;
        }
    }
    return true;
}

void ExtUps::resetAllInitialized()
{
    mcont.clear();
}

}
}
} // namespace vle extension differential_equation

