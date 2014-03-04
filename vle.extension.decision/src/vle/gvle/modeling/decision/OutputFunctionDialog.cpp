/*
 * @file vle/gvle/modeling/decision/OutputFunctionDialog.cpp
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

#include <vle/gvle/modeling/decision/OutputFunctionDialog.hpp>

namespace vle {
namespace gvle {
namespace modeling {
namespace decision {

    OutputFunctionDialog::OutputFunctionDialog(
            Glib::RefPtr < Gtk::Builder >& xml,
            std::vector < std::string > pOFName,
            std::map < std::string, std::string > pOFFunction) :
        mXml(xml), mTreeOFList(0)
{
    mOFName = pOFName;
    mOFFunction = pOFFunction;

    xml->get_widget("OutputFunctionDialog", mDialog);
    xml->get_widget("treeViewOF", mTreeOFList);
    xml->get_widget("OFName", mOFNameEntry);
    mOFNameEntry->set_sensitive(true);
    mOFNameEntry->set_editable(false);
    xml->get_widget("OFFunction", mTextViewFunction);
    xml->get_widget("HeaderOutput", mHeaderOutput);

    mOFNameEntry->set_text("");
    mHeaderOutput->set_text("");
    mTextViewFunction->get_buffer()->set_text("");

    initTreeOFList();
    fillTreeOFList();
    initMenuPopupTreeOFList();

    m_cntTreeOFListButtonRelease = mTreeOFList->
            signal_button_release_event().connect(
    sigc::mem_fun( *this, &OutputFunctionDialog::onButtonRealeaseTreeOFList));
    m_cntTreeOFCursorChanged = mTreeOFList->
            signal_cursor_changed().connect(
    sigc::mem_fun( *this, &OutputFunctionDialog::onCursorChangeTreeOFList));

    setSensitiveOF(false);
}

OutputFunctionDialog::~OutputFunctionDialog()
{
    if (mTreeOFList) {
        mTreeOFList->remove_all_columns();
    }

    m_cntTreeOFListButtonRelease.disconnect();
    m_cntTreeOFCursorChanged.disconnect();

    mDialog->hide();
}

int OutputFunctionDialog::run()
{
    mDialog->show_all();
    int state = mDialog->run();

    if (state == Gtk::RESPONSE_ACCEPT) {
        savePreviousOF(mOFNameEntry->get_text());
    }
    mDialog->hide();
    return state;
}

void OutputFunctionDialog::initMenuPopupTreeOFList()
{
    Glib::RefPtr <Gtk::ActionGroup> mPopupActionGroup = Gtk::ActionGroup::create("OutputFunctionDialog");
    mPopupActionGroup->add(Gtk::Action::create("OFD_Add", _("_Add")), sigc::mem_fun(*this, &OutputFunctionDialog::onAddOF));
    mPopupActionGroup->add(Gtk::Action::create("OFD_Delete", _("_Delete")), sigc::mem_fun(*this, &OutputFunctionDialog::onDeleteOF));
    mPopupActionGroup->add(Gtk::Action::create("OFD_Rename", _("_Rename")), sigc::mem_fun(*this, &OutputFunctionDialog::onRenameOF));
    
    Glib::RefPtr <Gtk::UIManager> mUIManager = Gtk::UIManager::create();
    mUIManager->insert_action_group(mPopupActionGroup);
    
    Glib::ustring ui_info =
                "<ui>"
                "  <popup name='OFD_Popup'>"
                "    <menuitem action='OFD_Add'/>"
                "    <menuitem action='OFD_Delete'/>"
                "    <menuitem action='OFD_Rename'/>"
                 "  </popup>"
                "</ui>";
    
    try {
      mUIManager->add_ui_from_string(ui_info);
      mMenuTreeViewItems = (Gtk::Menu *) (mUIManager->get_widget("/OFD_Popup"));
    } catch(const Glib::Error& ex) {
      std::cerr << "building menus failed: OFD_Popup " <<  ex.what();
    }
    
    if (!mMenuTreeViewItems)
      std::cerr << "not a menu : OFD_Popup\n";
/*
    Gtk::Menu::MenuList& menulist(mMenuTreeViewItems.items());

    menulist.push_back(
        Gtk::Menu_Helpers::MenuElem(
            ("_Add"), sigc::mem_fun(
                *this, &OutputFunctionDialog::onAddOF)));
    menulist.push_back(
        Gtk::Menu_Helpers::MenuElem(
            ("_Delete"), sigc::mem_fun(
                *this, &OutputFunctionDialog::onDeleteOF)));
    menulist.push_back(
        Gtk::Menu_Helpers::MenuElem(
            ("_Rename"), sigc::mem_fun(
                *this, &OutputFunctionDialog::onRenameOF)));

    mMenuTreeViewItems.accelerate(*mTreeOFList);
*/
}

