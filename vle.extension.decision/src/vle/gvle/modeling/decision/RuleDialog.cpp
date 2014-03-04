/*
 * @file vle/gvle/RuleDialog.cpp
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

#include <iostream>
#include <vle/gvle/modeling/decision/RuleDialog.hpp>

namespace vle {
namespace gvle {
namespace modeling {
namespace decision {

    RuleDialog::RuleDialog(Glib::RefPtr < Gtk::Builder >& xml,
        std::map < std::string, strings_t > rule,
        strings_t predicateName,
        hierarchicalPred pPred):
    mXml(xml), mPred(pPred)
{
    mRule = rule;
    mPredicateName = predicateName;

    xml->get_widget("RuleDialog", mDialog);

    //Rules
    xml->get_widget("TreeViewRules", mTreeViewRules);
    mRefTreeRules = Gtk::TreeStore::create(mColumnsRules);
    mTreeViewRules->set_model(mRefTreeRules);
    mTreeViewRules->append_column("Rules", mColumnsRules.m_col_name);

    mList.push_back(mTreeViewRules->signal_button_press_event().connect_notify(
        sigc::mem_fun(*this, &RuleDialog::on_button_press)));

    Glib::RefPtr < Gtk::TreeSelection > refTreeSelection =
        mTreeViewRules->get_selection();

    //Predicates
    xml->get_widget("TreeViewPredicates", mTreeViewPred);
    mRefTreePred = Gtk::TreeStore::create(mColumnsPred);
    mTreeViewPred->set_model(mRefTreePred);
    mTreeViewPred->append_column("Predicates", mColumnsPred.m_col_name);

    //Selections
    mDstSelect = mTreeViewRules->get_selection();
    mDstSelect->set_mode(Gtk::SELECTION_MULTIPLE);
    mSrcSelect = mTreeViewPred->get_selection();
    mSrcSelect->set_mode(Gtk::SELECTION_MULTIPLE);

    //Buttons
    xml->get_widget("ButtonAddPredicate", mButtonAdd);
    mList.push_back(mButtonAdd->signal_clicked().connect(
        sigc::mem_fun(*this, &RuleDialog::on_add_pred)));

    xml->get_widget("ButtonDelPredicate", mButtonDelete);
    mList.push_back(mButtonDelete->signal_clicked().connect(
        sigc::mem_fun(*this, &RuleDialog::on_del_pred)));
    
    Glib::RefPtr <Gtk::ActionGroup> mPopupActionGroup = Gtk::ActionGroup::create("RuleDialog");
    mPopupActionGroup->add(Gtk::Action::create("RD_Add", _("_Add")), sigc::mem_fun(*this, &RuleDialog::on_add_rule));
    mPopupActionGroup->add(Gtk::Action::create("RD_Remove", _("_Remove")), sigc::mem_fun(*this, &RuleDialog::on_del_rule));
    mPopupActionGroup->add(Gtk::Action::create("RD_Rename", _("_Rename")), sigc::mem_fun(*this, &RuleDialog::on_rename_rule));
    
    Glib::RefPtr <Gtk::UIManager> mUIManager = Gtk::UIManager::create();
    mUIManager->insert_action_group(mPopupActionGroup);
    
    Glib::ustring ui_info =
                "<ui>"
                "  <popup name='RD_Popup'>"
                "    <menuitem action='RD_Add'/>"
                "    <menuitem action='RD_Remove'/>"
                "    <menuitem action='RD_Rename'/>"
                 "  </popup>"
                "</ui>";
    
    try {
      mUIManager->add_ui_from_string(ui_info);
      mMenuPopup = (Gtk::Menu *) (mUIManager->get_widget("/RD_Popup"));
    } catch(const Glib::Error& ex) {
      std::cerr << "building menus failed: RD_Popup " <<  ex.what();
    }
    
    if (!mMenuPopup)
      std::cerr << "not a menu : RD_Popup\n";
    
    /*
    // Fill popup menu
    {
        Gtk::Menu::MenuList& menulist = mMenuPopup.items();
        menulist.push_back(Gtk::Menu_Helpers::MenuElem(
            "_Add", sigc::mem_fun(*this, &RuleDialog::on_add_rule)));
        menulist.push_back(Gtk::Menu_Helpers::MenuElem(
            "_Remove", sigc::mem_fun(*this, &RuleDialog::on_del_rule)));
        menulist.push_back(Gtk::Menu_Helpers::MenuElem(
            "_Rename", sigc::mem_fun(*this, &RuleDialog::on_rename_rule)));
    }
    mMenuPopup.accelerate(*mTreeViewRules);
    */
}

