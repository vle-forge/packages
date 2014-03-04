/*
 * @file vle/gvle/modeling/decision/AckFunctionDialog.cpp
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

#include <vle/gvle/modeling/decision/AckFunctionDialog.hpp>

namespace vle {
namespace gvle {
namespace modeling {
namespace decision {

    AckFunctionDialog::AckFunctionDialog(
            Glib::RefPtr < Gtk::Builder >& xml,
            std::vector < std::string > pAckName,
            std::map < std::string, std::string > pAckFunction) :
        mXml(xml), mTreeAckList(0)
{
    mAckName = pAckName;
    mAckFunction = pAckFunction;

    xml->get_widget("AckFunctionDialog", mDialog);
    xml->get_widget("treeViewAck", mTreeAckList);
    xml->get_widget("AckName", mAckNameEntry);
    mAckNameEntry->set_sensitive(true);
    mAckNameEntry->set_editable(false);
    xml->get_widget("AckFunction", mTextViewFunction);
    xml->get_widget("HeaderAck", mHeaderAck);

    mAckNameEntry->set_text("");
    mHeaderAck->set_text("");
    mTextViewFunction->get_buffer()->set_text("");

    initTreeAckList();
    fillTreeAckList();
    initMenuPopupTreeAckList();

    m_cntTreeAckListButtonRelease = mTreeAckList->
            signal_button_release_event().connect(
    sigc::mem_fun( *this, &AckFunctionDialog::onButtonRealeaseTreeAckList));
    m_cntTreeAckCursorChanged = mTreeAckList->
            signal_cursor_changed().connect(
    sigc::mem_fun( *this, &AckFunctionDialog::onCursorChangeTreeAckList));

    setSensitiveAck(false);
}

AckFunctionDialog::~AckFunctionDialog()
{
    if (mTreeAckList) {
        mTreeAckList->remove_all_columns();
    }

    m_cntTreeAckListButtonRelease.disconnect();
    m_cntTreeAckCursorChanged.disconnect();

    mDialog->hide();
}

int AckFunctionDialog::run()
{
    mDialog->show_all();
    int state = mDialog->run();

    if (state == Gtk::RESPONSE_ACCEPT) {
        savePreviousAck(mAckNameEntry->get_text());
    }
    mDialog->hide();
    return state;
}

void AckFunctionDialog::initMenuPopupTreeAckList()
{
    Glib::RefPtr <Gtk::ActionGroup> mPopupActionGroup = Gtk::ActionGroup::create("AckFunctionDialog");
    mPopupActionGroup->add(Gtk::Action::create("AFD_Add", _("_Add")), sigc::mem_fun(*this, &AckFunctionDialog::onAddAck));
    mPopupActionGroup->add(Gtk::Action::create("AFD_Delete", _("_Delete")), sigc::mem_fun(*this, &AckFunctionDialog::onDeleteAck));
    mPopupActionGroup->add(Gtk::Action::create("AFD_Rename", _("_Rename")), sigc::mem_fun(*this, &AckFunctionDialog::onRenameAck));
    
    Glib::RefPtr <Gtk::UIManager> mUIManager = Gtk::UIManager::create();
    mUIManager->insert_action_group(mPopupActionGroup);
    
    Glib::ustring ui_info =
                "<ui>"
                "  <popup name='AFD_Popup'>"
                "    <menuitem action='AFD_Add'/>"
                "    <menuitem action='AFD_Delete'/>"
                "    <menuitem action='AFD_Rename'/>"
                 "  </popup>"
                "</ui>";
    
    try {
      mUIManager->add_ui_from_string(ui_info);
      mMenuTreeViewItems = (Gtk::Menu *) (mUIManager->get_widget("/AFD_Popup"));
    } catch(const Glib::Error& ex) {
      std::cerr << "building menus failed: AFD_Popup " <<  ex.what();
    }
    
    if (!mMenuTreeViewItems)
      std::cerr << "not a menu : AFD_Popup\n";
/*
    Gtk::Menu::MenuList& menulist(mMenuTreeViewItems.items());

    menulist.push_back(
        Gtk::Menu_Helpers::MenuElem(
            ("_Add"), sigc::mem_fun(
                *this, &AckFunctionDialog::onAddAck)));
    menulist.push_back(
        Gtk::Menu_Helpers::MenuElem(
            ("_Delete"), sigc::mem_fun(
                *this, &AckFunctionDialog::onDeleteAck)));
    menulist.push_back(
        Gtk::Menu_Helpers::MenuElem(
            ("_Rename"), sigc::mem_fun(
                *this, &AckFunctionDialog::onRenameAck)));

    mMenuTreeViewItems.accelerate(*mTreeAckList);
*/
}

void AckFunctionDialog::initTreeAckList()
{
    m_model = Gtk::ListStore::create(m_viewscolumnrecord);
    m_columnName = mTreeAckList->
                            append_column_editable(("Name"),
                            m_viewscolumnrecord.name);
    m_cellrenderer = dynamic_cast<Gtk::CellRendererText*>(
    mTreeAckList->get_column_cell_renderer(m_columnName - 1));
    m_cellrenderer->property_editable() = false;
    mTreeAckList->set_model(m_model);
}

void AckFunctionDialog::fillTreeAckList()
{
    // Ajout dans la liste du TreeView
    for (std::vector < std::string > ::const_iterator it = mAckName.begin();
        it != mAckName.end() ; ++it ) {
        Gtk::TreeIter iter = m_model->append();
        if (iter) {
            Gtk::ListStore::Row row = *iter;
            row[m_viewscolumnrecord.name] = *it;
        }
    }
    m_iter = m_model->children().end();
}

