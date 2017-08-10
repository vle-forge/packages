/*
 * This file is part of VLE, a framework for multi-modeling, simulation
 * and analysis of complex dynamical systems
 * http://www.vle-project.org
 *
 * Copyright (c) 2014-2014 INRA http://www.inra.fr
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

#include <AgentDT.hpp>
#include <vle/utils/DateTime.hpp>
#include <vle/utils/Package.hpp>
#include <vle/utils/Context.hpp>
#include <vle/value/Set.hpp>
#include <vle/value/Map.hpp>

#include <boost/algorithm/string/split.hpp>
#include <boost/algorithm/string.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/format.hpp>

#include <sstream>
#include <numeric>
#include <iterator>
#include <iostream>
#include <fstream>
#include <iomanip>

// raccourcis de nommage des namespaces frequement utilises
namespace vd = vle::devs;
namespace vv = vle::value;
namespace ved = vle::extension::decision;
namespace vdd = vle::discrete_time::decision;
namespace vu = vle::utils;

namespace vle {
namespace discrete_time {
namespace decision {

typedef std::vector < std::string > strings_t;
typedef strings_t::iterator strings_it;

/**
 * @brief provide a agent with a behaviour totally defined by plans
 * and conditions
 *
 * Expected to start at zero.
 *
 * port parameters:
 * - Rotation (map) (madatory): to specify the plans to load
 * - Resource (map) : to specify the resources of the agent
 * - begin (double) (mandatory) : set to 0 (mandatory value by convention)
 * - begin_date (string) (mandatory) : "YYYY-MM-DD" the human date of
 * simulation start
 * - dyn_allow (boolean) (mandatory) : set to true (mandatory value by convention)
 *   enables an automatic management of variables
 * - PlansLocation (string) : the package name where to find plan files
 *
 * The rotation map stores define where an when to load plans
 * The key of the map is used as a location.
 * The set stored stores a period of rotations (in year)
 * and pairs of relative year indexes and plan to load.
 * Example ("p1" (5 (wheat 1) (pea 1) (corn 3)) :
 * it means that the period last 5 years, that the wheat plan,
 * and the pea plan are loaded the first year, the sixth year, and so
 * on.
 * Nothing hapen o, year 2,4,5,7,9,10...
 * The corn plan is loaded on years 3,8,13,...
 *
 * The Resource map if needed define ressources.
 * The key of the map is used as suffix id of a ressource
 * Inside the stored set we stores the quantity of resources
 * and the categories.
 * Example ("tractor" (2, "big", "red"))("man" (1, "boss")):
 * it means that there is 3 resources:
 * - tractor1 is "big" and "red"
 * - tractor2 is "big" and "red"
 * - man1 is "boss"
 *
 */
