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

#include <boost/algorithm/string/split.hpp>
#include <boost/algorithm/string/classification.hpp>

#include <vle/devs/Dynamics.hpp>
#include <vle/value/Value.hpp>
#include <vle/utils/Exception.hpp>
#include <vle/recursive/MetaManager.hpp>

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
                    meta(0), minit(0),stepDuration(0.0), mstate(IDLE)
    {
        if (events.exist("id_vpz") and
                not events.getString("id_vpz").empty()){
            minit = (vle::value::Map*) events.clone();
        }
    }

    /**
     * @brief AbstractExpe destructor
     */
    virtual ~MetaManagerDyn()
    {
        if(minit){
            delete minit;
        }
        if(meta){
            delete meta;
        }
    }

    /**
     * @brief Implementation of Dynamics::init
     */
    vd::Time init(const vd::Time& /*time*/)
    {
        if(minit){
            meta = new MetaManager();
            meta->init(*minit);
            meta->launchSimulations();
            mstate = EXPE_LAUNCH;
        } else {
            mstate = IDLE;
        }
        return timeAdvance();
    }
    /**
     * @brief Implementation of Dynamics::timeAdvance
     */
    vd::Time timeAdvance() const
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
    void internalTransition(const vd::Time& /* time */)
    {
        switch(mstate){
        case EXPE_LAUNCH: {
            mstate = IDLE;
            break;
        } case IDLE: {
            throw vu::InternalError(vle::fmt("[%1%] Internal error in "
                    "dynamic (1)") % getModelName());
            break;
        }}
    }
    /**
     * @brief Implementation of Dynamics::output
     */
    void output(const vd::Time& /* time */, vd::ExternalEventList& output) const
    {
        switch(mstate){
        case EXPE_LAUNCH: {
            if (meta and meta->getResults() and
                    getModel().existOutputPort("outputs")) {
                vd::ExternalEvent* ee = new vd::ExternalEvent("outputs");
                ee->putAttribute("inputs", meta->getResults()->clone());
                output.push_back(ee);
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
        const vd::Time& /* time */)
    {
        switch(mstate){
        case EXPE_LAUNCH: {
            throw vu::ModellingError(vle::fmt("[%1%] A simulation of an "
                    "experiment plan is already ongoing") % getModelName());
            break;
        } case IDLE: {
            vd::ExternalEventList::const_iterator itb = event.begin();
            vd::ExternalEventList::const_iterator ite = event.end();
            bool found = false;
            for (; itb != ite or not found; itb++) {
                found =  ((*itb)->getPortName() == "inputs");
            }
            if (found) {
                delete meta;
                delete minit;
                minit = (vle::value::Map*)
                        (*itb)->getAttributeValue("inputs").clone();
                meta = new MetaManager();
                meta->init(*minit);
                meta->launchSimulations();
                mstate = EXPE_LAUNCH;
            }
            break;
        }}
    }

    virtual vle::value::Value* observation(
    const vle::devs::ObservationEvent& event) const
    {
        if (meta and meta->getResults()) {
            if (event.onPort("outputs")) {
                 return meta->getResults()->clone();
            }
            std::string prefix;
            std::string indexes;
            if (!prefix.assign("output_").empty() and
                    !event.getPortName().compare(0, prefix.size(), prefix)) {
                indexes.assign(event.getPortName().substr(prefix.size(),
                        event.getPortName().size()));
                std::vector<std::string>  splitVec;
                boost::split(splitVec, indexes, boost::is_any_of("_"),
                        boost::token_compress_on);
                if (splitVec.size() != 2){
                    return 0;
                }
                int row = atoi(splitVec[0].c_str());
                int col = atoi(splitVec[1].c_str());
                if (not meta->getResults()) {
                    return 0;
                }
                if (not meta->getResults()->get(col,row)) {
                    return 0;
                }
                return meta->getResults()->get(col,row)->clone();
            }
        }
        return 0;
    }

    /**
     * @brief Instance of MetaManager
     */
    vle::recursive::MetaManager* meta;
    /**
     * @brief Initialization of one experiment process
     */
    vv::Map* minit;
    /**
     * @brief the duration of one step (duration of one call of
     * processStep)
     */
    double stepDuration;
    /**
     * @brief State of the PDEVS dynamics
     */
    STATE mstate;

};


}}}// namespaces

DECLARE_DYNAMICS(vle::recursive::models::MetaManagerDyn)

#endif
