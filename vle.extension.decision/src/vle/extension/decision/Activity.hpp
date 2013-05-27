/*
 * @file vle/extension/decision/Activity.hpp
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


#ifndef VLE_EXT_DECISION_ACTIVITY_HPP
#define VLE_EXT_DECISION_ACTIVITY_HPP 1

#include <vle/extension/decision/Rules.hpp>
#include <vle/devs/ExternalEventList.hpp>
#include <vle/devs/Time.hpp>
#include <boost/function.hpp>
#include <boost/bind.hpp>

namespace vle { namespace extension { namespace decision {

class Activity
{
public:
    typedef boost::function <
        void (const std::string&,
              const Activity&) > AckFct;

    typedef boost::function <
        void (const std::string&,
              const Activity&,
              vle::devs::ExternalEventList&) > OutFct;

    typedef boost::function <
        void (const std::string&,
              const Activity&) > UpdateFct;

    /** Defines the state of the temporal constraints. You can use multiple
     * value for a same DateType by combining the with operator|. For
     * instance:
     * @code
     * vle::model::decision::Activity::DateType v = START | FINISH;
     * @endcode
     */
    enum DateType {
        START = 1 << 0,  /**< Define a strong begin date activity. */
        FINISH = 1 << 1, /**< Define a strong end date activity. */
        MINS = 1 << 2,   /**< The minimum value of the begin range of the
                           activity. */
        MAXS = 1 << 3,   /**< The maximum value of the begin range of the
                           activity. */
        MINF = 1 << 4,   /**< The minimum value of the end range of the
                           activity. */
        MAXF = 1 << 5    /**< The maximum value of the end range of the
                           activity. */
    };

    /** Defines the state of an activity. */
    enum State {
        WAIT, /**< Before the activation of the activity. */
        STARTED, /**< If the activity is launched and operation too. */
        FF, /**< Activity is done, but need to check all FF precedence
              constraint. */
        DONE, /**< If the activity and operation are done. */
        FAILED /**< If the operation failed. */
    };

    Activity()
        : m_state(WAIT), m_waitall(true),
        m_date((Activity::DateType)(Activity::START | Activity::FINISH)),
        m_start(devs::negativeInfinity),
        m_finish(devs::infinity),
        m_minstart(devs::negativeInfinity),
        m_maxstart(devs::negativeInfinity),
        m_minfinish(devs::infinity),
        m_maxfinish(devs::infinity),
        m_started(devs::negativeInfinity),
        m_ff(devs::negativeInfinity),
        m_done(devs::negativeInfinity)
    {}

    //
    // Slot functions to acknowledge an change of state, to send and output or
    // to update the state of an activity.
    //

    /**
     * @brief Assign an acknowledge function to this activity.
     * @param fct An acknowledge function.
     */
    void addAcknowledgeFunction(const AckFct& fct)
    { mAckFct = fct; }

    /**
     * @brief Call the acknowledge function if it exists.
     * @param name The name of the activity.
     */
    void acknowledge(const std::string& name)
    { if (mAckFct) { mAckFct(name, *this); } }

    /**
     * @brief Assign an output function to this activity.
     * @param fct An output function.
     */
    void addOutputFunction(const OutFct& fct)
    { mOutFct = fct; }

    /**
     * @brief Call the output function if it exists.
     * @param name The name of the activity.
     * @param events A list to add ExternalEvent.
     */
    void output(const std::string& name,
                devs::ExternalEventList& events)
    { if (mOutFct) { mOutFct(name, *this, events); } }

    /**
     * @brief Assign an update function to this activity.
     * @param fct An update function.
     */
    void addUpdateFunction(const UpdateFct& fct)
    { mUpdateFct = fct; }

    /**
     * @brief Call the update function if it exists.
     * @param name The name of the activity.
     */
    void update(const std::string& name)
    { if (mUpdateFct) { mUpdateFct(name, *this); } }

    //
    // Settings the activity.
    //

    void addRule(const std::string& name, const Rule& rule)
    { m_rules.add(name, rule); }

    Rule& addRule(const std::string& name)
    { return m_rules.add(name, Rule()); }

    void setRules(const Rules& rules)
    { m_rules = rules; }

    bool validRules() const;

    //
    // manage time constraint
    //

    void initStartTimeFinishTime(const devs::Time& start,
                                 const devs::Time& finish);

    void initStartTimeFinishRange(const devs::Time& time,
                                  const devs::Time& minfinish,
                                  const devs::Time& maxfinish);

    void initStartRangeFinishTime(const devs::Time& minstart,
                                  const devs::Time& maxstart,
                                  const devs::Time& time);

    void initStartRangeFinishRange(const devs::Time& minstart,
                                   const devs::Time& maxstart,
                                   const devs::Time& minfinish,
                                   const devs::Time& maxfinish);

    /**
     * @brief Activity have valid time constraint if the time is:
     * - starttime <= time
     * - time <= finishtime
     * - starttime <= time < finishtime
     *
     * @param time The time to check.
     * @return True if constraint are valid.
     */
    bool isValidTimeConstraint(const devs::Time& time) const;
    bool isBeforeTimeConstraint(const devs::Time& time) const;
    bool isAfterTimeConstraint(const devs::Time& time) const;

    const State& state() const { return m_state; }
    bool isInWaitState() const { return m_state == WAIT; }
    bool isInStartedState() const { return m_state == STARTED; }
    bool isInFFState() const { return m_state == FF; }
    bool isInFailedState() const { return m_state == FAILED; }
    bool isInDoneState() const { return m_state == DONE; }

    void wait() { m_state = WAIT; }
    void start(const devs::Time& date) { m_state = STARTED; startedDate(date); }
    void ff(const devs::Time& date) { m_state = FF; ffDate(date); }
    void end(const devs::Time& date) { m_state = DONE; doneDate(date); }
    void fail(const devs::Time& date) { m_state = FAILED; doneDate(date); }

    const Rules& rules() const { return m_rules; }
    const DateType& date() const { return m_date; }
    const devs::Time& start() const { return m_start; }
    const devs::Time& finish() const { return m_finish; }
    const devs::Time& minstart() const { return m_minstart; }
    const devs::Time& maxstart() const { return m_maxstart; }
    const devs::Time& minfinish() const { return m_minfinish; }
    const devs::Time& maxfinish() const { return m_maxfinish; }

    const devs::Time& startedDate() const { return m_started; }
    const devs::Time& doneDate() const { return m_done; }
    const devs::Time& ffDate() const { return m_ff; }

    bool waitAllFsBeforeStart() const { return m_waitall; }
    bool waitOnlyOneFsBeforeStart() const { return not m_waitall; }
    void waitAllFs() { m_waitall = true; }
    void waitOnlyOneFs() { m_waitall = false; }

    /**
     * @brief Compute the next date when change in activity status.
     * @param time The current time.
     * @return A date in range ]devs::Time::negativeInfinity,
     * devs::infinity[.
     */
    devs::Time nextTime(const devs::Time& time);