class AgentDTG: public vdd::AgentDT
{
public:

AgentDTG(const vd::DynamicsInit& mdl, const vd::InitEventList& evts) :
    vdd::AgentDT(mdl, evts), firstLoad(true), mPlansLocation()
{
    if (evts.exist("PlansLocation")) {
        mPlansLocation = evts.getString("PlansLocation");
    }

    if (evts.exist("Rotation")) {
        mRotationP = evts.getMap("Rotation").clone();
    } else {
        throw vle::utils::ModellingError("Parameter Rotation not found");
    }

    vv::MapValue::const_iterator it;
    for (it = mRotationP->toMap().begin(); it != mRotationP->toMap().end(); it++) {
        mPlanPerLocationCounter[it->first] = 0;
    }

    if (evts.exist("Resource")) {
        mResourceP = evts.getMap("Resource").clone();
        fillResources();
    }

    addPredicates(this) +=
        P("GPred", &AgentDTG::GPred);

    addOutputFunctions(this) +=
        O("GOut", &AgentDTG::GOut);

    addUpdateFunctions(this) +=
        U("GUpdate", &AgentDTG::GUpdate);
}

virtual ~AgentDTG() {}

/**
 * @brief fill the resources inside the knowledge base
 *
 */
void
fillResources ()
{
    vv::MapValue::const_iterator it;
    for (it = mResourceP->toMap().begin(); it != mResourceP->toMap().end(); it++) {
        vv::Set ress = mResourceP->toMap().getSet(it->first);
        uint nbres = ress.getInt(0);
        for(uint i = 0; i < nbres; i++) {
            std::string resId = (it->first) + boost::lexical_cast<std::string>(i) ;
            for(uint j = 1; j < ress.size(); j++) {
                    addResources(ress.getString(j), resId);
            }
        }
    }
}

/**
 * @brief provide the "system" activity mnaging plans
 *
 */
vd::Time
init(vd::Time t)
{
    mStartTime = t;


    ved::Activity& a = addActivity("plan");

    a.addOutputFunction(
        boost::bind(&AgentDTG::out_plan,
                    this, _1, _2, _3));
    a.addAcknowledgeFunction(
        boost::bind(&AgentDTG::ack_plan,
                    this, _1, _2));
    a.addUpdateFunction(
        boost::bind(&AgentDTG::loadPlan,
                    this, _1, _2));

    ved::Rule& r1 = addRule("rSecondDayOfYear");
    ved::Rule& r2 = addRule("rForceFirstLoad");

    r1.add(
        boost::bind(&AgentDTG::pSecondDayOfYear,
                    this, _1, _2, _3));

    r2.add(
        boost::bind(&AgentDTG::pForceFirstLoad,
                    this, _1, _2, _3));

    a.addRule("rSecondDayOfYear", r1);
    a.addRule("rForceFirstLoad", r2);

    return AgentDT::init(t);
}

/**
 * @brief provide the number of years since the begining of simulation
 *
 */
int
getCurrentYear() const
{
    return vu::DateTime::year(current_date) -
        vu::DateTime::year(begin_date) + 1;
}

/**
 * @brief retrieve the location name from the name of the activity
 *
 * the location name should be after the @
 *
 * exemple sowing@p1:5 means this the fifth sowing activity
 * dedicated to the p1 location
 *
 */
std::string
getLocationName(const std::string activity) const
{
    strings_t lst;
    boost::split(lst, activity, boost::is_any_of("@:"));
    if (lst.size() <= 1) {
        return {};
    } else {
        return lst[1];
    }
}

/**
 * @brief retrieve the name the port from the plan activity
 *
 * the location name should be before #
 *
 */
std::string
getPortName(const std::string activity) const
{
    strings_t lst;
    boost::split(lst, activity, boost::is_any_of("#:"));
    if (lst.empty()) {
        return activity;
    } else {
        return lst[0];
   }
}

/**
 * @brief retrieve the length of the period for the plan of a location
 *
 *
 */
int getNbYear(std::string location) const
{
    vv::Set rot = mRotationP->toMap().getSet(location);
    return rot.getInt(0);
}

/**
 * @brief retrieve the list og plans to load for the current year
 *
 *
 */
strings_t
getPlansOfTheYear(std::string location) const
{
    strings_t plans;
    vv::Set rot = mRotationP->toMap().getSet(location);
    int yearOfLocation = getYearOfLocation(location);
    for(uint i = 1; i < rot.size(); i++) {
        int year = rot[i]->toSet().getInt(0);
        if (year == yearOfLocation) {
            plans.push_back(rot[i]->toSet().getString(1));
        }
    }
    return plans;
}

/**
 * @brief retrieve the relative current year of a location depending
 * on the period.
 *
 *
 */
int
getYearOfLocation(std::string location) const
    {
        vv::MapValue::const_iterator it = mRotationP->toMap().find(location);

    return ((getCurrentYear() + getNbYear(it->first) - 1 ) % getNbYear(it->first)) + 1;
}

/**
 * @brief get the prefix name of the activity
 *
 * used by the plan activities
 *
 *
 */
std::string
getPrefixName(const std::string activity) const {
    strings_t lst;
    boost::split(lst, activity, boost::is_any_of("#"));
    if (lst.empty()) {
        return activity;
    } else {
       return lst[0];
    }
}

/**
 * @brief get the prefix of a param name
 *
 * used by to manage if values to send needs to be located
 *
 *
 */
std::string
getParamName(const std::string activity) const {
    strings_t lst;
    boost::split(lst, activity, boost::is_any_of("@"));
    if (lst.empty()) {
        return activity;
    } else {
       return lst[0];
    }
}

/**
 * @brief compute a counter to insert inside the name of a activity
 *
 *
 */
std::string
getSuffixName(int n) const {
   std::stringstream ret("#");
   ret << boost::format("%1$02d") % n;
   return ret.str();
}

/**
 * @brief provide a kind of generic predicate
 *
 * All the information to evaluate the predicate
 * are got from the parameters of the predicate.
 *
 * The parameters expected are:
 * "_op" a string parameters that shoud be "=="| "<"|
 * ">"|"<="|">="|"!="
 * _opLeft a string or a double
 * _opRight a string or a double
 * _opLeftType a string defining the type of of operand "Var"|"Var@"|"Par"|"Par@"|"Val"
 * _opRightType idem
 *
 * TODO: managing model parameters and string values
 * better managment of double equality
 */
bool
GPred(const std::string& activity,
      const std::string& /* rule */,
      const ved::PredicateParameters& param) const {

    std::string op;
    std::string leftOpS;
    std::string rightOpS;
    double leftOpD;
    double rightOpD;
    bool leftDouble;
    bool rightDouble;

    if (param.exist("_op")) {
        op = param.getString("_op");
    } else {
        throw vle::utils::ModellingError("Operator missing");
    }

    std::string locationName = getLocationName(activity);
    std::string varSuffix;

    if (locationName.empty()) {
        varSuffix = {};
    } else {
        varSuffix = "@" + locationName;
    }

    if (param.exist("_opLeft")) {
        if (param.exist("_opLeftType") &&
            param.getString("_opLeftType") == "Val") {
            leftOpD = param.getDouble("_opLeft");
            leftDouble = true;
        } else if (param.exist("_opLeftType") &&
                   param.getString("_opLeftType") == "Var") {
            Variables::const_iterator itv =
                getVariables().find(param.getString("_opLeft"));
            VarMono* v = (VarMono*) itv->second;
            leftOpD = v->getVal(current_date, 0.0);
            leftDouble = true;
        } else if (param.exist("_opLeftType") &&
                   param.getString("_opLeftType") == "Var@") {
            if (locationName.empty()) {
                throw vle::utils::ModellingError("Location missing");
            }
            Variables::const_iterator itv =
                getVariables().find(param.getString("_opLeft") + varSuffix);
            VarMono* v = (VarMono*) itv->second;
            leftOpD = v->getVal(current_date, 0.0);
            leftDouble = true;
        } else {
            throw vle::utils::ModellingError("Bad left operand");
        }
    } else {
        throw vle::utils::ModellingError("Left operand missing");
    }

    if (param.exist("_opRight")) {
        if (param.exist("_opRightType") &&
            param.getString("_opRightType") == "Val"){
            rightOpD = param.getDouble("_opRight");
            rightDouble = true;
        } else if (param.exist("_opRightType") &&
                  param.getString("_opRightType") == "Var") {
            Variables::const_iterator itv =
                getVariables().find(param.getString("_opRight"));
            VarMono* v = (VarMono*) itv->second;
            rightOpD = v->getVal(current_date, 0.0);
            rightDouble = true;
        } else if (param.exist("_opRightType") &&
                   param.getString("_opRightType") == "Var@") {
            if (locationName.empty()) {
                throw vle::utils::ModellingError("Location missing");
            }
            Variables::const_iterator itv =
                getVariables().find(param.getString("_opRight") + varSuffix);
            VarMono* v = (VarMono*) itv->second;
            rightOpD = v->getVal(current_date, 0.0);
            rightDouble = true;
        } else {
               throw vle::utils::ModellingError("Bad right operand");
        }
    } else {
        throw vle::utils::ModellingError("Left operand missing");
    }

    if(leftDouble != rightDouble) {
        throw vle::utils::ModellingError("Predicate operand mot compatible");
    }

    if (op == "<") {
        if (leftDouble){
            return leftOpD < rightOpD;
        }
    }

    if (op == ">") {
        if (leftDouble){
            return leftOpD > rightOpD;
        }
    }

    if (op == "<=") {
        if (leftDouble){
            return leftOpD <= rightOpD;
        }
    }

    if (op == ">=") {
        if (leftDouble){
            return leftOpD >= rightOpD;
        }
    }

    if (op == "==") {
        if (leftDouble){
            return leftOpD == rightOpD;
        }
    }

    throw vle::utils::ModellingError(
        vle::utils::format("farmer predicate penetrability: unknown operator %s", op.c_str()));
}

bool
pSecondDayOfYear(const std::string& /*activity*/,
                 const std::string& /*rule*/,
                 const ved::PredicateParameters& /*param*/) const
{

    return vu::DateTime::dayOfYear(current_date) == 2u;
}

bool
pForceFirstLoad(const std::string& /*activity*/,
                const std::string& /*rule*/,
                const ved::PredicateParameters& /*param*/) const
{
    return vu::DateTime::dayOfYear(current_date) >= 2u && firstLoad;
}

void
out_plan(const std::string& name,
         const ved::Activity& activity,
         vd::ExternalEventList& output) {

    if (activity.isInStartedState()) {

        output.emplace_back("out_" + getPortName(name));
        vle::value::Map& outputMap = output.back().addMap();
        outputMap.addString("name", name);
        outputMap.addString("activity", name);
        outputMap.addString("value", "done");
    }
}

void
ack_plan(const std::string&activityname,
              const ved::Activity& activity) {

    std::string suffixRetourNum;
    std::string activityPrefix = getPrefixName(activityname);

    if (mCounter.find(activityPrefix) != mCounter.end()) {
        mCounter[activityPrefix]++;
    } else {
        mCounter[activityPrefix] = 1;
    }

    suffixRetourNum = getSuffixName(mCounter[activityPrefix]);

    ved::Activity& a = addActivity(activityPrefix + "#" + suffixRetourNum);

    a.initStartRangeFinishRange(current_date + 1,
                                vd::infinity,
                                vd::negativeInfinity,
                                activity.maxfinish());
    a.addOutputFunction(
        boost::bind(&AgentDTG::out_plan,
                    this, _1, _2, _3));
    a.addAcknowledgeFunction(
        boost::bind(&AgentDTG::ack_plan,
                    this, _1, _2));
    a.addUpdateFunction(
        boost::bind(&AgentDTG::loadPlan,
                    this, _1, _2));
    a.addRule("rSecondDayOfYear", KnowledgeBase::rules().get("rSecondDayOfYear"));
}

