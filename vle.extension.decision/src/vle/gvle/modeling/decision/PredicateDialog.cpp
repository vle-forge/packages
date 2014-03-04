/*
 * @file vle/gvle/modeling/decision/PredicateDialog.cpp
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
#include <vle/gvle/modeling/decision/PredicateDialog.hpp>

namespace vle {
namespace gvle {
namespace modeling {
namespace decision {

    PredicateDialog::PredicateDialog(Glib::RefPtr < Gtk::Builder >& xml,
        std::vector < std::string > pPredicateName,
    std::map < std::string, std::string > pPredicateFunction,
    hierarchicalPred pPred) :
        mXml(xml),
        mTreePredicateList(0),
        mPredicateName(pPredicateName),
        mPredicateFunction(pPredicateFunction),
        mPred(pPred)
{
    xml->get_widget("PredicateDialog", mDialog);
    xml->get_widget("treeViewPredicate", mTreePredicateList);
    xml->get_widget("PredicateName", mPredicateNameEntry);
    mPredicateNameEntry->set_sensitive(true);
    mPredicateNameEntry->set_editable(false);
    xml->get_widget("PredicateFunction", mTextViewFunction);
    xml->get_widget("FooterPred", mHeaderPred);
    mHeaderPred->set_justify(Gtk::JUSTIFY_LEFT);
    xml->get_widget("HeaderPred", mHeaderPred);

    mPredicateNameEntry->set_text("");
    mHeaderPred->set_text("");
    mTextViewFunction->get_buffer()->set_text("");

    initTreePredicateList();
    fillTreePredicateList();
    initMenuPopupTreePredicateList();

    m_cntTreePredicateListButtonRelease = mTreePredicateList->
            signal_button_release_event().connect(
    sigc::mem_fun( *this, &PredicateDialog::onButtonRealeaseTreePredicateList));
    m_cntTreePredicateCursorChanged = mTreePredicateList->
            signal_cursor_changed().connect(
    sigc::mem_fun( *this, &PredicateDialog::onCursorChangeTreePredicateList));

    setSensitivePredicate(false);
}

PredicateDialog::~PredicateDialog()
{
    if (mTreePredicateList) {
        mTreePredicateList->remove_all_columns();
    }

    m_cntTreePredicateListButtonRelease.disconnect();
    m_cntTreePredicateCursorChanged.disconnect();

    mDialog->hide();
}

int PredicateDialog::run()
{
    mDialog->show_all();
    int state = mDialog->run();

    if (state == Gtk::RESPONSE_ACCEPT || state == Gtk::RESPONSE_OK) {
        savePreviousPredicate(mPredicateNameEntry->get_text());
    }
    mDialog->hide();
    return state;
}

void PredicateDialog::initMenuPopupTreePredicateList()
{
    Glib::RefPtr <Gtk::ActionGroup> mPopupActionGroup = Gtk::ActionGroup::create("PredicateDialog");
    
    mPopupActionGroup->add(Gtk::Action::create("PrD_Add", _("_Add")), sigc::mem_fun(*this, &PredicateDialog::onAddPredicate));
    mPopupActionGroup->add(Gtk::Action::create("PrD_Delete", _("_Delete")), sigc::mem_fun(*this, &PredicateDialog::onDeletePredicate));
    mPopupActionGroup->add(Gtk::Action::create("PrD_Rename", _("_Rename")), sigc::mem_fun(*this, &PredicateDialog::onRenamePredicate));
    
    Glib::RefPtr <Gtk::UIManager> mUIManager = Gtk::UIManager::create();
    mUIManager->insert_action_group(mPopupActionGroup);
    
    Glib::ustring ui_info =
                "<ui>"
                "  <popup name='PrD_Popup'>"
                "    <menuitem action='PrD_Add'/>"
                "    <menuitem action='PrD_Delete'/>"
                "    <menuitem action='PrD_Delete'/>"
                 "  </popup>"
                "</ui>";
    
    try {
      mUIManager->add_ui_from_string(ui_info);
      mMenuTreeViewItems = (Gtk::Menu *) (mUIManager->get_widget("/PrD_Popup"));
    } catch(const Glib::Error& ex) {
      std::cerr << "building menus failed: PrD_Popup " <<  ex.what();
    }
    
    if (!mMenuTreeViewItems)
      std::cerr << "not a menu : PrD_Popup\n";
/*
    Gtk::Menu::MenuList& menulist(mMenuTreeViewItems.items());

    menulist.push_back(
        Gtk::Menu_Helpers::MenuElem(
            ("_Add"), sigc::mem_fun(
                *this, &PredicateDialog::onAddPredicate)));
    menulist.push_back(
        Gtk::Menu_Helpers::MenuElem(
            ("_Delete"), sigc::mem_fun(
                *this, &PredicateDialog::onDeletePredicate)));
    menulist.push_back(
        Gtk::Menu_Helpers::MenuElem(
            ("_Rename"), sigc::mem_fun(
                *this, &PredicateDialog::onRenamePredicate)));

    mMenuTreeViewItems.accelerate(*mTreePredicateList);
*/
}

