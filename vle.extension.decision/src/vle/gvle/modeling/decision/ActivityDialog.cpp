/*
 * @file vle/gvle/modeling/decision/ActivityDialog.cpp
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

#include <vle/gvle/modeling/decision/ActivityDialog.hpp>
#include <vle/gvle/modeling/decision/Utils.hpp>

namespace vle {
namespace gvle {
namespace modeling {
namespace decision {

ActivityDialog::ActivityDialog(const Glib::RefPtr < Gtk::Builder >& xml,
            Decision* decision, const ActivityModel* activityModel,
            std::map < std::string, strings_t > rule,
            strings_t outputFunctionName,
            strings_t ackFunctionName) :
    mXml(xml), mDecision(decision), mActivityModel(activityModel),
    mRule(rule), mOutput(outputFunctionName), mAck(ackFunctionName)
{
    xml->get_widget("TreeViewActRule", mTreeViewActRules);
    mRefTreeActRules = Gtk::TreeStore::create(mColumnsActRules);
    mTreeViewActRules->set_model(mRefTreeActRules);
    mTreeViewActRules->append_column("Rules", mColumnsActRules.m_col_name);
    xml->get_widget("TreeViewPlanRule", mTreeViewPlanRules);
    mRefTreePlanRules = Gtk::TreeStore::create(mColumnsPlanRules);
    mTreeViewPlanRules->set_model(mRefTreePlanRules);
    mTreeViewPlanRules->append_column("Rules", mColumnsPlanRules.m_col_name);

    xml->get_widget("TreeViewActOutput", mTreeViewActOut);
    mRefTreeActOut = Gtk::TreeStore::create(mColumnsActOut);
    mTreeViewActOut->set_model(mRefTreeActOut);
    mTreeViewActOut->append_column("Outputs", mColumnsActOut.m_col_name);
    xml->get_widget("TreeViewOutput", mTreeViewOutput);
    mRefTreeOutput = Gtk::TreeStore::create(mColumnsOutput);
    mTreeViewOutput->set_model(mRefTreeOutput);
    mTreeViewOutput->append_column("Outputs", mColumnsOutput.m_col_name);

    xml->get_widget("TreeViewActAck", mTreeViewActAck);
    mRefTreeActAck = Gtk::TreeStore::create(mColumnsActAck);
    mTreeViewActAck->set_model(mRefTreeActAck);
    mTreeViewActAck->append_column("Acks", mColumnsActAck.m_col_name);
    xml->get_widget("TreeViewAck", mTreeViewAck);
    mRefTreeAck = Gtk::TreeStore::create(mColumnsAck);
    mTreeViewAck->set_model(mRefTreeAck);
    mTreeViewAck->append_column("Acks", mColumnsAck.m_col_name);

    xml->get_widget("ActivityDialog", mDialog);
    xml->get_widget("ActivityNameEntry", mNameEntry);
    xml->get_widget("PlaceStatusImage", mStatusName);
    xml->get_widget("ActivityOkButton", mOkButton);
    xml->get_widget("ActivityCalendarStartButton", mCalendarStartButton);
    xml->get_widget("ActivityCalendarFinishButton", mCalendarFinishButton);
    xml->get_widget("ActivityMinStart", mDateStartEntry);
    xml->get_widget("ActivityMaxFinish", mDateFinishEntry);
    xml->get_widget("ActivityAddRule", mAddButton);
    xml->get_widget("ActivityDelRule", mDelButton);
    xml->get_widget("ActivityAddOutput", mAddButtonOut);
    xml->get_widget("ActivityDelOutput", mDelButtonOut);
    xml->get_widget("ActivityAddAck", mButtonAddAck);
    xml->get_widget("ActivityDelAck", mButtonDelAck);
    xml->get_widget("buttonRelativeDate", mRelativeButton);
    initActivityDialogActions();
}

ActivityDialog::ActivityDialog(const Glib::RefPtr < Gtk::Builder >& xml,
            Decision* decision,
            std::map < std::string, strings_t > rule,
            strings_t outputFunctionName,
            strings_t ackFunctionName) :
    mXml(xml), mDecision(decision)
{
    mOutput = outputFunctionName;
    mAck = ackFunctionName;
    mRule = rule;

    xml->get_widget("TreeViewActRule", mTreeViewActRules);
    mRefTreeActRules = Gtk::TreeStore::create(mColumnsActRules);
    mTreeViewActRules->set_model(mRefTreeActRules);
    mTreeViewActRules->append_column("Rules", mColumnsActRules.m_col_name);
    xml->get_widget("TreeViewPlanRule", mTreeViewPlanRules);
    mRefTreePlanRules = Gtk::TreeStore::create(mColumnsPlanRules);
    mTreeViewPlanRules->set_model(mRefTreePlanRules);
    mTreeViewPlanRules->append_column("Rules", mColumnsPlanRules.m_col_name);

    xml->get_widget("TreeViewActOutput", mTreeViewActOut);
    mRefTreeActOut = Gtk::TreeStore::create(mColumnsActOut);
    mTreeViewActOut->set_model(mRefTreeActOut);
    mTreeViewActOut->append_column("Outputs", mColumnsActOut.m_col_name);
    xml->get_widget("TreeViewOutput", mTreeViewOutput);
    mRefTreeOutput = Gtk::TreeStore::create(mColumnsOutput);
    mTreeViewOutput->set_model(mRefTreeOutput);
    mTreeViewOutput->append_column("Outputs", mColumnsOutput.m_col_name);

    xml->get_widget("TreeViewActAck", mTreeViewActAck);
    mRefTreeActAck = Gtk::TreeStore::create(mColumnsActAck);
    mTreeViewActAck->set_model(mRefTreeActAck);
    mTreeViewActAck->append_column("Acks", mColumnsActAck.m_col_name);
    xml->get_widget("TreeViewAck", mTreeViewAck);
    mRefTreeAck = Gtk::TreeStore::create(mColumnsAck);
    mTreeViewAck->set_model(mRefTreeAck);
    mTreeViewAck->append_column("Acks", mColumnsAck.m_col_name);

    xml->get_widget("ActivityDialog", mDialog);
    xml->get_widget("ActivityNameEntry", mNameEntry);
    xml->get_widget("PlaceStatusImage", mStatusName);
    xml->get_widget("ActivityOkButton", mOkButton);
    xml->get_widget("ActivityCalendarStartButton", mCalendarStartButton);
    xml->get_widget("ActivityCalendarFinishButton", mCalendarFinishButton);
    xml->get_widget("ActivityMinStart", mDateStartEntry);
    xml->get_widget("ActivityMaxFinish", mDateFinishEntry);
    xml->get_widget("ActivityAddRule", mAddButton);
    xml->get_widget("ActivityDelRule", mDelButton);
    xml->get_widget("ActivityAddOutput", mAddButtonOut);
    xml->get_widget("ActivityDelOutput", mDelButtonOut);
    xml->get_widget("ActivityAddAck", mButtonAddAck);
    xml->get_widget("ActivityDelAck", mButtonDelAck);
    xml->get_widget("buttonRelativeDate", mRelativeButton);
    initActivityDialogActions();
}

void ActivityDialog::initActivityDialogActions() {
    //Selections
    mSrcSelect = mTreeViewPlanRules->get_selection();
    mSrcSelect->set_mode(Gtk::SELECTION_MULTIPLE);
    mDstSelect = mTreeViewActRules->get_selection();
    mDstSelect->set_mode(Gtk::SELECTION_MULTIPLE);
    mSrcOutSelect = mTreeViewOutput->get_selection();
    mDstOutSelect = mTreeViewActOut->get_selection();
    mSrcAckSelect = mTreeViewAck->get_selection();
    mDstAckSelect = mTreeViewActAck->get_selection();

    mOkButton->set_sensitive(true);
    mStatusName->set(Gtk::StockID(Gtk::Stock::NO),
                         Gtk::IconSize(1));
    mList.push_back(mNameEntry->signal_changed().connect(
            sigc::mem_fun(*this, &ActivityDialog::onChangeName)));
    mList.push_back(mCalendarStartButton->signal_clicked().connect(
            sigc::mem_fun(*this, &ActivityDialog::onCalendarStart)));
    mList.push_back(mCalendarFinishButton->signal_clicked().connect(
            sigc::mem_fun(*this, &ActivityDialog::onCalendarFinish)));
    mList.push_back(mAddButton->signal_clicked().connect(
            sigc::mem_fun(*this, &ActivityDialog::on_add_rule)));
    mList.push_back(mDelButton->signal_clicked().connect(
            sigc::mem_fun(*this, &ActivityDialog::on_del_rule)));
    mList.push_back(mAddButtonOut->signal_clicked().connect(
            sigc::mem_fun(*this, &ActivityDialog::on_add_output)));
    mList.push_back(mDelButtonOut->signal_clicked().connect(
            sigc::mem_fun(*this, &ActivityDialog::on_del_output)));
    mList.push_back(mButtonAddAck->signal_clicked().connect(
            sigc::mem_fun(*this, &ActivityDialog::on_add_ack)));
    mList.push_back(mButtonDelAck->signal_clicked().connect(
            sigc::mem_fun(*this, &ActivityDialog::on_del_ack)));
    mList.push_back(mRelativeButton->signal_clicked().connect(
            sigc::mem_fun(*this, &ActivityDialog::on_set_rel_date)));
    mList.push_back(mDateStartEntry->signal_changed().connect(
            sigc::mem_fun(*this, &ActivityDialog::onDateRangeChange)));
    mList.push_back(mDateFinishEntry->signal_changed().connect(
            sigc::mem_fun(*this, &ActivityDialog::onDateRangeChange)));

    makePlanRules();
    makeOutput();
    makeAck();
}

void ActivityDialog::onDateRangeChange()
{
    std::string start = mDateStartEntry->get_text();
    std::string finish = mDateFinishEntry->get_text();
    int istart = 0, ifinish = 0 ;
    double dstart = .0, dfinish = .0;

    if (start != "") {
        if ( mIsRelativeDate) {
            try {
                istart = boost::lexical_cast<int>(start);
                mOkButton->set_sensitive(true);
            } catch (...) {
                mOkButton->set_sensitive(false);
            }
        } else {
            try {
                dstart = vle::utils::DateTime::toJulianDayNumber(start);
                mOkButton->set_sensitive(true);
            } catch (...) {
                mOkButton->set_sensitive(false);
            }
        }
    }

    if (finish != "") {
        if ( mIsRelativeDate) {
            try {
                ifinish = boost::lexical_cast<int>(finish);
                mOkButton->set_sensitive(true);
            } catch (...) {
                mOkButton->set_sensitive(false);
            }
        } else {
            try {
                dfinish = vle::utils::DateTime::toJulianDayNumber(finish);
                mOkButton->set_sensitive(true);
            } catch (...) {
                mOkButton->set_sensitive(false);
            }
        }
    }

    if (start != "" && finish != "") {
        if ( mIsRelativeDate) {
            if ( istart < ifinish ) {
                mOkButton->set_sensitive(true);
            } else {
                mOkButton->set_sensitive(false);
            }
        } else {
            if ( dstart < dfinish ) {
                mOkButton->set_sensitive(true);
            } else {
                mOkButton->set_sensitive(false);
            }
        }
    }
}

void ActivityDialog::onChangeName()
{
    std::string entryName = name();

    if (entryName.empty()) {
        mStatusName->set(Gtk::StockID(Gtk::Stock::NO),
                         Gtk::IconSize(1));
        mOkButton->set_sensitive(false);
    } else {
        if (not(mDecision->existActivityModel(entryName)) &&
            Utils::isValidName(entryName)) {

            mStatusName->set(Gtk::StockID(Gtk::Stock::YES),
                             Gtk::IconSize(1));
            mOkButton->set_sensitive(true);
        }
        else if (mDecision->existActivityModel(entryName) &&
            Utils::isValidName(entryName) && mOriginalName == entryName) {

            mStatusName->set(Gtk::StockID(Gtk::Stock::YES),
                             Gtk::IconSize(1));
            mOkButton->set_sensitive(true);
        }
        else {
            mStatusName->set(Gtk::StockID(Gtk::Stock::NO),
                             Gtk::IconSize(1));
            mOkButton->set_sensitive(false);
        }
    }
}

void ActivityDialog::onCalendarStart()
{
    vle::gvle::CalendarBox cal(mXml);
    std::string date;
    std::string dateFromField = mDateStartEntry->get_text();

    if (dateFromField != "") {
        long year, month, day, hours, minutes, seconds;
        try {
            double ms = vle::utils::DateTime::toJulianDayNumber
                (mDateStartEntry->get_text());
            utils::DateTime::toTime(ms, year, month, day, hours, minutes, seconds);

            cal.selectDate(day, month, year);
        } catch (...) {
        }
    }

    cal.dateBegin(date);
    if (not date.empty()) {
        mDateStartEntry->set_text(date);
    }
}

void ActivityDialog::onCalendarFinish()
{
    vle::gvle::CalendarBox cal(mXml);
    std::string date;
    std::string dateFromField = mDateFinishEntry->get_text();

    if (dateFromField != "") {
        long year, month, day, hours, minutes, seconds;
        try {
            double ms = vle::utils::DateTime::toJulianDayNumber
                (mDateFinishEntry->get_text());
            utils::DateTime::toTime(ms, year, month, day, hours, minutes, seconds);

            cal.selectDate(day, month, year);
        } catch (...) {
        }
    }

    cal.dateBegin(date);
    if (not date.empty()) {
        mDateFinishEntry->set_text(date);
    }
}

int ActivityDialog::run()
{
    onSetRelDate(mActivityModel->getRelativeDate());
    mNameEntry->set_text(mActivityModel->name());
    mOriginalName = mActivityModel->name();

    if (mIsRelativeDate) {
        mDateStartEntry->set_text(mActivityModel->minstart());
        mDateFinishEntry->set_text(mActivityModel->maxfinish());
    }
    else {
        if (mActivityModel->minstart() != "") {
            double x = vle::utils::convert < double >(
                mActivityModel->minstart(),
                true);
            mDateStartEntry->set_text(utils::DateTime::toJulianDayNumber(x));
        }
        if (mActivityModel->maxfinish() != "") {
            double x = vle::utils::convert < double >(
                mActivityModel->maxfinish(),
                true);
            mDateFinishEntry->set_text(utils::DateTime::toJulianDayNumber(x));
        }
    }

    mActRule = mActivityModel->getRules();
    makeActRules();
    mActOut = mActivityModel->getOutputFunc();
    makeActOutput();
    mActAck = mActivityModel->getAckFunc();
    makeActAck();

    mDialog->set_default_response(Gtk::RESPONSE_ACCEPT);
    mOkButton->set_sensitive(true);
    onChangeName();
    mDialog->set_title(mActivityModel->name() + " Activity");
    int response = mDialog->run();
    mDialog->hide();
    return response;
}

void ActivityDialog::makePlanRules()
{
    mRefTreePlanRules->clear();
    std::map < std::string, strings_t >::const_iterator it;
    it = mRule.begin();
    while (it != mRule.end()) {
        Gtk::TreeModel::Row row = *(mRefTreePlanRules->append());
        row[mColumnsPlanRules.m_col_name] = it->first;
        ++it;
    }
}

void ActivityDialog::makeActRules()
{
    mRefTreeActRules->clear();
    strings_t::const_iterator it;
    it = mActRule.begin();
    while (it != mActRule.end()) {
        if (mRule.find(*it) != mRule.end()) {
            Gtk::TreeModel::Row row = *(mRefTreeActRules->append());
            row[mColumnsActRules.m_col_name] = *it;
        }
        ++it;
    }
}

void ActivityDialog::makeOutput()
{
    mRefTreeOutput->clear();
    strings_t::const_iterator it = mOutput.begin();
    while (it != mOutput.end()) {
        Gtk::TreeModel::Row row = *(mRefTreeOutput->append());
        row[mColumnsOutput.m_col_name] = *it;
        ++it;
    }
}

void ActivityDialog::makeAck()
{
    mRefTreeAck->clear();
    strings_t::const_iterator it = mAck.begin();
    while (it != mAck.end()) {
        Gtk::TreeModel::Row row = *(mRefTreeAck->append());
        row[mColumnsAck.m_col_name] = *it;
        ++it;
    }
}

void ActivityDialog::makeActOutput()
{
    mRefTreeActOut->clear();
    strings_t::const_iterator it;
    it = mActOut.begin();
    while (it != mActOut.end()) {
        Gtk::TreeModel::Row row = *(mRefTreeActOut->append());
        row[mColumnsActOut.m_col_name] = *it;
        ++it;
    }
}

void ActivityDialog::makeActAck()
{
    mRefTreeActAck->clear();
    strings_t::const_iterator it;
    it = mActAck.begin();
    while (it != mActAck.end()) {
        Gtk::TreeModel::Row row = *(mRefTreeActAck->append());
        row[mColumnsActAck.m_col_name] = *it;
        ++it;
    }
}

void ActivityDialog::on_add_rule()
{
    using namespace Gtk;

    TreeSelection::ListHandle_Path lstSrc = mSrcSelect->get_selected_rows();
    Glib::RefPtr < TreeModel > modelSrc = mTreeViewPlanRules->get_model();

    for (TreeSelection::ListHandle_Path::iterator iSrc = lstSrc.begin();
            iSrc != lstSrc.end(); ++iSrc) {
        TreeModel::Row rowSrc(*(modelSrc->get_iter(*iSrc)));
        std::string data_name(rowSrc.get_value(mColumnsPlanRules.m_col_name));
        if (std::find(mActRule.begin(), mActRule.end(), data_name) ==
                mActRule.end()) {
            mActRule.push_back(data_name);
        }
    }
    makeActRules();
}

void ActivityDialog::on_del_rule()
{
    using namespace Gtk;

    TreeSelection::ListHandle_Path lstSrc = mDstSelect->get_selected_rows();
    Glib::RefPtr < TreeModel > modelSrc = mTreeViewActRules->get_model();

    for (TreeSelection::ListHandle_Path::iterator iSrc = lstSrc.begin();
            iSrc != lstSrc.end(); ++iSrc) {
        TreeModel::Row rowSrc(*(modelSrc->get_iter(*iSrc)));
        std::string data_name(rowSrc.get_value(mColumnsActRules.m_col_name));

        // Delete the element in the vector
        for (std::vector < std::string > ::iterator it =
                mActRule.begin(); it != mActRule.end(); ) {
            if ( *it == data_name ) {
                it = mActRule.erase(it);
            }
            else {
                ++it;
            }
        }
    }
    makeActRules();
}

void ActivityDialog::on_add_output()
{
    using namespace Gtk;

    TreeSelection::ListHandle_Path lstSrc = mSrcOutSelect->get_selected_rows();
    Glib::RefPtr < TreeModel > modelSrc = mTreeViewOutput->get_model();

    if (mActOut.empty()) {
        for (TreeSelection::ListHandle_Path::iterator iSrc = lstSrc.begin();
                iSrc != lstSrc.end(); ++iSrc) {
            TreeModel::Row rowSrc(*(modelSrc->get_iter(*iSrc)));
            std::string data_name(rowSrc.get_value(mColumnsOutput.m_col_name));
            if (std::find(mActOut.begin(), mActOut.end(), data_name) ==
                    mActOut.end()) {
                mActOut.push_back(data_name);
            }
        }
    }
    makeActOutput();
}

void ActivityDialog::on_del_output()
{
    using namespace Gtk;

    TreeSelection::ListHandle_Path lstSrc = mDstOutSelect->get_selected_rows();
    Glib::RefPtr < TreeModel > modelSrc = mTreeViewActOut->get_model();

    for (TreeSelection::ListHandle_Path::iterator iSrc = lstSrc.begin();
            iSrc != lstSrc.end(); ++iSrc) {
        TreeModel::Row rowSrc(*(modelSrc->get_iter(*iSrc)));
        std::string data_name(rowSrc.get_value(mColumnsActOut.m_col_name));

        // Delete the element in the vector
        for (std::vector < std::string > ::iterator it =
                mActOut.begin(); it != mActOut.end(); ) {
            if ( *it == data_name ) {
                it = mActOut.erase(it);
            }
            else {
                ++it;
            }
        }
    }
    makeActOutput();
}

void ActivityDialog::on_add_ack()
{
    using namespace Gtk;

    TreeSelection::ListHandle_Path lstSrc = mSrcAckSelect->get_selected_rows();
    Glib::RefPtr < TreeModel > modelSrc = mTreeViewAck->get_model();

    if (mActAck.empty()) {
        for (TreeSelection::ListHandle_Path::iterator iSrc = lstSrc.begin();
                iSrc != lstSrc.end(); ++iSrc) {
            TreeModel::Row rowSrc(*(modelSrc->get_iter(*iSrc)));
            std::string data_name(rowSrc.get_value(mColumnsAck.m_col_name));
            if (std::find(mActAck.begin(), mActAck.end(), data_name) ==
                    mActAck.end()) {
                mActAck.push_back(data_name);
            }
        }
    }
    makeActAck();
}

void ActivityDialog::on_del_ack()
{
    using namespace Gtk;
    mActAck.clear();
    makeActAck();
}

void ActivityDialog::onSetRelDate(bool state)
{
    std::string start = mDateStartEntry->get_text();
    std::string finish = mDateFinishEntry->get_text();
    int istart, ifinish;
    double dstart, dfinish;

    mIsRelativeDate = state;

    if (state) {
        mRelativeButton->set_inconsistent(false);
        mCalendarStartButton->set_sensitive(false);
        mCalendarFinishButton->set_sensitive(false);

        try {
            dstart = vle::utils::DateTime::toJulianDayNumber(start);
            dfinish = vle::utils::DateTime::toJulianDayNumber(finish);

            mDateStartEntry->set_text(boost::lexical_cast<string>((int)dstart));
            mDateFinishEntry->set_text(boost::lexical_cast<string>((int)dfinish));
        } catch (...) {}
    }
    else {
        mRelativeButton->set_inconsistent(true);
        mCalendarStartButton->set_sensitive(true);
        mCalendarFinishButton->set_sensitive(true);

        try {
            istart = boost::lexical_cast<int>(start);
            ifinish = boost::lexical_cast<int>(finish);

            mDateStartEntry->set_text(
                vle::utils::DateTime::toJulianDay((long)istart));
            mDateFinishEntry->set_text(
                vle::utils::DateTime::toJulianDay((long)ifinish));

        } catch (...) {}

    }
}

}
}
}
}    // namespace vle gvle modeling decision
