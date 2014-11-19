/*
 * @file vle/gvle/modeling/decision/ActivityDialog.hpp
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

#ifndef VLE_GVLE_MODELING_DECISION_ACTIVITY_DIALOG_HPP
#define VLE_GVLE_MODELING_DECISION_ACTIVITY_DIALOG_HPP

#include <vle/gvle/modeling/decision/Decision.hpp>
#include <vle/gvle/CalendarBox.hpp>
#include <vle/utils/DateTime.hpp>
#include <vle/utils/Tools.hpp>
#include <boost/algorithm/string.hpp>
#include <boost/algorithm/string/split.hpp>
#include <gtkmm.h>

namespace vle {
namespace gvle {
namespace modeling {
namespace decision {

typedef std::vector < std::string > strings_t;

/**
 * @class ActivityDialog
 * @brief This class is used to edit the activities.
 */

class ActivityDialog
{
public:
/**
 * @brief Constructor of the ActivityDialog
 * @param reference to the xml builder
 * @param reference to the Decision model
 * @param reference to the selected ActivityModel
 * @param reference to a rule vector
 * @param reference to a output functions vector
 * @param reference to a acknowledge functions vector
 */
    ActivityDialog(const Glib::RefPtr < Gtk::Builder >& xml,
                Decision* decision, const ActivityModel* activityModel,
                std::map < std::string, strings_t > rule,
                strings_t, strings_t);

/**
 * @brief Constructor of the ActivityDialog
 * @param reference to the xml builder
 * @param reference to the Decision model
 * @param reference to a rule vector
 * @param reference to a output functions vector
 * @param reference to a acknowledge functions vector
 */
    ActivityDialog(const Glib::RefPtr < Gtk::Builder >& xml,
                Decision* decision,
                std::map < std::string, strings_t > rule,
                strings_t, strings_t);

/**
 * @brief Destructor of the class
 */
    virtual ~ActivityDialog()
    {
        for (std::list < sigc::connection >::iterator it = mList.begin();
             it != mList.end(); ++it) {
            it->disconnect();
        }
        mTreeViewActRules->remove_all_columns();
        mTreeViewPlanRules->remove_all_columns();
        mTreeViewOutput->remove_all_columns();
        mTreeViewActOut->remove_all_columns();
        mTreeViewAck->remove_all_columns();
        mTreeViewActAck->remove_all_columns();
    }

/**
 * @brief Return the name entered into the mNameEntry
 * @return the name into the text input
 */
    std::string name() const
    {
        std::string entryName = mNameEntry->get_text();
        boost::trim(entryName);
        return entryName;
    }

/**
 * @brief Return the minstart entered into the mDateStartEntry
 * @return the date into the text input
 */
    std::string minstart() const
    {
        if (mDateStartEntry->get_text() == "" ||  isRelativeDate()) {
            return mDateStartEntry->get_text();
        }

        try {
            double ms = vle::utils::DateTime::toJulianDayNumber
                (mDateStartEntry->get_text());
            return (utils::toScientificString(ms));
        } catch (...) {
            std::string entryText = mDateStartEntry->get_text();
            boost::trim(entryText);
            return entryText;
        }
    }

/**
 * @brief Return the maxfinish entered into the mDateFinishEntry
 * @return the date into the text input
 */
    std::string maxfinish() const
    {
        if (mDateFinishEntry->get_text() == "" || isRelativeDate()) {
            return mDateFinishEntry->get_text();
        }

        try {
            double mf = vle::utils::DateTime::toJulianDayNumber
                (mDateFinishEntry->get_text());
            return (utils::toScientificString(mf));
        } catch (...) {
            std::string entryText = mDateFinishEntry->get_text();
            boost::trim(entryText);
            return entryText;
        }
    }

/**
 * @brief Launch the window
 */
    int run();

