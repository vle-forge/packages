/*
 * @file vle/gvle/modeling/decision/FactDialog.cpp
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
#include <vle/gvle/modeling/decision/FactDialog.hpp>

namespace vle {
namespace gvle {
namespace modeling {
namespace decision {

    FactDialog::FactDialog(Glib::RefPtr < Gtk::Builder >& xml,
                           std::vector < std::string > pFactName,
                           std::map < std::string, std::string > pFactFunction,
                           std::map < std::string, std::string > pFactAttribute,
                           std::map < std::string, std::string > pFactInit) : mXml(xml),
        mTreeFactList(0)
{
    mFactName = pFactName;
    mFactFunction = pFactFunction;
    mFactAttribute = pFactAttribute;
    mFactInit = pFactInit;

    xml->get_widget("FactDialog", mDialog);
    xml->get_widget("treeViewFactList", mTreeFactList);
    xml->get_widget("factName", mFactNameEntry);
    mFactNameEntry->set_sensitive(true);
    mFactNameEntry->set_editable(false);
    xml->get_widget("factAttributes", mTextViewAttributes);
    xml->get_widget("factInit", mTextViewInit);
    xml->get_widget("factFunction", mTextViewFunction);
    xml->get_widget("HeaderFact", mHeaderFact);

    mFactNameEntry->set_text("");
    mHeaderFact->set_text("");
    mTextViewFunction->get_buffer()->set_text("");

    initTreeFactList();
    fillTreeFactList();
    initMenuPopupTreeFactList();

    m_cntTreeFactListButtonRelease = mTreeFactList->
            signal_button_release_event().connect(
    sigc::mem_fun( *this, &FactDialog::onButtonRealeaseTreeFactList));
    m_cntTreeFactCursorChanged = mTreeFactList->
            signal_cursor_changed().connect(
    sigc::mem_fun( *this, &FactDialog::onCursorChangeTreeFactList));

    setSensitiveFact(false);
}

FactDialog::~FactDialog()
{
    if (mTreeFactList) {
        mTreeFactList->remove_all_columns();
    }

    m_cntTreeFactListButtonRelease.disconnect();
    m_cntTreeFactCursorChanged.disconnect();

    mDialog->hide();
}

int FactDialog::run()
{
    mDialog->show_all();
    int state = mDialog->run();

    if (state == Gtk::RESPONSE_ACCEPT) {
        savePreviousFact(mFactNameEntry->get_text());
    }
    mDialog->hide();
    return state;
}

void FactDialog::initMenuPopupTreeFactList()
{
    Glib::RefPtr <Gtk::ActionGroup> mPopupActionGroup = Gtk::ActionGroup::create("FactDialog");
    
    mPopupActionGroup->add(Gtk::Action::create("FD_Add", _("_Add")), sigc::mem_fun(*this, &FactDialog::onAddFact));
    mPopupActionGroup->add(Gtk::Action::create("FD_Delete", _("_Delete")), sigc::mem_fun(*this, &FactDialog::onDeleteFact));
    mPopupActionGroup->add(Gtk::Action::create("FD_Rename", _("_Rename")), sigc::mem_fun(*this, &FactDialog::onRenameFact));
    
    Glib::RefPtr <Gtk::UIManager> mUIManager = Gtk::UIManager::create();
    mUIManager->insert_action_group(mPopupActionGroup);
    
    Glib::ustring ui_info =
                "<ui>"
                "  <popup name='FD_Popup'>"
                "    <menuitem action='FD_Add'/>"
                "    <menuitem action='FD_Delete'/>"
                "    <menuitem action='FD_Delete'/>"
                 "  </popup>"
                "</ui>";
    
    try {
      mUIManager->add_ui_from_string(ui_info);
      mMenuTreeViewItems = (Gtk::Menu *) (mUIManager->get_widget("/FD_Popup"));
    } catch(const Glib::Error& ex) {
      std::cerr << "building menus failed: FD_Popup " <<  ex.what();
    }
    
    if (!mMenuTreeViewItems)
      std::cerr << "not a menu : FD_Popup\n";
    
/*
    Gtk::Menu::MenuList& menulist(mMenuTreeViewItems.items());

    menulist.push_back(
        Gtk::Menu_Helpers::MenuElem(
            ("_Add"), sigc::mem_fun(
                *this, &FactDialog::onAddFact)));
    menulist.push_back(
        Gtk::Menu_Helpers::MenuElem(
            ("_Delete"), sigc::mem_fun(
                *this, &FactDialog::onDeleteFact)));
    menulist.push_back(
        Gtk::Menu_Helpers::MenuElem(
            ("_Rename"), sigc::mem_fun(
                *this, &FactDialog::onRenameFact)));

    mMenuTreeViewItems.accelerate(*mTreeFactList);
*/
}

