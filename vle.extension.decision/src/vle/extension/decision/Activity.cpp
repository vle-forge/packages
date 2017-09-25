/*
 * @file vle/extension/decision/Activity.cpp
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


#include <vle/extension/decision/Activity.hpp>
#include <vle/utils/Exception.hpp>
#include <vle/utils/Tools.hpp>

namespace vle { namespace extension { namespace decision {

bool ActivityParameters::exist(const std::string& name) const
{
    auto it = m_lst.find(name);
    if (it == m_lst.cend())
        return false;
    return true;
}

void ActivityParameters::addDouble(const std::string& name, double param)
{
    m_lst.insert(
        std::make_pair(name, bx::parameter(param)));
}

void ActivityParameters::addString(const std::string& name, const std::string& param)
{
    m_lst.insert(
        std::make_pair(name, bx::parameter(param)));
}

void ActivityParameters::resetDouble(const std::string& name, double param)
{
    m_lst[name] = param;
}

void ActivityParameters::resetString(const std::string& name, const std::string& param)
{
    m_lst[name] = param;
}

double ActivityParameters::getDouble(const std::string& name) const
{
   auto it = m_lst.find(name);
    if (it == m_lst.cend())
        return 0.;

    if (it->second.type == bx::parameter::tag::real)
        return it->second.d;

    return 0.;
}

std::string ActivityParameters::getString(const std::string& name) const
{
   auto it = m_lst.find(name);
    if (it == m_lst.cend())
        return {};

    if (it->second.type == bx::parameter::tag::string)
        return it->second.s;

    return {};
}

bool Activity::validRules(const std::string& activity) const
{
    if (not m_rules.empty()) {
        Rules::result_t result = m_rules.apply(activity);
        return not result.empty();
    }
    return true;
}

void Activity::initStartTimeFinishTime(const devs::Time& start,
                                       const devs::Time& finish)
{
    if (start > finish) {
        throw vle::utils::ModellingError(vle::utils::format(
          "Decision: temporal constraint expected : "
                "start (%f) before finish (%f)",
                start, finish));
    }

    m_date = (DateType)(Activity::START | Activity::FINISH);
    m_start = start;
    m_finish = finish;
}

void Activity::initStartTimeFinishRange(const devs::Time& start,
                                        const devs::Time& minfinish,
                                        const devs::Time& maxfinish)
{
    if (not (start < minfinish and minfinish < maxfinish)) {
        throw vle::utils::ModellingError(vle::utils::format(
                "Decision: temporal constraint expected : start (%f) "
                        "before minfinish (%f) before maxfinish (%f)",
                start, minfinish, maxfinish));
    }

    m_date = (DateType)(Activity::START | Activity::MINF | Activity::MAXF);
    m_start = start;
    m_minfinish = minfinish;
    m_maxfinish = maxfinish;
}

void Activity::initStartRangeFinishTime(const devs::Time& minstart,
                                        const devs::Time& maxstart,
                                        const devs::Time& finish)
{
    if (not (minstart < maxstart and maxstart < finish)) {
        throw vle::utils::ModellingError(vle::utils::format(
                "Decision: temporal constraint expected : minstart (%f)"
                " before maxstart (%f) before finish (%f)",
       minstart, maxstart, finish));
    }

    m_date = (DateType)(Activity::MINS | Activity::MAXS | Activity::FINISH);
    m_minstart = minstart;
    m_maxstart = maxstart;
    m_finish = finish;
}

void Activity::initStartRangeFinishRange(const devs::Time& minstart,
                                         const devs::Time& maxstart,
                                         const devs::Time& minfinish,
                                         const devs::Time& maxfinish)
{
    if (not (minstart < maxstart and minstart < maxfinish
             and minfinish < maxfinish)) {
        throw vle::utils::ModellingError(vle::utils::format(
                "Decision: temporal constraint expected : minstart (%f)"
                        " before maxstart (%f) and minfinish (%f) before"
                        " maxfinish (%f) and minstart (%f) before"
                        " maxfinish (%f)",
               minstart, maxstart, minfinish, maxfinish, minstart, maxfinish));
    }

    m_date = (DateType)(Activity::MINF | Activity::MAXF | Activity::MINS |
                        Activity::MAXS);
    m_minstart = minstart;
    m_maxstart = maxstart;
    m_minfinish = minfinish;
    m_maxfinish = maxfinish;
}

devs::Time Activity::nextTime(const devs::Time& time)
{
    devs::Time result = devs::infinity;

    switch (m_state) {
    case WAIT:
    case STARTED:
    case FF:
        if (m_date & START and time <= m_start) {
            result = m_start;
        } else if (m_date & MINS and time <= m_minstart) {
            result = m_minstart;
        } else if (m_date & MAXS and time <= m_maxstart) {
            result = m_maxstart;
        } else {
            if (m_date & FINISH and time <= m_finish) {
                result = m_finish;
            } else if (m_date & MINF and time <= m_minfinish) {
                result = m_minfinish;
            } else if (m_date & MAXF and time <= m_maxfinish) {
                result = m_maxfinish;
            } else {
                result = devs::infinity;
            }
        }
        break;
    case DONE:
    case FAILED:
        result = devs::infinity;
        break;
    }

    return result;
}

bool Activity::isValidTimeConstraint(const devs::Time& time) const
{
    switch (m_date & (START | FINISH | MINS | MAXS | MINF | MAXF)) {
    case START | FINISH:
        return m_start <= time and time <= m_finish;

    case START | MINF | MAXF:
        return m_start <= time and time <= m_maxfinish;

    case MINS | MAXS | FINISH:
        return m_minstart <= time and time <= m_maxfinish;

    case MINS | MAXS | MINF | MAXF:
        return m_minstart <= time and time <= m_maxfinish;

    default:
        break;
    }
    throw vle::utils::InternalError(vle::utils::format(
            "Decision: activity time type invalid: %i",
           (int)m_date));
}

bool Activity::isBeforeTimeConstraint(const devs::Time& time) const
{
    switch (m_date & (START | FINISH | MINS | MAXS | MINF | MAXF)) {
    case START | FINISH:
        return time < m_start;

    case START | MINF | MAXF:
        return time < m_start;

    case MINS | MAXS | FINISH :
        return time < m_minstart;

    case MINS | MAXS | MINF | MAXF:
        return time < m_minstart;

    default:
        break;
    }
    throw vle::utils::InternalError(vle::utils::format(
            "Decision: activity time type invalid: %i",
           (int)m_date));
}

bool Activity::isAfterTimeConstraint(const devs::Time& time) const
{
    switch (m_date & (START | FINISH | MINS | MAXS | MINF | MAXF)) {
    case START | FINISH:
        return m_finish < time;

    case START | MINF | MAXF:
        return m_maxfinish < time;

    case MINS | MAXS | FINISH:
        return m_finish < time;

    case MINS | MAXS | MINF | MAXF:
        return m_maxfinish < time;

    default:
        break;
    }
    throw vle::utils::InternalError(vle::utils::format(
            "Decision: activity time type invalid: %i",
           (int)m_date));
}

bool Activity::isAfterStartTimeConstraint(const devs::Time& time) const
{
    switch (m_date & (START | FINISH | MINS | MAXS | MINF | MAXF)) {
    case START | FINISH:
        return m_finish < time;

    case START | MINF | MAXF:
        return m_maxfinish < time;

    case MINS | MAXS | FINISH:
        return m_maxstart < time;

    case MINS | MAXS | MINF | MAXF:
        return m_maxstart < time;

    default:
        break;
    }
    throw utils::InternalError(
        utils::format("Decision: activity time type invalid: %i",
                      (int)m_date));
}

bool Activity::isBeforeFinishTimeConstraint(const devs::Time& time) const
{
    switch (m_date & (START | FINISH | MINS | MAXS | MINF | MAXF)) {
    case START | FINISH:
        return m_start > time;

    case START | MINF | MAXF:
        return m_minfinish > time;

    case MINS | MAXS | FINISH:
        return m_minstart > time;

    case MINS | MAXS | MINF | MAXF:
        return m_minfinish > time;

    default:
        break;
    }
    throw vle::utils::InternalError(
        utils::format(
            "Decision: activity time type invalid: %i",
            (int)m_date));
}

bool Activity::isValidHorizonTimeConstraint(const devs::Time& lowerBound,
                                            const devs::Time& upperBound) const
{
    switch (m_date & (START | FINISH | MINS | MAXS | MINF | MAXF)) {
    case START | FINISH:
        return m_start <= upperBound and lowerBound <= m_finish;

    case START | MINF | MAXF:
        return m_start <= upperBound  and lowerBound <= m_maxfinish;

    case MINS | MAXS | FINISH:
        return m_minstart <= upperBound and lowerBound <= m_maxfinish;

    case MINS | MAXS | MINF | MAXF:
        return m_minstart <= upperBound and lowerBound <= m_maxfinish;

    default:
        break;
    }
    throw vle::utils::InternalError(vle::utils::format(
           "Decision: activity time type invalid: %i",
           (int)m_date));
}

}}} // namespace vle model decision