void PredicateDialog::initTreePredicateList()
{
    m_model = Gtk::ListStore::create(m_viewscolumnrecord);
    m_columnName = mTreePredicateList->
                            append_column_editable(("Name"),
                            m_viewscolumnrecord.name);
    m_cellrenderer = dynamic_cast<Gtk::CellRendererText*>(
    mTreePredicateList->get_column_cell_renderer(m_columnName - 1));
    m_cellrenderer->property_editable() = false;
    mTreePredicateList->set_model(m_model);
}

void PredicateDialog::fillTreePredicateList()
{
    // Ajout dans la liste du TreeView
    for ( std::vector < std::string > ::const_iterator it =
            mPredicateName.begin();
        it != mPredicateName.end() ; ++it ) {
        Gtk::TreeIter iter = m_model->append();
        if (iter) {
            Gtk::ListStore::Row row = *iter;
            row[m_viewscolumnrecord.name] = *it;
        }
    }
    m_iter = m_model->children().end();
}

bool PredicateDialog::onButtonRealeaseTreePredicateList(GdkEventButton* event)
{
    if (event->button == 3) {
        mMenuTreeViewItems->popup(event->button, event->time);
    }
    return true;
}

void PredicateDialog::onAddPredicate() {
    SimpleTypeBox box( ("New predicate name?"), "");
    std::string name = boost::trim_copy(box.run());
    if (box.valid() and checkName(name)) {
        setSensitivePredicate(false);
        Gtk::TreeIter iter = m_model->append();
        if (iter) {
            savePreviousPredicate(mPredicateNameEntry->get_text());
            mPredicateNameEntry->set_text(name);
            Gtk::ListStore::Row row = *iter;
            row[m_viewscolumnrecord.name] = name;

            mTextViewFunction->get_buffer()->set_text("");
            mHeaderPred->set_text( "bool " + name
            + "() const {" );

            m_iter = iter;
            mTreePredicateList->set_cursor(m_model->get_path(iter));
            mPredicateName.push_back(name);
            setSensitivePredicate(true);
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

void PredicateDialog::onDeletePredicate()
{
    Glib::RefPtr < Gtk::TreeView::Selection > ref = mTreePredicateList->
            get_selection();
    if (ref) {
        Gtk::TreeModel::iterator iter = ref->get_selected();
        if (iter) {
            if (Question(_("Are you sure you want to delete this predicate?"))) {
                mPredicateNameEntry->set_text("");
                mTextViewFunction->get_buffer()->set_text("");
                mHeaderPred->set_text("");
                Gtk::TreeModel::Row row = *iter;
                std::string name(row.get_value(m_viewscolumnrecord.name));
                m_model->erase(iter);

                Gtk::TreeModel::Children children = m_model->children();
                m_iter = children.begin();

                // Delete the element in the vector
                for (std::vector < std::string > ::iterator it =
                        mPredicateName.begin();
                        it != mPredicateName.end(); ) {
                    if ( *it == name ) {
                        it = mPredicateName.erase(it);
                    }
                    else {
                        ++it;
                    }
                }

                if (mPredicateFunction.find(name) != mPredicateFunction.end()) {
                    mPredicateFunction.erase(name);
                }
                setSensitivePredicate(false);
            }
        }
    }
}

void PredicateDialog::onRenamePredicate() {
    Glib::RefPtr < Gtk::TreeView::Selection > ref = mTreePredicateList->
        get_selection();
    if (ref) {
        Gtk::TreeModel::iterator iter = ref->get_selected();
        if (iter) {
            Gtk::TreeModel::Row row = *iter;
            std::string oldName(row.get_value(m_viewscolumnrecord.name));
            savePreviousPredicate(oldName);

            SimpleTypeBox box(("Predicate new name?"), "");
            std::string name = boost::trim_copy(box.run());
            if (box.valid() and checkName(name)) {

                setSensitivePredicate(false);
                m_model->erase(iter);

                Gtk::TreeModel::Children children = m_model->children();
                m_iter = children.begin();

                iter = m_model->append();
                mPredicateNameEntry->set_text(name);
                Gtk::ListStore::Row row = *iter;
                row[m_viewscolumnrecord.name] = name;

                if (mPredicateFunction.find(oldName) !=
                        mPredicateFunction.end()) {
                    mTextViewFunction->get_buffer()->
                            set_text(mPredicateFunction[oldName]);
                    mPredicateFunction[name] = mPredicateFunction[oldName];
                    mPredicateFunction.erase(oldName);
                }

                mPredicateName.push_back(name);
                // Delete the element in the vector
                for (std::vector < std::string > ::iterator it =
                        mPredicateName.begin(); it != mPredicateName.end(); ) {
                    if ( *it == oldName ) {
                        it = mPredicateName.erase(it);
                    }
                    else {
                        ++it;
                    }
                }
                mTreePredicateList->set_cursor(m_model->get_path(iter));
                setSensitivePredicate(true);
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

void PredicateDialog::onCursorChangeTreePredicateList() {
    Glib::RefPtr < Gtk::TreeView::Selection > ref = mTreePredicateList->
            get_selection();

    if (ref) {
        Gtk::TreeModel::iterator iter = ref->get_selected();
        if (iter) {
            Gtk::TreeModel::Row row = *iter;
            std::string name(row.get_value(m_viewscolumnrecord.name));
            savePreviousPredicate(mPredicateNameEntry->get_text());
            mPredicateNameEntry->set_text(name);

            if ( mPredicateFunction.find(name) != mPredicateFunction.end()) {
                mTextViewFunction->get_buffer()->
                        set_text(mPredicateFunction[ name ]);
                mHeaderPred->set_text( "bool " + name
                        + "() const {" );
            }
            else {
                mTextViewFunction->get_buffer()->set_text("");
            }

            setSensitivePredicate( true );
        }
    }
}

void PredicateDialog::setSensitivePredicate(bool state)
{
    mTextViewFunction->set_sensitive(state);
}

void PredicateDialog::savePreviousPredicate(std::string name)
{
    if ( !name.empty() ) {
        if ( mPredicateFunction.find(name) != mPredicateFunction.end() ) {
            mPredicateFunction.erase(name);
            mPredicateFunction[ name ] = mTextViewFunction->get_buffer()->
                    get_text();
        }
        else {
            mPredicateFunction[ name ] = mTextViewFunction->get_buffer()->
                    get_text();
        }
    }
}

}
}
}
}    // namespace vle gvle modeling decision
