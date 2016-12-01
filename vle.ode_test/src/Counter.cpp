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

/**
 * @@tagdynamic@@
 */

#include <vle/devs/Dynamics.hpp>
#include <vle/value/Value.hpp>
#include <vle/utils/Exception.hpp>


#include <iostream>

namespace vle_ode { namespace test { namespace dynamics  {

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
        vd::Time init(vd::Time /*time*/) override
        {
            return vd::infinity;
        }
        /**
         * @brief Implementation of Dynamics::timeAdvance
         */
        vd::Time timeAdvance() const override
        {
            return vd::infinity;
        }
        /**
         * @brief Implementation of Dynamics::internalTransition
         */
        void internalTransition(vd::Time /* time */) override
        {
        }
        /**
         * @brief Implementation of Dynamics::output
         */
        void output(vd::Time /* time */,
                vd::ExternalEventList& /*output*/) const override
        {
        }

        /**
         * @brief Implementation of Dynamics::externalTransition
         */
        void externalTransition(const vd::ExternalEventList& /*event*/,
                                vd::Time /* time */) override
        {
            nbExtEvent++;
        }
        /**
         * @brief Implementation of Dynamics::observation
         */
        std::unique_ptr<vv::Value> observation(
                const vd::ObservationEvent& /*event*/) const override
        {
            return vv::Double::create((double) nbExtEvent);
        }
    };


}}} // namespace vle_ode test dynamics

DECLARE_DYNAMICS(vle_ode::test::dynamics::Counter)
    //DECLARE_DYNAMICS(vle_ode::test::dynamics::Counter)