    class ModelColumns : public Gtk::TreeModel::ColumnRecord
    {
    public:
        ModelColumns()
        {
            add(m_col_name);
        }
        Gtk::TreeModelColumn < std::string > m_col_name;
    };

    strings_t getActRule() {
        return mActRule;
    }

    strings_t getActOut() {
        return mActOut;
    }

    strings_t getActAck() {
        return mActAck;
    }

    bool isRelativeDate() const
    {
        return mRelativeButton->get_active();
    }

    bool explodeNonGregorianHumanDate(const std::string &date, long &year, long &month, long &day, double &arbitraryDate) const
    {
        std::string humanDate;
        strings_t dateItems;
        try {
            boost::split(dateItems,
                         date,
                         boost::is_any_of("-"));

            year =  boost::lexical_cast<int>(dateItems.at(0));
            month =  boost::lexical_cast<int>(dateItems.at(1));
            day =  boost::lexical_cast<int>(dateItems.at(2));
            std::string yS = boost::lexical_cast<std::string>(year + 1400);
            std::string mS = boost::lexical_cast<std::string>(month);
            std::string dS = boost::lexical_cast<std::string>(day);

            humanDate = yS + "-" + mS + "-" + dS;

            arbitraryDate = utils::DateTime::toJulianDayNumber(humanDate);
            return true;
        } catch (...) {
            return false;
        }
    }

    bool isHumanDate(const std::string &date) const
    {
        long year, month, day;
        double numeric;
        return explodeNonGregorianHumanDate(date, year, month, day, numeric);
    }

    bool relativeHumanDateLessThan(const std::string &date1,
                                   const std::string &date2) const
    {
        long year, month, day;
        double arbitrary1, arbitrary2;

        explodeNonGregorianHumanDate(date1, year, month, day, arbitrary1);
        explodeNonGregorianHumanDate(date2, year, month, day, arbitrary2);

        return arbitrary1 < arbitrary2;
    }


    bool is29OfFebruary(const std::string &date) const
    {
        long year, month, day;
        double numeric;
        explodeNonGregorianHumanDate(date, year, month, day, numeric);

        return month == 2 and day == 29;
    }

    bool isHumanDate() const
    {
        std::string start = mDateStartEntry->get_text();
        std::string finish = mDateStartEntry->get_text();

        return isHumanDate(start) || isHumanDate(finish);
    }


protected:
/**
 * @brief Function called when the name into the mNameEntry is modified.
 * Check if the name doesn't already exists and if it is valid.
 * Desactive the valid button on the window if the name isn't correct.
 */
    void onChangeName();

    void onDateRangeChange();

/**
 * @brief Function called when the user clicked on the Start activity calendar button.
 */
    void onCalendarStart();

/**
 * @brief Function called when the user clicked on the Finish activity calendar button.
 */
    void onCalendarFinish();

    Glib::RefPtr < Gtk::Builder > mXml;
    Decision* mDecision;
    const ActivityModel* mActivityModel;

    Gtk::Dialog* mDialog;
    Gtk::Entry* mNameEntry;
    Gtk::Entry* mDateStartEntry;
    Gtk::Entry* mDateFinishEntry;
    Gtk::Image* mStatusName;
    Gtk::Button* mOkButton;
    Gtk::Button* mCalendarStartButton;
    Gtk::Button* mCalendarFinishButton;
    Gtk::Button* mAddButton;
    Gtk::Button* mDelButton;
    Gtk::ToggleButton* mRelativeButton;

    Gtk::Button* mAddButtonOut;
    Gtk::Button* mDelButtonOut;

    std::list < sigc::connection > mList;

// Store the original ActivityModel name
    std::string mOriginalName;
// Store the plan Rules
    std::map < std::string, strings_t > mRule;
// Store the activity Rules
    strings_t mActRule;
// Store the output functions
    strings_t mOutput;
// Store the activity output function
    strings_t mActOut;
// Store the acknowledge functions
    strings_t mAck;
// Store the activity acknowledge function
    strings_t mActAck;

