/*
 * @file vle/glue/Scale.cpp
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
 * Copyright (C) 2009-2011 INRA
 * Copyright (C) 2009-2011 ULCO http://www.univ-littoral.fr
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

namespace glue { namespace scale {

class Base : public vle::devs::Dynamics
{
public:
    Base(const vle::devs::DynamicsInit& init,
         const vle::devs::InitEventList& events) :
        vle::devs::Dynamics(init, events)
    {
        dbgLog = (events.exist("dbgLog")) ? vle::value::toDouble(events.get("dbgLog")) : 0;

        mInputTimeStep = vle::value::toDouble(events.get("InputTimeStep"));
        mOutputTimeStep = vle::value::toDouble(events.get("OutputTimeStep"));
        if (mInputTimeStep < mOutputTimeStep) {
            mChangeType = UP;
        } else if (mInputTimeStep == mOutputTimeStep) {
            mChangeType = NONE;
        } else {
            mChangeType = DOWN;
        }
    }

    virtual ~Base() { }

    /*  - - - - - - - - - - - - - --ooOoo-- - - - - - - - - - - -  */

    virtual void clearValues(const vle::devs::Time& time) = 0;

    virtual double getValue(const std::string& name) const = 0;

    virtual bool isReceivedValue(const vle::devs::Time& time,
                                 const std::string& name) = 0;

    virtual void processValue(const vle::devs::Time& time,
                              const std::string& name,
                              double value) = 0;

    /*  - - - - - - - - - - - - - --ooOoo-- - - - - - - - - - - -  */

    virtual vle::devs::Time init(const vle::devs::Time& time)
    {
        mVariableNumber = getModel().getInputPortNumber();
        mReceived = 0;
        mLastTime = time;
        mLastReceivedTime = time;
        mPhase = WAITING;
        mInSigma = 0;
        mOutSigma = 0;
        return vle::devs::Time::infinity;
    }

    virtual void output(const vle::devs::Time& time,
                        vle::devs::ExternalEventList& output) const
    {
        if (mPhase == SEND) {
            if (dbgLog >= 2) {
                std::cout << "OUTPUT at " << time << std::endl;
            }

            Names::const_iterator it = mNames.begin();

            while (it != mNames.end()) {
                vle::devs::ExternalEvent* ee =
                    new vle::devs::ExternalEvent(it->first);

                ee << vle::devs::attribute("name", it->second);
                ee << vle::devs::attribute("value", getValue(it->first));
                output.push_back(ee);
                ++it;
            }
        }
    }

    virtual vle::devs::Time timeAdvance() const
    {
        if (mPhase == WAITING) {
            return vle::devs::Time::infinity;
        } else if (mPhase == STEP) {
            return mOutSigma;
        } else if (mPhase == SEND or mPhase == POST) {
            return 0;
        }
        return vle::devs::Time::infinity;
    }

    virtual void internalTransition(const vle::devs::Time& time)
    {
        if (dbgLog>=2) {
            std::cout << "BEGIN internalTransition at " << time << std::endl;
            std::cout << " |   phase    = " << mPhase << std::endl;
            std::cout << " |   inSigma  = " << mInSigma << std::endl;
            std::cout << " |   outSigma = " << mOutSigma << std::endl;
        }
        mInSigma -= time - mLastTime;
        mOutSigma -= time - mLastTime;
        mLastTime = time;

        if (mPhase == STEP) {
            if (mChangeType == UP) {
                if (mInSigma < 1.e-10) {
                    mPhase = WAITING;
                } else {
                    mLastReceivedTime = time;
                    mPhase = SEND;
                }
            } else if (mChangeType == DOWN) {
                if (mInSigma < 1.e-10) {
                    mPhase = WAITING;
                } else {
                    mPhase = SEND;
                }
            } else {
                mPhase = WAITING;
            }
        } else if (mPhase == POST) {
            mPhase = SEND;
        } else if (mPhase == SEND) {
            mOutSigma = mOutputTimeStep;
            mPhase = STEP;
        }

        if (dbgLog>=2) {
            std::cout << "END internalTransition" << std::endl;
            std::cout << " |   phase    = " << mPhase << std::endl;
            std::cout << " |   inSigma  = " << mInSigma << std::endl;
            std::cout << " |   outSigma = " << mOutSigma << std::endl;
        }
    }

    virtual void externalTransition(
        const vle::devs::ExternalEventList& events,
        const vle::devs::Time& time)
    {
        if (dbgLog>=2) {
            std::cout << "BEGIN externalTransition at " << time << std::endl;
            std::cout << " |   phase    = " << mPhase << std::endl;
            std::cout << " |   inSigma  = " << mInSigma << std::endl;
            std::cout << " |   outSigma = " << mOutSigma << std::endl;
        }
        vle::devs::ExternalEventList::const_iterator it = events.begin();

        if (mLastReceivedTime < time) {
            clearValues(time);
            mReceived = 0;
        }
        while (it != events.end()) {
            std::string name = (*it)->getPortName();
            double value = (*it)->getDoubleAttributeValue("value");

            if (mNames.find(name) == mNames.end() or
                isReceivedValue(time, name)) {
                ++mReceived;
            }

            if (dbgLog>=2) {
                std::cout << " |   " << name << " = " << value << std::endl;
            }

            if (mNames.find(name) == mNames.end()) {
                mNames[name] = (*it)->getStringAttributeValue("name");
            }
            processValue(time, name, value);
            ++it;
        }

        mInSigma -= time - mLastTime;
        mOutSigma -= time - mLastTime;
        mLastTime = time;

        if (mLastReceivedTime < time) {
            mLastReceivedTime = time;
        }

        if (mReceived == mVariableNumber) {
            mInSigma = mInputTimeStep;
            if (mPhase == WAITING) {
                mPhase = POST;
            }
        }

        if (dbgLog>=2) {
            std::cout << "END externalTransition " << std::endl;
            std::cout << " |   phase    = " << mPhase << std::endl;
            std::cout << " |   inSigma  = " << mInSigma << std::endl;
            std::cout << " |   outSigma = " << mOutSigma << std::endl;
        }
    }

    virtual void confluentTransitions(
        const vle::devs::Time& time,
        const vle::devs::ExternalEventList& extEventlist)
    {
        internalTransition(time);
        externalTransition(extEventlist, time);
    }

    virtual vle::value::Value* observation(
        const vle::devs::ObservationEvent& /* event */) const
    { return 0; }

