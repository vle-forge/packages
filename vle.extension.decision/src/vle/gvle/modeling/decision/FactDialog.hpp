/*
 * @file vle/gvle/modeling/decision/FactDialog.hpp
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

#ifndef VLE_GVLE_FACTDIALOG_HPP
#define VLE_GVLE_FACTDIALOG_HPP

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

/**
 * @class FactDialog
 * @brief This class is used to manage the facts.
 */

class FactDialog
{
public:
/**
 * @brief FactDialog parameterized constructor.
 * @param link to the xml file
 * @param vector with the fatcs names
 * @param vector with the facts functions
 * @param vector with the facts attributes
 * @param vector with the facts init
 */
    FactDialog(Glib::RefPtr < Gtk::Builder >& xml,
               std::vector < std::string > pFactName,
               std::map < std::string, std::string > pFactFunction,
               std::map < std::string, std::string > pFactAttribute,
               std::map < std::string, std::string > pFactInit);
    virtual ~FactDialog();

/**
 * @brief Launch the FactDialog window.
 * @return the action of the user, cancel or validate
 */
    int run();

/**
 * @brief Get the vector which contains the list of the facts names
 * @return the vector wich contains the list of the facts names
 */
    std::vector < std::string > getFactName() {
        return mFactName;
    }

/**
 * @brief Get the map which contains the list of the facts functions
 * associated with the vector name
 * @return the map
 */
    std::map < std::string, std::string > getFactFunction() {
        return mFactFunction;
    }

/**
 * @brief Get the map which contains the list of the facts attributes
 * associated with the vector name
 * @return the map
 */
    std::map < std::string, std::string > getFactAttribute() {
        return mFactAttribute;
    }

/**
 * @brief Get the map which contains the list of the facts Init
 * associated with the vector name
 * @return the map
 */
    std::map < std::string, std::string > getFactInit() {
        return mFactInit;
    }
private:
    Glib::RefPtr < Gtk::Builder > mXml;

/**
 * @brief The function allow the user to add a fact.
 * It's open a window which ask for the fact name
 * and initialize the vectors.
 */
    void onAddFact();

/**
 * @brief This function delete a fact in the window and into the
 * vectors.
 */
    void onDeleteFact();

/**
 * @brief This function allow a user to rename a fact.
 */
    void onRenameFact();

/**
 * @brief This function check if a fact name is valid
 * @param the name of the fact
 */
    bool checkName(std::string name)
    {
        if (name.empty()) {
            return false;
        }

        unsigned int i = 0;
        if (isdigit(name[0])) {
            return false;
        }

        while (i < name.length()) {
            if (!isalnum(name[i])) {
                return false;
            }
            i++;
        }

        for ( std::vector < std::string > ::const_iterator it = mFactName.begin() ;
            it != mFactName.end() ; ++it ) {
            if (name == *it) {
                return false;
            }
        }
        return true;
    }

/**
 * @brief This function initialize the Fact tree view.
 */
    void initTreeFactList();

/**
 * @brief This function fill the Fact tree view with the names
 * on the vector.
 */
    void fillTreeFactList();

/**
 * @brief This function initialize the popup menu in the tree view.
 */
    void initMenuPopupTreeFactList();

/**
 * @brief This function save the fact who was edited by the user before
 * an action which change the screen content.
 * @param the name of the fact to save
 */
    void savePreviousFact(std::string name);

    Gtk::Dialog* mDialog;
    Gtk::TreeView* mTreeFactList;
    Gtk::Entry* mFactNameEntry;
    Gtk::TextView* mTextViewAttributes;
    Gtk::TextView* mTextViewInit;
    Gtk::TextView* mTextViewFunction;
    Gtk::Label* mHeaderFact;

    Gtk::Menu *mMenuTreeViewItems;

/**
 * @brief This function activate or disactivate the TextView in the window.
 * @param a boolean, true to activate, false to disactivate.
 */
    void setSensitiveFact( bool );

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

    sigc::connection m_cntTreeFactListButtonRelease;
    sigc::connection m_cntTreeFactCursorChanged;

/**
 * @brief This function is called when the user click on the TreeView.
 * @param the event.
 * @return a boolean.
 */
    bool onButtonRealeaseTreeFactList(GdkEventButton* event);

/**
 * @brief This fonction is called when the user click on a fact in
 * the TreeView.
 */
    void onCursorChangeTreeFactList();

    std::vector < std::string > mFactName;
    std::map < std::string, std::string > mFactFunction;
    std::map < std::string, std::string > mFactAttribute;
    std::map < std::string, std::string > mFactInit;
};

}
}
}
}    // namespace vle gvle modeling decision

#endif
