/*
 * @file vle/extension/decision/KnowledgeBase.cpp
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


#include <vle/extension/decision/KnowledgeBase.hpp>
#include <vle/utils/Tools.hpp>
#include <boost/algorithm/string.hpp>
#include <boost/lexical_cast.hpp>
#include <algorithm>
#include <cassert>
#include <boost/lexical_cast.hpp>
#include <iostream>

using boost::lexical_cast;

namespace vle { namespace extension { namespace decision {

ResourcesExtended KnowledgeBase::extendResources(const std::string& resources) const
{
    ResourceSolution line;
    ResourcesExtended listOfline;

    std::vector<std::string> resourcesAlt;

    boost::split(resourcesAlt, resources, boost::is_any_of("|"));

    for (unsigned i=0; i < resourcesAlt.size(); i++) {
        boost::trim(resourcesAlt[i]);
        ResourcesExtended listOflinePerAlter;

        std::vector<std::string> strs;

        boost::split(strs, resourcesAlt[i], boost::is_any_of("+"));

        for (uint j = 0; j < strs.size(); j++) {
            boost::trim(strs[j]);
            std::string toEx = strs[j];
            size_t n = std::count(toEx.begin(), toEx.end(), '.');
            if ( n !=0 ) {
                if ( n > 1 ) {
                    throw utils::ArgError(
                        vle::utils::format("Decision: resource syntax error, only single dot allowed: %s", strs[j].c_str()));
                } else {
                    std::size_t pos = toEx.find(".");
                    std::string resQ = toEx.substr(0, pos);
                    boost::trim(resQ);
                    std::string resI = toEx.substr(pos + 1);
                    boost::trim(resI);
                    int iterResQ = lexical_cast<int>(resI);
                    if (iterResQ < 2) {
                        throw utils::ArgError(
                            vle::utils::format("Decision: resource syntax error, resource quantity < 2:  '%s'", toEx.c_str()));
                    }
                    strs.at(j) = resQ;
                    for (int k = 1; k < iterResQ; k++) {
                        strs.insert(strs.begin() + j, resQ);
                    }
                }
            }
        }

        std::vector<std::string> strsUniq = strs;
        std::vector<std::string>::iterator it,jt;
        std::sort(strsUniq.begin(), strsUniq.end());
        it = std::unique (strsUniq.begin(), strsUniq.end());
        strsUniq.resize(std::distance(strsUniq.begin(),it));
        for (jt = strsUniq.begin(); jt != strsUniq.end(); jt++) {
            if (count(strs.begin(), strs.end(), *jt) > getResourceQuantity(*jt)){
                throw utils::ArgError(vle::utils::format(
                                          "Decision: resource syntax error, more ressources wanted than available: '%s'",(*jt).c_str()));
            }
        }

        for (uint j=0; j < strs.size(); j++) {
            boost::trim(strs[j]);

            if (not resourceTypeExist(strs[j])) {
                throw utils::ArgError(vle::utils::format(
                                          "Decision: resource '%s' is not defined", strs[j].c_str()));
            } else {
                ResourceSolution resTypeList = getResources(strs[j]);
                ResourcesExtended listOflineLocal;
                for (ResourceSolution::iterator it = resTypeList.begin();
                     it != resTypeList.end(); it++) {
                    if (listOflinePerAlter.empty()) {
                        ResourceSolution line;
                        line.push_back(*it);
                        listOflineLocal.push_back(line);
                    } else {
                        for (ResourcesExtended::iterator jt = listOflinePerAlter.begin();
                             jt != listOflinePerAlter.end(); jt++) {
                            if (std::find((*jt).begin(),
                                          (*jt).end(),
                                          *it) ==  (*jt).end()) {
                                ResourceSolution line;
                                line.push_back(*it);
                                for (ResourceSolution::iterator kt = (*jt).begin();
                                     kt != (*jt).end(); kt++) {
                                    line.push_back(*kt);
                                }
                                bool solutionExist = false;
                                std::vector< std::string > linev(line.begin(),
                                                                 line.end());

                                //std::vector< std::string> linev = line;
                                std::sort(linev.begin(), linev.end());
                                for (ResourcesExtended::iterator kt =
                                         listOflineLocal.begin();
                                     kt != listOflineLocal.end(); kt++) {
                                    std::vector< std::string > currentLinev((*kt).begin(),
                                                                            (*kt).end());
                                    std::sort(currentLinev.begin(), currentLinev.end());
                                    std::vector<std::string>::iterator iit;
                                    if ( currentLinev == linev ) {
                                        solutionExist = true;
                                        break;
                                    }
                                }
                                if (not solutionExist) {
                                listOflineLocal.push_back(line);
                                }
                            }
                        }
                    }
                }
                if (listOflineLocal.empty()) {
                      throw utils::ArgError(vle::utils::format(
                                                "Decision: resource syntax error, ressource alternative impossible: '%s'", resourcesAlt[i].c_str()));
                }

                listOflinePerAlter.clear();
                listOflinePerAlter = listOflineLocal;
            }
        }
        for (ResourcesExtended::iterator iit = listOflinePerAlter.begin();
             iit != listOflinePerAlter.end(); iit++) {
            listOfline.push_back(*iit);
        }
        listOflinePerAlter.clear();
    }
    return listOfline;
}

void KnowledgeBase::setActivityDone(const std::string& name,
                                    const devs::Time& date)
{
    Activities::iterator it(mPlan.activities().get(name));

    if (not it->second.isInStartedState()) {
        throw utils::ArgError(vle::utils::format(
                "Decision: activity '%s' is not started", name.c_str()));
    }

    mPlan.activities().freeRessources(name);
    mPlan.activities().setFFAct(it);
    it->second.ff(date);
    it->second.acknowledge(name);
}

void KnowledgeBase::setActivityFailed(const std::string& name,
                                      const devs::Time& date)
{
    Activities::iterator it(mPlan.activities().get(name));

    if (it->second.isInDoneState()) {
        throw utils::ArgError(vle::utils::format(
                "Decision: activity '%s' is already finish", name.c_str()));
    }

    if (not it->second.isInFailedState()) {
        mPlan.activities().setFailedAct(it);
        it->second.fail(date);
        it->second.acknowledge(name);
    }
}

devs::Time KnowledgeBase::duration(const devs::Time& time)
{
    devs::Time next = nextDate(time);

    if (next == devs::negativeInfinity) {
        return 0.0;
    } else {
        assert(next >= time);
        return next - time;
    }
}

KnowledgeBase::Result KnowledgeBase::processChanges(const devs::Time& time)
{
    Activities::Result r = mPlan.activities().process(time);
    return std::make_pair(r.first, r.second);
}

//struct AddRuleToken
//{
    //KnowledgeBase& kb;

    //AddRuleToken(KnowledgeBase& kb) : kb(kb) {}

    //void operator()(const RuleToken& rule)
    //{
        //Rule& r = kb.addRule(rule.id);

        //for (Strings::const_iterator it = rule.predicates.begin(); it !=
             //rule.predicates.end(); ++it) {
            //r.add(kb.predicates()[*it]);
        //}
    //}
//};

//struct AddActivityToken
//{
    //KnowledgeBase& kb;

    //AddActivityToken(KnowledgeBase& kb) : kb(kb) {}

    //void operator()(const ActivityToken& activity)
    //{
        //decision::Activity& act = kb.addActivity(activity.id);

        //if (not activity.ack.empty()) {
            //act.addAcknowledgeFunction(
                //kb.acknowledgeFunctions()[activity.ack]);
        //}

        //for (Strings::const_iterator it = activity.rules.begin();
             //it != activity.rules.end(); ++it) {
            //act.addRule(*it, kb.rules().get(*it));
        //}

        //if (not activity.output.empty()) {
            //act.addOutputFunction(
                //kb.outputFunctions()[activity.output]);
        //}

        //switch (activity.temporal.type) {
        //case 1:
        //case 2:
            //act.initStartTimeFinishTime(activity.temporal.start,
                                        //activity.temporal.finish);
            //break;
        //case 3:
            //act.initStartTimeFinishRange(activity.temporal.start,
                                         //activity.temporal.minfinish,
                                         //activity.temporal.maxfinish);
            //break;
        //case 4:
        //case 5:
            //act.initStartRangeFinishTime(activity.temporal.minstart,
                                         //activity.temporal.maxstart,
                                         //activity.temporal.finish);
            //break;
        //case 6:
            //act.initStartRangeFinishRange(activity.temporal.minstart,
                                          //activity.temporal.maxstart,
                                          //activity.temporal.minfinish,
                                          //activity.temporal.maxfinish);
            //break;
        //default:
            //throw;
        //}
    //}
//};

//struct AddPrecedenceToken
//{
    //KnowledgeBase& kb;

    //AddPrecedenceToken(KnowledgeBase& kb) : kb(kb) {}

    //void operator()(const PrecedenceToken& pre)
    //{
        //switch (pre.type) {
        //case 1:
            //kb.addFinishToStartConstraint(pre.first, pre.second,
                                          //pre.mintimelag, pre.maxtimelag);
            //break;
        //case 2:
            //kb.addStartToStartConstraint(pre.first, pre.second,
                                         //pre.mintimelag, pre.maxtimelag);
            //break;
        //case 3:
            //kb.addFinishToFinishConstraint(pre.first, pre.second,
                                           //pre.mintimelag, pre.maxtimelag);
            //break;
        //default:
            //throw utils::ArgErrorvle::utils::format(
                        //"Decision: unknow precedence type `%s'") %
                //pre.type);
        //}
    //}
//};

//void KnowledgeBase::instantiatePlan(const std::string& name)
//{
    //const Plan& plan = library()[name];

    //std::for_each(plan.rules.rules.begin(),
                  //plan.rules.rules.end(),
                  //AddRuleToken(*this));

    //std::for_each(plan.activities.activities.begin(),
                  //plan.activities.activities.end(),
                  //AddActivityToken(*this));

    //std::for_each(plan.precedences.precedences.begin(),
                  //plan.precedences.precedences.end(),
                  //AddPrecedenceToken(*this));
//}

}}} // namespace vle model decision