RuleDialog::~RuleDialog()
{
    for (std::list < sigc::connection >::iterator it = mList.begin();
            it != mList.end(); ++it) {
        it->disconnect();
    }
    mTreeViewRules->remove_all_columns();
    mTreeViewPred->remove_all_columns();

    mDialog->hide();
}

int RuleDialog::run()
{
    makeRules();
    makePredicates();

    mDialog->set_title("Rules");
    mDialog->show_all();

    int response = mDialog->run();
    mDialog->hide();
    return response;
}

void RuleDialog::on_button_press(GdkEventButton* event)
{
    if (event->type == GDK_BUTTON_PRESS and event->button == 3) {
        mMenuPopup->popup(event->button, event->time);
    }
}

void RuleDialog::on_add_pred()
{
    using namespace Gtk;

    //TreeSelection::ListHandle_Path lstDst = mDstSelect->get_selected_rows();
    std::vector<TreePath> v1 = mDstSelect->get_selected_rows();
    std::list<TreePath> lstDst (v1.begin(), v1.end());
    
    Glib::RefPtr < TreeModel > modelDst = mTreeViewRules->get_model();

    for (std::list<TreePath>::iterator iDst = lstDst.begin();
            iDst != lstDst.end(); ++iDst) {
        TreeModel::Row rowDst(*(modelDst->get_iter(*iDst)));
        std::string data_name(rowDst.get_value(mColumnsRules.m_col_name));
        std::string data_type(rowDst.get_value(mColumnsRules.m_col_type));
        if (data_type == "Rule" ) {
            //TreeSelection::ListHandle_Path lstSrc = mSrcSelect->get_selected_rows();
            std::vector<TreePath> v2 = mSrcSelect->get_selected_rows();
            std::list<TreePath> lstSrc (v1.begin(), v1.end());
            
            Glib::RefPtr < TreeModel > modelSrc = mTreeViewPred->get_model();

            for (std::list<TreePath>::iterator iSrc = lstSrc.begin();
                    iSrc != lstSrc.end(); ++iSrc) {
                TreeModel::Row rowSrc(*(modelSrc->get_iter(*iSrc)));
                std::string pred(rowSrc.get_value(mColumnsPred.m_col_name));
                if (std::find(mRule[data_name].begin(), mRule[data_name].end(),
                        pred) == mRule[data_name].end()) {
                    mRule[data_name].push_back(pred);
                }
            }
            makeRules();
        }
    }
}

void RuleDialog::makeRules()
{
    mRefTreeRules->clear();
    std::map < std::string, strings_t > ::const_iterator it_rule = mRule.begin();
    while (it_rule != mRule.end()) {
        Gtk::TreeModel::Row row = *(mRefTreeRules->append());
        row[mColumnsRules.m_col_name] = it_rule->first;
        row[mColumnsRules.m_col_type] = "Rule";

        strings_t ::const_iterator it_pred = it_rule->second.begin();
        while (it_pred != it_rule->second.end()) {
            Gtk::TreeModel::Row childrow =
                    *(mRefTreeRules->append(row.children()));
            childrow[mColumnsRules.m_col_name] = *it_pred;
            childrow[mColumnsRules.m_col_type] = "Pred";
            ++it_pred;
        }
        ++it_rule;
    }
    mTreeViewRules->expand_all();
}

void RuleDialog::makePredicates()
{
    mRefTreePred->clear();
    strings_t::const_iterator it = mPredicateName.begin();
    while (it != mPredicateName.end()) {
        Gtk::TreeModel::Row row = *(mRefTreePred->append());
        row[mColumnsPred.m_col_name] = *it;
        ++it;
    }
    hierarchicalPred::iterator it2 = mPred.begin();
    while (it2 != mPred.end()) {
        Gtk::TreeModel::Row row = *(mRefTreePred->append());
        row[mColumnsPred.m_col_name] = it2->first;
        ++it2;
    }
}

