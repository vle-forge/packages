/*
 * @file vle/examples/fsa/Meteo.cpp
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

/*
 * @@tagdynamic@@
 */

#include <vle/devs/Dynamics.hpp>
#include <vle/utils/Rand.hpp>

namespace vle { namespace examples { namespace fsa {

namespace vd = vle::devs;

class Meteo : public vd::Dynamics
{
public:
    Meteo(const vd::DynamicsInit& init,
          const vd::InitEventList& events) :
        vd::Dynamics(init, events)
    {
        if (events.exist("seed")) {
            mRand.seed(events.getInt("seed"));
        } else {
            mRand.seed(12389);
        }
    }

    virtual ~Meteo()
{ }

    virtual vd::Time init(vd::Time /* time */) override
    {
        mPhase = INIT;
        mValue = mRand.getDouble(5, 30);
        return 0.0;
    }

    virtual vd::Time timeAdvance() const override
    {
        if (mPhase == RUN) return 1.0;
        if (mPhase == SEND) return 0.0;
        return vd::infinity;
    }

    virtual void output(vd::Time /* time */,
                        vd::ExternalEventList& outputs) const override
    {
        if (mPhase == SEND or mPhase == INIT) {
            outputs.emplace_back("out");
            value::Map& m = outputs.back().addMap();
            m.addString("name", "Tmoy");
            m.addDouble("value", mValue);
        }
    }

    virtual void internalTransition(vd::Time /* time */) override
    {
        if (mPhase == INIT or mPhase == SEND) {
            mPhase = RUN;
        } else if (mPhase == RUN) {
            mValue = mRand.getDouble(5, 30);
            mPhase = SEND;
        }
    }

    virtual std::unique_ptr<vle::value::Value> observation(
        const vd::ObservationEvent& event) const override
    {
        if (event.onPort("state")) {
            return value::Double::create(mValue);
        }
        return 0;
    }

private:
    enum Phase { INIT, RUN, SEND };

    Phase mPhase;
    double mValue;
    utils::Rand mRand;
};

DECLARE_DYNAMICS(Meteo)

}}} // namespace vle examples fsa
