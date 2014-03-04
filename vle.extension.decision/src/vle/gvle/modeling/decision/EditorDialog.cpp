/*
 * @file vle/gvle/modeling/decision/EditorDialog.cpp
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

#include <vle/gvle/modeling/decision/EditorDialog.hpp>

namespace vle {
namespace gvle {
namespace modeling {
namespace decision {

    EditorDialog::EditorDialog(Glib::RefPtr < Gtk::Builder >& xml,
        hierarchicalPred pPred,
        std::vector < std::string > pFactName,
        std::vector < std::string > pParam,
        strings_t lPred) :
    mPred(pPred), mPredName(lPred), mFactName(pFactName),
    mParam(pParam), mXml(xml)
{
    mXml->get_widget("EditorDialog", mDialog);
    mXml->get_widget("labelTVPred", mLabelLPred);
    mXml->get_widget("buttonAddPred", mAddPred);
    mXml->get_widget("buttonDeletePred", mDelPred);
    mXml->get_widget("buttonRenamePred", mRenamePred);
    mXml->get_widget("treeVPred", mTreeVPred);

    breakLine = false;
    initTVPred();

    fillTVPred();

    initPopupMenus();
}

EditorDialog::~EditorDialog()
{
    mTreeVPred->remove_all_columns();

    for (std::list < sigc::connection >::iterator it = mList.begin();
         it != mList.end(); ++it) {
        it->disconnect();
    }

    m_cntPredButtonRelease.disconnect();

    mDialog->hide();
}

int EditorDialog::run()
{
    mDialog->show_all();
    int state = mDialog->run();

    if (state == Gtk::RESPONSE_ACCEPT) {
        mPred.clear();
        SaveRow(m_refModelPred->children());
    }

    mDialog->hide();
    return state;
}

void EditorDialog::initPopupMenus()
{
    mList.push_back(mAddPred->signal_clicked().connect(
            sigc::mem_fun(*this, &EditorDialog::onAddPred)));
    mList.push_back(mDelPred->signal_clicked().connect(
            sigc::mem_fun(*this, &EditorDialog::onDeletePred)));
    mList.push_back(mRenamePred->signal_clicked().connect(
            sigc::mem_fun(*this, &EditorDialog::onRenamePred)));


    Glib::RefPtr <Gtk::ActionGroup> mPopupActionGroup = Gtk::ActionGroup::create("EditorDialog");
    mPopupActionGroup->add(Gtk::Action::create("ED_AddLogicalOp", _("Add Logical Op.")), sigc::mem_fun(*this, &EditorDialog::onAddSubLog));
    mPopupActionGroup->add(Gtk::Action::create("ED_AddLine", _("Add Line")), sigc::mem_fun(*this, &EditorDialog::onAddSubLine));
    mPopupActionGroup->add(Gtk::Action::create("ED_Rename", _("_Rename")), sigc::mem_fun(*this, &EditorDialog::onDeleteLog));
    
    Glib::RefPtr <Gtk::UIManager> mUIManager = Gtk::UIManager::create();
    mUIManager->insert_action_group(mPopupActionGroup);
    
    Glib::ustring ui_info =
                "<ui>"
                "  <popup name='ED_Popup'>"
                "    <menuitem action='ED_AddLogicalOp'/>"
                "    <menuitem action='ED_AddLine'/>"
                "    <menuitem action='ED_Rename'/>"
                 "  </popup>"
                "</ui>";
    
    try {
      mUIManager->add_ui_from_string(ui_info);
      mMenuPred = (Gtk::Menu *) (mUIManager->get_widget("/ED_Popup"));
    } catch(const Glib::Error& ex) {
      std::cerr << "building menus failed: ED_Popup " <<  ex.what();
    }
    
    if (!mMenuPred)
      std::cerr << "not a menu : ED_Popup\n";
/*
    Gtk::Menu::MenuList& menulist1(mMenuPred.items());
    menulist1.push_back(Gtk::Menu_Helpers::MenuElem(("Add Logical Op."),
            sigc::mem_fun(*this, &EditorDialog::onAddSubLog)));
    menulist1.push_back(Gtk::Menu_Helpers::MenuElem(("Add Line"),
            sigc::mem_fun(*this, &EditorDialog::onAddSubLine)));
    menulist1.push_back(Gtk::Menu_Helpers::MenuElem(("_Delete"),
            sigc::mem_fun(*this, &EditorDialog::onDeleteLog)));
    mMenuPred.accelerate(*mTreeVPred);
*/
    m_cntPredButtonRelease = mTreeVPred->
            signal_button_release_event().connect(
            sigc::mem_fun(*this, &EditorDialog::onButtonRealeasePred));
}

