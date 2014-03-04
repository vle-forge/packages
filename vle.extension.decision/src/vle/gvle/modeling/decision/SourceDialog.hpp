/*
 * @file vle/gvle/modeling/decision/SourceDialog.hpp
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

#ifndef VLE_GVLE_DIALOGSOURCE_HPP
#define VLE_GVLE_DIALOGSOURCE_HPP

#include <vle/version.hpp>
#include <vle/gvle/Settings.hpp>
#include <gtkmm.h>
#include <gtkmm/builder.h>
#include <vle/gvle/Editor.hpp>
#include <vle/gvle/SimpleTypeBox.hpp>
#include <vle/gvle/Message.hpp>
#include <boost/algorithm/string.hpp>
#include <boost/format.hpp>

#include <vle/gvle/modeling/decision/Utils.hpp>

namespace vle {
namespace gvle {
namespace modeling {
namespace decision {

/**
 * @class SourceDialog
 * @brief This class is used to manage the user sources.
 */

class SourceDialog
{
public:
    SourceDialog(Glib::RefPtr < Gtk::Builder >& xml,
                 std::map < std::string, std::string > pParam,
                 std::string pIncludes,
                 std::string pConstructor,
                 std::string pMembers);
    virtual ~SourceDialog();

    void add(const std::string& name, const std::string& buffer);
    std::string buffer(const std::string& name) const;

    int run();

/**
 * @brief This function initialize the popup menu in the tree view.
 */
    void initMenuPopupTreeParam();