private:
    void startedDate(const devs::Time& date) { m_started = date; }
    void ffDate(const devs::Time& date) { m_ff = date; }
    void doneDate(const devs::Time& date) { m_done = date; }

    State m_state;
    Rules m_rules;

    bool m_waitall; /**< if true, all FF relationship must be valid, if
                      false, only one can be used. */

    DateType m_date;

    devs::Time m_start;
    devs::Time m_finish;
    devs::Time m_minstart;
    devs::Time m_maxstart;
    devs::Time m_minfinish;
    devs::Time m_maxfinish;

    devs::Time m_started; /** Date when the activity is started. */
    devs::Time m_ff; /**< Date when the activity is valid ff. */
    devs::Time m_done; /** Date when the activity is done. */

    AckFct mAckFct;
    OutFct mOutFct;
    UpdateFct mUpdateFct;
};

inline std::ostream& operator<<(
    std::ostream& o, const Activity::State& dt)
{
    return o << "state: " <<
        ((dt == Activity::WAIT) ? "wait" :
         (dt == Activity::STARTED) ? "started" :
         (dt == Activity::FF) ? "started-ff" :
         (dt == Activity::DONE) ? "done" : "failed");
}

inline std::ostream& operator<<(
    std::ostream& out, const Activity& a)
{
    out << a.state() << " " << a.rules() << " Temporal ctr: ";

    switch (a.date() & (Activity::START | Activity::FINISH | Activity::MINS
                        | Activity::MAXS | Activity::MINF |
                        Activity::MAXF)) {
    case Activity::START:
    case Activity::FINISH:
    case Activity::START | Activity::FINISH:
        out << "[" << devs::convertTimeToString(a.start()) << ","
            << devs::convertTimeToString(a.finish()) << "]";
        break;
    case Activity::START | Activity::MAXF:
    case Activity::START | Activity::MINF:
    case Activity::START | Activity::MINF | Activity::MAXF:
        out << "[" << devs::convertTimeToString(a.start()) << ", ["
            << devs::convertTimeToString(a.minfinish()) << ","
            << devs::convertTimeToString(a.maxfinish()) << "]]";
        break;
    case Activity::FINISH | Activity::MAXS:
    case Activity::FINISH | Activity::MINS:
    case Activity::FINISH | Activity::MINS | Activity::MAXS:
        out << "[[" << devs::convertTimeToString(a.minstart()) << ","
            << devs::convertTimeToString(a.maxstart()) << "],"
            << devs::convertTimeToString(a.finish()) << "]";
        break;
    case Activity::MAXF:
    case Activity::MINF:
    case Activity::MINF | Activity::MAXF:
    case Activity::MAXS:
    case Activity::MAXS | Activity::MAXF:
    case Activity::MAXS | Activity::MINF:
    case Activity::MAXS | Activity::MINF | Activity::MAXF:
    case Activity::MINS:
    case Activity::MINS | Activity::MAXF:
    case Activity::MINS | Activity::MINF:
    case Activity::MINS | Activity::MINF | Activity::MAXF:
    case Activity::MINS | Activity::MAXS:
    case Activity::MINS | Activity::MAXS | Activity::MAXF:
    case Activity::MINS | Activity::MAXS | Activity::MINF:
    case Activity::MINS | Activity::MAXS | Activity::MINF | Activity::MAXF:
        out << "[["
            << devs::convertTimeToString(a.minstart()) << ","
            << devs::convertTimeToString(a.maxstart()) << "],["
            << devs::convertTimeToString(a.minfinish()) << ","
            << devs::convertTimeToString(a.maxfinish()) << "]]";
        break;
    default:
        out << "[internal error, report bug]";
    }

    if (not devs::isNegativeInfinity(a.startedDate())) {
        out << " Started at: (" << devs::convertTimeToString(a.startedDate())<< ")";
    }

    if (not devs::isNegativeInfinity(a.ffDate())) {
        out << " FF valid at: (" << devs::convertTimeToString(a.ffDate()) << ")";
    }

    if (not devs::isNegativeInfinity(a.doneDate())) {
        out << " Done at: (" << devs::convertTimeToString(a.doneDate()) << "))";
    }

    return out;
}

}}} // namespace vle model decision

#endif
