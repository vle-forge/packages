/*
 * @file vle/examples/recursives/RecursiveSimulation.cpp
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


#include <vle/devs/Dynamics.hpp>
#include <vle/vpz/Vpz.hpp>
#include <vle/manager/Simulation.hpp>
#include <vle/utils/Package.hpp>
#include <boost/lexical_cast.hpp>

namespace vle { namespace examples { namespace recursives {

class RecursiveSimulation : public devs::Dynamics
{
public:
    RecursiveSimulation(const devs::DynamicsInit& mdl,
                        const devs::InitEventList& lst)
        : devs::Dynamics(mdl, lst)
    {}

    virtual ~RecursiveSimulation()
    {}

    virtual devs::Time init(const devs::Time& /* time */)
    {
        if (getModelName() == "10") {    // close the recursive simulation
            return devs::infinity;       // creation after
        }                                // 10 recursive models
        return 0.0;
    }

    virtual void internalTransition(const devs::Time& /* time */)
    {
        const std::string& myname(getModelName());
        const int nb = boost::lexical_cast < int >(myname);

        vle::utils::Package pack("vle.examples");

        vpz::Vpz *file = new vpz::Vpz(pack.getExpFile("recursive.vpz",
                vle::utils::PKG_BINARY));

        vpz::BaseModel::rename(file->project().model().model(),
                               boost::lexical_cast < std::string >(nb + 1));

        utils::ModuleManager man;

        manager::Simulation sim(manager::LOG_NONE,
                                manager::SIMULATION_NO_RETURN,
                                NULL);

        value::Map *result = sim.run(file, man, NULL);

        assert(not result);
    }
};

}}} // namespace vle examples recursives

DECLARE_DYNAMICS(vle::examples::recursives::RecursiveSimulation)
