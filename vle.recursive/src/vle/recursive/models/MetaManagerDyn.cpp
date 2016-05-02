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

#ifndef VLE_RECURSIVE_MODELS_METAMANAGERDYN_HPP_
#define VLE_RECURSIVE_MODELS_METAMANAGERDYN_HPP_

#include <string>
#include <boost/format.hpp>

#include <vle/devs/Dynamics.hpp>
#include <vle/value/Value.hpp>
#include <vle/utils/Exception.hpp>
#include <vle/recursive/MetaManager.hpp>
#include <iostream>

namespace vle {
namespace recursive {
namespace models {

namespace vv = vle::value;
namespace vd = vle::devs;
namespace vu = vle::utils;

/**
 * @brief A dynamic for launchinf multi simulation based on MetaManager API
 */
class MetaManagerDyn: public vd::Dynamics
{
public:
    /**
     * @brief STATE, the state of the Dynamics
     */
    enum STATE {
        EXPE_LAUNCH,
        IDLE
    };

    /**
     * @brief MetaManagerDyn constructor
     */
    MetaManagerDyn(const vd::DynamicsInit& init,
            const vd::InitEventList& events): vd::Dynamics(init,events),
                    meta(), stepDuration(0.0), mstate(IDLE), mresults(nullptr)
    {
        if (events.exist("vpz") and
                not events.getString("vpz").empty()){
            mstate = EXPE_LAUNCH;
            mresults = meta.run(events);

        }
    }

    /**
     * @brief AbstractExpe destructor
     */
    virtual ~MetaManagerDyn()
    {
    }

    /**
     * @brief Implementation of Dynamics::init
     */
    vd::Time init(vd::Time /*time*/) override
    {
        return timeAdvance();
    }
    /**
     * @brief Implementation of Dynamics::timeAdvance
     */
    vd::Time timeAdvance() const override
    {
        switch(mstate){
        case EXPE_LAUNCH: {
            return stepDuration;
            break;
        } case IDLE: {
            return vd::infinity;
            break;
        }}
        return 0;
    }
    /**
     * @brief Implementation of Dynamics::internalTransition
     */
    void internalTransition(vd::Time /* time */) override
    {
        switch(mstate){
        case EXPE_LAUNCH: {
            mstate = IDLE;
            break;
        } case IDLE: {
            throw vu::InternalError((boost::format("[%1%] Internal error in "
                    "dynamic (1)") % getModelName()).str());
            break;
        }}
    }
    /**
     * @brief Implementation of Dynamics::output
     */
    void output(vd::Time /*t*/, vd::ExternalEventList& output) const override
    {
        switch(mstate){
        case EXPE_LAUNCH: {
            if (mresults and getModel().existOutputPort("outputs")) {
                output.emplace_back("outputs");
                output.back().attributes().reset(new value::Matrix(*mresults));
            }
            break;
        } case IDLE: {
            break;
        }}
    }

    /**
     * @brief Implementation of Dynamics::externalTransition
     */
    void externalTransition(const vd::ExternalEventList& event,
        vd::Time /* time */) override
    {
        switch(mstate){
        case EXPE_LAUNCH: {
            throw vu::ModellingError((boost::format("[%1%] A simulation of an "
                    "experiment plan is already ongoing") % getModelName()).str());
            break;
        } case IDLE: {
            vd::ExternalEventList::const_iterator itb = event.begin();
            vd::ExternalEventList::const_iterator ite = event.end();
            bool found = false;
            for (; itb != ite or not found; itb++) {
                found =  (itb->getPortName() == "inputs");
            }
            if (found) {
                mresults = std::move(meta.run(
                        itb->attributes()->toMap().getMap("inputs")));
                mstate = EXPE_LAUNCH;
            }
            break;
        }}
    }


    /**
     * @brief Observation function
     * @note event port names:
     * - outputs
     * - output_Y_i: where Y is the id of an output and i is the index of
     * simulation input
     */
    virtual std::unique_ptr<vle::value::Value> observation(
    const vle::devs::ObservationEvent& event) const override
    {
        if (mresults) {
            std::string portName = event.getPortName();
            if (portName == "outputs") {
                 return mresults->clone();
            }
            std::string prefix;
            std::string indexes;
            if (!prefix.assign("output_").empty() and
                    !portName.compare(0, prefix.size(), prefix)) {
                indexes.assign(portName.substr(prefix.size(),
                        portName.size()));
                std::vector<std::string>  spl;
                MetaManager::split(spl, indexes, '_');
                if (spl.size() != 2){
                    return 0;
                }
                std::string colname = spl[0];
                int col = -1;
                for (unsigned int c = 0; c < mresults->columns(); c++) {
                    if (mresults->get(c,0)->toString().value() == colname) {
                        col = c;
                    }
                }
                if (col == -1) {
                    return 0;
                }
                int row = std::stoi(spl[1]);
                if (not mresults->get(col,row)) {
                    return 0;
                }
                return mresults->get(col,row)->clone();
            }
        }
        return nullptr;
    }

    /**
     * @brief Instance of MetaManager
     */
    vle::recursive::MetaManager meta;
    /**
     * @brief the duration of one step (duration of one call of
     * processStep)
     */
    double stepDuration;
    /**
     * @brief State of the PDEVS dynamics
     */
    STATE mstate;

    /**
     * @brief Results of simulations
     */
    std::unique_ptr<vle::value::Matrix> mresults;

};


}}}// namespaces

DECLARE_DYNAMICS(vle::recursive::models::MetaManagerDyn)

#endif
