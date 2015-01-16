/*
 * @file vle/extension/decision/Plan.cpp
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


#include <vle/extension/decision/Plan.hpp>
#include <vle/extension/decision/KnowledgeBase.hpp>
#include <vle/utils/Parser.hpp>
#include <vle/utils/i18n.hpp>
#include <vle/utils/DateTime.hpp>
#include <vle/utils/Trace.hpp>
#include <boost/algorithm/string.hpp>
#include <boost/lexical_cast.hpp>
#include <string>
#include <sstream>
#include <iostream>

namespace vle { namespace extension { namespace decision {

typedef utils::Block UB;
typedef utils::Block::Blocks UBB;
typedef utils::Block::Strings UBS;
typedef utils::Block::Reals UBR;

using boost::lexical_cast;

Plan::Plan(KnowledgeBase& kb, const std::string& buffer)
    : mKb(kb)
{
    try {
        std::istringstream in(buffer);
        utils::Parser parser(in);
        fill(parser.root(), 0);
    } catch (const std::exception& e) {
        throw utils::ArgError(fmt(_("Decision plan error in %1%")) % e.what());
    }
}

Plan::Plan(KnowledgeBase& kb, std::istream& stream)
    : mKb(kb)
{
    try {
        utils::Parser parser(stream);
        fill(parser.root(), 0);
    } catch (const std::exception& e) {
        throw utils::ArgError(fmt(_("Decision plan error: %1%")) % e.what());
    }
}

void Plan::fill(const std::string& buffer, const devs::Time& loadTime)
{
    try {
        std::istringstream in(buffer);
        utils::Parser parser(in);
        fill(parser.root(), loadTime);
    } catch (const std::exception& e) {
        throw utils::ArgError(fmt(_("Decision plan error in %1%")) % e.what());
    }
}

void Plan::fill(const std::string& buffer, const devs::Time& loadTime,
                const std::string suffixe)
{
    try {
        std::istringstream in(buffer);
        utils::Parser parser(in);
        fill(parser.root(), loadTime, suffixe);
    } catch (const std::exception& e) {
        throw utils::ArgError(fmt(_("Decision plan error in %1%")) % e.what());
    }
}

void Plan::fill(std::istream& stream, const devs::Time& loadTime)
{
    try {
        utils::Parser parser(stream);
        fill(parser.root(), loadTime);
    } catch (const std::exception& e) {
        throw utils::ArgError(fmt(_("Decision plan error: %1%")) % e.what());
    }
}

void Plan::fill(std::istream& stream, const devs::Time& loadTime,
                const std::string suffixe)
{
    try {
        utils::Parser parser(stream);
        fill(parser.root(), loadTime, suffixe);
    } catch (const std::exception& e) {
        throw utils::ArgError(fmt(_("Decision plan error: %1%")) % e.what());
    }
}

void Plan::fill(const std::string& buffer)
{
    try {
        std::istringstream in(buffer);
        utils::Parser parser(in);
        fill(parser.root(), 0);
    } catch (const std::exception& e) {
        throw utils::ArgError(fmt(_("Decision plan error in %1%")) % e.what());
    }
}

void Plan::fill(std::istream& stream)
{
    try {
        utils::Parser parser(stream);
        fill(parser.root(), 0);
    } catch (const std::exception& e) {
        throw utils::ArgError(fmt(_("Decision plan error: %1%")) % e.what());
    }
}

void Plan::fill(const utils::Block& root, const devs::Time& loadTime)
{
    fill(root, loadTime, "");
}

struct AssignStringParameter
{
    PredicateParameters& m_params;

    AssignStringParameter(PredicateParameters& params)
        : m_params(params)
    {}

    void operator()(const vle::utils::Block::Strings::value_type& p)
    {
        m_params.addString(p.first, p.second);
    }
};

struct AssignDoubleParameter
{
    PredicateParameters& m_params;

    AssignDoubleParameter(PredicateParameters& params)
        : m_params(params)
    {}

    void operator()(const vle::utils::Block::Reals::value_type& p)
    {
        m_params.addDouble(p.first, p.second);
    }
};

void __fill_predicate(const utils::Block::BlocksResult& root,
                      Predicates& predicates,
                      const PredicatesTable& table)
{
    for (UBB::const_iterator it = root.first; it != root.second; ++it) {
        const utils::Block& block = it->second;

        UB::StringsResult id = block.strings.equal_range("id");
        if (id.first == id.second)
            throw utils::ArgError(_("Decision: predicate needs id"));

        if (not predicates.exist(id.first->second)) {
            UB::StringsResult type = block.strings.equal_range("type");
            if (type.first == type.second)
                throw utils::ArgError(_("Decision: predicate needs type"));

            PredicatesTable::const_iterator fctit = table.get(type.first->second);
            if (fctit == table.end())
                throw utils::ArgError(
                    vle::fmt(_("Decision: unknown predicate function %1% in knowledgebase"))
                    % type.first->second);

            utils::Block::BlocksResult parameters = block.blocks.equal_range("parameter");
            if (parameters.first == parameters.second) {
                TraceModel(vle::fmt("predicate %1% added")
                           % id.first->second);

                predicates.add(id.first->second, fctit->second);
            } else {
                PredicateParameters params;

                std::for_each(parameters.first->second.strings.begin(),
                              parameters.first->second.strings.end(),
                              AssignStringParameter(params));

                std::for_each(parameters.first->second.reals.begin(),
                              parameters.first->second.reals.end(),
                              AssignDoubleParameter(params));

                params.sort();

                TraceModel(vle::fmt("Predicate %1% added with parameters:") %
                            id.first->second);

                for (PredicateParameters::const_iterator it = params.begin();
                     it != params.end(); ++it)
                    TraceModel(vle::fmt("    - %1%") % it->first);

                predicates.add(id.first->second, fctit->second, params);
            }
        } else {
            TraceModel(vle::fmt("Predicate %1% already exists, we forget the new") %
                        id.first->second);
        }
    }
}

void Plan::fill(const utils::Block& root, const devs::Time& loadTime,
                const std::string suffixe)
{
    utils::Block::BlocksResult mainpredicates, mainrules, mainactivities,
        mainprecedences;

    mainpredicates = root.blocks.equal_range("predicates");
    mainrules = root.blocks.equal_range("rules");
    mainactivities = root.blocks.equal_range("activities");
    mainprecedences = root.blocks.equal_range("precedences");

    utils::Block::Blocks::const_iterator it;

    for (it = mainpredicates.first; it != mainpredicates.second; ++it)
        __fill_predicate(it->second.blocks.equal_range("predicate"),
                         mPredicates, mKb.predicates());

    for (it = mainrules.first; it != mainrules.second; ++it) {
        utils::Block::BlocksResult rules;
        rules = it->second.blocks.equal_range("rule");
        fillRules(rules, loadTime);
    }

    for (it = mainactivities.first; it != mainactivities.second; ++it) {
        utils::Block::BlocksResult activities;
        activities = it->second.blocks.equal_range("activity");
        fillActivities(activities, loadTime, suffixe);
    }

    for (it = mainprecedences.first; it != mainprecedences.second; ++it) {
        utils::Block::BlocksResult precedences;
        precedences = it->second.blocks.equal_range("precedence");
        fillPrecedences(precedences, loadTime, suffixe);
    }
}

void Plan::fillRules(const utils::Block::BlocksResult& rules, const devs::Time&)
{
    for (UBB::const_iterator it = rules.first; it != rules.second; ++it) {
        const utils::Block& block = it->second;

        UB::StringsResult id = block.strings.equal_range("id");
        if (id.first == id.second)
            throw utils::ArgError(_("Decision: rule needs id"));

        if (not mRules.exist(id.first->second))  {
            Rule& rule = mRules.add(id.first->second);

            UB::StringsResult preds = block.strings.equal_range("predicates");
            for (UB::Strings::const_iterator jt = preds.first;
                 jt != preds.second; ++jt) {

                // Trying to found a parametred parameter in this plan.
                Predicates::const_iterator p = mPredicates.find(jt->second);
                if (p != mPredicates.end()) {
                    TraceModel(vle::fmt("rule %1% adds predicate %2%") %
                                id.first->second % jt->second);

                    rule.add(&*p);
                } else {
                    // If it fails, trying to use the oldtest API to use
                    // directly the predicate function.
                    PredicatesTable::const_iterator p2 = mKb.predicates().get(jt->second);
                    if (p2 == mKb.predicates().end())
                        throw vle::utils::ArgError(
                            vle::fmt(_("Decision: unknown predicate function %1%")) %
                            jt->second);

                    TraceModel(vle::fmt("rule %1% adds old predicate (c++ function) %2%")
                                % id.first->second % jt->second);

                    rule.add(p2->second);
                }
            }
        } else {
            TraceModel(vle::fmt("Rule %1% already exists, we forget the new") %
                       id.first->second);
        }
    }
}

void Plan::fillActivities(const utils::Block::BlocksResult& acts,
                          const devs::Time& loadTime)
{
    fillActivities(acts, loadTime, "");
}

void Plan::fillActivities(const utils::Block::BlocksResult& acts,
                          const devs::Time& loadTime,
                          const std::string suffixe)
{
    for (UBB::const_iterator it = acts.first; it != acts.second; ++it) {
        const utils::Block& block = it->second;

        UB::StringsResult id = block.strings.equal_range("id");
        if (id.first == id.second) {
            throw utils::ArgError(_("Decision: activity needs id"));
        }

        Activity& act = mActivities.add(id.first->second + suffixe, Activity());

        UB::StringsResult rules = block.strings.equal_range("rules");
        for (UBS::const_iterator jt = rules.first; jt != rules.second; ++jt) {
            act.addRule(jt->second, mRules.get(jt->second));
        }

        UB::StringsResult ack = block.strings.equal_range("ack");
        if (ack.first != ack.second) {
            act.addAcknowledgeFunction((mKb.acknowledgeFunctions().get(
                        ack.first->second))->second);
        }

        UB::StringsResult out = block.strings.equal_range("output");
        if (out .first != out.second) {
            act.addOutputFunction((mKb.outputFunctions().get(
                        out.first->second))->second);
        }

        UB::StringsResult upd = block.strings.equal_range("update");
        if (upd.first != upd.second) {
            act.addUpdateFunction((mKb.updateFunctions().get(
                        upd.first->second))->second);
        }

        UB::BlocksResult temporal = block.blocks.equal_range("temporal");
        if (temporal.first != temporal.second) {
            fillTemporal(temporal, act, loadTime);
        }
    }
}

void Plan::fillTemporal(const utils::Block::BlocksResult& temps,
                        Activity& activity,
                        const devs::Time& loadTime)
{
    for (UBB::const_iterator it = temps.first; it != temps.second; ++it) {
        const utils::Block& block = it->second;


        DateResult start = getDate("start",block,loadTime);
        DateResult mins = getDate("minstart",block,loadTime);
        DateResult maxs = getDate("maxstart",block,loadTime);
        DateResult finish = getDate("finish",block,loadTime);
        DateResult minf = getDate("minfinish",block,loadTime);
        DateResult maxf = getDate("maxfinish",block,loadTime);

        if (start.first) {
            if (finish.first) {
                activity.initStartTimeFinishTime(start.second, finish.second);
            } else {
                double vmin, vmax;
                if (minf.first) {
                    if (maxf.first) {
                        vmin = minf.second;
                        vmax = maxf.second;
                    } else {
                        vmin = minf.second;
                        vmax = devs::infinity;
                    }
                } else {
                    if (maxf.first != maxf.second) {
                        vmin = 0;
                        vmax = maxf.second;
                    } else {
                        vmin = 0;
                        vmax = devs::infinity;
                    }
                }
                activity.initStartTimeFinishRange(start.second,vmin,vmax);
            }
        } else {
            double vmin, vmax;
            if (mins.first) {
                vmin = mins.second;
            } else {
                vmin = devs::negativeInfinity;
            }
            if (maxs.first) {
                vmax = maxs.second;
            } else {
                vmax = devs::infinity;
            }
            if (finish.first) {
                activity.initStartRangeFinishTime(vmin, vmax,finish.second);
            } else {
                double vminf, vmaxf;
                if (minf.first) {
                    if (maxf.first) {
                        vminf = minf.second;
                        vmaxf = maxf.second;
                    } else {
                        vminf = minf.second;
                        vmaxf = devs::infinity;
                    }
                } else {
                    if (maxf.first) {
                        vminf = 0;
                        vmaxf = maxf.second;
                    } else {
                        vminf = 0;
                        vmaxf = devs::infinity;
                    }
                }
                activity.initStartRangeFinishRange(
                    vmin, vmax, vminf, vmaxf);
            }
        }
    }
}

void Plan::fillPrecedences(const utils::Block::BlocksResult& preds,
                           const devs::Time& loadTime)
{
    fillPrecedences(preds, loadTime, "");
}

void Plan::fillPrecedences(const utils::Block::BlocksResult& preds,
                           const devs::Time&,
                           const std::string suffixe)
{
    for (UBB::const_iterator it = preds.first; it != preds.second; ++it) {
        const utils::Block& block = it->second;

        std::string valuefirst, valuesecond;
        double valuemintl = 0.0;
        double valuemaxtl = devs::infinity;

        UB::StringsResult first = block.strings.equal_range("first");
        if (first.first != first.second) {
            valuefirst = first.first->second + suffixe;
        }

        UB::StringsResult second = block.strings.equal_range("second");
        if (second.first != second.second) {
            valuesecond = second.first->second + suffixe;
        }

        UB::RealsResult mintl = block.reals.equal_range("mintimelag");
        if (mintl.first != mintl.second) {
            valuemintl = mintl.first->second;
        }

        UB::RealsResult maxtl = block.reals.equal_range("maxtimelag");
        if (maxtl.first != maxtl.second) {
            valuemaxtl = maxtl.first->second;
        }

        UB::StringsResult type = block.strings.equal_range("type");
        if (type.first != type.second) {
            if (type.first->second == "SS") {
                mActivities.addStartToStartConstraint(valuefirst, valuesecond,
                                                      valuemintl, valuemaxtl);
            } else if (type.first->second == "FS") {
                mActivities.addFinishToStartConstraint(valuefirst, valuesecond,
                                                       valuemintl, valuemaxtl);
            } else if (type.first->second == "FF") {
                mActivities.addFinishToFinishConstraint(valuefirst,
                                                        valuesecond,
                                                        valuemintl, valuemaxtl);
            } else {
                throw utils::ArgError(fmt(
                        _("Decision: precendence type `%1%' unknown")) %
                    type.first->second);
            }
        } else {
            throw utils::ArgError(_("Decision: precedences type unknown"));
        }
    }
}

Plan::DateResult Plan::getDate(const std::string& dateName,
               const utils::Block& block, const devs::Time& loadTime) const
{
    UB::RealsResult dateReal = block.reals.equal_range(dateName);
    UB::StringsResult dateString = block.strings.equal_range(dateName);
    UB::RelativeRealsResult dateRelative =
        block.relativeReals.equal_range(dateName);
    bool hasRealDate = dateReal.first != dateReal.second;
    bool hasStringDate = dateString.first != dateString.second;

    bool hasRelativeDate = dateRelative.first != dateRelative.second;
    if((hasRealDate && hasStringDate) ||
       (hasRealDate && hasRelativeDate) ||
       (hasStringDate && hasRelativeDate)) {
        throw utils::ArgError(fmt(_(
          "Decision: date '%1%' should not be given twice ")) % dateName);
    }
    if (hasRealDate){
        return DateResult(true,devs::Time((double) dateReal.first->second));
    } else if (hasStringDate){
        bool hasRelativeStringDate =  dateString.first->second[0] == '+';
        if (hasRelativeStringDate) {
            std::string relativeDate = dateString.first->second.substr(1);
            std::vector< std::string > explosedDate;

            boost::split(explosedDate, relativeDate, boost::is_any_of("-") );

            std::string year = explosedDate[0];
            std::string month = explosedDate[1];
            std::string day = explosedDate[2];

            if (utils::DateTime::isValidYear(loadTime)) {

                year = lexical_cast<std::string>(
                    lexical_cast<int>(year) +
                    utils::DateTime::year(loadTime));

                return DateResult(true, devs::Time(
                                      (int) utils::DateTime::toJulianDayNumber(
                                          year + "-" + month + "-" + day)));
            } else {
                std::string firstNonLeapYear = "1401";

                int daysOfLastYear = utils::DateTime::dayOfYear(
                    utils::DateTime::toJulianDayNumber(
                        firstNonLeapYear + "-" + month + "-" + day));
                int daysOfFullYears = 365 * lexical_cast<int>(year);

                return DateResult(true, devs::Time(daysOfLastYear + daysOfFullYears));

            }
        } else {
            return DateResult(true,devs::Time(
                                  (int) utils::DateTime::toJulianDayNumber(
                                      dateString.first->second)));
        }
    } else if (hasRelativeDate){
        return DateResult(true,
                          loadTime + devs::Time((double) dateRelative.first->second));
    } else {
        return DateResult(false,devs::infinity);
    }
}

}}} // namespace vle ext decision
