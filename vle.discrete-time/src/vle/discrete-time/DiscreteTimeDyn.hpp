/*
 *
 * This file is part of VLE, a framework for multi-modeling, simulation
 * and analysis of complex dynamical systems
 * http://www.vle-project.org
 *
 * Copyright (c) 2013-2013 INRA http://www.inra.fr
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


#ifndef VLE_DISCRETE_TIME_DYN_HPP
#define VLE_DISCRETE_TIME_DYN_HPP 1

#include <vle/devs/Dynamics.hpp>
#include <vle/discrete-time/TemporalValues.hpp>

namespace vle {
namespace discrete_time {





class  DiscreteTimeDyn : public vle::devs::Dynamics,
                         public TemporalValuesProvider

{
public:

    DiscreteTimeDyn(const vle::devs::DynamicsInit& init,
            const vle::devs::InitEventList&  events);

    virtual ~DiscreteTimeDyn();

    virtual void compute(const vle::devs::Time& t) = 0;

    /*  - - - - - - - - - - - - - --ooOoo-- - - - - - - - - - - -  */

    virtual vle::devs::Time init(const vle::devs::Time& time);

    virtual vle::devs::Time timeAdvance() const;

    virtual void internalTransition(
        const vle::devs::Time& time);

    virtual void externalTransition(
        const vle::devs::ExternalEventList& event,
        const vle::devs::Time& time);

    virtual void confluentTransitions(
        const vle::devs::Time& internal,
        const vle::devs::ExternalEventList& extEventlist);

    virtual void output(const vle::devs::Time& /* time */,
                        vle::devs::ExternalEventList& /* output */) const;

    virtual vle::value::Value* observation(
    const vle::devs::ObservationEvent& /* event */) const;

    /*  - - - - - - - - - - - - - --ooOoo-- - - - - - - - - - - -  */
    ////////////////////
    //Internal
    ////////////////////


    /**
     * @brief DEVS state of the dynamic
     */
    enum DEVS_State
    {
        INIT,
        WAIT,
        WAIT_SYNC,
        WAIT_BAGS,
        COMPUTE
    };

    /**
     * @brief Types of DEVS transitions
     */
    enum DEVS_TransitionType
    {
        INTERNAL, EXTERNAL, CONFLUENT
    };

    /**
     * @brief Guards structure for transition
     * between DEVS states
     */
    struct DEVS_TransitionGuards
    {
        bool has_sync;
        bool all_synchronized;
        bool bags_to_eat_eq_0;
        bool bags_eaten_eq_bags_to_eat;
        bool LWUt_sup_NCt;
        bool LWUt_eq_NCt;
        DEVS_TransitionGuards();
    };



    /**
     * @brief Options structure for
     * DifferenceEquation models
     */
    struct  DEVS_Options
    {
        typedef std::map <std::string, unsigned int> SyncsType;

        unsigned int bags_to_eat;
        double dt;
        SyncsType syncs;

        DEVS_Options();
        ~DEVS_Options();

    };

    /**
     * @brief Internal State
     */
    struct  DEVS_Internal
    {
        unsigned bags_eaten;
        double NCt; //next compute time
        double LWUt; //last wake up time
        DEVS_Internal() : bags_eaten(0), NCt(0), LWUt(0)
        {
        }
    };

    /**
     * @brief Process method used for DEVS state entrance
     * @param t, the current time
     * @param trans, the type of the transition
     */
    void processIn(const vle::devs::Time& t, DEVS_TransitionType trans);

    /**
     * @brief Process method used for DEVS state exit
     * @param t, the current time
     * @param trans, the type of the transition
     */
    void processOut(const vle::devs::Time& t, DEVS_TransitionType trans);

    /**
     * @brief Update guards for internal transition
     * @param t, the current time
     * @param trans, the type of the transition
     */
    void updateGuards(const vle::devs::Time& t, DEVS_TransitionType trans);

    /**
     * @brief Handles the list of external even
     * @param t, the current time
     * @param ext, the list of external event
     */
    virtual void handleExtEvt(const vle::devs::Time& t,
            const vle::devs::ExternalEventList& ext);

    /**
     * @brief Handles one external even
     * @param t, the current time
     * @param port, the name of the port
     * @param attr, a map of attributes
     */
    virtual void handleExtEvt(const vle::devs::Time& t,
            const std::string& port, const vle::value::Map& attrs);

    /*  - - - - - - - - - - - - - --ooOoo-- - - - - - - - - - - -  */
    ////////////////////
    //Internal
    ////////////////////
    virtual void outputVar(const vle::devs::Time& time,
            vle::devs::ExternalEventList& output) const;
    void updateGuardAllSynchronized(const vle::devs::Time& t);
    void updateGuardHasSync(const vle::devs::Time& t);
    void varOnSyncError(std::string& v);
    bool isSync(const std::string& var_name, unsigned int currTimeStep) const;


    DEVS_State devs_state;
    DEVS_Options devs_options;
    DEVS_TransitionGuards devs_guards;
    DEVS_Internal devs_internal;

    bool declarationOn;
    unsigned int currentTimeStep;
};


inline std::ostream&
operator<<(std::ostream& o, const DiscreteTimeDyn::DEVS_TransitionGuards& g)
{
    o << " g.LWUt_eq_NCt:" << g.LWUt_eq_NCt << "\n";
    o << " g.LWUt_sup_NCt:" << g.LWUt_sup_NCt << "\n";
    o << " g.all_synchronized:" << g.all_synchronized << "\n";
    o << " g.bags_eaten_eq_bags_to_eat:" << g.bags_eaten_eq_bags_to_eat << "\n";
    o << " g.bags_to_eat_eq_0:" << g.bags_to_eat_eq_0 << "\n";
    o << " g.has_sync:" << g.has_sync << "\n";
    return o;
}


inline std::ostream&
operator<<(std::ostream& o, const DiscreteTimeDyn::DEVS_State& s)
{
    switch(s) {
    case DiscreteTimeDyn::INIT :
        o << "INIT" ;
        break;
    case DiscreteTimeDyn::WAIT :
        o << "WAIT" ;
        break;
    case DiscreteTimeDyn::WAIT_SYNC :
        o << "WAIT_SYNC" ;
        break;
    case DiscreteTimeDyn::WAIT_BAGS :
        o << "WAIT_BAGS" ;
        break;
    case DiscreteTimeDyn::COMPUTE :
        o << "COMPUTE" ;
        break;
    }
    return o;
}

}} // namespaces

#endif
