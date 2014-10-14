/*
 * Copyright (C) 2009-2014 INRA
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
#include <vle/value/Map.hpp>
#include <vle/recursive/EmbeddedSimulatorMPI.hpp>
#include <vle/recursive/AbstractExpe.hpp>

namespace vle {
namespace recursive {
namespace models {


namespace vd = vle::devs;
namespace vv = vle::value;
namespace rr = vle::recursive;

class SimulatorMPI: public rr::AbstractExpe
{

public:
    SimulatorMPI(const vd::DynamicsInit& init, const vd::InitEventList& events) :
        rr::AbstractExpe(init,events), res(), simMpi()
    {
    }

    virtual ~SimulatorMPI()
    {
    }

    virtual void initExpe()
    {
        simMpi.init(getInitializations());
    }

    bool processStep(const vv::Integer& /*current_step*/)
    {
        const vv::Tuple& inputPlan = getInitializations().getTuple("design");
        simMpi.simulatePlan(inputPlan);
        simMpi.fillWithLastValuesOfOutputs(res);
        return true;
    }

    vv::Value* observe(const std::string& obs) const
    {
        if (obs == "expeRes") {
            return res.clone();
        }
        if (obs == "expeRes[0]") {
            return new vv::Double(res[0]);
        }
        if (obs == "expeRes[1]") {
            return new vv::Double(res[1]);
        }
        return 0;
    }

    vv::Tuple res;
    rr::EmbeddedSimulatorMPI simMpi;
};

}}}// namespaces

DECLARE_DYNAMICS(vle::recursive::models::SimulatorMPI)
