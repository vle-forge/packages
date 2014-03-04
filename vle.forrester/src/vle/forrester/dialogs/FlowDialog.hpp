/*
 * @file vle/forrester/dialogs/FlowDialog.hpp
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

#ifndef VLE_GVLE_MODELING_FORRESTER_FLOW_DIALOG_HPP
#define VLE_GVLE_MODELING_FORRESTER_FLOW_DIALOG_HPP

#include <vle/forrester/Forrester.hpp>
#include <vle/forrester/graphicalItems/Flow.hpp>
#include <vle/forrester/graphicalItems/Variable.hpp>
#include <vle/forrester/utilities/ComputeParser.hpp>

#include <gtkmm/dialog.h>
#include <gtkmm/entry.h>
#include <gtkmm/spinbutton.h>
#include <gtkmm/builder.h>
#include <gtkmm/image.h>
#include <boost/algorithm/string.hpp>
#include <gtkmm/stock.h>
#include <gtkmm/treeview.h>
#include <gtkmm/liststore.h>
#include <gtkmm/statusbar.h>
#include <gtkmm/checkbutton.h>

namespace vle {
namespace gvle {
namespace modeling {
namespace forrester {

/**
 * @class FlowDialog
 * @brief Launch a new window to modify a flow of the Forrester diagram
 */
class FlowDialog
{
public:
    FlowDialog(const Glib::RefPtr < Gtk::Builder >& xml,
               const Forrester& forrester_,
               const std::string title);

    FlowDialog(const Glib::RefPtr < Gtk::Builder >& xml,
               const Flow& flow, const Forrester& forrester_);

    FlowDialog(const Glib::RefPtr < Gtk::Builder >& xml,
               const Variable& var, const Forrester& forrester_);

    virtual ~FlowDialog();

    std::string getName() const
    { return mNameEntry->get_text(); }

    std::string getValue() const
    { return mValueEntry->get_text(); }

    std::string getPredicate() const
    { return mPredicateEntry->get_text(); }

    std::string getTrueValue() const
    { return mValueIfTrue->get_text(); }

    std::string getFalseValue() const
    { return mValueIfFalse->get_text(); }

    bool isConditionnal() const
    { return mConditionnalButton->get_active(); }

    int run ();

private:
    //Callback functions
    void onTextChange ();
    void onConditionnalityChanged();
    void onButtonReleasedCompartmentTreeView();
    void onButtonReleasedVarParaTreeView();
    void onAritmeticButtonReleased(Gtk::Button*);
    void onFunctionButtonReleased(Gtk::Button*);
    void onTimeButtonPressed();

    //Initialization functions
    void init(const Glib::RefPtr < Gtk::Builder >&);
    void initCompartmentList();
    void initParVarList();

    void insertInLastEntry(const std::string& str,int index);
    void checkFields();

    bool setLastActivatedEntry(GdkEventFocus*);

    //Gtk elements
    Gtk::Dialog* mDialog;
    Gtk::Entry* mNameEntry;
    Gtk::Image* mOkImage;
    Gtk::Button* mOkButton;
    Gtk::CheckButton* mConditionnalButton;
    Gtk::Entry* mValueEntry;
    Gtk::Entry* mPredicateEntry;
    Gtk::Entry* mValueIfTrue;
    Gtk::Entry* mValueIfFalse;
    Gtk::Entry* mLastActivatedEntry;
    Gtk::TreeView* mCompartmentsTreeView;
    Gtk::TreeView* mVarParaTreeView;
    Gtk::Button* mAdditionButton;
    Gtk::Button* mSubstractionButton;
    Gtk::Button* mMultiplicationButton;
    Gtk::Button* mDivisionButton;
    Gtk::Button* mOpeningParenthsisButton;
    Gtk::Button* mClosingParenthsisButton;
    Gtk::Button* mTimeButton;

    Gtk::Button* mCosButton;
    Gtk::Button* mSinButton;
    Gtk::Button* mTanButton;
    Gtk::Button* mExpButton;
    Gtk::Button* mSqrtButton;
    Gtk::Button* mAbsButton;

    Gtk::Statusbar* mStatusBar;

    //Gtk elements > tree views
    std::vector<sigc::connection> connectionsHandlers;

    struct ViewsColumnRecord : public Gtk::TreeModel::ColumnRecord
    {
        Gtk::TreeModelColumn < std::string > name;
        ViewsColumnRecord() {
            add(name);
        }
    } m_viewscolumnrecord;

    Glib::RefPtr < Gtk::ListStore > mCompartmentModel;
    int mCompartmentColumnName;
    Gtk::CellRendererText* mCompartmentCellRenderer;

    Glib::RefPtr < Gtk::ListStore > mParVarModel;
    int mParVarColumnName;
    Gtk::CellRendererText* mParVarCellRenderer;

    //members
    std::string mOldName;
    const Forrester &mForrester;
    utilities::ComputeParser mParser;

    std::string mTitle;
};

}
}
}
}
#endif
