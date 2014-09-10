/*
 * @file vle/forrester/dialogs/FlowDialog.cpp
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

#include <vle/forrester/dialogs/FlowDialog.hpp>
#include <vle/forrester/graphicalItems/DependencyArrow.hpp>
#include <vle/forrester/graphicalItems/Compartment.hpp>
#include <vle/forrester/graphicalItems/Parameter.hpp>
#include <vle/forrester/graphicalItems/Variable.hpp>
#include <vle/forrester/utilities/Tokenizer.hpp>
#include <boost/lexical_cast.hpp>

namespace vle {
namespace gvle {
namespace modeling {
namespace forrester {

FlowDialog::FlowDialog(const Glib::RefPtr < Gtk::Builder >& xml,
                       const Forrester& forrester_,
                       const std::string title):
    mOldName(""),
    mForrester(forrester_),
    mTitle(title)
{
    Flow emptyFlow(forrester_);
    init(xml);
    onTextChange();
}

FlowDialog::FlowDialog(const Glib::RefPtr < Gtk::Builder >& xml,
                       const Flow& flow,  const Forrester& _forrester):
    mOldName(flow.getName()),
    mForrester(_forrester),
    mTitle("Flow dialog")
{
    init(xml);
    mNameEntry->set_text(mOldName);
    mValueEntry->set_text(flow.getValue());
    mPredicateEntry->set_text(flow.getPredicate());
    mValueIfTrue->set_text(flow.getTrueValue());
    mValueIfFalse->set_text(flow.getFalseValue());
    mConditionnalButton->set_active(flow.isConditionnal());
    onConditionnalityChanged();
    mParser.addConstant("t");
    onTextChange();
}

FlowDialog::FlowDialog(const Glib::RefPtr < Gtk::Builder >& xml,
                       const Variable& var,  const Forrester& _forrester):
    mOldName(var.getName()),
    mForrester(_forrester),
    mTitle("Variable dialog")
{
    init(xml);
    mNameEntry->set_text(mOldName);
    mValueEntry->set_text(var.getValue());
    mPredicateEntry->set_text(var.getPredicate());
    mValueIfTrue->set_text(var.getTrueValue());
    mValueIfFalse->set_text(var.getFalseValue());
    mConditionnalButton->set_active(var.isConditionnal());
    onConditionnalityChanged();
    mParser.addConstant("t");
    onTextChange();
}

int FlowDialog::run()
{
    int response = mDialog->run();

    mDialog->hide();
    return response;
}

void FlowDialog::onTextChange()
{
    std::string entryName = getName();

    mStatusBar->pop();
    try
    {
        checkFields();
        mOkImage->set(Gtk::StockID(Gtk::Stock::YES),
                         Gtk::IconSize(1));
        mOkButton->set_sensitive(true);
        mStatusBar->push("Ok!");
    }
    catch (const std::string& e)
    {
        mOkImage->set(Gtk::StockID(Gtk::Stock::DIALOG_WARNING),
                         Gtk::IconSize(1));
        mOkButton->set_sensitive(false);
        mStatusBar->push(e);
    }
}

void FlowDialog::init(const Glib::RefPtr < Gtk::Builder >& xml)
{
    xml->get_widget("FlowDialog", mDialog);
    xml->get_widget("FlowNameEntry", mNameEntry);
    xml->get_widget("FlowImage",mOkImage);
    xml->get_widget("FlowDialogOk",mOkButton);
    xml->get_widget("FlowValueTextEntry",mValueEntry);

    xml->get_widget("ConditionnalCheckButton", mConditionnalButton);
    xml->get_widget("PredicateEntry",mPredicateEntry);
    xml->get_widget("FlowValueTextEntryTrue",mValueIfTrue);
    xml->get_widget("FlowValueTextEntryFalse",mValueIfFalse);
    xml->get_widget("compartmentTreeView",mCompartmentsTreeView);
    xml->get_widget("varParaTreeView",mVarParaTreeView);

    xml->get_widget("AdditionButton",mAdditionButton);
    xml->get_widget("SubstractionButton",mSubstractionButton);
    xml->get_widget("MultiplicationButton",mMultiplicationButton);
    xml->get_widget("DivisionButton",mDivisionButton);
    xml->get_widget("OpeningParenthesisButton",mOpeningParenthsisButton);
    xml->get_widget("ClosingParenthesisButton",mClosingParenthsisButton);
    xml->get_widget("TimeButton",mTimeButton);

    xml->get_widget("CosButton",mCosButton);
    xml->get_widget("SinButton",mSinButton);
    xml->get_widget("TanButton",mTanButton);
    xml->get_widget("ExpButton",mExpButton);
    xml->get_widget("SqrtButton",mSqrtButton);
    xml->get_widget("AbsButton",mAbsButton);

    xml->get_widget("statusbar1",mStatusBar);

    mNameEntry->set_text("");
    mValueEntry->set_text("");
    mValueIfFalse->set_text("");
    mValueIfTrue->set_text("");
    mPredicateEntry->set_text("");

    mOkImage->set(Gtk::StockID(Gtk::Stock::YES),
                        Gtk::IconSize(1));

    initCompartmentList();
    initParVarList();

    onConditionnalityChanged();
    connectionsHandlers.push_back(mNameEntry->signal_changed().connect(
            sigc::mem_fun(*this, &FlowDialog::onTextChange)));
    connectionsHandlers.push_back(mValueEntry->signal_changed().connect(
            sigc::mem_fun(*this, &FlowDialog::onTextChange)));
    connectionsHandlers.push_back(mConditionnalButton->signal_toggled().connect(
            sigc::mem_fun(*this, &FlowDialog::onConditionnalityChanged)));
    connectionsHandlers.push_back(mPredicateEntry->signal_changed().connect(
            sigc::mem_fun(*this, &FlowDialog::onTextChange)));
    connectionsHandlers.push_back(mValueIfTrue->signal_changed().connect(
            sigc::mem_fun(*this, &FlowDialog::onTextChange)));
    connectionsHandlers.push_back(mValueIfFalse->signal_changed().connect(
            sigc::mem_fun(*this, &FlowDialog::onTextChange)));
    connectionsHandlers.push_back(mCompartmentsTreeView->
        signal_cursor_changed().connect(sigc::mem_fun( *this,
        &FlowDialog::onButtonReleasedCompartmentTreeView)));
    connectionsHandlers.push_back(mVarParaTreeView->
        signal_cursor_changed().connect(sigc::mem_fun( *this,
        &FlowDialog::onButtonReleasedVarParaTreeView)));

    connectionsHandlers.push_back(mValueEntry->signal_focus_in_event().connect(
      sigc::mem_fun(*this, &FlowDialog::setLastActivatedEntry)));
    connectionsHandlers.push_back(mPredicateEntry->signal_focus_in_event().connect(
      sigc::mem_fun(*this, &FlowDialog::setLastActivatedEntry)));
    connectionsHandlers.push_back(mValueIfFalse->signal_focus_in_event().connect(
      sigc::mem_fun(*this, &FlowDialog::setLastActivatedEntry)));
    connectionsHandlers.push_back(mValueIfTrue->signal_focus_in_event().connect(
      sigc::mem_fun(*this, &FlowDialog::setLastActivatedEntry)));

    connectionsHandlers.push_back(mAdditionButton->signal_released().connect(
      sigc::bind<Gtk::Button*>(
      sigc::mem_fun(*this,&FlowDialog::onAritmeticButtonReleased),
      mAdditionButton)));
    connectionsHandlers.push_back(mSubstractionButton->signal_released().connect(
      sigc::bind<Gtk::Button*>(
      sigc::mem_fun(*this,&FlowDialog::onAritmeticButtonReleased),
      mSubstractionButton)));
    connectionsHandlers.push_back(mMultiplicationButton->signal_released().connect(
      sigc::bind<Gtk::Button*>(
      sigc::mem_fun(*this,&FlowDialog::onAritmeticButtonReleased),
      mMultiplicationButton)));
    connectionsHandlers.push_back(mDivisionButton->signal_released().connect(
      sigc::bind<Gtk::Button*>(
      sigc::mem_fun(*this,&FlowDialog::onAritmeticButtonReleased),
      mDivisionButton)));
    connectionsHandlers.push_back(mOpeningParenthsisButton->signal_released().connect(
      sigc::bind<Gtk::Button*>(
      sigc::mem_fun(*this,&FlowDialog::onAritmeticButtonReleased),
      mOpeningParenthsisButton)));
    connectionsHandlers.push_back(mClosingParenthsisButton->signal_released().connect(
      sigc::bind<Gtk::Button*>(
      sigc::mem_fun(*this,&FlowDialog::onAritmeticButtonReleased),
      mClosingParenthsisButton)));

    connectionsHandlers.push_back(mCosButton->signal_released().connect(
      sigc::bind<Gtk::Button*>(
      sigc::mem_fun(*this,&FlowDialog::onFunctionButtonReleased),
      mCosButton)));
    connectionsHandlers.push_back(mSinButton->signal_released().connect(
      sigc::bind<Gtk::Button*>(
      sigc::mem_fun(*this,&FlowDialog::onFunctionButtonReleased),
      mSinButton)));
    connectionsHandlers.push_back(mTanButton->signal_released().connect(
      sigc::bind<Gtk::Button*>(
      sigc::mem_fun(*this,&FlowDialog::onFunctionButtonReleased),
      mTanButton)));
    connectionsHandlers.push_back(mExpButton->signal_released().connect(
      sigc::bind<Gtk::Button*>(
      sigc::mem_fun(*this,&FlowDialog::onFunctionButtonReleased),
      mExpButton)));
    connectionsHandlers.push_back(mAbsButton->signal_released().connect(
      sigc::bind<Gtk::Button*>(
      sigc::mem_fun(*this,&FlowDialog::onFunctionButtonReleased),
      mAbsButton)));
    connectionsHandlers.push_back(mSqrtButton->signal_released().connect(
      sigc::bind<Gtk::Button*>(
      sigc::mem_fun(*this,&FlowDialog::onFunctionButtonReleased),
      mSqrtButton)));

    connectionsHandlers.push_back(mTimeButton->signal_released().connect(
      sigc::mem_fun(*this,&FlowDialog::onTimeButtonPressed)));

    mDialog->set_title(mTitle);
}

void FlowDialog::onConditionnalityChanged() {
    if(mConditionnalButton->get_active()) {
        mValueEntry->set_sensitive(false);
        mPredicateEntry->set_sensitive(true);
        mValueIfTrue->set_sensitive(true);
        mValueIfFalse->set_sensitive(true);
        mLastActivatedEntry = mValueIfTrue;
    } else {
        mValueEntry->set_sensitive(true);
        mPredicateEntry->set_sensitive(false);
        mValueIfTrue->set_sensitive(false);
        mValueIfFalse->set_sensitive(false);
        mLastActivatedEntry = mValueEntry;
    }
    onTextChange();
}

void FlowDialog::onButtonReleasedCompartmentTreeView() {
    Glib::RefPtr < Gtk::TreeView::Selection > ref = mCompartmentsTreeView->
            get_selection();

    if (ref) {
        Gtk::TreeModel::iterator iter = ref->get_selected();
        if (iter) {
            Gtk::TreeModel::Row row = *iter;
            std::string name(row.get_value(m_viewscolumnrecord.name));

            if(mLastActivatedEntry->get_text() == ""
                || mLastActivatedEntry->get_text() == "0")
                insertInLastEntry(name, 0);
            else
                insertInLastEntry(name, mLastActivatedEntry->get_position());
        }
    }
}

void FlowDialog::onButtonReleasedVarParaTreeView()
{
    Glib::RefPtr < Gtk::TreeView::Selection > ref = mVarParaTreeView->
            get_selection();

    if (ref) {
        Gtk::TreeModel::iterator iter = ref->get_selected();
        if (iter) {
            Gtk::TreeModel::Row row = *iter;
            std::string name(row.get_value(m_viewscolumnrecord.name));

            if(mLastActivatedEntry->get_text() == ""
                || mLastActivatedEntry->get_text() == "0")
                insertInLastEntry(name, 0);
            else
                insertInLastEntry(name, mLastActivatedEntry->get_position());
        }
    }
}

void FlowDialog::initCompartmentList()
{
    std::list<std::string> compartmentList;

    mCompartmentModel = Gtk::ListStore::create(m_viewscolumnrecord);
    mCompartmentColumnName = mCompartmentsTreeView->
        append_column_editable(("Name"),
                               m_viewscolumnrecord.name);
    mCompartmentCellRenderer = dynamic_cast<Gtk::CellRendererText*>(
        mCompartmentsTreeView->get_column_cell_renderer(mCompartmentColumnName - 1));
    mCompartmentCellRenderer->property_editable() = false;
    mCompartmentsTreeView->set_model(mCompartmentModel);

    for(arrows::const_iterator it = mForrester.getArrows().begin();
        it != mForrester.getArrows().end(); ++it) {
        if(dynamic_cast<DependencyArrow*>(*it)) {
            Compartment* tmp = 0;
            if((tmp = dynamic_cast<Compartment*>((*it)->getOrigin()->getOwner()))) {

                std::string destinationItem =
                    (*it)->getDestination()->getOwner()->getName();
                std::string originItem =
                    (*it)->getOrigin()->getOwner()->getName();
                std::string currentItem = mOldName;

                if (destinationItem == currentItem) {
                    compartmentList.push_front(originItem);
                }

                mParser.addVariable(tmp->getName(),
                                    tmp->getInitialValue());
            }
        }
    }

    compartmentList.unique();

    for (std::list<std::string>::iterator it = compartmentList.begin();
         it != compartmentList.end(); ++it) {
        Gtk::TreeIter iter = mCompartmentModel->append();
        Gtk::ListStore::Row row = *iter;
        row[m_viewscolumnrecord.name] = *it;
    }
}

void FlowDialog::initParVarList()
{
    std::list<std::string> parVarList;

    mParVarModel = Gtk::ListStore::create(m_viewscolumnrecord);
    mParVarColumnName = mVarParaTreeView->
        append_column_editable(("Name"),
                               m_viewscolumnrecord.name);
    mParVarCellRenderer = dynamic_cast<Gtk::CellRendererText*>(
        mVarParaTreeView->get_column_cell_renderer(mParVarColumnName - 1));
    mParVarCellRenderer->property_editable() = false;
    mVarParaTreeView->set_model(mParVarModel);

    for (arrows::const_iterator it = mForrester.getArrows().begin();
         it != mForrester.getArrows().end(); ++it) {
        if(dynamic_cast<DependencyArrow*>(*it)) {
            if(not dynamic_cast<Compartment*>((*it)->getOrigin()->getOwner())) {

                std::string destinationItem =
                    (*it)->getDestination()->getOwner()->getName();
                std::string originItem =
                    (*it)->getOrigin()->getOwner()->getName();
                std::string currentItem = mOldName;

                if (destinationItem == currentItem) {
                    parVarList.push_front(originItem);
                }

                Parameter* tmp;
                if((tmp = dynamic_cast<Parameter*>((*it)->getOrigin()->getOwner())))
                    mParser.addConstant(tmp->getName(),tmp->getValue());
                else
                    mParser.addVariable((*it)->getOrigin()->getOwner()->getName());
            }
        }
    }

    parVarList.unique();

    for (std::list<std::string>::iterator it = parVarList.begin();
         it != parVarList.end(); ++it) {
        Gtk::TreeIter iter = mParVarModel->append();
        Gtk::ListStore::Row row = *iter;
        row[m_viewscolumnrecord.name] = *it;
    }
}

void FlowDialog::onFunctionButtonReleased(Gtk::Button* b) {
    int size = 1 + b->get_label().size() + 2 ;
    int previousSize = mLastActivatedEntry->get_position();
    insertInLastEntry(" "+b->get_label()+"()", mLastActivatedEntry->get_position());
    mLastActivatedEntry->set_position(previousSize + size - 1);
}

void FlowDialog::onAritmeticButtonReleased(Gtk::Button* b) {
    if(mLastActivatedEntry->get_text() != "")
        insertInLastEntry(" "+ b->get_label()+" ", mLastActivatedEntry->get_position());
}

void FlowDialog::onTimeButtonPressed() {
    insertInLastEntry("t", mLastActivatedEntry->get_position());

}

void FlowDialog::insertInLastEntry(const std::string& str,int index) {
    mLastActivatedEntry->set_text(mLastActivatedEntry->get_text().insert(index, str));
    mLastActivatedEntry->set_position(index + str.size());
}

FlowDialog::~FlowDialog()
{
    for(std::vector<sigc::connection>::iterator it =
        connectionsHandlers.begin(); it != connectionsHandlers.end(); ++it)
        it->disconnect();
    if (mCompartmentsTreeView)
        mCompartmentsTreeView->remove_all_columns();
    if (mVarParaTreeView)
        mVarParaTreeView->remove_all_columns();
}

void FlowDialog::checkFields() {
    std::string entryName = getName();

    const std::string namePrefix("Name : ");
    const std::string valuePrefix("Value : ");
    const std::string predicatePrefix("Predicate : ");
    const std::string valueIfTruePrefix("Value (If true) : ");
    const std::string valueIfFalsePrefix("Value (If false) : ");

    if (entryName.empty())
        throw std::string(namePrefix+"empty field");
    else
    {
        if (mForrester.exist(entryName) && entryName != mOldName)
            throw std::string(namePrefix+"Already taken name");
        else if(not mForrester.isValidName(entryName))
            throw std::string(namePrefix+"Invalid name");

        if(not mConditionnalButton->get_active())
        {
            if (mValueEntry->get_text().empty())
                throw std::string(valuePrefix+"empty field");
            else
            {
                mParser.SetExpr(mValueEntry->get_text());
                try
                {
                    mParser.Eval();
                }
                catch(mu::Parser::exception_type &e)
                {
                    throw std::string(valuePrefix + e.GetMsg());
                }
            }
        }
        else
        {
            if (mPredicateEntry->get_text().empty())
                throw std::string(predicatePrefix+"Empty field");
            else if(mValueIfTrue->get_text().empty())
                throw std::string(valueIfTruePrefix+"Empty field");
            else if (mValueIfFalse->get_text().empty())
                throw std::string(valueIfFalsePrefix+"Empty field");
            else
            {
                mParser.SetExpr(mPredicateEntry->get_text());
                try
                {
                    mParser.Eval();
                }
                catch(mu::Parser::exception_type &e)
                {
                    throw std::string(predicatePrefix + e.GetMsg());
                }

                mParser.SetExpr(mValueIfTrue->get_text());
                try
                {
                    mParser.Eval();
                }
                catch(mu::Parser::exception_type &e)
                {
                    throw std::string(valueIfTruePrefix + e.GetMsg());
                }

                mParser.SetExpr(mValueIfFalse->get_text());
                try
                {
                    mParser.Eval();
                }
                catch(mu::Parser::exception_type &e)
                {
                    throw std::string(valueIfFalsePrefix + e.GetMsg());
                }
            }
        }
    }
}

bool FlowDialog::setLastActivatedEntry(GdkEventFocus*) {
    if (mValueEntry->has_focus())
        mLastActivatedEntry = mValueEntry;
    else if(mValueIfFalse->has_focus())
        mLastActivatedEntry = mValueIfFalse;
    else if(mValueIfTrue->has_focus())
        mLastActivatedEntry = mValueIfTrue;
    else if(mPredicateEntry->has_focus())
        mLastActivatedEntry = mPredicateEntry;

    return true;
}

}
}
}
}
