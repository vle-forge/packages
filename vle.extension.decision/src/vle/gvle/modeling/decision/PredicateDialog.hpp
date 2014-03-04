/*
 * @file vle/gvle/modeling/decision/PredicateDialog.hpp
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

#ifndef VLE_GVLE_PREDICATEDIALOG_HPP
#define VLE_GVLE_PREDICATEDIALOG_HPP

#include <vle/gvle/SimpleTypeBox.hpp>
#include <vle/gvle/Message.hpp>
#include <vle/gvle/Settings.hpp>
#include <vle/gvle/Editor.hpp>
#include <vle/version.hpp>
#include <boost/algorithm/string.hpp>
#include <boost/format.hpp>
#include <gtkmm.h>
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

/**
 * @class PredicateDialog
 * @brief This class is used to manage the predicates.
 */

class PredicateDialog
{
public:
/**
 * @brief PredicateDialog parameterized constructor.
 * @param link to the xml file
 * @param vector with the facts methods
 * @param vector with the predicates attributes
 */
    PredicateDialog(Glib::RefPtr < Gtk::Builder >& xml,
        std::vector < std::string > pPredicateName,
        std::map < std::string, std::string > pPredicateFunction,
        hierarchicalPred);
    virtual ~PredicateDialog();

/**
 * @brief Launch the PredicateDialog window.
 * @return the action of the user, cancel or validate
 */
    int run();

/**
 * @brief Get the vector which contains the list of the predicates names
 * @return the vector wich contains the list of the predicates names
 */
    std::vector < std::string > getPredicateName() {
        return mPredicateName;
    }

/**
 * @brief Get the map which contains the list of the predicates functions
 * associated with the vector name
 * @return the map
 */
    std::map < std::string, std::string > getPredicateFunction() {
        return mPredicateFunction;
    }

private:
    Glib::RefPtr < Gtk::Builder > mXml;

/**
 * @brief The function allow the user to add a predicate.
 * It's open a window which ask for the predicate name
 * and initialize the vectors.
 */
    void onAddPredicate();

/**
 * @brief This function delete a predicate in the window and into the
 * vectors.
 */
    void onDeletePredicate();

/**
 * @brief This function allow a user to rename a predicate.
 */
    void onRenamePredicate();

    /**
     * @brief This function check if a predicate name is valid
     * @param the name of the predicate
     */
    bool checkName(std::string name)
    {
        if (!Utils::isValidName(name)) {
            return false;
        }
        if (find(mPredicateName.begin(), mPredicateName.end(), name) !=
            mPredicateName.end() ||
            mPred.find(name) != mPred.end()) {
            return false;
        }
        return true;
    }

/**
 * @brief This function initialize the Predicate tree view.
 */
    void initTreePredicateList();

/**
 * @brief This function fill the Predicate tree view with the names
 * on the vector.
 */
    void fillTreePredicateList();

/**
 * @brief This function initialize the popup menu in the tree view.
 */
    void initMenuPopupTreePredicateList();

/**
 * @brief This function save the predicate who was edited by the user before
 * an action which change the screen content.
 * @param the name of the predicate to save
 */
    void savePreviousPredicate(std::string name);

    Gtk::Dialog* mDialog;
    Gtk::TreeView* mTreePredicateList;
    Gtk::Entry* mPredicateNameEntry;
    Gtk::TextView* mTextViewFunction;
    Gtk::Label* mHeaderPred;

    Gtk::Menu *mMenuTreeViewItems;

/**
 * @brief This function activate or disactivate the TextView in the window.
 * @param a boolean, true to activate, false to disactivate.
 */
    void setSensitivePredicate( bool );

// To manage the tree view
    Glib::RefPtr < Gtk::ListStore > m_model;
    int m_columnName;
    Gtk::CellRendererText* m_cellrenderer;
    Gtk::TreeModel::iterator m_iter;
    struct ViewsColumnRecord : public Gtk::TreeModel::ColumnRecord
    {
        Gtk::TreeModelColumn < std::string > name;
        ViewsColumnRecord() {
            add(name);
        }
    } m_viewscolumnrecord;

    sigc::connection m_cntTreePredicateListButtonRelease;
    sigc::connection m_cntTreePredicateCursorChanged;

/**
 * @brief This function is called when the user click on the TreeView.
 * @param the event.
 * @return a boolean.
 */
    bool onButtonRealeaseTreePredicateList(GdkEventButton* event);

/**
 * @brief This fonction is called when the user click on a predicate in
 * the TreeView.
 */
    void onCursorChangeTreePredicateList();

    strings_t mPredicateName;
    std::map < std::string, std::string > mPredicateFunction;
    hierarchicalPred mPred;
};

}
}
}
}    // namespace vle gvle modeling decision

#endif