    void loadPlan(const std::string& /*name*/,
              const ved::Activity& activity)
{
    firstLoad = false;

    if (activity.isInStartedState()) {
        vv::MapValue::const_iterator it;

        for (it = mRotationP->toMap().begin(); it != mRotationP->toMap().end(); it++) {

            strings_t plans = getPlansOfTheYear(it->first);

            for (strings_it jt = plans.begin(); jt != plans.end(); jt++) {

                std::string filePath;

                if (not mPlansLocation.empty()) {
                    filePath = vu::Package(context(), mPlansLocation).getDataFile(*jt + ".txt");
                } else {
                    filePath = getPackageDataFile(*jt + ".txt");
                }

                std::ifstream fileStream(filePath.c_str());

                int counter = mPlanPerLocationCounter.find(it->first)->second++;
                std::stringstream ss;
                ss << boost::format("%1$02d") % counter;
                std::string suf = "@" + it->first + ":" + ss.str();

                KnowledgeBase::plan().fill(fileStream, current_date, suf);
            }
        }
    }
}

private:

    std::unique_ptr<vle::value::Value> mRotationP;
    std::unique_ptr<vle::value::Value> mResourceP;
    vd::Time mStartTime;
    std::map < std::string, int > mPlanPerLocationCounter;

    std::map < std::string, int > mCounter;

