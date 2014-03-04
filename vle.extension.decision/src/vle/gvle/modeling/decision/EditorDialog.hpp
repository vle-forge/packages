/*
 * @file vle/gvle/modeling/decision/EditorDialog.hpp
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

#ifndef VLE_GVLE_EDITORDIALOG_HPP
#define VLE_GVLE_EDITORDIALOG_HPP

#include <vle/gvle/SimpleTypeBox.hpp>
#include <vle/gvle/Message.hpp>
#include <vle/gvle/Settings.hpp>
#include <vle/gvle/Editor.hpp>
#include <vle/version.hpp>

#include <boost/algorithm/string.hpp>
#include <boost/format.hpp>

#include <gtkmm.h>

#include <sigc++/bind.h>
#include <iostream>
#include <vector>
#include <string>

#include <vle/gvle/modeling/decision/Utils.hpp>

namespace vle {
namespace gvle {
namespace modeling {
namespace decision {

typedef std::vector < std::string > strings_t;
typedef std::map < std::string, strings_t > hierarchicalPred;

#define ROW_TYPE_EMPTY 0
#define ROW_TYPE_PRED 1
#define ROW_TYPE_LOG 2
#define ROW_TYPE_LINE 3

/**
 * @class EditorDialog
 * @brief This class is used to manage the hierarchical predicates.
 */

class EditorDialog
{
public:
/**
 * @brief EditorDialog parameterized constructor.
 * @param link to the xml file
 * @param vector with the pred names
 * @param vector with the facts names
 * @param map with the parameters
 */
    EditorDialog(Glib::RefPtr < Gtk::Builder >& xml,
        hierarchicalPred pPred,
        std::vector < std::string > pFactName,
        std::vector < std::string > pParam,
        strings_t lPred);

    virtual ~EditorDialog();

/**
 * @brief Launch the EditorDialog window.
 * @return the action of the user, cancel or validate
 */
    int run();

hierarchicalPred getPred();

private:

    void onAddPred();

    void onDeletePred();

    void onRenamePred();

    /**
     * @brief This function check if a predicate name is valid
     * @param the name of the fact
     */
    bool checkName(std::string name)
    {
        if (!Utils::isValidName(name)) {
            return false;
        }
        if (find(mPredName.begin(), mPredName.end(), name) != mPredName.end()||
            mPred.find(name) != mPred.end()) {
            return false;
        }
        return true;
    }

/**
 * @brief This function initializes the treeview popup menus.
 */
    void initPopupMenus();

