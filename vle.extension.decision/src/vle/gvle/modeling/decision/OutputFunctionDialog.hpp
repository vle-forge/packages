/*
 * @file vle/gvle/modeling/decision/OutputFunctionDialog.hpp
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


#ifndef VLE_GVLE_OFDIALOG_HPP
#define VLE_GVLE_OFDIALOG_HPP

#include <vle/gvle/SimpleTypeBox.hpp>
#include <vle/gvle/Settings.hpp>
#include <vle/gvle/Message.hpp>
#include <vle/gvle/Editor.hpp>
#include <vle/version.hpp>
#include <boost/algorithm/string.hpp>
#include <boost/format.hpp>
#include <gtkmm.h>
#include <string>
#include <iostream>
#include <vector>

#include <vle/gvle/modeling/decision/Utils.hpp>

namespace vle {
namespace gvle {
namespace modeling {
namespace decision {

typedef std::vector < std::string > strings_t;

/**
 * @class OutputFunctionDialog
 * @brief This class is used to manage the output functions.
 */

class OutputFunctionDialog
{
public:
/**
 * @brief OutputFunctionDialog parameterized constructor.
 * @param link to the xml file
 * @param vector with the facts methods
 * @param vector with the output functions attributes
 */
    OutputFunctionDialog(Glib::RefPtr < Gtk::Builder >& xml,
        std::vector < std::string > pOFName,
    std::map < std::string, std::string > pOFFunction);
    virtual ~OutputFunctionDialog();

/**
 * @brief Launch the OutputFunctionDialog window.
 * @return the action of the user, cancel or validate
 */
    int run();

/**
 * @brief Get the vector which contains the list of the output functions names
 * @return the vector wich contains the list of the output functions names
 */
    std::vector < std::string > getOFName() {
        return mOFName;
    }

/**
 * @brief Get the map which contains the list of the output functions
 * functions associated with the vector name
 * @return the map
 */
    std::map < std::string, std::string > getOFFunction() {
        return mOFFunction;
    }

private:
    Glib::RefPtr < Gtk::Builder > mXml;

/**
 * @brief The function allow the user to add a output function.
 * It's open a window which ask for the output function name
 * and initialize the vectors.
 */
    void onAddOF();

/**
 * @brief This function delete a output function in the window and into the
 * vectors.
 */
    void onDeleteOF();

/**
 * @brief This function allow a user to rename a output function.
 */
    void onRenameOF();

    /**
     * @brief This function check if the output function name is valid
     * @param the name of the output function
     */
    bool checkName(std::string name)
    {
        if (!Utils::isValidName(name)) {
            return false;
        }
        for ( std::vector < std::string > ::const_iterator it = mOFName.begin();
            it != mOFName.end() ; ++it ) {
            if (name == *it) {
                return false;
            }
        }
        return true;
    }

/**
 * @brief This function initialize the output function tree view.
 */
    void initTreeOFList();

/**
 * @brief This function fill the output function tree view with the names
 * on the vector.
 */
    void fillTreeOFList();

/**
 * @brief This function initialize the popup menu in the tree view.
 */
    void initMenuPopupTreeOFList();

/**
 * @brief This function save the output function who was edited by the user
 * before an action which change the screen content.
 * @param the name of the output function to save
 */
    void savePreviousOF(std::string name);

    Gtk::Dialog* mDialog;
    Gtk::TreeView* mTreeOFList;
    Gtk::Entry* mOFNameEntry;
    Gtk::TextView* mTextViewFunction;
    Gtk::Label* mHeaderOutput;

    Gtk::Menu *mMenuTreeViewItems;

/**
 * @brief This function activate or disactivate the TextView in the window.
 * @param a boolean, true to activate, false to disactivate.
 */
    void setSensitiveOF( bool );

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

    sigc::connection m_cntTreeOFListButtonRelease;
    sigc::connection m_cntTreeOFCursorChanged;

/**
 * @brief This function is called when the user click on the TreeView.
 * @param the event.
 * @return a boolean.
 */
    bool onButtonRealeaseTreeOFList(GdkEventButton* event);

/**
 * @brief This fonction is called when the user click on a output function in
 * the TreeView.
 */
    void onCursorChangeTreeOFList();

    strings_t mOFName;
    std::map < std::string, std::string > mOFFunction;
};

}
}
}
}    // namespace vle gvle modeling decision

#endif