void EditorDialog::initTVPred()
{
    Gtk::TreeModel::Row row;

    // Create and fill the combo models
    m_refTreeModelComboLogs = Gtk::ListStore::create(m_modelColumnPredComboLogs);
    row = *(m_refTreeModelComboLogs->append());
    row[m_modelColumnPredComboLogs.name] = "AND";
    row = *(m_refTreeModelComboLogs->append());
    row[m_modelColumnPredComboLogs.name] = "OR";

    m_refTreeModelComboFacts = Gtk::ListStore::create(m_modelColumnPredComboFacts);
    for (std::vector < std::string > ::iterator it = mFactName.begin();
            it != mFactName.end(); ++it) {
        row = *(m_refTreeModelComboFacts->append());
        row[m_modelColumnPredComboFacts.name] = *it;
    }

    m_refTreeModelComboOps = Gtk::ListStore::create(m_modelColumnPredComboOps);
    row = *(m_refTreeModelComboOps->append());
    row[m_modelColumnPredComboOps.name] = "==";
    row = *(m_refTreeModelComboOps->append());
    row[m_modelColumnPredComboOps.name] = "!=";
    row = *(m_refTreeModelComboOps->append());
    row[m_modelColumnPredComboOps.name] = "<";
    row = *(m_refTreeModelComboOps->append());
    row[m_modelColumnPredComboOps.name] = "<=";
    row = *(m_refTreeModelComboOps->append());
    row[m_modelColumnPredComboOps.name] = ">";
    row = *(m_refTreeModelComboOps->append());
    row[m_modelColumnPredComboOps.name] = ">=";

    m_refTreeModelComboPars = Gtk::ListStore::create(m_modelColumnPredComboPars);
    for (std::vector < std::string > ::iterator it = mParam.begin();
            it != mParam.end(); ++it) {
        row = *(m_refTreeModelComboPars->append());
        row[m_modelColumnPredComboPars.name] = *it;
    }

// Create the tree model
    m_refModelPred = Gtk::TreeStore::create(m_modelColumnPred);
    mTreeVPred->set_model(m_refModelPred);

// Add the TreeView's view columns
    mTreeVPred->append_column(("Predicates"), m_modelColumnPred.pred);

    {
// Create a Log Combo CellRenderer
    Gtk::TreeView::Column* pColumn = Gtk::manage(
            new Gtk::TreeView::Column("Log. Op."));
    Gtk::CellRendererCombo* pRenderer = Gtk::manage(
            new Gtk::CellRendererCombo);
    pColumn->pack_start(*pRenderer);
    mTreeVPred->append_column(*pColumn);
//Make this View column represent the log op model column
#ifdef GLIBMM_PROPERTIES_ENABLED
    pColumn->add_attribute(pRenderer->property_text(),
            m_modelColumnPred.log);
#else
    pColumn->add_attribute(*pRenderer, "text", m_modelColumnPred.log);
#endif
// Allow the user to choose from this list to set the value in log
#ifdef GLIBMM_PROPERTIES_ENABLED
    pColumn->add_attribute(pRenderer->property_model(), m_modelColumnPred.logs);
#else
    pColumn->add_attribute(*pRenderer, "model", m_modelColumnPred.log);
#endif
#ifdef GLIBMM_PROPERTIES_ENABLED
    pRenderer->property_text_column() = 0;
#else
    pRenderer->set_property("text_column", 0);
#endif
#ifdef GLIBMM_PROPERTIES_ENABLED
    pRenderer->property_editable() = true;
#else
    pRenderer->set_property("editable", true);
#endif
    pRenderer->signal_edited().connect( sigc::mem_fun(*this,
            &EditorDialog::on_cellrenderer_choice_edited_logs));
    }

    {
// Create a Fact Combo CellRenderer
    Gtk::TreeView::Column* pColumn = Gtk::manage(
            new Gtk::TreeView::Column("Facts"));
    Gtk::CellRendererCombo* pRenderer = Gtk::manage(
            new Gtk::CellRendererCombo);
    pColumn->pack_start(*pRenderer);
    mTreeVPred->append_column(*pColumn);
//Make this View column represent the fact model column
#ifdef GLIBMM_PROPERTIES_ENABLED
    pColumn->add_attribute(pRenderer->property_text(),
            m_modelColumnPred.fact);
#else
    pColumn->add_attribute(*pRenderer, "text", m_modelColumnPred.fact);
#endif
// Allow the user to choose from this list to set the value in fact
#ifdef GLIBMM_PROPERTIES_ENABLED
    pColumn->add_attribute(pRenderer->property_model(),m_modelColumnPred.facts);
#else
    pColumn->add_attribute(*pRenderer, "model", m_modelColumnPred.fact);
#endif
#ifdef GLIBMM_PROPERTIES_ENABLED
    pRenderer->property_text_column() = 0;
#else
    pRenderer->set_property("text_column", 0);
#endif
#ifdef GLIBMM_PROPERTIES_ENABLED
    pRenderer->property_editable() = true;
#else
    pRenderer->set_property("editable", true);
#endif
    pRenderer->signal_edited().connect( sigc::mem_fun(*this,
            &EditorDialog::on_cellrenderer_choice_edited_facts));
    }

    {
// Create a Op Combo CellRenderer
    Gtk::TreeView::Column* pColumn = Gtk::manage(
            new Gtk::TreeView::Column("Comp. Op."));
    Gtk::CellRendererCombo* pRenderer = Gtk::manage(
            new Gtk::CellRendererCombo);
    pColumn->pack_start(*pRenderer);
    mTreeVPred->append_column(*pColumn);
//Make this View column represent the op model column
#ifdef GLIBMM_PROPERTIES_ENABLED
    pColumn->add_attribute(pRenderer->property_text(),
            m_modelColumnPred.op);
#else
    pColumn->add_attribute(*pRenderer, "text", m_modelColumnPred.op);
#endif
// Allow the user to choose from this list to set the value in op
#ifdef GLIBMM_PROPERTIES_ENABLED
    pColumn->add_attribute(pRenderer->property_model(), m_modelColumnPred.ops);
#else
    pColumn->add_attribute(*pRenderer, "model", m_modelColumnPred.op);
#endif
#ifdef GLIBMM_PROPERTIES_ENABLED
    pRenderer->property_text_column() = 0;
#else
    pRenderer->set_property("text_column", 0);
#endif
#ifdef GLIBMM_PROPERTIES_ENABLED
    pRenderer->property_editable() = true;
#else
    pRenderer->set_property("editable", true);
#endif
    pRenderer->signal_edited().connect( sigc::mem_fun(*this,
            &EditorDialog::on_cellrenderer_choice_edited_ops));
    }

    {
// Create a Parameter Combo CellRenderer
    Gtk::TreeView::Column* pColumn = Gtk::manage(
            new Gtk::TreeView::Column("Parameters"));
    Gtk::CellRendererCombo* pRenderer = Gtk::manage(
            new Gtk::CellRendererCombo);
    pColumn->pack_start(*pRenderer);
    mTreeVPred->append_column(*pColumn);
//Make this View column represent the par model column
#ifdef GLIBMM_PROPERTIES_ENABLED
    pColumn->add_attribute(pRenderer->property_text(),
            m_modelColumnPred.par);
#else
    pColumn->add_attribute(*pRenderer, "text", m_modelColumnPred.par);
#endif
// Allow the user to choose from this list to set the value in par
#ifdef GLIBMM_PROPERTIES_ENABLED
    pColumn->add_attribute(pRenderer->property_model(), m_modelColumnPred.pars);
#else
    pColumn->add_attribute(*pRenderer, "model", m_modelColumnPred.par);
#endif
#ifdef GLIBMM_PROPERTIES_ENABLED
    pRenderer->property_text_column() = 0;
#else
    pRenderer->set_property("text_column", 0);
#endif
#ifdef GLIBMM_PROPERTIES_ENABLED
    pRenderer->property_editable() = true;
#else
    pRenderer->set_property("editable", true);
#endif
    pRenderer->signal_edited().connect( sigc::mem_fun(*this,
            &EditorDialog::on_cellrenderer_choice_edited_pars));
    }
}

