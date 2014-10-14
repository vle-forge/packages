/*
 * Copyright (C) 2009-2014 INRA
 *
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

#ifndef VLE_RECURSIVE_ABSTRACTEXPE_HPP_
#define VLE_RECURSIVE_ABSTRACTEXPE_HPP_


#include <vle/devs/Dynamics.hpp>
#include <vle/value/Value.hpp>
#include <vle/utils/Exception.hpp>

#include <iostream>


namespace vle {
namespace recursive {

namespace vv = vle::value;
namespace vd = vle::devs;
namespace vu = vle::utils;

/**
 * @brief Abstract Experiment (either a function optimization process or
 * a reinforcement learning process)
 */
class AbstractExpe: public vd::Dynamics
{
private:

    /**
     * @brief Sets the initialization map
     * @param m, the init map to copy
     */
    void setInitializations(const vv::Map& m)
    {
        if(minitialisations){
            delete minitialisations;
        }
        minitialisations = new vv::Map(m);
    }
    /**
     * @brief Build the mode from the initialisation map
     * @param onRequest, if true the experiment
     * cannot be performed at initialization since it comes after.
     */
    void buildMode(bool onRequest)
    {
        if(getInitializations().exist("mode")){
            const vv::Map& mode = getInitializations().getMap("mode");
            if (onRequest){
                mmode.onRequest = true;
            } else {
                if(mode.exist("on-request")){
                    mmode.onRequest = mode.getBoolean("on-request");
                }
            }
            if(mode.exist("step-duration")){
                mmode.stepDuration = mode.getDouble("step-duration");
            }
            if(mode.exist("outputs")){
                const vv::Set& out = mode.getSet("outputs");
                for(unsigned int i = 0; i < out.size(); i++){
                    mmode.outputs.push_back(out.getString(i));
                }
            }
        }
    }
protected:
    /**
     * @brief Mode, the operating mode of the Experiment
     */
    struct Mode{
        /**
         * @brief Mode default constructor
         */
        Mode(): onRequest(true), stepDuration(1.0), outputs()
        {
        }
        /**
         * @brief boolean to true if experiment is required on an
         * external event
         */
        bool onRequest;
        /**
         * @brief the duration of one step (duration of one call of
         * processStep)
         */
        double stepDuration;
        /**
         * @brief the set of observables that should be
         * in output for an external event. If empty no output is sent.
         */
        std::vector<std::string> outputs;
    };
    /**
     * @brief STATE, the state of the Dynamics
     */
    enum STATE {
        BEFORE_EXPE,
        DURING_EXPE,
        JUST_AFTER_EXPE,
        AFTER_EXPE
    };
    /**
     * @brief Initialization of one experiment process
     */
    vv::Map* minitialisations;
    /**
     * @brief The current process step
     */
    vv::Integer mcurrentProcessStep;
    /**
     * @brief The operating mode of the Experiment
     */
    Mode mmode;
    /**
     * @brief The current Dynamics state
     */
    STATE mstate;

    /**
     * @brief Gets initialization map
     * @return the current simulator
     */
    vv::Map& getInitializations()
    {
        if(!minitialisations){
            throw vu::InternalError(vle::fmt(
                "[%1%] AbstractExpe : unexpected empty inits ")
                % getModelName());
        }
        return *minitialisations;
    }


public:
    /**
     * @brief AbstratExpe constructor
     */
    AbstractExpe(const vd::DynamicsInit& init, const vd::InitEventList& events):
        vd::Dynamics(init, events), minitialisations(0), mcurrentProcessStep(0),
        mmode(), mstate(BEFORE_EXPE)
    {
        setInitializations(events);
        buildMode(false);
    }

    /**
     * @brief AbstractExpe destructor
     */
    virtual ~AbstractExpe()
    {
        if(minitialisations){
            delete minitialisations;
        }
    }