bool AckFunctionDialog::onButtonRealeaseTreeAckList(GdkEventButton* event)
{
    if (event->button == 3) {
        mMenuTreeViewItems->popup(event->button, event->time);
    }
    return true;
}

void AckFunctionDialog::onAddAck() {
    SimpleTypeBox box( ("New acknowledge function name?"), "");
    std::string name = boost::trim_copy(box.run());

    if (box.valid()) {
        if (checkName(name)) {
            setSensitiveAck(false);
            Gtk::TreeIter iter = m_model->append();
            if (iter) {
                savePreviousAck(mAckNameEntry->get_text());
                mAckNameEntry->set_text(name);
                Gtk::ListStore::Row row = *iter;
                row[m_viewscolumnrecord.name] = name;

                mHeaderAck->set_text("void " + name + "(const std::string&"\
                        "activityname,\n\t"\
                        "const ved::Activity& activity) {");

                std::string generatedFunc = "";

                mTextViewFunction->get_buffer()->set_text(generatedFunc);

                m_iter = iter;
                mTreeAckList->set_cursor(m_model->get_path(iter));
                mAckName.push_back(name);
                setSensitiveAck(true);
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

void AckFunctionDialog::onDeleteAck()
{
    Glib::RefPtr < Gtk::TreeView::Selection > ref = mTreeAckList->
            get_selection();
    if (ref) {
        Gtk::TreeModel::iterator iter = ref->get_selected();
        if (iter) {
            if (Question(_("Are you sure you want to delete this function?"))) {
                mAckNameEntry->set_text("");
                mTextViewFunction->get_buffer()->set_text("");
                mHeaderAck->set_text("");

                Gtk::TreeModel::Row row = *iter;
                std::string name(row.get_value(m_viewscolumnrecord.name));
                m_model->erase(iter);

                Gtk::TreeModel::Children children = m_model->children();
                m_iter = children.begin();

                // Delete the element in the vector
                for (std::vector <std::string> ::iterator it = mAckName.begin();
                        it != mAckName.end(); ) {
                    if ( *it == name ) {
                        it = mAckName.erase(it);
                    }
                    else {
                        ++it;
                    }
                }

                if ( mAckFunction.find(name) != mAckFunction.end() ) {
                    mAckFunction.erase(name);
                }
                setSensitiveAck(false);
            }
        }
    }
}

void AckFunctionDialog::onRenameAck() {
    Glib::RefPtr < Gtk::TreeView::Selection > ref = mTreeAckList->
        get_selection();
    if (ref) {
        Gtk::TreeModel::iterator iter = ref->get_selected();
        if (iter) {
            Gtk::TreeModel::Row row = *iter;
            std::string oldName(row.get_value(m_viewscolumnrecord.name));
            savePreviousAck(oldName);

            SimpleTypeBox box(("Acknowledge function new name?"), "");
            std::string name = boost::trim_copy(box.run());
            if (box.valid()) {
                if (checkName(name)) {
                    setSensitiveAck(false);
                    m_model->erase(iter);

                    Gtk::TreeModel::Children children = m_model->children();
                    m_iter = children.begin();

                    iter = m_model->append();
                    mAckNameEntry->set_text(name);
                    Gtk::ListStore::Row row = *iter;
                    row[m_viewscolumnrecord.name] = name;

                    if (mAckFunction.find(oldName) != mAckFunction.end()) {
                        mTextViewFunction->get_buffer()->
                                set_text(mAckFunction[oldName]);
                        mAckFunction[name] = mAckFunction[oldName];
                        mAckFunction.erase(oldName);
                    }

                    mAckName.push_back(name);
                    mHeaderAck->set_text("void " + name + "(const std::string&"\
                        "activityname,\n\t"\
                        "const ved::Activity& activity) {");
                    // Delete the element in the vector
                    for (std::vector < std::string > ::iterator it =
                            mAckName.begin(); it != mAckName.end(); ) {
                        if ( *it == oldName ) {
                            it = mAckName.erase(it);
                        }
                        else {
                            ++it;
                        }
                    }
                    mTreeAckList->set_cursor(m_model->get_path(iter));
                    setSensitiveAck(true);
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

void AckFunctionDialog::onCursorChangeTreeAckList() {
    Glib::RefPtr < Gtk::TreeView::Selection > ref = mTreeAckList->
            get_selection();

    if (ref) {
        Gtk::TreeModel::iterator iter = ref->get_selected();
        if (iter) {
            Gtk::TreeModel::Row row = *iter;
            std::string name(row.get_value(m_viewscolumnrecord.name));
            savePreviousAck(mAckNameEntry->get_text());
            mAckNameEntry->set_text(name);

            if ( mAckFunction.find(name) != mAckFunction.end() ) {
                mTextViewFunction->get_buffer()->
                        set_text(mAckFunction[ name ]);
                mHeaderAck->set_text("void " + name + "(const std::string&"\
                    "activityname,\n\t"\
                    "const ved::Activity& activity) {");
            }
            else {
                mTextViewFunction->get_buffer()->set_text("");
                mHeaderAck->set_text("void " + name + "(const std::string&"\
                    "activityname,\n\t"\
                    "const ved::Activity& activity) {");
            }

            setSensitiveAck( true );
        }
    }
}

void AckFunctionDialog::setSensitiveAck(bool state)
{
    mTextViewFunction->set_sensitive(state);
}

void AckFunctionDialog::savePreviousAck(std::string name)
{
    if ( !name.empty() ) {
        if ( mAckFunction.find(name) != mAckFunction.end() ) {
            mAckFunction.erase(name);
            mAckFunction[ name ] = mTextViewFunction->get_buffer()->
                    get_text();
        }
        else {
            mAckFunction[ name ] = mTextViewFunction->get_buffer()->
                    get_text();
        }
    }
}

}
}
}
}    // namespace vle gvle modeling decision
