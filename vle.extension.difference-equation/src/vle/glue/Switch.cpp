/*
 * @file vle/glue/Switch.cpp
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
 * Copyright (C) 2009 INRA
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
#include <vle/devs/DynamicsDbg.hpp>

namespace glue {

class Switch : public vle::devs::Dynamics
{
public:
    Switch(const vle::devs::DynamicsInit& init,
           const vle::devs::InitEventList& events) :
        vle::devs::Dynamics(init, events)
    {
    }

    virtual ~Switch()
    { }

    void clear()
    {
        for (std::map < std::string, vle::value::Value* >
             ::const_iterator it = mValues.begin();
             it != mValues.end(); ++it) {
            delete it->second;
        }
        mValues.clear();
    }


    virtual void finish()
    { clear(); }

    virtual void confluentTransitions(
        const vle::devs::Time& time,
        const vle::devs::ExternalEventList& extEventlist)
    {
        externalTransition(extEventlist, time);
        internalTransition(time);
    }

    virtual vle::devs::Time init(const vle::devs::Time& /* time */)
    {
        mPhase = WAIT;
        return 0;
    }

    virtual vle::devs::Time timeAdvance() const
    {
        if (mPhase == WAIT) {
            return vle::devs::Time::infinity;
        } else {
            return 0;
        }
    }

    virtual void output(const vle::devs::Time& /* time */,
                        vle::devs::ExternalEventList& outputs) const
    {
        if (mPhase == SEND) {
            for (std::map < std::string, vle::value::Value* >
                 ::const_iterator it = mValues.begin();
                 it != mValues.end(); ++it) {
                vle::devs::ExternalEvent* ee =
                    new vle::devs::ExternalEvent(it->first);

                ee << vle::devs::attribute("name", it->first);
                ee << vle::devs::attribute("value", it->second->clone());
                outputs.push_back(ee);
            }
        }
    }

    virtual void internalTransition(const vle::devs::Time& /* time */)
    {
        if (mPhase == SEND) {
            clear();
            mPhase = WAIT;
        }
    }

    virtual void externalTransition(const vle::devs::ExternalEventList& events,
                                    const vle::devs::Time& /* time */)
    {
        vle::devs::ExternalEventList::const_iterator it = events.begin();

        while (it != events.end()) {
            if ((*it)->getPortName() == "in") {
                mValues[(*it)->getStringAttributeValue("name")] =
                    (*it)->getAttributeValue("value").clone();
                mPhase = SEND;
            }
            ++it;
        }
    }

    virtual vle::value::Value* observation(
        const vle::devs::ObservationEvent& /* event */) const
    {
        return 0;
    }

    enum Phase { WAIT, SEND };

    // state
    Phase mPhase;
    vle::devs::Time mSigma;
    vle::devs::Time mLastTime;
    std::map < std::string, vle::value::Value* > mValues;
};

} // namespace glue

DECLARE_DYNAMICS(Switch, glue::Switch)
DECLARE_DYNAMICS_DBG(SwitchDynamicsDbg, glue::Switch)