    /**
     * @brief Implementation of Dynamics::init
     */
    vd::Time init(const vd::Time& /*time*/)
    {
        if(!mmode.onRequest){
            initExpe();
            if(mmode.stepDuration <= 0){
                bool stopProcess = false;
                while(!stopProcess){
                    mcurrentProcessStep.set(mcurrentProcessStep.value()+1);
                    stopProcess = processStep(mcurrentProcessStep);
                }
                mstate = JUST_AFTER_EXPE;
            } else {
                mstate = DURING_EXPE;
            }
        }
        return timeAdvance();
    }
    /**
     * @brief Implementation of Dynamics::timeAdvance
     */
    vd::Time timeAdvance() const
    {
        switch(mstate){
        case BEFORE_EXPE: {
            return vd::infinity;
            break;
        } case DURING_EXPE: {
            return mmode.stepDuration;
            break;
        } case JUST_AFTER_EXPE: {
            return 0;
            break;
        } case AFTER_EXPE: {
            return vd::infinity;
            break;
        }}
        return 0;
    }
    /**
     * @brief Implementation of Dynamics::internalTransition
     */
    void internalTransition(const vd::Time& /* time */)
    {
        switch(mstate){
        case BEFORE_EXPE: {
            break;
        } case DURING_EXPE: {
            mcurrentProcessStep.set(mcurrentProcessStep.value()+1);
            bool stopProcess = processStep(mcurrentProcessStep);
            if(stopProcess){
                mstate = JUST_AFTER_EXPE;
            }
            break;
        } case JUST_AFTER_EXPE: {
            mstate = AFTER_EXPE;
            break;
        } case AFTER_EXPE: {
            break;
        }}
    }
    /**
     * @brief Implementation of Dynamics::output
     */
    void output(const vd::Time& /* time */, vd::ExternalEventList& output) const
    {
        switch(mstate){
        case BEFORE_EXPE: {
            break;
        } case DURING_EXPE: {
            break;
        } case JUST_AFTER_EXPE: {
            if(mmode.outputs.size() > 0){
                vd::ExternalEvent* ee = new vd::ExternalEvent("expeRes");
                for(unsigned int i =0; i< mmode.outputs.size() ; i++){
                    const std::string& o = mmode.outputs[i];
                    ee->putAttribute(o,observe(o));
                }
                output.push_back(ee);
            }
            break;
        } case AFTER_EXPE: {
            break;
        }}
    }

    /**
     * @brief Implementation of Dynamics::externalTransition
     */
    void externalTransition(const vd::ExternalEventList& event,
        const vd::Time& /* time */)
    {
        switch(mstate){
        case BEFORE_EXPE:
        case AFTER_EXPE: {
            vd::ExternalEventList::const_iterator itb = event.begin();
            vd::ExternalEventList::const_iterator ite = event.end();
            for(;itb!=ite;itb++){
                if ((*itb)->onPort("expeReq")) {
                    setInitializations((*itb)->getAttributes());
                    buildMode(true);
                    initExpe();
                    if(mmode.stepDuration <= 0){
                        bool stopProcess = false;
                        while(!stopProcess){
                            mcurrentProcessStep.set(mcurrentProcessStep.value()+1);
                            stopProcess = processStep(mcurrentProcessStep);
                        }
                        mstate = JUST_AFTER_EXPE;
                    } else {
                        mstate = DURING_EXPE;
                    }
                }
            }
            break;
        } case DURING_EXPE:
          case JUST_AFTER_EXPE: {
            throw vu::ArgError(vle::fmt(
                  "[%1%] AbstractExpe : is already performing an experiment ")
                  % getModelName());
            break;
        }}
    }
    /**
     * @brief Implementation of Dynamics::observation
     */
    vv::Value* observation(const vd::ObservationEvent& event) const
    {
        return observe(event.getPortName());
    }
    /**
     * @brief Abstract method for initializing the experiment
     */
    virtual void initExpe()=0;
    /**
     * @brief Abstract method for a process step
     * @param current_step the current step
     * @return true if the process should stop
     */
    virtual bool processStep(const vv::Integer& current_step)=0;

    /**
     * @brief Abstract method that observe ports
     * @param obs, the port to observe
     * @return the observed values
     */
    virtual vv::Value* observe(const std::string& obs) const = 0;
};


}}// namespaces

#endif
