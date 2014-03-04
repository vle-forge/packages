/*
 * @file vle/gvle/modeling/decision/AckFunctionDialog.hpp
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


#ifndef VLE_GVLE_ACKDIAL_HPP
#define VLE_GVLE_ACKDIAL_HPP

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
 * @class AckFunctionDialog
 * @brief This class is used to manage the acknowledge functions.
 */

class AckFunctionDialog
{
public:
/**
 * @brief AckFunctionDialog parameterized constructor.
 * @param link to the xml file
 * @param vector with the facts methods
 * @param vector with the acknowledge functions attributes
 */
    AckFunctionDialog(Glib::RefPtr < Gtk::Builder >& xml,
        std::vector < std::string > pAckName,
    std::map < std::string, std::string > pAckFunction);
    virtual ~AckFunctionDialog();

/**
 * Launch the AckFunctionDialog window.
 * @return the action of the user, cancel or validate
 */
    int run();

/**
 * Get the vector which contains the list of the acknowledge functions names
 * @return the vector wich contains the list of the acknowledge functions names
 */
    std::vector < std::string > getAckName() {
        return mAckName;
    }

/**
 * Get the map which contains the list of the acknowledge functions functions
 * associated with the vector name
 * @return the map
 */
    std::map < std::string, std::string > getAckFunction() {
        return mAckFunction;
    }

private:
    Glib::RefPtr < Gtk::Builder > mXml;

/**
 * The function allow the user to add a acknowledge function.
 * It's open a window which ask for the acknowledge function name
 * and initialize the vectors.
 */
    void onAddAck();

/**
 * This function delete a acknowledge function in the window and into the
 * vectors.
 */
    void onDeleteAck();

/**
 * This function allow a user to rename a acknowledge function.
 */
    void onRenameAck();

/**
 * This function check if the acknowledge function name is valid
 * @param the name of the acknowledge function
 */
    bool checkName(std::string name)
    {
        if (!Utils::isValidName(name)) {
            return false;
        }
        std::vector < std::string > ::const_iterator it;
        for (it = mAckName.begin();
             it != mAckName.end() ; ++it ) {
            if (name == *it) {
                return false;
            }
        }
        return true;
    }

/**
 * This function initialize the acknowledge function tree view.
 */
    void initTreeAckList();

/**
 * This function fill the acknowledge function tree view with the names
 * on the vector.
 */
    void fillTreeAckList();

/**
 * This function initialize the popup menu in the tree view.
 */
    void initMenuPopupTreeAckList();

/**
 * This function save the acknowledge function who was edited by the user before
 * an action which change the screen content.
 * @param the name of the acknowledge function to save
 */
    void savePreviousAck(std::string name);

    Gtk::Dialog* mDialog;
    Gtk::TreeView* mTreeAckList;
    Gtk::Entry* mAckNameEntry;
    Gtk::TextView* mTextViewFunction;
    Gtk::Label* mHeaderAck;

    Gtk::Menu *mMenuTreeViewItems;

/**
 * This function activate or disactivate the TextView in the window.
 * @param a boolean, true to activate, false to disactivate.
 */
    void setSensitiveAck( bool );

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

    sigc::connection m_cntTreeAckListButtonRelease;
    sigc::connection m_cntTreeAckCursorChanged;

/**
 * This function is called when the user click on the TreeView.
 * @param the event.
 * @return a boolean.
 */
    bool onButtonRealeaseTreeAckList(GdkEventButton* event);

/**
 * This fonction is called when the user click on a acknowledge function in
 * the TreeView.
 */
    void onCursorChangeTreeAckList();

    strings_t mAckName;
    std::map < std::string, std::string > mAckFunction;
};

}
}
}
}    // namespace vle gvle modeling decision

#endif
