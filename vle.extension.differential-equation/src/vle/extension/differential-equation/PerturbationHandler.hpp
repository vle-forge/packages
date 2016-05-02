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

#ifndef VLE_EXTENSION_DIFFERENTIAL_EQUATION_PERTURBATIONHANDLER_HPP
#define VLE_EXTENSION_DIFFERENTIAL_EQUATION_PERTURBATIONHANDLER_HPP 1

#include <iostream>
#include <sstream>
#include <map>
#include <set>

#include <vle/vpz/AtomicModel.hpp>
#include <vle/utils/Exception.hpp>
#include <vle/value/Set.hpp>

#include <vle/extension/differential-equation/DifferentialEquationImpl.hpp>

namespace vle {
namespace extension {
namespace differential_equation {

namespace vd = vle::devs;
namespace vv = vle::value;
namespace vu = vle::utils;
namespace vz = vle::vpz;

class Discontinuities
{

    typedef std::map<std::string, std::set<std::string> > RegisteredDiscs;

    typedef vv::Map DiscsToPropagate;

    typedef vv::Set PertsForReinit;

    /**
     * @brief The set of registered discontinuities
     * at the current time
     */
    RegisteredDiscs registeredDiscs;

    /**
     * @brief The set discontinuities that should be
     * propagated at the current time
     */
    DiscsToPropagate discsToPropagate;

    /**
     * @brief The set of perturbation messages
     * used for reinitialisation
     * at the current time
     */
    PertsForReinit pertsForReinit;

    /**
     * @brief The last time discontinuities
     * have been observed
     */
    vd::Time registeredTime;

    /**
     * @brief The identifier of the atomic model
     */

    std::string modelId;
    /**
     * @brief Boolean at true if the current
     * there is perturbation
     */
    bool registeredPerturb;

    /**
     * @brief Boolean at true it there is a
     * discontinuity
     */
    bool registeredDisc;

    /**
     * @brief Boolean at true it there is a
     * new observed discontinuity
     */
    bool registeredNewDisc;

public:

    /**
     * @brief Discontinuities constructor
     * @param modId, identifier of the model
     */
    Discontinuities(const std::string& modId);

    /**
     * @brief Function for registering a new observed discontinuity
     * @param time, the current time
     * @param dataCarried, the data carried on the discontinuity event
     */
    void registerExtDisc(const vd::Time& time, const vv::Map& dataCarried);

    /**
     * @brief Function for registering a new observed perturbation
     * @param time, the current time
     * @param dataCarried, the data carried on the perturbation event
     */
    void registerPerturb(const vd::Time& time, const vv::Map& dataCarried);

    /**
     * @brief Function for reseting discontinuities to propagate
     * and to use for re-initialization
     */
    void resetDiscontinuities();

    /**
     * @brief Builds the set of discontinuities to propagate
     * @param time, the current time
     * @return a new Set containing the discontinuities to
     * propagate
     */
    std::unique_ptr<DiscsToPropagate>  buildDiscsToPropagate(
            const vd::Time& time) const;

    /**
     * @brief Acces function to discontinuities to use
     * for re-initialization
     * @return the set of perturbation messages to use for
     * re-initialization
     */
    const PertsForReinit& getPerturbsForReinit() const;

    /**
     * @brief Access function that tell if current discontinuities
     * contain a perturbation
     * @return true, if discontinuities contain a perturbation
     */
    bool hasPerturb() const;

    /**
     * @brief Access function that tell if current discontinuities
     * contain a new discontinuity
     * @return true, if discontinuities contain a new discontinuity
     */
    bool hasNewDiscontinuity() const;

    /**
     * @brief Access function that tells if there is a discontinuity
     * @return true, if there is a discontinuity
     */
    bool hasDiscontinuity() const;

    friend std::ostream& operator<<(std::ostream& o,
            const Discontinuities& dis);
};

class ExtUp
{
public:

    ExtUp();

    inline double getVal() const
    {
        return value;
    }

    inline double getGrad() const
    {
        return gradient;
    }

    inline const vd::Time& getTime() const
    {
        return upTime;
    }

    inline bool hasGradient() const
    {
        return hasgradient;
    }

    inline void set(double v, double g)
    {
        value = v;
        gradient = g;
        hasgradient = true;
    }

    inline void set(double v)
    {
        value = v;
        hasgradient = false;
    }

    inline void setTime(const vd::Time& t)
    {
        upTime = t;
    }

private:
    double value;
    double gradient;
    vd::Time upTime;
    bool hasgradient;
};

class ExtUps
{
public:

    typedef std::vector<std::pair<ExternVariables::iterator, ExtUp> > cont;

    cont mcont;

    ExtUps();

    cont::iterator find(ExternVariables::iterator it);

    void registerExtUp(ExternVariables::iterator it, double value,
            const vd::Time& time);

    void registerExtUp(ExternVariables::iterator it, double value,
            double gradient, const vd::Time& time);

    bool allInitialized(ExternVariables& ext);

    void resetAllInitialized();
};

inline std::ostream& operator<<(std::ostream& o, const Discontinuities& dis)
{
    o << "Dicontinuities:[registeredDiscs:(";
    Discontinuities::RegisteredDiscs::const_iterator itb =
            dis.registeredDiscs.begin();
    Discontinuities::RegisteredDiscs::const_iterator ite =
            dis.registeredDiscs.end();
    for (; itb != ite; itb++) {
        const std::string& idPerturb = itb->first;
        o << idPerturb << "{";
        const std::set<std::string>& idModels = itb->second;
        std::set<std::string>::const_iterator itbi = idModels.begin();
        std::set<std::string>::const_iterator itei = idModels.end();
        for (; itbi != itei; itbi++) {
            const std::string& idMod = *itbi;
            o << idMod << ",";
        }
        o << idPerturb << "};";
    }
    o << "), toPropagate:" << dis.discsToPropagate;
    o << ", forReinit:" << dis.pertsForReinit;
    return o;
}

inline std::ostream& operator<<(std::ostream& o, const ExtUps& eus)
{
    ExtUps::cont::const_iterator itb = eus.mcont.begin();
    ExtUps::cont::const_iterator ite = eus.mcont.end();
    o << "ExtUps:[";
    for (; itb != ite; itb++) {
        ExternVariables::iterator itev = itb->first;
        const ExternVariable& ev = itev->second;
        const ExtUp& eu = itb->second;
        o << "(" << ev.getName() << ": v=" << eu.getVal() << ", grad="
                << eu.getGrad() << "),";
    }
    o << "]";
    return o;
}

}
}
} // namespace vle extension differential_equation

#endif
