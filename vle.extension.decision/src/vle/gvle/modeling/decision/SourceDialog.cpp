/*
 * @file vle/gvle/modeling/decision/SourceDialog.cpp
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
#include <vle/gvle/modeling/decision/SourceDialog.hpp>

namespace vle {
namespace gvle {
namespace modeling {
namespace decision {

SourceDialog::SourceDialog(Glib::RefPtr < Gtk::Builder >& xml,
                           std::map < std::string, std::string > pParam,
                           std::string pIncludes,
                           std::string pConstructor,
                           std::string pMembers) : mXml(xml)
{
    //~ xml->get_widget("SourceDialog", mDialog);
    //~ xml->get_widget("NotebookSource", mNotebook);
    xml->get_widget("ParametersDialog", mDialog);
    xml->get_widget("IncludeText", mIncludes);
    xml->get_widget("ConstructorText", mConstructor);
    xml->get_widget("MembersText", mMembers);
    xml->get_widget("addParam", mAddParam);
    xml->get_widget("delParam", mDelParam);
    xml->get_widget("nameParam", mNameParam);
    mNameParam->set_editable(false);
    xml->get_widget("valueParam", mValueParam);
    xml->get_widget("treeViewParam", mTreeViewParam);

    mParam = pParam;
    mIncludes->get_buffer()->set_text(pIncludes);
    mConstructor->get_buffer()->set_text(pConstructor);
    mMembers->get_buffer()->set_text(pMembers);
    initTreeParam();
    fillParam();
    initMenuPopupTreeParam();

    mList.push_back(mAddParam->signal_clicked().connect(
        sigc::mem_fun(*this, &SourceDialog::onAddParam)));
    mList.push_back(mDelParam->signal_clicked().connect(
        sigc::mem_fun(*this, &SourceDialog::onDeleteParam)));

    m_cntParamButtonRelease = mTreeViewParam->
            signal_button_release_event().connect(
    sigc::mem_fun( *this, &SourceDialog::onButtonRealeaseParam));
    m_cntParamCursorChanged = mTreeViewParam->
            signal_cursor_changed().connect(
    sigc::mem_fun( *this, &SourceDialog::onCursorChangeParam));

    setSensitive(false);
}

SourceDialog::~SourceDialog()
{
    if (mTreeViewParam) {
        mTreeViewParam->remove_all_columns();
    }
    for (std::list < sigc::connection >::iterator it = mList.begin();
            it != mList.end(); ++it) {
        it->disconnect();
    }

    m_cntParamButtonRelease.disconnect();
    m_cntParamCursorChanged.disconnect();

    mDialog->hide();
}

void SourceDialog::add(const std::string& name, const std::string& buffer)
{
    mFunctions[name] = Gtk::manage(new DocumentText(buffer));
    mNotebook->append_page(*mFunctions[name], name);
}

bool SourceDialog::onButtonRealeaseParam(GdkEventButton* event)
{
    if (event->button == 3) {
        mMenuTreeViewItems->popup(event->button, event->time);
    }
    return true;
}

std::string SourceDialog::buffer(const std::string& name) const
{
    return mFunctions.find(name)->second->getBuffer();
}

int SourceDialog::run()
{
    mDialog->show_all();
    return mDialog->run();
}

void SourceDialog::initMenuPopupTreeParam()
{
    Glib::RefPtr <Gtk::ActionGroup> mPopupActionGroup = Gtk::ActionGroup::create("SourceDialog");
    
    mPopupActionGroup->add(Gtk::Action::create("SD_Add", _("_Add")), sigc::mem_fun(*this, &SourceDialog::onAddParam));
    mPopupActionGroup->add(Gtk::Action::create("SD_Delete", _("_Delete")), sigc::mem_fun(*this, &SourceDialog::onDeleteParam));
    mPopupActionGroup->add(Gtk::Action::create("SD_Rename", _("_Rename")), sigc::mem_fun(*this, &SourceDialog::onRenameParam));
    
    Glib::RefPtr <Gtk::UIManager> mUIManager = Gtk::UIManager::create();
    mUIManager->insert_action_group(mPopupActionGroup);
    
    Glib::ustring ui_info =
                "<ui>"
                "  <popup name='SD_Popup'>"
                "    <menuitem action='SD_Add'/>"
                "    <menuitem action='SD_Delete'/>"
                "    <menuitem action='SD_Delete'/>"
                 "  </popup>"
                "</ui>";
    
    try {
      mUIManager->add_ui_from_string(ui_info);
      mMenuTreeViewItems = (Gtk::Menu *) (mUIManager->get_widget("/SD_Popup"));
    } catch(const Glib::Error& ex) {
      std::cerr << "building menus failed: SD_Popup " <<  ex.what();
    }
    
    if (!mMenuTreeViewItems)
      std::cerr << "not a menu : SD_Popup\n";
/*
    Gtk::Menu::MenuList& menulist(mMenuTreeViewItems.items());

    menulist.push_back(
        Gtk::Menu_Helpers::MenuElem(
            ("_Add"), sigc::mem_fun(
                *this, &SourceDialog::onAddParam)));
    menulist.push_back(
        Gtk::Menu_Helpers::MenuElem(
            ("_Delete"), sigc::mem_fun(
                *this, &SourceDialog::onDeleteParam)));
    menulist.push_back(
        Gtk::Menu_Helpers::MenuElem(
            ("_Rename"), sigc::mem_fun(
                *this, &SourceDialog::onRenameParam)));

    mMenuTreeViewItems.accelerate(*mTreeViewParam);
*/
}

}
}
}
}    // namespace vle gvle modeling decision
