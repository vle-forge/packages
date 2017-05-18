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


#ifndef VLE_DISCRETE_TIME_GEN_HPP
#define VLE_DISCRETE_TIME_GEN_HPP 1

#include <vle/devs/Dynamics.hpp>
#include <vle/discrete-time/TemporalValues.hpp>
#include <vle/discrete-time/details/ComputeInterface.hpp>


namespace vle {
namespace discrete_time {

/**
 * @brief DEVS state of the dynamic
 */
enum DEVS_State
{
    INIT,
    WAIT,
    WAIT_SYNC,
    WAIT_BAGS,
    COMPUTE,
    DYN_UPDATE
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
 * @brief Options structure for DisreteTime models
 */
struct  DEVS_Options
{
    //for a given variable: gives the sync parameter
    typedef std::map <std::string, unsigned int> SyncsType;
    //for a given variable: gives the output period parameter
    typedef std::map <std::string, unsigned int> OutputPeriods;
    //for a given variable: gives the type of output, if true, then outputs
    //nill if the last update time is not the current time
    typedef std::map <std::string, bool> OutputNils;
    //for a given variable: gives the set of forcing event
    //a forcing event is a map containing
    //- time: time of forcing event (double)
    //- value: forcing value (either a double or a tuple)
    //- before_output (optionnal): true if event occurs before output
    typedef std::map <std::string, vle::value::Set> ForcingEvents;
    //for a given variable: tells if updates are allowed
    typedef std::map <std::string, bool> AllowUpdates;

    unsigned int bags_to_eat;
    double dt;
    SyncsType syncs;
    OutputPeriods outputPeriods;
    OutputNils outputNils;
    ForcingEvents* forcingEvents;
    AllowUpdates* allowUpdates;

    vle::value::Integer* outputPeriodsGlobal;
    vle::value::Boolean* outputNilsGlobal;
    bool snapshot_before;
    bool snapshot_after;
    bool dyn_allow;
    VAR_TYPE dyn_type;
    unsigned int dyn_sync;
    std::unique_ptr<vle::value::Value> dyn_init_value;
    unsigned int dyn_dim;



    DEVS_Options();
    ~DEVS_Options();

    void
    setGlobalOutputNils(bool type);

    void
    setGlobalOutputPeriods(const std::string& dtd, int period);

    void
    addForcingEvents(const std::string& dtd,
            const vle::value::Value& fe, const std::string& varname);

    void
    addAllowUpdate(bool allowUpdate,  const std::string& varname);

    bool
    shouldOutput(unsigned int currentTimeStep,
            const std::string& varname) const;

    bool
    shouldOutputNil(double lastUpdateTime, double currentTime,
            const std::string& varname) const;

    std::unique_ptr<vle::value::Value>
    getForcingEvent(double currentTime, bool beforeCompute,
            const std::string& varname) const;

    template <class InitializationMap>
    void configDynOptions(const InitializationMap& events)
    {
        if (events.exist("dyn_type")) {
            std::string dyn_type =  events.getString("dyn_type");
            if (dyn_type == "Var") {
                dyn_type = MONO;
            } else if (dyn_type == "Vect") {
                dyn_type = MULTI;
            } else if (dyn_type == "ValueVle") {
                dyn_type = VALUE_VLE;
            } else {
                dyn_type = MONO;
            }
        } else {
            dyn_type = MONO;
        }
        if (events.exist("dyn_sync")) {
            if (events.get("dyn_sync")->isInteger()) {
                dyn_sync = events.getInt("dyn_sync");
            } else {
                dyn_sync = (unsigned int) events.getBoolean("dyn_sync");
            }
        }
        dyn_init_value.reset();
        if (events.exist("dyn_init_value")) {
            dyn_init_value = std::unique_ptr<vle::value::Value>(
                    events.get("dyn_init_value")->clone());
        } else {
            dyn_init_value = std::unique_ptr<vle::value::Value>(
                    new value::Double(0.0));
        }
    }

