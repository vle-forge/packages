/*
 * This file is part of VLE, a framework for multi-modeling, simulation
 * and analysis of complex dynamical systems
 * http://www.vle-project.org
 *
 * Copyright (c) 2014 INRA http://www.inra.fr
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

#include <vle/devs/Dynamics.hpp>
#include <vle/value/Value.hpp>
#include <vle/utils/Exception.hpp>
#include <vle/devs/DynamicsDbg.hpp>

#include <iostream>

namespace differential_equation { namespace test { namespace dynamics  {

    namespace vv = vle::value;
    namespace vd = vle::devs;
    namespace vu = vle::utils;

    /**
     * @brief Counter counts the number of external transition
     */
    class Counter: public vd::Dynamics
    {
    private:
        enum STATE {
            BEFORE_PERT,
            DURING_PERT,
            AFTER_PERT
        };

        int nbExtEvent;

    public:
        Counter(const vd::DynamicsInit& init, const vd::InitEventList& events):
            vd::Dynamics(init, events), nbExtEvent(0)
        {
        }

        virtual ~Counter()
        {
        }

        /**
         * @brief Implementation of Dynamics::init
         */
        vd::Time init(const vd::Time& /*time*/)
        {
            return vd::infinity;
        }
        /**
         * @brief Implementation of Dynamics::timeAdvance
         */
        vd::Time timeAdvance() const
        {
            return vd::infinity;
        }
        /**
         * @brief Implementation of Dynamics::internalTransition
         */
        void internalTransition(const vd::Time& /* time */)
        {
        }
        /**
         * @brief Implementation of Dynamics::output
         */
        void output(const vd::Time& /* time */,
                vd::ExternalEventList& /*output*/) const
        {
        }

        /**
         * @brief Implementation of Dynamics::externalTransition
         */
        void externalTransition(const vd::ExternalEventList& /*event*/,
                                const vd::Time& /* time */)
        {
            nbExtEvent++;
        }
        /**
         * @brief Implementation of Dynamics::observation
         */
        vv::Value* observation(const vd::ObservationEvent& /*event*/) const
        {
            return new vv::Double((double) nbExtEvent);
        }
    };


}}} // namespace differential_equation test dynamics

DECLARE_DYNAMICS(differential_equation::test::dynamics::Counter)
    //DECLARE_DYNAMICS_DBG(differential_equation::test::dynamics::Counter)
