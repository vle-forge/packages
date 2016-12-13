/*
 * @file vle/ode/DifferentialEquation.hpp
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

#ifndef VLE_ODE_DIFFERENTIAL_EQUATION_HPP
#define VLE_ODE_DIFFERENTIAL_EQUATION_HPP 1

#include <map>
#include <set>

#include <vle/devs/Dynamics.hpp>
#include <vle/utils/Exception.hpp>
#include <vle/utils/Tools.hpp>



namespace vle {
namespace ode {

namespace vd = vle::devs;
namespace vv = vle::value;
namespace vu = vle::utils;

//implementation struct includes
class DifferentialEquationImpl;
class Variable;

/**
 * @brief Main class for ode dynamics
 */
class DifferentialEquation : public vle::devs::Dynamics
{
public:
    /**
     * @bried User interface for handling variables
     */
    class Var
    {
    public:
        std::string               name;
        Variable*                 itVar;
        DifferentialEquationImpl* meqImpl;

        Var() :
            name(""), itVar(0), meqImpl(0)
        {
        }

        void init(DifferentialEquation* de, const std::string& varName,
                const vd::InitEventList& initMap);

        void operator=(double value);

        double operator()();

        double operator()() const;
    };


    /**
     * @brief Internal structure to handle derivatives
     */
    class grad_intern
    {
    public:

        Var& var;

        grad_intern(Var& v) :
                var(v)
        {
        }
        grad_intern(const grad_intern& g) :
                var(g.var)
        {
        }

        double operator()();

        void operator=(double g);
    };

    DifferentialEquation(const vd::DynamicsInit& model,
            const vd::InitEventList& events);
    virtual ~DifferentialEquation();

    /*  - - - - - - - - - - - - - --ooOoo-- - - - - - - - - - - -  */
    /////////////
    //User API
    /////////////

    /**
     * @brief Computation of gradients, which is defined by the user
     * @param time, the time at which derivative are required
     */
    virtual void compute(const vle::devs::Time& time) = 0;
    inline grad_intern grad(Var& v)
    {
        return grad_intern(v);
    }

    /*  - - - - - - - - - - - - - --ooOoo-- - - - - - - - - - - -  */
    //////////////////////
    //DEVS implementation
    //////////////////////
    vd::Time init(vd::Time time) override;
    void output(vd::Time time, vd::ExternalEventList& output) const override;
    vd::Time timeAdvance() const override;
    void internalTransition(vd::Time time) override;
    void externalTransition(const vd::ExternalEventList& event,
            vd::Time time) override;
    void confluentTransitions(vd::Time time,
            const vd::ExternalEventList& extEventlist) override;
    std::unique_ptr<vv::Value> observation(
            const vd::ObservationEvent& event) const override;

    DifferentialEquationImpl* meqImpl;
};

}
} // namespace vle ode

#endif
