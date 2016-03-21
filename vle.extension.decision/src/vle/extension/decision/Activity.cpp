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
#include <vle/utils/i18n.hpp>
#include <boost/format.hpp>
#include <boost/variant/get.hpp>

namespace vle { namespace extension { namespace decision {

struct ActivityParametersFind
{
    bool operator()(const ActivityParameters::name_parameter_type& p,
                    const std::string& str) const
    {
        return p.first < str;
    }

    bool operator()(const std::string& str,
                    const ActivityParameters::name_parameter_type& p) const
    {
        return str < p.first;
    }
};

struct ActivityParametersFindToReset
{
    bool operator()(ActivityParameters::name_parameter_type& p,
                    const std::string& str) const
    {
        return p.first < str;
    }

    bool operator()(const std::string& str,
                    ActivityParameters::name_parameter_type& p) const
    {
        return str < p.first;
    }
};

struct ActivityParametersCompare
{
    bool operator()(const ActivityParameters::name_parameter_type& a,
                    const ActivityParameters::name_parameter_type& b) const
    {
        return a.first < b.first;
    }
};

template <typename T>
void addParam(ActivityParameters::container_type& p,
              const std::string& name,
              const T& param)
{
    p.push_back(std::make_pair(name, param));
}

template <typename T>
void resetParam(ActivityParameters::container_type& p,
                const std::string& name,
                const T& param)
{
    ActivityParameters::iterator it =
        std::lower_bound(p.begin(),
                         p.end(),
                         name,
                         ActivityParametersFindToReset());

    if (it == p.end() or it->first != name)
        throw vle::utils::ModellingError(
            vle::fmt("Decision fails to find parameter %1%") % name);

    it->second = param;
}

struct comp
{
    comp(const std::string& s) : _s(s) { }

    bool operator()(const ActivityParameters::name_parameter_type& p)
    {
        return (p.first == _s);
    }

    std::string _s;
};

bool ActivityParameters::exist(const std::string& name) const
{
    return (std::find_if(m_lst.begin(), m_lst.end(), comp(name)) != m_lst.end());
}


template <typename T>
T getParam(const ActivityParameters::container_type& p,
           const std::string& name)
{
    ActivityParameters::const_iterator it =
        std::lower_bound(p.begin(),
                         p.end(),
                         name,
                         ActivityParametersFind());

    if (it == p.end() or it->first != name)
        throw vle::utils::ModellingError(
            vle::fmt("Decision fails to get parameter %1%") % name);

    const T* ret = boost::get <T>(&it->second);
    if (ret)
        return *ret;

    throw vle::utils::ModellingError(
        vle::fmt("Decision fails to convert parameter %1%") % name);
}

void ActivityParameters::addDouble(const std::string& name, double param)
{
    addParam <double>(m_lst, name, param);
}

void ActivityParameters::addString(const std::string& name, const std::string& param)
{
    addParam <std::string>(m_lst, name, param);
}

void ActivityParameters::resetDouble(const std::string& name, double param)
{
    resetParam <double>(m_lst, name, param);
}

void ActivityParameters::resetString(const std::string& name, const std::string& param)
{
    resetParam <std::string>(m_lst, name, param);
}

void ActivityParameters::sort()
{
    std::sort(m_lst.begin(), m_lst.end(), ActivityParametersCompare());
}

double ActivityParameters::getDouble(const std::string& name) const
{
    return getParam <double>(m_lst, name);
}

std::string ActivityParameters::getString(const std::string& name) const
{
    return getParam <std::string>(m_lst, name);
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
        throw utils::ModellingError(
        vle::fmt(_("Decision: temporal constraint expected : "
                "start (%1%) before finish (%2%)"))
        % start % finish);
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
        throw utils::ModellingError(
                vle::fmt(_("Decision: temporal constraint expected : start (%1%) "
                        "before minfinish (%2%) before maxfinish (%3%)"))
                % start % minfinish % maxfinish);
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
        throw utils::ModellingError(
        vle::fmt(_("Decision: temporal constraint expected : minstart (%1%)"
                " before maxstart (%2%) before finish (%3%)"))
        % minstart % maxstart % finish);
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
        throw utils::ModellingError(
                vle::fmt(_("Decision: temporal constraint expected : minstart (%1%)"
                        " before maxstart (%2%) and minfinish (%3%) before"
                        " maxfinish (%4%) and minstart (%1%) before"
                        " maxfinish (%4%)"))
                % minstart % maxstart % minfinish % maxfinish );
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
    throw utils::InternalError(fmt(
            _("Decision: activity time type invalid: %1%")) % (int)m_date);
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
    throw utils::InternalError(fmt(
            _("Decision: activity time type invalid: %1%")) % (int)m_date);
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
    throw utils::InternalError(fmt(
            _("Decision: activity time type invalid: %1%")) % (int)m_date);
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
    throw utils::InternalError(fmt(
            _("Decision: activity time type invalid: %1%")) % (int)m_date);
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
    throw utils::InternalError(fmt(
            _("Decision: activity time type invalid: %1%")) % (int)m_date);
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
    throw utils::InternalError(fmt(
            _("Decision: activity time type invalid: %1%")) % (int)m_date);
}

}}} // namespace vle model decision