    //internal function called after user constructor
    void
    finishInitialization(Variables& vars);
};



/**
 * @brief Internal State
 */
struct  DEVS_Internal
{
    bool initialized; //true if initialization is finished
    unsigned bags_eaten;
    double NCt; //next compute time
    double LWUt; //last wake up time
    DEVS_Internal() : initialized(false), bags_eaten(0), NCt(0), LWUt(0)
    {
    }
};

struct Pimpl
{
    TemporalValuesProvider& tvp;
    DEVS_State devs_state;
    DEVS_Options devs_options;
    DEVS_TransitionGuards devs_guards;
    DEVS_Internal devs_internal;
    bool mfirstCompute;
    bool declarationOn;
    unsigned int currentTimeStep;
    ComputeInterface*  devs_atom;

    Pimpl(TemporalValuesProvider& tempvp,
            const vle::devs::InitEventList&  events);
    void time_step(double val);
    void init_value(const std::string& v, const vle::value::Value& val);
    void dim(const std::string& v, unsigned int val);
    void history_size(const std::string& v, unsigned int val);
    void sync(const std::string& v, unsigned int val);
    void output_nil(const std::string& v, bool val);
    void output_period(const std::string& v, unsigned int val);
    void allow_update(const std::string& v, bool val);
    void error_no_sync(const std::string& v, bool val);
    void bags_to_eat(unsigned int val);
    void global_output_nils(bool val);

    double time_step() const;
    unsigned int dim(const Vect& v) const;
    bool firstCompute() const;
    DEVS_Options& getOptions();


    void outputVar(const vle::vpz::AtomicModel& model,
            const vle::devs::Time& time,
            vle::devs::ExternalEventList& output) const;
    void updateGuardAllSynchronized(const vle::devs::Time& t);
    void updateGuardHasSync(const vle::devs::Time& /*t*/);
    void varOnSyncError(std::string& v);
    bool isSync(const std::string& var_name,
            unsigned int currTimeStep) const;
    void initializeFromInitEventList(
            const vle::devs::InitEventList&  events);
    //introspection of input ports to build or remove new variables
    void updateDynState(const vle::devs::Time& t);


    vle::devs::Time
    init(ComputeInterface* atom, const vle::devs::Time& t);

    vle::devs::Time timeAdvance() const;

   void
   internalTransition(const vle::devs::Time& t);

   void
   externalTransition(const vle::devs::ExternalEventList& event,
       const vle::devs::Time& t);

   void
   confluentTransitions(const vle::devs::Time& t,
       const vle::devs::ExternalEventList& event);


   void
   output(const vle::vpz::AtomicModel& model,
           const vle::devs::Time& time,
           vle::devs::ExternalEventList& output) const;

   std::unique_ptr<vle::value::Value>
   observation(
           const vle::devs::ObservationEvent& event) const;

   void
   processIn(const vle::devs::Time& t, DEVS_TransitionType /*trans*/);

   void
   processOut(const vle::devs::Time& t, DEVS_TransitionType /*trans*/);

   void
   updateGuards(const vle::devs::Time& t,
           DEVS_TransitionType /*trans*/);

   void
   handleExtEvt(const vle::devs::Time& t,
           const vle::devs::ExternalEventList& ext);

   void
   handleExtVar(const vle::devs::Time& t,
           const std::string& port, const value::Value& attrs);

};

inline std::ostream&
operator<<(std::ostream& o, const DEVS_TransitionGuards& g)
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
operator<<(std::ostream& o, const DEVS_State& s)
{
    switch(s) {
    case INIT :
        o << "INIT" ;
        break;
    case WAIT :
        o << "WAIT" ;
        break;
    case WAIT_SYNC :
        o << "WAIT_SYNC" ;
        break;
    case WAIT_BAGS :
        o << "WAIT_BAGS" ;
        break;
    case COMPUTE :
        o << "COMPUTE" ;
        break;
    case DYN_UPDATE :
        o << "DYN_UPDATE" ;
        break;
    }
    return o;
}

}} // namespaces

#endif
