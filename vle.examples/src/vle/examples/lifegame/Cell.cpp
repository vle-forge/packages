/*
 * @file vle/examples/lifegame/Cell.cpp
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
 * @@tagdepends: vle.extension.celldevs @@endtagdepends
 */


#include <vle/extension/celldevs/CellDevs.hpp>
#include <vle/devs/Dynamics.hpp>
#include <vle/utils/Rand.hpp>

namespace vle { namespace examples { namespace lifegame {

class Cell : public extension::CellDevs
{
private:
    enum state { INIT, IDLE, NEWSTATE };

    double mTimeStep;
    state mState;
    utils::Rand mRand;

public:
    Cell(const devs::DynamicsInit& model,
         const devs::InitEventList& events) :
        extension::CellDevs(model, events)
    {
        if (events.exist("seed")) {
            mRand.seed(events.getInt("seed"));
        }
    }

    virtual ~Cell()
    {
    }

    virtual devs::Time init(const devs::Time& /* time */)
    {
        mTimeStep = value::toDouble(m_parameters["TimeStep"]);

        initBooleanNeighbourhood("s",false);
        if (!existState("s")) {
            double colour = mRand.getDouble();

            if (colour > 0.5) initBooleanState("s", true);
            else initBooleanState("s", false);
        }

        mState = INIT;
        neighbourModify();
        setSigma(devs::Time(0.0));
        return devs::Time(0.0);
    }

    virtual void internalTransition(const devs::Time& time)
    {
        CellDevs::internalTransition(time);
        switch (mState) {
        case INIT:
            mState = IDLE;
            setSigma(devs::Time(0.0));
            break;
        case IDLE:
            setLastTime(time);
            mState = NEWSTATE;
            setSigma(devs::Time(0.0));
            break;
        case NEWSTATE:
            bool v_state = getBooleanState("s");
            unsigned int n = getBooleanNeighbourStateNumber("s", true);

            if (v_state && (n < 2 || n > 3)) {
                setBooleanState("s",false);
                modify();
                mState = INIT;
                setSigma(mTimeStep);
            }
            else if (!v_state && (n == 3)) {
                setBooleanState("s",true);
                modify();
                mState = INIT;
                setSigma(mTimeStep);
            }
            else {
                mState = IDLE;
                setSigma(devs::infinity);
            }
            break;
        }
    }

    virtual void processPerturbation(const vle::devs::ExternalEvent& /* event */)
    {
    }

    virtual void updateSigma(devs::Time)
    {
        setSigma(devs::Time(0.0));
    }

};

}}} // namespace vle examples lifegame

DECLARE_DYNAMICS(vle::examples::lifegame::Cell)