private:
    typedef std::map < std::string, std::string > Names;

    enum ChangeType { UP, DOWN, NONE };
    enum Phase { STEP /* 0 */,
        WAITING /* 1 */,
        SEND /* 2 */,
        POST /* 3 */};

    // parameters
    vle::devs::Time mInputTimeStep;
    vle::devs::Time mOutputTimeStep;
    ChangeType mChangeType;
    unsigned int mVariableNumber;

    // state
    Phase mPhase;
    vle::devs::Time mOutSigma;
    vle::devs::Time mInSigma;
    Names mNames;
    unsigned int mReceived;
    vle::devs::Time mLastTime;
    vle::devs::Time mLastReceivedTime;

protected:
    double dbgLog;
    typedef std::pair < vle::devs::Time, double > Value;
    typedef std::map < std::string, Value > Values;
    Values mValues;
};

class Constant : public Base
{
public:
    Constant(const vle::devs::DynamicsInit& init,
             const vle::devs::InitEventList& events) :
        Base(init, events)
    { }

    virtual ~Constant() { }

    /*  - - - - - - - - - - - - - --ooOoo-- - - - - - - - - - - -  */

    virtual void clearValues(const vle::devs::Time& /* time */)
    { }

    virtual double getValue(const std::string& name) const
    { return mValues.find(name)->second.second; }

    virtual bool isReceivedValue(const vle::devs::Time& time,
                                 const std::string& name)
    { return mValues[name].first < time; }

    virtual void processValue(const vle::devs::Time& time,
                              const std::string& name,
                              double value)
    {
        mValues[name] = std::make_pair(time, value);
        if (Base::dbgLog>=1) {
            std::cout << "ConstantScale:\t" <<  time << "\tName: "<< name << "\tvalue: "<< value << "\tnewValue: "<< mValues[name].second << std::endl;
        }
    }

};

class Integrative : public Base
{
public:
    Integrative(const vle::devs::DynamicsInit& init,
         const vle::devs::InitEventList& events) :
        Base(init, events)
    { }

    virtual ~Integrative() { }

    /*  - - - - - - - - - - - - - --ooOoo-- - - - - - - - - - - -  */

    virtual void clearValues(const vle::devs::Time& /* time */)
    { }

    virtual double getValue(const std::string& name) const
    { return mValues.find(name)->second.second; }

    virtual bool isReceivedValue(const vle::devs::Time& time,
                                 const std::string& name)
    { return mValues[name].first < time; }

    virtual void processValue(const vle::devs::Time&  time ,
                              const std::string& name,
                              double value)
    {
        mValues[name].second += value;
        if (Base::dbgLog>=1) {
            std::cout << "IntegrativeScale:\t" << time << "\tName: "<< name << "\tvalue: "<< value << "\tnewValue: "<< mValues[name].second << std::endl;
        }

    }

};

class Mean : public Base
{
public:
    Mean(const vle::devs::DynamicsInit& init,
         const vle::devs::InitEventList& events) :
        Base(init, events)
    { }

    virtual ~Mean() { }

    /*  - - - - - - - - - - - - - --ooOoo-- - - - - - - - - - - -  */

    virtual void clearValues(const vle::devs::Time& /* time */)
    { }

    virtual double getValue(const std::string& name) const
    { return mValues.find(name)->second.second; }

    virtual bool isReceivedValue(const vle::devs::Time& time,
                                 const std::string& name)
    { return mValues[name].first < time; }

    virtual void processValue(const vle::devs::Time&  time ,
                              const std::string& name,
                              double value)
    {
        nReceivedValues[name] ++;
        mValues[name].second = (mValues[name].second * (nReceivedValues[name]-1) + value) / nReceivedValues[name];
        if (Base::dbgLog>=1) {
            std::cout << "MeanScale:\t" << time << "\tName: "<< name << "\tnReceivedValues[name]: "<< nReceivedValues[name] << "\tvalue: "<< value << "\tnewValue: "<< mValues[name].second << std::endl;
        }
    }

private:
    std::map < std::string, int > nReceivedValues;
};

}} // namespace glue scale

DECLARE_DYNAMICS(ConstantScale, glue::scale::Constant)
DECLARE_DYNAMICS(MeanScale, glue::scale::Mean)
DECLARE_DYNAMICS(IntegrativeScale, glue::scale::Integrative)
DECLARE_DYNAMICS_DBG(ConstantScaleDynamicsDbg, glue::scale::Constant)
DECLARE_DYNAMICS_DBG(MeanScaleDynamicsDbg, glue::scale::Mean)
DECLARE_DYNAMICS_DBG(IntegrativeScaleDynamicsDbg, glue::scale::Integrative)
