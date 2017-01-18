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
#include <vle/utils/Tools.hpp>

#include <vle/devs/Dynamics.hpp>
#include <vle/value/Value.hpp>
#include <vle/value/Table.hpp>
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
                    meta(), stepDuration(0.0), mstate(IDLE), mresults(nullptr)
    {
        if (events.exist("vpz") and
                not events.getString("vpz").empty()){
            mstate = EXPE_LAUNCH;
            wrapper_init init(&events);
            mresults = meta.run(init, merror);
        }
        if (merror.code != 0){
            mresults.reset(nullptr);
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
            throw vu::InternalError(vle::utils::format(
                    "[%s] Internal error in dynamic (1)",
                    getModelName().c_str()));
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
                output.back().attributes().reset(new value::Map(*mresults));
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
            throw vu::ModellingError(vle::utils::format(
                    "[%s] A simulation of an experiment plan is already "
                    "ongoing",  getModelName().c_str()));
            break;
        } case IDLE: {
            vd::ExternalEventList::const_iterator itb = event.begin();
            vd::ExternalEventList::const_iterator ite = event.end();
            bool found = false;
            for (; itb != ite or not found; itb++) {
                found =  (itb->getPortName() == "inputs");
            }
            if (found) {
                merror.code = 0;
                merror.message.clear();
                value::Map& init_map =
                        itb->attributes()->toMap().getMap("inputs");
                wrapper_init init(&init_map);
                mresults = std::move(meta.run(init, merror));
                mresults.reset(nullptr);
                mstate = EXPE_LAUNCH;
            }
            break;
        }}
    }


    /**
     * @brief Observation function
     * @note event port names:
     * - outputs
     * - output_Y: where Y is the id of an output
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
                if (spl.size() == 0){
                    return nullptr;
                }

                std::string outputName(spl[0]);
                if (not mresults->exist(outputName)) {
                    return nullptr;
                }
                const vle::value::Value& r = *mresults->get(outputName);
                if (spl.size() == 1) {
                    //look for output_Y
                    return r.clone();
                } else if (spl.size() == 2) {
                    //look for output_Y_i
                    unsigned int col = std::stoi(spl[1]);
                    if (r.isTable()) {
                        const vle::value::Table& rt = r.toTable();
                        if (rt.width() <= col) {
                            return nullptr;
                        }
                        if (rt.height() == 1) {
                            return value::Double::create(rt.get(col, 0));
                        }
                        std::unique_ptr<vle::value::Tuple> res(
                                new vle::value::Tuple(rt.height()));
                        for (unsigned int i=0; i<rt.height(); i++) {
                            res->value()[i] = rt.get(col, i);
                        }
                        return std::move(res);
                    }
                }
            }
        } else if(merror.code != 0){
            return value::String::create(merror.message);
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
    std::unique_ptr<vle::value::Map> mresults;
    /**
     * @brief Error filled if an error occcured during simulation
     */
    vle::manager::Error merror;

};


}}}// namespaces

DECLARE_DYNAMICS(vle::recursive::models::MetaManagerDyn)

#endif