void EditorDialog::fillTVPred()
{
    Gtk::TreeModel::Row row;
// Fill the Tree model
    for (hierarchicalPred::iterator it = mPred.begin();
            it != mPred.end(); ++it) {
        if (breakLine == true) {
            row = *(m_refModelPred->append());
            row[m_modelColumnPred.rowType] = ROW_TYPE_EMPTY;
            row[m_modelColumnPred.rowNumber] = 0;
            row = *(m_refModelPred->append());
            row[m_modelColumnPred.rowType] = ROW_TYPE_EMPTY;
            row[m_modelColumnPred.rowNumber] = 0;
        }
        else {
            breakLine = true;
        }
        row = *(m_refModelPred->append());
        row[m_modelColumnPred.pred] = it->first;
        row[m_modelColumnPred.predName] = it->first;
        row[m_modelColumnPred.rowType] = ROW_TYPE_PRED;
        row[m_modelColumnPred.rowNumber] = 0;

        if (it->second.size() > 1) {
            strings_t::iterator itParPred = it->second.begin();
            itParPred++;
            InsertPredicate(row, &itParPred, &it->second, 1);
        }
    }
}

void EditorDialog::onAddPred() {
    SimpleTypeBox box(("New predicate name?"), "");
    std::string name = boost::trim_copy(box.run());
    if (box.valid()) {
        if (checkName(name)) {
            Gtk::TreeIter iter;
            if (breakLine == true) {
                Gtk::TreeModel::Row row;
                iter = m_refModelPred->append();
                row = *iter;
                row[m_modelColumnPred.rowType] = ROW_TYPE_EMPTY;
                iter = m_refModelPred->append();
                row  = *iter;
                row[m_modelColumnPred.rowType] = ROW_TYPE_EMPTY;
            }
            else {
                breakLine = true;
            }
            iter = m_refModelPred->append();
            if (iter) {
                Gtk::ListStore::Row row = *iter;
                row[m_modelColumnPred.pred] = name;
                row[m_modelColumnPred.predName] = name;
                row[m_modelColumnPred.rowType] = ROW_TYPE_PRED;
                row[m_modelColumnPred.rowNumber] = 0;

                mTreeVPred->set_cursor(m_refModelPred->get_path(iter));
                mPredName.push_back(name);
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

void EditorDialog::onDeletePred()
{
    using namespace Gtk;

    Glib::RefPtr < TreeSelection > tree_selection = mTreeVPred->get_selection();
    //Gtk::TreeSelection::ListHandle_Path lst=tree_selection->get_selected_rows();
    std::vector<TreePath> v1 = tree_selection->get_selected_rows();
    std::list<TreePath> lst (v1.begin(), v1.end());
    
    std::list<TreePath>::iterator it = lst.begin();
    if (it != lst.end()) {
        if (Question(_("Are you sure you want to delete this predicate?"))) {
            Gtk::TreeIter iter = m_refModelPred->get_iter(*it) ;
            Gtk::TreeModel::Row row(*(iter));
            int rowType = row.get_value(m_modelColumnPred.rowType);
            std::string predName = row.get_value(m_modelColumnPred.predName);

            if (rowType == ROW_TYPE_PRED) {
                std::string name = row.get_value(m_modelColumnPred.pred);
                m_refModelPred->erase(iter);
                std::vector < std::string > ::iterator it;
                it = find(mPredName.begin(), mPredName.end(), name);
                if (it != mPredName.end()) {
                    mPredName.erase(it);
                }
            }
            else {
                bool terminated = false;
                typedef Gtk::TreeModel::Children type_children;
                type_children children = m_refModelPred->children();
                for(type_children::iterator iter = children.begin();
                        iter != children.end() && !terminated; ) {
                    Gtk::TreeModel::Row row = *iter;
                    rowType = row.get_value(m_modelColumnPred.rowType);
                    std::string pred = row.get_value(m_modelColumnPred.pred);
                    if (rowType == ROW_TYPE_PRED && pred == predName) {
                        iter = m_refModelPred->erase(iter);
                    }
                    else {
                        terminated = true;
                        ++iter;
                    }
                }
            }
            bool terminated = false;
            typedef Gtk::TreeModel::Children type_children;
            type_children children = m_refModelPred->children();
            for(type_children::iterator iter = children.begin();
                    iter != children.end() && !terminated; ) {
                Gtk::TreeModel::Row row = *iter;
                int rowType = row.get_value(m_modelColumnPred.rowType);
                if (rowType == ROW_TYPE_EMPTY) {
                    iter = m_refModelPred->erase(iter);
                }
                else {
                    terminated = true;
                    ++iter;
                }
            }
            terminated = false;
            children = m_refModelPred->children();
            for(type_children::reverse_iterator iter = children.rbegin();
                    iter != children.rend() && !terminated; ) {
                Gtk::TreeModel::Row row = *iter;
                int rowType = row.get_value(m_modelColumnPred.rowType);
                if (rowType == ROW_TYPE_EMPTY) {
                    iter.base() = m_refModelPred->erase(--iter.base());
                }
                else {
                    terminated = true;
                    ++iter;
                }
            }
            if (children.empty()) {
                breakLine = false;
            }
        }
    }
}

bool EditorDialog::SaveRow(Gtk::TreeModel::Children children)
{
    if (children.empty()) {
        return false;
    }

    Gtk::TreeModel::Children::iterator iter;
    iter = children.begin();

    while (iter != children.end()) {
        std::string toPush = (*iter)[m_modelColumnPred.pred] + "|" +
        (*iter)[m_modelColumnPred.log] + "|" +
        (*iter)[m_modelColumnPred.fact] + "|" +
        (*iter)[m_modelColumnPred.op] + "|" +
        (*iter)[m_modelColumnPred.par] + "|";

        std::ostringstream oss;
        oss << (*iter)[m_modelColumnPred.rowType];
        toPush += oss.str() + "|";
        oss.clear();
        toPush += (*iter)[m_modelColumnPred.predName] + "|";
        oss.str("");
        oss << (*iter)[m_modelColumnPred.rowNumber];
        toPush += oss.str();
        std::string predName = (*iter)[m_modelColumnPred.predName] + "";
        if ((*iter)[m_modelColumnPred.rowType] != ROW_TYPE_EMPTY) {
            mPred[predName].push_back(toPush);
        }

        if (SaveRow((*iter).children())) {
            return true;
        }
        iter++;
    }

    return false;
}

hierarchicalPred EditorDialog::getPred() {
    return mPred;
}

strings_t::iterator* EditorDialog::InsertPredicate(Gtk::TreeModel::Row pRow,
        strings_t::iterator *it,
        strings_t *pPred, int pRowNumber) {

    if (*it == pPred->end()) {
        return it;
    }

    Gtk::TreeModel::Row thisRow = pRow;

   do {
        strings_t parameters;
        boost::split(parameters, *(*it), boost::is_any_of("|"));

        Glib::ustring pred = parameters.at(0);
        Glib::ustring log = parameters.at(1);
        Glib::ustring fact = parameters.at(2);
        Glib::ustring op = parameters.at(3);
        Glib::ustring par = parameters.at(4);

        std::istringstream iss(parameters.at(5));
        int rowType;
        iss >> rowType;
        Glib::ustring predName = parameters.at(6);

        std::istringstream iss2(parameters.at(7));
        int rowNumber;
        iss2 >> rowNumber;

        if (pRowNumber == rowNumber) {
            pRow = *(m_refModelPred->append(thisRow.children()));
            pRow[m_modelColumnPred.pred] = pred;
            pRow[m_modelColumnPred.rowType] = rowType;
            pRow[m_modelColumnPred.predName] = predName;
            pRow[m_modelColumnPred.rowNumber] = rowNumber;

            if (rowType == ROW_TYPE_LOG) {
                pRow[m_modelColumnPred.logs] = m_refTreeModelComboLogs;
            }
            else if (rowType == ROW_TYPE_LINE) {
                pRow[m_modelColumnPred.facts] = m_refTreeModelComboFacts;
                pRow[m_modelColumnPred.ops] = m_refTreeModelComboOps;
                pRow[m_modelColumnPred.pars] = m_refTreeModelComboPars;
            }
            pRow[m_modelColumnPred.log] = log;
            pRow[m_modelColumnPred.fact] = fact;
            pRow[m_modelColumnPred.op] = op;
            pRow[m_modelColumnPred.par] = par;

            if (*it != pPred->end()) {
                ++(*it);
            }
        }
        else if (pRowNumber < rowNumber) {
            it = InsertPredicate(pRow, it,
                    pPred, rowNumber);
        }
        else if (pRowNumber > rowNumber) {
            return it;
        }

    } while (*it != pPred->end());

    return it;
}

void EditorDialog::onRenamePred() {
    mRenamePred->set_sensitive(false);
}

}
}
}
}    // namespace vle gvle modeling decision