    // Model used to represent the predicates list and the
    // Combo CellRendered
    struct ModelColumnCombo : public Gtk::TreeModel::ColumnRecord
    {
        public:
        ModelColumnCombo() {
            add(name);
        }
        Gtk::TreeModelColumn < std::string > name;
    };
    bool onButtonRealeasePred(GdkEventButton* event)
    {
        if (event->button == 3) {
            mMenuPred->popup(event->button, event->time);
        }
        return true;
    }

void onAddSubLog() {
    // Add a children to the selected row
    using namespace Gtk;

    Glib::RefPtr < TreeSelection > tree_selection = mTreeVPred->get_selection();
    
    //Gtk::TreeSelection::ListHandle_Path lst = tree_selection->get_selected_rows();
    std::vector<TreePath> v1 = tree_selection->get_selected_rows();
    std::list<TreePath> lst (v1.begin(), v1.end());

    //Gtk::TreeSelection::ListHandle_Path::iterator it = lst.begin();
    std::list<TreePath>::iterator it = lst.begin();
    
    if (it != lst.end()) {
        Gtk::TreeModel::Row row(*(m_refModelPred->get_iter(*it)));

        int rowType = row.get_value(m_modelColumnPred.rowType);
        std::string predName = row.get_value(m_modelColumnPred.predName);
        int rowNumber = row.get_value(m_modelColumnPred.rowNumber);

        if (rowType == ROW_TYPE_LOG) {
            row = *(m_refModelPred->append(row.children()));
            row[m_modelColumnPred.log] = "Click to select a logical op.";
            row[m_modelColumnPred.logs] = m_refTreeModelComboLogs;
            row[m_modelColumnPred.rowType] = ROW_TYPE_LOG;
            row[m_modelColumnPred.predName] = predName;
            row[m_modelColumnPred.rowNumber] = rowNumber + 1;
        }
        else if (rowType == ROW_TYPE_PRED) {
            Gtk::TreeModel::Children children = row.children();
            typedef Gtk::TreeModel::Children type_children;
            bool valid = true;
            bool first = true;
            for (type_children::iterator iter = children.begin();
                    iter != children.end() && first == true; ++iter) {
                Gtk::TreeModel::Row rowChild = *iter;
                int rowChildType = rowChild.get_value(m_modelColumnPred.rowType);
                if (rowChildType == ROW_TYPE_LINE ||
                        rowChildType == ROW_TYPE_LOG) {
                    valid = false;
                }
                first = false;
            }
            if (valid) {
                row = *(m_refModelPred->append(row.children()));
                row[m_modelColumnPred.log] = "Click to select a logical op.";
                row[m_modelColumnPred.logs] = m_refTreeModelComboLogs;
                row[m_modelColumnPred.rowType] = ROW_TYPE_LOG;
                row[m_modelColumnPred.predName] = predName;
                row[m_modelColumnPred.rowNumber] = rowNumber + 1;
            }
        }
    }
}

void onAddSubLine() {
    // Add a children to the selected row
    using namespace Gtk;

    Glib::RefPtr < TreeSelection > tree_selection = mTreeVPred->get_selection();
    //Gtk::TreeSelection::ListHandle_Path lst = tree_selection->get_selected_rows();
    std::vector<TreePath> v1 = tree_selection->get_selected_rows();
    std::list<TreePath> lst (v1.begin(), v1.end());

    //Gtk::TreeSelection::ListHandle_Path::iterator it = lst.begin();
    std::list<TreePath>::iterator it = lst.begin();
    
    if (it != lst.end()) {
        Gtk::TreeModel::Row row(*(m_refModelPred->get_iter(*it)));
        int rowType = row.get_value(m_modelColumnPred.rowType);
        std::string predName = row.get_value(m_modelColumnPred.predName);
        int rowNumber = row.get_value(m_modelColumnPred.rowNumber);

        if (rowType == ROW_TYPE_LOG) {
            row = *(m_refModelPred->append(row.children()));

            row[m_modelColumnPred.fact] = "Click to select a fact";
            row[m_modelColumnPred.facts] = m_refTreeModelComboFacts;
            row[m_modelColumnPred.op] = "Click to select a comp. operator";
            row[m_modelColumnPred.ops] = m_refTreeModelComboOps;
            row[m_modelColumnPred.par] = "Click to select a parameter";
            row[m_modelColumnPred.pars] = m_refTreeModelComboPars;
            row[m_modelColumnPred.rowType] = ROW_TYPE_LINE;
            row[m_modelColumnPred.predName] = predName;
            row[m_modelColumnPred.rowNumber] = rowNumber + 1;
        }
        else if (rowType == ROW_TYPE_PRED) {
            Gtk::TreeModel::Children children = row.children();
            typedef Gtk::TreeModel::Children type_children;
            bool valid = true;
            bool first = true;
            for (type_children::iterator iter = children.begin();
                    iter != children.end() && first == true; ++iter) {
                Gtk::TreeModel::Row rowChild = *iter;
                int rowChildType = rowChild.get_value(m_modelColumnPred.rowType);
                if (rowChildType == ROW_TYPE_LINE ||
                        rowChildType == ROW_TYPE_LOG) {
                    valid = false;
                }
                first = false;
            }
            if (valid) {
                row = *(m_refModelPred->append(row.children()));

                row[m_modelColumnPred.fact] = "Click to select a fact";
                row[m_modelColumnPred.facts] = m_refTreeModelComboFacts;
                row[m_modelColumnPred.op] = "Click to select a comp. operator";
                row[m_modelColumnPred.ops] = m_refTreeModelComboOps;
                row[m_modelColumnPred.par] = "Click to select a parameter";
                row[m_modelColumnPred.pars] = m_refTreeModelComboPars;
                row[m_modelColumnPred.rowType] = ROW_TYPE_LINE;
                row[m_modelColumnPred.predName] = predName;
                row[m_modelColumnPred.rowNumber] = rowNumber + 1;
            }
        }
    }
}

void onDeleteLog() {
    using namespace Gtk;

    Glib::RefPtr < TreeSelection > tree_selection = mTreeVPred->get_selection();
    
    //Gtk::TreeSelection::ListHandle_Path lst=tree_selection->get_selected_rows();
    std::vector<TreePath> v1 = tree_selection->get_selected_rows();
    std::list<TreePath> lst (v1.begin(), v1.end());

    //Gtk::TreeSelection::ListHandle_Path::iterator it = lst.begin();
    std::list<TreePath>::iterator it = lst.begin();
    
    if (it != lst.end()) {
        Gtk::TreeIter iter = m_refModelPred->get_iter(*it) ;
        Gtk::TreeModel::Row row(*(iter));
        int rowType = row.get_value(m_modelColumnPred.rowType);

        if (rowType != ROW_TYPE_EMPTY && rowType != ROW_TYPE_PRED) {
            m_refModelPred->erase(iter);
        }
    }
}

    ModelColumnCombo m_modelColumnPredComboLogs;
    Glib::RefPtr<Gtk::ListStore> m_refTreeModelComboLogs;

    ModelColumnCombo m_modelColumnPredComboFacts;
    Glib::RefPtr<Gtk::ListStore> m_refTreeModelComboFacts;

    ModelColumnCombo m_modelColumnPredComboOps;
    Glib::RefPtr<Gtk::ListStore> m_refTreeModelComboOps;

    ModelColumnCombo m_modelColumnPredComboPars;
    Glib::RefPtr<Gtk::ListStore> m_refTreeModelComboPars;