    bool firstLoad;
    std::string mPlansLocation;

/**
 * @brief provide a kind of automatic output function
 *
 * First the function check if the name of the activity does
 * contain a location. If this is the case outputs are
 * adressed to a port suffixed a location.
 * Example : if the activity name is sowing@P1
 * an output port could be density@p1.
 *
 * Then to decide what to output, the function use each
 * parameters of the activity, prefixed by _out_. And then
 * it provides as many dt values as output parameter available,
 * using the name of the parameter for the name of the variable,
 * and the value of the parameter for the value of the variable.
 *
 * Finaly the function send to the ackOut port, an ack event.
 * The activity has no duration
 *
 * TODO:providing the ack mecanism in case an OS is used between
 * decision and Bio, with an activity with duration
 */
void
GOut(const std::string& name,
     const ved::Activity& activity,
     vd::ExternalEventList& output) {

    if (activity.isInStartedState()) {

        std::string locationName = getLocationName(name);
        std::string portSuffix;

        if (locationName.empty()) {
            portSuffix = {};
        } else {
            portSuffix = "@" + locationName;
        }

        ved::ActivityParameters::const_iterator it;
        for (it = activity.params().begin(); it != activity.params().end(); it++) {

            std::string paramName = getParamName(it->first);

            if (paramName.compare(0,5,"_out_") == 0) {
                std::string variableName =  paramName.substr(5);

                if (it->second.which() == 0) {
                    double variableValue =  activity.params().getDouble(it->first);
                    output.emplace_back(variableName + portSuffix);
                    vle::value::Map& map = output.back().addMap();

                    map.addString("name", variableName);
                    map.addDouble("value", variableValue);

                } else {
                    std::string variableValue =  activity.params().getString(it->first);
                    output.emplace_back(variableName + portSuffix);
                    vle::value::Map& map = output.back().addMap();

                    map.addString("name", variableName);
                    map.addString("value", variableValue);
                }
            }
        }

        output.emplace_back("ackOut");
        vle::value::Map& map = output.back().addMap();
        map.addString("name", name);
        map.addString("activity", name);
        map.addString("value", "done");
    }
}

void
GUpdate(const std::string& name,
        const ved::Activity& activity) {

    std::string locationName = getLocationName(name);
    std::string portSuffix;
    std::string statePrefix;

    if (locationName.empty()) {
        portSuffix = {};
    } else {
        portSuffix = "@" + locationName;
    }
    if (activity.isInStartedState()) {
        statePrefix = "Started";
    } else if (activity.isInDoneState()) {
        statePrefix = "Done";
    }

    if (activity.isInStartedState() || activity.isInDoneState()) {

        ved::ActivityParameters::const_iterator it;
        for (it = activity.params().begin(); it != activity.params().end(); it++) {
            std::string paramName = getParamName(it->first);

            if (paramName.compare(0,9 + statePrefix.size() ,"_update_" + statePrefix + "_") == 0) {
                std::string variableName =  paramName.substr(9 + statePrefix.size()) + portSuffix;
                double variableValue =  activity.params().getDouble(it->first);
                Variables::const_iterator itv =
                    getVariables().find(variableName);
                VarMono* v = (VarMono*) itv->second;
                v->update(current_date - begin_date, variableValue);
            }
        }
    }
}

};

}}} // namespaces

DECLARE_DYNAMICS(vle::discrete_time::decision::AgentDTG)
