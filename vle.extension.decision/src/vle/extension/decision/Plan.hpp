/*
 * @file vle/extension/decision/Plan.hpp
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


#ifndef VLE_EXT_DECISION_PLAN_HPP
#define VLE_EXT_DECISION_PLAN_HPP 1

#include <vle/extension/decision/Activities.hpp>
#include <vle/extension/decision/Facts.hpp>
#include <vle/extension/decision/Rules.hpp>
#include <vle/utils/Parser.hpp>
#include <vle/devs/Time.hpp>
#include <string>
#include <istream>

namespace vle { namespace extension { namespace decision {

class KnowledgeBase;

/**
 * @brief A Plan stores Rules, Activites (with the PrecedencesGraph). The
 * functions Facts, Predicates, AcknowledgeFunctions, OutputFunctions and
 * UpdateFunctions must be defined.
 */
class Plan
{

    typedef std::pair<bool, devs::Time> DateResult;

public:
    Plan(KnowledgeBase& kb)
        : mKb(kb)
    {}
    /**
     * @brief Plan constructor
     * @param kb, ref to the knowledge base
     * @param buffer, string representation of the plan
     */
    Plan(KnowledgeBase& kb, const std::string& buffer);
    /**
     * @brief Plan constructor
     * @param kb, ref to the knowledge base
     * @param stream, stream containing the representation of the plan
     */
    Plan(KnowledgeBase& kb, std::istream& stream);
    /**
     * @brief Fill a plan from a string
     * @param buffer, string representation of the plan
     * @deprecated the loadTime should be given as second argument
     */
    void fill(const std::string& buffer);
    /**
     * @brief Fill a plan from a stream
     * @param buffer, string representation of the plan
     * @param loadTime, the time of plan loading.
     */
    void fill(const std::string& buffer, const devs::Time& loadTime);
    /**
     * @brief Fill a plan from a stream
     * @param buffer, string representation of the plan
     * @param loadTime, the time of plan loading.
     */
    void fill(const std::string& buffer, const devs::Time& loadTime,
         const std::string suffixe);
    /**
     * @brief Fill a plan from a string
     * @param stream, stream containing the representation of the plan
     * @deprecated the loadTime should be given as second argument
     */
    void fill(std::istream& stream);
    /**
     * @brief Fill a plan from a string
     * @param stream, stream containing the representation of the plan
     * @param loadTime, the time of plan loading.
     */
    void fill(std::istream& stream, const devs::Time& loadTime);
    /**
     * @brief Fill a plan from a string
     * @param stream, stream containing the representation of the plan
     * @param loadTime, the time of plan loading.
     */
    void fill(std::istream& stream, const devs::Time& loadTime,
              const std::string suffixe);

    const Rules& rules() const { return mRules; }
    const Activities& activities() const { return mActivities; }
    Rules& rules() { return mRules; }
    Activities& activities() { return mActivities; }

private:
    void fill(const utils::Block& root);
    void fillRules(const utils::Block::BlocksResult& rules);
    void fillActivities(const utils::Block::BlocksResult& activities);
    void fill(const utils::Block& root, const devs::Time& loadTime);
    void fill(const utils::Block& root, const devs::Time& loadTime,
              const std::string suffixe);
    void fillRules(const utils::Block::BlocksResult& rules,
            const devs::Time& loadTime);
    void fillActivities(const utils::Block::BlocksResult& activities,
                        const devs::Time& loadTime);
    void fillActivities(const utils::Block::BlocksResult& activities,
                        const devs::Time& loadTime,
                        const std::string suffixe);
    void fillTemporal(const utils::Block::BlocksResult& temporals,
                              Activity& activity,
                              const devs::Time& loadTime);
    void fillPrecedences(const utils::Block::BlocksResult& precedences,
                         const devs::Time& loadTime);
    void fillPrecedences(const utils::Block::BlocksResult& precedences,
                         const devs::Time& loadTime,
                         const std::string suffixe);
    /**
     * @brief Get a date into a block with real format (e.g. julian day 2452132)
     * or a date format (e.g. 2001-08-10)
     * @param dateName, the name of the date to search (e.g. start, maxfinish)
     * @param block, the block where to seach
     * @param loadTime, the time of plan loading.
     */
    DateResult getDate(const std::string& dateName,
                       const utils::Block& block,
                       const devs::Time& loadTime) const;

    KnowledgeBase& mKb;
    Predicates mPredicates;
    Rules mRules;
    Activities mActivities;
};

}}} // namespace vle model decision

#endif