    //Plan Rules
    Gtk::TreeView* mTreeViewPlanRules;
    Glib::RefPtr < Gtk::TreeStore > mRefTreePlanRules;
    ModelColumns mColumnsPlanRules;
    Glib::RefPtr < Gtk::TreeSelection > mSrcSelect;

    //Activity Rules
    Gtk::TreeView* mTreeViewActRules;
    Glib::RefPtr < Gtk::TreeStore > mRefTreeActRules;
    ModelColumns mColumnsActRules;
    Glib::RefPtr < Gtk::TreeSelection > mDstSelect;

    // Output functions
    Gtk::TreeView* mTreeViewOutput;
    Glib::RefPtr < Gtk::TreeStore > mRefTreeOutput;
    ModelColumns mColumnsOutput;
    Glib::RefPtr < Gtk::TreeSelection > mSrcOutSelect;

    //Activity Output
    Gtk::TreeView* mTreeViewActOut;
    Glib::RefPtr < Gtk::TreeStore > mRefTreeActOut;
    ModelColumns mColumnsActOut;
    Glib::RefPtr < Gtk::TreeSelection > mDstOutSelect;

    // Acknowledge functions
    Gtk::TreeView* mTreeViewAck;
    Glib::RefPtr < Gtk::TreeStore > mRefTreeAck;
    ModelColumns mColumnsAck;
    Glib::RefPtr < Gtk::TreeSelection > mSrcAckSelect;

    //Activity acknowledge
    Gtk::TreeView* mTreeViewActAck;
    Glib::RefPtr < Gtk::TreeStore > mRefTreeActAck;
    ModelColumns mColumnsActAck;
    Glib::RefPtr < Gtk::TreeSelection > mDstAckSelect;

    //Buttons
    Gtk::Button* mButtonAdd;
    Gtk::Button* mButtonDelete;
    //Buttons
    Gtk::Button* mButtonAddAck;
    Gtk::Button* mButtonDelAck;

    void makePlanRules();
    void makeActRules();
    void makeOutput();
    void makeActOutput();
    void makeAck();
    void makeActAck();

/**
 * @brief Delete the selected rules in the mTreeViewActRules
 */
    void on_del_rule();

/**
 * @brief Add the selected rules in the mTreeViewPlanRules
 */
    void on_add_rule();

/**
 * @brief Delete the selected outputs in the mTreeViewActOutput
 */
    void on_del_output();

/**
 * @brief Add the selected outputs in the mTreeViewOutput
 */
    void on_add_output();

/**
 * @brief Delete the selected outputs in the mTreeViewActOutput
 */
    void on_del_ack();

/**
 * @brief Add the selected outputs in the mTreeViewOutput
 */
    void on_add_ack();

private:
    void initActivityDialogActions();
};


class NewActivityDialog : public ActivityDialog
{
public:
/**
 * @brief Constructor of the NewsActivityDialog
 * @param reference to the xml builder
 * @param reference to the Decision model
 * @param reference to a rule vector
 */
    NewActivityDialog(const Glib::RefPtr < Gtk::Builder >& xml,
            Decision* decision,
            std::map < std::string, strings_t > rule,
            strings_t outputFunctionName,
            strings_t ackFunctionName) :
        ActivityDialog(xml, decision, rule, outputFunctionName,
                ackFunctionName)
{
}

/**
 * @brief Launch the window
 */
int run()
{
    mNameEntry->set_text("");
    mDateStartEntry->set_text("");
    mDateFinishEntry->set_text("");
    makeActRules();
    mDialog->set_default_response(Gtk::RESPONSE_ACCEPT);
    onChangeName();
    mOkButton->set_sensitive(false);
    mDialog->set_title("New Activity Dialog");
    mRelativeButton->set_active(false);
    int response = mDialog->run();
    mDialog->hide();
    return response;
}
};

}
}
}
}    // namespace vle gvle modeling decision

#endif