    void onAddParam()
    {
        SimpleTypeBox box(("New param name?"), "");
        std::string name = boost::trim_copy(box.run());
        if (box.valid()) {
            if (checkName(name)) {
                setSensitive(false);
                Gtk::TreeIter iter = m_model->append();
                if (iter) {
                    savePreviousParam(mNameParam->get_text());
                    mNameParam->set_text(name);
                    mValueParam->set_text("1");
                    mParam[name] = "1";
                    Gtk::ListStore::Row row = *iter;
                    row[m_viewscolumnrecord.name] = name;

                    m_iter = iter;
                    mTreeViewParam->set_cursor(m_model->get_path(iter));
                    setSensitive(true);
                }
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

    void fillParam()
    {
        // Ajout dans la liste du TreeView
        for (std::map < std::string, std::string > ::const_iterator it =
            mParam.begin();
            it != mParam.end() ; ++it ) {
            Gtk::TreeIter iter = m_model->append();
            if (iter) {
                Gtk::ListStore::Row row = *iter;
                row[m_viewscolumnrecord.name] = it->first;
            }
        }
        m_iter = m_model->children().end();
    }

    void onDeleteParam()
    {
        Glib::RefPtr < Gtk::TreeView::Selection > ref = mTreeViewParam->
                get_selection();
        if (ref) {
            Gtk::TreeModel::iterator iter = ref->get_selected();
            if (iter && Question(_("Are you sure you want to delete this param?"))) {
                mNameParam->set_text("");
                mValueParam->set_text("");

                Gtk::TreeModel::Row row = *iter;
                std::string name(row.get_value(m_viewscolumnrecord.name));
                m_model->erase(iter);

                Gtk::TreeModel::Children children = m_model->children();
                m_iter = children.begin();

                // Delete the element in the map
                std::map < std::string, std::string > ::iterator it =
                    mParam.find(name);
                if (it != mParam.end()) {
                    mParam.erase(it);
                }

                setSensitive(false);
            }
        }
    }

    void onRenameParam()
    {
        Glib::RefPtr < Gtk::TreeView::Selection > ref = mTreeViewParam->
            get_selection();
        if (ref) {
            Gtk::TreeModel::iterator iter = ref->get_selected();
            if (iter) {
                Gtk::TreeModel::Row row = *iter;
                std::string oldName(row.get_value(m_viewscolumnrecord.name));
                savePreviousParam(oldName);

                SimpleTypeBox box(("Param new name?"), "");
                std::string name = boost::trim_copy(box.run());
                if (box.valid()) {
                    if (checkName(name)) {
                        setSensitive(false);
                        m_model->erase(iter);

                        Gtk::TreeModel::Children children = m_model->children();
                        m_iter = children.begin();

                        iter = m_model->append();
                        mNameParam->set_text(name);
                        Gtk::ListStore::Row row = *iter;
                        row[m_viewscolumnrecord.name] = name;

                        // Delete the element in the map
                        std::map < std::string, std::string > ::iterator it =
                                mParam.find(oldName);
                        if (it != mParam.end()) {
                            mParam.erase(it);
                        }

                        mParam[name] = mValueParam->get_text();

                        mTreeViewParam->set_cursor(m_model->get_path(iter));
                        setSensitive(true);
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
    }

    /**
     * @brief This function check if a fact name is valid
     * @param the name of the fact
     */
    bool checkName(std::string name)
    {
        if (!Utils::isValidName(name)) {
            return false;
        }
        std::map < std::string, std::string > ::const_iterator it =
            mParam.find(name);
        if (it != mParam.end())
        {
            return false;
        }
        return true;
    }

    std::string getIncludes()
    {
        return mIncludes->get_buffer()->get_text();
    }

    std::string getMembers()
    {
        return mMembers->get_buffer()->get_text();
    }

    std::string getCustomConstructor()
    {
        return mConstructor->get_buffer()->get_text();
    }

    std::map <std::string, std::string > getParam()
    {
        return mParam;
    }

private:
    Glib::RefPtr < Gtk::Builder > mXml;

    Gtk::Dialog* mDialog;
    Gtk::Notebook* mNotebook;
    Gtk::TextView* mIncludes;
    Gtk::TextView* mConstructor;
    Gtk::TextView* mMembers;
    Gtk::Button* mAddParam;
    Gtk::Button* mDelParam;
    Gtk::Entry* mNameParam;
    Gtk::Entry* mValueParam;
    std::map < std::string, DocumentText* > mFunctions;
    std::list < sigc::connection > mList;

    // To manage the tree view param
    Gtk::TreeView* mTreeViewParam;
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

    sigc::connection m_cntParamButtonRelease;
    sigc::connection m_cntParamCursorChanged;

    std::map < std::string, std::string > mParam;
    Gtk::Menu *mMenuTreeViewItems;

    void initTreeParam()
    {
        m_model = Gtk::ListStore::create(m_viewscolumnrecord);
        m_columnName = mTreeViewParam->
                                append_column_editable(("Name"),
                                m_viewscolumnrecord.name);
        m_cellrenderer = dynamic_cast<Gtk::CellRendererText*>(
        mTreeViewParam->get_column_cell_renderer(m_columnName - 1));
        m_cellrenderer->property_editable() = false;
        mTreeViewParam->set_model(m_model);
    }

/**
 * @brief This function is called when the user click on the TreeView.
 * @param the event.
 * @return a boolean.
 */
    bool onButtonRealeaseParam(GdkEventButton* event);

/**
 * @brief This fonction is called when the user click on a fact in
 * the TreeView.
 */
    void onCursorChangeParam()
    {
        Glib::RefPtr < Gtk::TreeView::Selection > ref = mTreeViewParam->
                get_selection();

        if (ref) {
            Gtk::TreeModel::iterator iter = ref->get_selected();
            if (iter) {
                Gtk::TreeModel::Row row = *iter;
                std::string name(row.get_value(m_viewscolumnrecord.name));
                savePreviousParam(mNameParam->get_text());
                mNameParam->set_text(name);
                mValueParam->set_text(mParam[name]);

                setSensitive(true);
            }
        }
    }

    void savePreviousParam(std::string name)
    {
        if (!mValueParam->get_text().empty()) {
            if (mParam.find(name) != mParam.end()) {
                mParam.erase(name);
                mParam[name] = mValueParam->get_text();
            }
        }
    }

    void setSensitive(bool state)
    {
        mNameParam->set_sensitive(state);
        mValueParam->set_sensitive(state);
    }
};

}
}
}
}    // namespace vle gvle modeling decision

#endif
