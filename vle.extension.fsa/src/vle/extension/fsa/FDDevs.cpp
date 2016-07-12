/*
 * @file vle/extension/fsa/FDDevs.cpp
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


#include <vle/extension/fsa/FDDevs.hpp>

namespace vle { namespace extension { namespace fsa {

void FDDevs::process(const std::string& portname,
                        const devs::Time& time)
{
    if (mExternals.find(currentState()) != mExternals.end() and
        (mExternals[currentState()].find(portname) !=
         mExternals[currentState()].end())) {
        currentState(mExternals[currentState()]
                              [portname]);

        if (mDurations.find(currentState()) == mDurations.end()) {
            throw utils::InternalError(vle::utils::format(
                    "FSA::FDDevs model, unknow duration of state %i",
                    currentState()));
        }

        mSigma = mDurations[currentState()];
    }
    else mSigma -= time - mLastTime;
}

/*  - - - - - - - - - - - - - --ooOoo-- - - - - - - - - - - -  */

void FDDevs::output(devs::Time time,
                       devs::ExternalEventList& output) const
{
    OutputFuncsIterator it = mOutputFuncs.find(currentState());

    if (it != mOutputFuncs.end()) {
        (it->second)(time, output);
    } else {
        OutputsIterator ito = mOutputs.find(currentState());

        if (ito != mOutputs.end()) {
            output.emplace_back(ito->second);
        }
    }
}

devs::Time FDDevs::init(devs::Time /* time */)
{
    if (not isInit()) {
        throw utils::InternalError(
            "FSA::FDDevs model, initial state not defined");
    }

    currentState(initialState());

    if (mDurations.find(currentState()) == mDurations.end()) {
        throw utils::InternalError(vle::utils::format(
                "FSA::FDDevs model, unknow duration of state %i",
                currentState()));
    }

    mSigma = mDurations[currentState()];
    return mSigma;
}

void FDDevs::externalTransition(const devs::ExternalEventList& events,
                                devs::Time time)
{
    if (events.size() > 1) {
        devs::ExternalEventList sortedEvents = select(events);
        devs::ExternalEventList::const_iterator it = sortedEvents.begin();

        while (it != sortedEvents.end()) {
            process(it->getPortName(), time);
            ++it;
        }
    } else {
        devs::ExternalEventList::const_iterator it = events.begin();
        process(it->getPortName(), time);
    }
}

void FDDevs::internalTransition(devs::Time time)
{
    if (mInternals.find(currentState()) == mInternals.end()) {
        throw utils::InternalError(vle::utils::format(
                "FSA::FDDevs model, unknow internal transition on state %i",
                currentState()));
    }

    currentState(mInternals[currentState()]);

    if (mDurations.find(currentState()) == mDurations.end()) {
        throw utils::InternalError(vle::utils::format(
                "FSA::FDDevs model, unknow duration of state %i",
                currentState()));
    }

    mSigma = mDurations[currentState()];
    mLastTime = time;
}

}}} // namespace vle extension fsa