void FactDialog::initTreeFactList()
{
    m_model = Gtk::ListStore::create(m_viewscolumnrecord);
    m_columnName = mTreeFactList->
                            append_column_editable(("Name"),
                            m_viewscolumnrecord.name);
    m_cellrenderer = dynamic_cast<Gtk::CellRendererText*>(
    mTreeFactList->get_column_cell_renderer(m_columnName - 1));
    m_cellrenderer->property_editable() = false;
    mTreeFactList->set_model(m_model);
}

void FactDialog::fillTreeFactList()
{
    // Ajout dans la liste du TreeView
    for ( std::vector < std::string > ::const_iterator it = mFactName.begin();
        it != mFactName.end() ; ++it ) {
        Gtk::TreeIter iter = m_model->append();
        if (iter) {
            Gtk::ListStore::Row row = *iter;
            row[m_viewscolumnrecord.name] = *it;
        }
    }
    m_iter = m_model->children().end();
}

bool FactDialog::onButtonRealeaseTreeFactList(GdkEventButton* event)
{
    if (event->button == 3) {
        mMenuTreeViewItems->popup(event->button, event->time);
    }
    return true;
}

void FactDialog::onAddFact() {
    SimpleTypeBox box( ("New fact name?"), "");
    std::string name = boost::trim_copy(box.run());
    if (box.valid()) {
        if (checkName(name)) {
            setSensitiveFact(false);
            Gtk::TreeIter iter = m_model->append();
            if (iter) {
                savePreviousFact(mFactNameEntry->get_text());
                mFactNameEntry->set_text(name);
                Gtk::ListStore::Row row = *iter;
                row[m_viewscolumnrecord.name] = name;

                mHeaderFact->set_text("void " + name
                + "(const vv::Value& value) {");
                mTextViewFunction->get_buffer()->set_text("var" + name +
                        " = value.toDouble().value();");
                mTextViewAttributes->get_buffer()->set_text( "double var" +
                        name +";");
                mTextViewInit->get_buffer()->set_text( "var" +
                        name + "(0.0)");

                m_iter = iter;
                mTreeFactList->set_cursor(m_model->get_path(iter));
                mFactName.push_back(name);
                setSensitiveFact(true);
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

void FactDialog::onDeleteFact()
{
    Glib::RefPtr < Gtk::TreeView::Selection > ref = mTreeFactList->
            get_selection();
    if (ref) {
        Gtk::TreeModel::iterator iter = ref->get_selected();
        if (iter && Question(_("Are you sure you want to delete this fact?"))) {
            mFactNameEntry->set_text("");
            mTextViewFunction->get_buffer()->set_text("");
            mTextViewAttributes->get_buffer()->set_text("");
            mTextViewInit->get_buffer()->set_text("");
            Gtk::TreeModel::Row row = *iter;
            std::string name(row.get_value(m_viewscolumnrecord.name));
            m_model->erase(iter);

            Gtk::TreeModel::Children children = m_model->children();
            m_iter = children.begin();
            mHeaderFact->set_text("");

            // Delete the element in the vector
            for (std::vector < std::string > ::iterator it = mFactName.begin();
                    it != mFactName.end(); ) {
                if ( *it == name ) {
                    it = mFactName.erase(it);
                }
                else {
                    ++it;
                }
            }

            if ( mFactFunction.find(name) != mFactFunction.end() ) {
                mFactFunction.erase(name);
            }
            if ( mFactAttribute.find(name) != mFactAttribute.end()) {
                mFactAttribute.erase(name);
            }
            if ( mFactInit.find(name) != mFactInit.end()) {
                mFactInit.erase(name);
            }
            setSensitiveFact(false);
        }
    }
}

void FactDialog::onRenameFact() {
    Glib::RefPtr < Gtk::TreeView::Selection > ref = mTreeFactList->
        get_selection();
    if (ref) {
        Gtk::TreeModel::iterator iter = ref->get_selected();
        if (iter) {
            Gtk::TreeModel::Row row = *iter;
            std::string oldName(row.get_value(m_viewscolumnrecord.name));
            savePreviousFact(oldName);

            SimpleTypeBox box(("Fact new name?"), "");
            std::string name = boost::trim_copy(box.run());
            if (box.valid() and checkName(name)) {

                setSensitiveFact(false);
                m_model->erase(iter);

                Gtk::TreeModel::Children children = m_model->children();
                m_iter = children.begin();

                iter = m_model->append();
                mFactNameEntry->set_text(name);
                Gtk::ListStore::Row row = *iter;
                row[m_viewscolumnrecord.name] = name;

                if (mFactFunction.find(oldName) != mFactFunction.end()) {
                    mTextViewFunction->get_buffer()->
                            set_text(mFactFunction[oldName]);
                    mFactFunction[name] = mFactFunction[oldName];
                    mFactFunction.erase(oldName);
                }

                if (mFactAttribute.find(oldName) != mFactAttribute.end()) {
                    mTextViewAttributes->get_buffer()->
                            set_text(mFactAttribute[oldName]);
                    mFactAttribute[name] = mFactAttribute[oldName];
                    mFactAttribute.erase(oldName);
                }

                if (mFactInit.find(oldName) != mFactInit.end()) {
                    mTextViewInit->get_buffer()->
                            set_text(mFactInit[oldName]);
                    mFactInit[name] = mFactInit[oldName];
                    mFactInit.erase(oldName);
                }

                mFactName.push_back(name);
                // Delete the element in the vector
                for (std::vector < std::string > ::iterator it =
                        mFactName.begin(); it != mFactName.end(); ) {
                    if ( *it == oldName ) {
                        it = mFactName.erase(it);
                    }
                    else {
                        ++it;
                    }
                }
                mTreeFactList->set_cursor(m_model->get_path(iter));
                mHeaderFact->set_text("void " + name
                        + "(const vv::Value& value) {");
                setSensitiveFact(true);
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

void FactDialog::onCursorChangeTreeFactList() {
    Glib::RefPtr < Gtk::TreeView::Selection > ref = mTreeFactList->
            get_selection();

    if (ref) {
        Gtk::TreeModel::iterator iter = ref->get_selected();
        if (iter) {
            Gtk::TreeModel::Row row = *iter;
            std::string name(row.get_value(m_viewscolumnrecord.name));
            savePreviousFact(mFactNameEntry->get_text());
            mFactNameEntry->set_text(name);

            if ( mFactFunction.find(name) != mFactFunction.end() ) {
                mTextViewFunction->get_buffer()->
                        set_text(mFactFunction[ name ]);
                mHeaderFact->set_text("void " + name
                        + "(const vv::Value& value) {");
            }
            else {
                mTextViewFunction->get_buffer()->set_text("");
                mHeaderFact->set_text("void " + name
                        + "(const vv::Value& value) {");
            }

            if ( mFactAttribute.find(name) != mFactAttribute.end()) {
                mTextViewAttributes->get_buffer()->
                        set_text(mFactAttribute[ name]);
            }
            else {
                mTextViewAttributes->get_buffer()->set_text("");
            }
            if ( mFactInit.find(name) != mFactInit.end()) {
                mTextViewInit->get_buffer()->
                        set_text(mFactInit[ name]);
            }
            else {
                mTextViewInit->get_buffer()->set_text("");
            }
            setSensitiveFact( true );
        }
    }
}

void FactDialog::setSensitiveFact(bool state)
{
    mTextViewInit->set_sensitive(state);
    mTextViewAttributes->set_sensitive(state);
    mTextViewFunction->set_sensitive(state);
}

void FactDialog::savePreviousFact(std::string name)
{
    if ( !name.empty() ) {
        if ( mFactFunction.find(name) != mFactFunction.end() ) {
            mFactFunction.erase(name);
            mFactFunction[ name ] = mTextViewFunction->get_buffer()->
                    get_text();
        }
        else {
            mFactFunction[ name ] = mTextViewFunction->get_buffer()->
                    get_text();
        }

        if ( mFactAttribute.find(name) != mFactAttribute.end()) {
            mFactAttribute.erase(name);
            mFactAttribute[ name ] = mTextViewAttributes->get_buffer()->
                    get_text();
        }
        else {
            mFactAttribute[name] = mTextViewAttributes->get_buffer()->
                    get_text();
        }
        if ( mFactInit.find(name) != mFactInit.end()) {
            mFactInit.erase(name);
            mFactInit[ name ] = mTextViewInit->get_buffer()->
                    get_text();
        }
        else {
            mFactInit[name] = mTextViewInit->get_buffer()->
                    get_text();
        }
    }
}

}
}
}
}    // namespace vle gvle modeling decision