    // Predicate treeview
    Gtk::TreeView* mTreeVPred;
    Gtk::Menu *mMenuPred;
    sigc::connection m_cntPredButtonRelease;
    // Model used to represent a predicate
    struct ModelColumnPred : public Gtk::TreeModel::ColumnRecord {
        public:
        ModelColumnPred() {
            add(pred);
            add(log);
            add(logs);
            add(fact);
            add(facts);
            add(op);
            add(ops);
            add(par);
            add(pars);
            add(rowType);
            add(predName);
            add(rowNumber);
        }
        Gtk::TreeModelColumn < Glib::ustring > pred;
        Gtk::TreeModelColumn < Glib::ustring > log;
        Gtk::TreeModelColumn < Glib::RefPtr<Gtk::TreeModel> > logs;
        Gtk::TreeModelColumn < Glib::ustring > fact;
        Gtk::TreeModelColumn < Glib::RefPtr<Gtk::TreeModel> > facts;
        Gtk::TreeModelColumn < Glib::ustring > op;
        Gtk::TreeModelColumn < Glib::RefPtr<Gtk::TreeModel> > ops;
        Gtk::TreeModelColumn < Glib::ustring > par;
        Gtk::TreeModelColumn < Glib::RefPtr<Gtk::TreeModel> > pars;
        Gtk::TreeModelColumn < int > rowType;
        Gtk::TreeModelColumn < Glib::ustring > predName;
        Gtk::TreeModelColumn < int > rowNumber;
    };

    void initTVPred();
    void fillTVPred();

    Glib::RefPtr < Gtk::TreeStore > m_refModelPred;
    ModelColumnPred m_modelColumnPred;

void on_cellrenderer_choice_edited_logs(
        const Glib::ustring& path_string, const Glib::ustring& new_text)
{
    Gtk::TreePath path(path_string);

    //Get the row from the path:
    Gtk::TreeModel::iterator iter = m_refModelPred->get_iter(path);
    if(iter)
    {
        if (new_text == "OR" || new_text == "AND") {
            //Store the user's new text in the model:
            Gtk::TreeRow row = *iter;
            row[m_modelColumnPred.log] = new_text;
        }
    }
}
void on_cellrenderer_choice_edited_facts(
        const Glib::ustring& path_string, const Glib::ustring& new_text)
{
    Gtk::TreePath path(path_string);

    //Get the row from the path:
    Gtk::TreeModel::iterator iter = m_refModelPred->get_iter(path);
    if(iter)
    {
        std::string test = new_text + "";
        if (find(mFactName.begin(), mFactName.end(), test) != mFactName.end()) {
            //Store the user's new text in the model:
            Gtk::TreeRow row = *iter;
            row[m_modelColumnPred.fact] = new_text;
        }
    }
}
void on_cellrenderer_choice_edited_ops(
        const Glib::ustring& path_string, const Glib::ustring& new_text)
{
    Gtk::TreePath path(path_string);

    //Get the row from the path:
    Gtk::TreeModel::iterator iter = m_refModelPred->get_iter(path);
    if(iter)
    {
        if (new_text == "==" || new_text == "!=" || new_text == "<=" ||
                new_text == "<" || new_text == ">=" || new_text == ">") {
            //Store the user's new text in the model:
            Gtk::TreeRow row = *iter;
            row[m_modelColumnPred.op] = new_text;
        }
    }
}
void on_cellrenderer_choice_edited_pars(
        const Glib::ustring& path_string, const Glib::ustring& new_text)
{
    Gtk::TreePath path(path_string);

    //Get the row from the path:
    Gtk::TreeModel::iterator iter = m_refModelPred->get_iter(path);
    if(iter)
    {
        std::string test = new_text + "";
        if (find(mParam.begin(), mParam.end(), test) != mParam.end()) {
            //Store the user's new text in the model:
            Gtk::TreeRow row = *iter;
            row[m_modelColumnPred.par] = new_text;
        }
        else {
            unsigned int i = 0;
            bool valid = true;
            while (i < test.size()) {
                if (!isdigit(test[i])) {
                    valid = false;
                }
                i++;
            }
            //Store the user's new text in the model:
            if (valid) {
                Gtk::TreeRow row = *iter;
                row[m_modelColumnPred.par] = new_text;
            }
        }
    }
}

bool SaveRow(Gtk::TreeModel::Children children);
strings_t::iterator* InsertPredicate(Gtk::TreeModel::Row pRow,
        strings_t::iterator *it,
        strings_t*, int);

    bool breakLine;
    // Stocks the datas
    hierarchicalPred mPred;
    std::vector < std::string > mPredName;
    std::vector < std::string > mFactName;
    std::vector < std::string > mParam;

    Gtk::Button* mAddPred;
    Gtk::Button* mDelPred;
    Gtk::Button* mRenamePred;
    std::list < sigc::connection > mList;
    Gtk::Dialog* mDialog;
    Gtk::Label* mLabelLPred;
    Glib::RefPtr < Gtk::Builder > mXml;

    // Editor Model

};

}
}
}
}    // namespace vle gvle modeling decision

#endif