void OutputFunctionDialog::initTreeOFList()
{
    m_model = Gtk::ListStore::create(m_viewscolumnrecord);
    m_columnName = mTreeOFList->
                            append_column_editable(("Name"),
                            m_viewscolumnrecord.name);
    m_cellrenderer = dynamic_cast<Gtk::CellRendererText*>(
    mTreeOFList->get_column_cell_renderer(m_columnName - 1));
    m_cellrenderer->property_editable() = false;
    mTreeOFList->set_model(m_model);
}

void OutputFunctionDialog::fillTreeOFList()
{
    // Ajout dans la liste du TreeView
    for ( std::vector < std::string > ::const_iterator it = mOFName.begin() ;
        it != mOFName.end() ; ++it ) {
        Gtk::TreeIter iter = m_model->append();
        if (iter) {
            Gtk::ListStore::Row row = *iter;
            row[m_viewscolumnrecord.name] = *it;
        }
    }
    m_iter = m_model->children().end();
}

bool OutputFunctionDialog::onButtonRealeaseTreeOFList(GdkEventButton* event)
{
    if (event->button == 3) {
        mMenuTreeViewItems->popup(event->button, event->time);
    }
    return true;
}

void OutputFunctionDialog::onAddOF() {
    SimpleTypeBox box( ("New output function name?"), "");
    std::string name = boost::trim_copy(box.run());
    if (box.valid() and checkName(name)) {
        setSensitiveOF(false);
        Gtk::TreeIter iter = m_model->append();
        if (iter) {
            savePreviousOF(mOFNameEntry->get_text());
            mOFNameEntry->set_text(name);
            Gtk::ListStore::Row row = *iter;
            row[m_viewscolumnrecord.name] = name;

            mHeaderOutput->set_text("void " + name + "(\n"\
                "const std::string& name,\nconst ved::Activity& activity,\n"\
                "vd::ExternalEventList& output) {");

            std::string generatedFunc = "if (activity.isInStartedState()) {\n"\
                "        vd::ExternalEvent* evt = new vd::ExternalEvent(\""
                + name + "\");\n" +
                "        evt->putAttribute(\"name\", new vv::String(name));\n"\
                "        evt->putAttribute(\"activity\", new vv::String"\
                "(name));\n"\
                "        evt->putAttribute(\"value\", new vv::Double(1.));\n"\
                "        output.push_back(evt);\n}";

            mTextViewFunction->get_buffer()->set_text(generatedFunc);

            m_iter = iter;
            mTreeOFList->set_cursor(m_model->get_path(iter));
            mOFName.push_back(name);
            setSensitiveOF(true);
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

void OutputFunctionDialog::onDeleteOF()
{
    Glib::RefPtr < Gtk::TreeView::Selection > ref = mTreeOFList->
            get_selection();
    if (ref) {
        Gtk::TreeModel::iterator iter = ref->get_selected();
        if (iter) {
            if (Question(_("Are you sure you want to delete this function?"))) {
                mOFNameEntry->set_text("");
                mTextViewFunction->get_buffer()->set_text("");
                mHeaderOutput->set_text("");

                Gtk::TreeModel::Row row = *iter;
                std::string name(row.get_value(m_viewscolumnrecord.name));
                m_model->erase(iter);

                Gtk::TreeModel::Children children = m_model->children();
                m_iter = children.begin();

                // Delete the element in the vector
                for (std::vector < std::string > ::iterator it = mOFName.begin();
                        it != mOFName.end(); ) {
                    if ( *it == name ) {
                        it = mOFName.erase(it);
                    }
                    else {
                        ++it;
                    }
                }

                if ( mOFFunction.find(name) != mOFFunction.end() ) {
                    mOFFunction.erase(name);
                }
                setSensitiveOF(false);
            }
        }
    }
}

void OutputFunctionDialog::onRenameOF() {
    Glib::RefPtr < Gtk::TreeView::Selection > ref = mTreeOFList->
        get_selection();
    if (ref) {
        Gtk::TreeModel::iterator iter = ref->get_selected();
        if (iter) {
            Gtk::TreeModel::Row row = *iter;
            std::string oldName(row.get_value(m_viewscolumnrecord.name));
            savePreviousOF(oldName);

            SimpleTypeBox box(("New name of the output function?"), "");
            std::string name = boost::trim_copy(box.run());
            if (box.valid() and checkName(name)) {

                setSensitiveOF(false);
                m_model->erase(iter);

                Gtk::TreeModel::Children children = m_model->children();
                m_iter = children.begin();

                iter = m_model->append();
                mOFNameEntry->set_text(name);
                Gtk::ListStore::Row row = *iter;
                row[m_viewscolumnrecord.name] = name;

                if (mOFFunction.find(oldName) != mOFFunction.end()) {
                    mTextViewFunction->get_buffer()->
                            set_text(mOFFunction[oldName]);
                    mOFFunction[name] = mOFFunction[oldName];
                    mOFFunction.erase(oldName);
                }

                mOFName.push_back(name);
                mHeaderOutput->set_text("void " + name + "(\n"\
                    "const std::string& name,\nconst ved::Activity& activity,\n"\
                    "vd::ExternalEventList& output) {");
                // Delete the element in the vector
                for (std::vector < std::string > ::iterator it =
                        mOFName.begin(); it != mOFName.end(); ) {
                    if ( *it == oldName ) {
                        it = mOFName.erase(it);
                    }
                    else {
                        ++it;
                    }
                }
                mTreeOFList->set_cursor(m_model->get_path(iter));
                setSensitiveOF(true);
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

void OutputFunctionDialog::onCursorChangeTreeOFList() {
    Glib::RefPtr < Gtk::TreeView::Selection > ref = mTreeOFList->
            get_selection();

    if (ref) {
        Gtk::TreeModel::iterator iter = ref->get_selected();
        if (iter) {
            Gtk::TreeModel::Row row = *iter;
            std::string name(row.get_value(m_viewscolumnrecord.name));
            savePreviousOF(mOFNameEntry->get_text());
            mOFNameEntry->set_text(name);

            if ( mOFFunction.find(name) != mOFFunction.end() ) {
                mTextViewFunction->get_buffer()->
                        set_text(mOFFunction[ name ]);
                mHeaderOutput->set_text("void " + name + "(\n"\
                        "const std::string& name,\nconst ved::Activity& activity,\n"\
                        "vd::ExternalEventList& output) {");
            }
            else {
                mTextViewFunction->get_buffer()->set_text("");
                mHeaderOutput->set_text("void " + name + "(\n"\
                        "const std::string& name,\nconst ved::Activity& activity,\n"\
                        "vd::ExternalEventList& output) {");
            }

            setSensitiveOF( true );
        }
    }
}

void OutputFunctionDialog::setSensitiveOF(bool state)
{
    mTextViewFunction->set_sensitive(state);
}

void OutputFunctionDialog::savePreviousOF(std::string name)
{
    if ( !name.empty() ) {
        if ( mOFFunction.find(name) != mOFFunction.end() ) {
            mOFFunction.erase(name);
            mOFFunction[ name ] = mTextViewFunction->get_buffer()->
                    get_text();
        }
        else {
            mOFFunction[ name ] = mTextViewFunction->get_buffer()->
                    get_text();
        }
    }
}

}
}
}
}    // namespace vle gvle modeling decision
