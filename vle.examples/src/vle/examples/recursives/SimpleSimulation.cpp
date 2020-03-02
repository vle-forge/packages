/*
 * @file vle/examples/recursives/SimpleSimulation.cpp
 *
 * This file is part of VLE, a framework for multi-modeling, simulation
 * and analysis of complex dynamical systems
 * http://www.vle-project.org
 *
 * Copyright (c) 2003-2007 Gauthier Quesnel <quesnel@users.sourceforge.net>
 * Copyright (c) 2003-2011 ULCO http://www.univ-littoral.fr
 * Copyright (c) 2007-2011 INRA http://www.inra.fr
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


#include <iostream>
#include <cassert>
#include <algorithm>
#include <vle/devs/Dynamics.hpp>
#include <vle/vpz/Vpz.hpp>
#include <vle/manager/Simulation.hpp>
#include <vle/utils/Package.hpp>


namespace vle { namespace examples { namespace recursives {

class SimpleSimulation : public devs::Dynamics
{
public:
    SimpleSimulation(const devs::DynamicsInit& mdl,
                     const devs::InitEventList& lst)
        : devs::Dynamics(mdl, lst)
    {}

    virtual ~SimpleSimulation()
    {}

    devs::Time init(devs::Time /* time */) override
    {
        vle::utils::Package pack(context(), "vle.examples");

        std::unique_ptr<vpz::Vpz> file(new vpz::Vpz(
                pack.getExpFile("counter.vpz", vle::utils::PKG_BINARY)));

        manager::Simulation sim(context(), manager::SIMULATION_NONE,
                std::chrono::milliseconds(0));
        std::unique_ptr<value::Map> result = sim.run(std::move(file), NULL);

        assert(result);

        // Show the results
        std::for_each(result->begin(), result->end(), ShowMatrix());

        // le modèle ne fait rien d'autre.
        return devs::infinity;
    }

    /**
     * @brief A functor to show the matrix result.
     */
    struct ShowMatrix
    {
        void operator()(
            const value::Map::value_type& x) const
        {
            assert(x.second);

            std::cout << "view: " << x.first << "\n"
                << x.second->writeToString() << "\n";
        }
    };
};

}}} // namespace vle examples recursives

DECLARE_DYNAMICS(vle::examples::recursives::SimpleSimulation)