void RuleDialog::on_add_rule()
{
    SimpleTypeBox box(("New rule name?"), "");
    std::string name = boost::trim_copy(box.run());

    if (box.valid() && checkName(name)) {
        strings_t vPred;
        mRule.insert(std::pair < std::string, strings_t > (name, vPred));
        makeRules();
    }
    else {
        Gtk::MessageDialog errorDial ("Name error !",
            false,
            Gtk::MESSAGE_ERROR,
            Gtk::BUTTONS_OK,
            true);
        errorDial.set_title("Error !");
        errorDial.run();
    }
}

void RuleDialog::on_del_rule()
{
    using namespace Gtk;

    Glib::RefPtr < TreeSelection > tree_selection = mTreeViewRules->get_selection();
    //Gtk::TreeSelection::ListHandle_Path lst = tree_selection->get_selected_rows();
    std::vector<TreePath> v1 = tree_selection->get_selected_rows();
    std::list<TreePath> lst (v1.begin(), v1.end());
    
    Glib::RefPtr < TreeModel > model = mTreeViewRules->get_model();

    for (std::list<TreePath>::iterator it = lst.begin();
            it != lst.end(); ++it) {
        Gtk::TreeModel::Row row(*(model->get_iter(*it)));
        std::string data_name(row.get_value(mColumnsRules.m_col_name));
        std::string data_type(row.get_value(mColumnsRules.m_col_type));
        if (data_type == "Rule") {
            if (Question(_("Are you sure you want to delete this rule?"))) {
                mRule.erase(data_name);
                makeRules();
            }
        }
    }
}

void RuleDialog::on_rename_rule()
{
    using namespace Gtk;

    Glib::RefPtr < TreeSelection > tree_selection = mTreeViewRules->get_selection();
    //Gtk::TreeSelection::ListHandle_Path lst = tree_selection->get_selected_rows();
    std::vector<TreePath> v1 = tree_selection->get_selected_rows();
    std::list<TreePath> lst (v1.begin(), v1.end());
    
    Glib::RefPtr < TreeModel > model = mTreeViewRules->get_model();

    std::list<TreePath>::iterator it = lst.begin();
    if (it != lst.end()) {
        Gtk::TreeModel::Row row(*(model->get_iter(*it)));
        std::string data_name(row.get_value(mColumnsRules.m_col_name));
        std::string data_type(row.get_value(mColumnsRules.m_col_type));
        if (data_type == "Rule") {
            SimpleTypeBox box(("Rule new name?"), "");
            std::string name = boost::trim_copy(box.run());
            if (box.valid() and checkName(name)) {
                // change rule name
                strings_t vPred = mRule[data_name];
                mRule.erase(data_name);
                mRule.insert(std::pair < std::string, strings_t > (name, vPred));
                makeRules();
            }
            else {
                Gtk::MessageDialog errorDial ("Name error !",
                    false,
                    Gtk::MESSAGE_ERROR,
                    Gtk::BUTTONS_OK,
                    true);
                errorDial.set_title("Error !");
                errorDial.run();
            }
        }
    }
}

void RuleDialog::on_del_pred()
{
    using namespace Gtk;

    Glib::RefPtr < TreeSelection > tree_selection = mTreeViewRules->get_selection();
    //TreeSelection::ListHandle_Path lst = tree_selection->get_selected_rows();
    std::vector<TreePath> v1 = tree_selection->get_selected_rows();
    std::list<TreePath> lst (v1.begin(), v1.end());
    
    Glib::RefPtr < TreeModel > model = mTreeViewRules->get_model();

    for (std::list<TreePath>::iterator it = lst.begin();
            it != lst.end(); ++it) {
        TreeModel::Row row(*(model->get_iter(*it)));
        std::string data_name(row.get_value(mColumnsRules.m_col_name));
        std::string data_type(row.get_value(mColumnsRules.m_col_type));

        if ( data_type == "Pred" ) {
            TreeModel::Row parent(*(row.parent()));
            std::string rule_name(parent.get_value(mColumnsRules.m_col_name));
            for (std::vector < std::string > ::iterator it =
                    mRule[rule_name].begin(); it != mRule[rule_name].end(); ) {
                if ( *it == data_name ) {
                    it = mRule[rule_name].erase(it);
                }
                else {
                    ++it;
                }
            }
        }
    }
    makeRules();
}

std::map < std::string, strings_t > RuleDialog::getRule () {
    return mRule;
}

}
}
}
}

// namespace vle gvle modeling decision
