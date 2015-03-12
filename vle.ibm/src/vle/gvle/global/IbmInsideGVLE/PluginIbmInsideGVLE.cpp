/*
 * @file vle/gvle/global/IbmInsideGVLE/PluginIbmInsideGVLE.cpp
 *
 * This file is part of VLE, a framework for multi-modeling, simulation
 * and analysis of complex dynamical systems
 * http://www.vle-project.org
 *
 * Copyright (c) 2013-2015 INRA http://www.inra.fr
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
#include <vle/gvle/GVLE.hpp>
#include <vle/gvle/GVLEMenuAndToolbar.hpp>
#include <vle/gvle/FileTreeView.hpp>
#include <vle/gvle/Message.hpp>
#include <vle/gvle/Editor.hpp>
#include <vle/gvle/Modeling.hpp>
#include <vle/utils/Package.hpp>
#include <gtkmm/actiongroup.h>
#include <gtkmm/uimanager.h>
#include <gtkmm/button.h>
#include <gtkmm/action.h>
#include <gtkmm/textbuffer.h>
#include <vle/utils/Template.hpp>
#include <vle/vpz/Classes.hpp>
#include <vle/vpz/Class.hpp>
#include <vle/vpz/BaseModel.hpp>
#include <vle/vpz/AtomicModel.hpp>
#include <fstream>
#include <iostream>
#include <boost/lexical_cast.hpp>
#include <boost/algorithm/string/trim.hpp>
#include <gtkmm/radioaction.h>
#include <gtkmm/stock.h>
#include <gtkmm/tooltip.h>
#include <gtkmm/notebook.h>

#include <vle/vpz/Condition.hpp>

namespace vle {
namespace gvle {
namespace global {
namespace ibminsidegvle {

typedef std::vector < std::string > strings_t;

const Glib::ustring UI_DEFINITION =
    "<ui>"
    "    <popup name='FileTV_Popup'>"
    "        <menuitem name='FileTV_ContextLaunchIbm' action='FileTV_ContextLaunchIbm'/>"
    "    </popup>"
    "</ui>";

const std::string SCRIPT_DEFAULT =
    "-- Write your code for the event here\n";

/**
 * @class PluginIbminsideGVLE
 * @brief testing the global plugin
 *
 */

class PluginIbmInsideGVLE : public GlobalPlugin
{
public:

    /**
     * Class constructor.
     *
     * @param[in] package
     * @param[in] library
     * @param[in/out] gvle : an access to everything
     */
    PluginIbmInsideGVLE(const std::string& package,
                        const std::string& library,
                        vle::gvle::GVLE* gvle):
        GlobalPlugin(package, library, gvle), isMakingClass(false)
    {
    }

    /**
     * Class destructor.
     */
    virtual ~PluginIbmInsideGVLE()
    {
        mConnection.disconnect();

        if (mClasses) {
            mClasses->remove_all_columns();
        }

        for (std::list < sigc::connection >::iterator it = mList.begin();
             it != mList.end(); ++it) {
            it->disconnect();
        }
    }

    /**
     * @brief Run
     */
    void run()
    {

      Gtk::Menu& popupMenu = mGVLE->getFileTreeView()->getMenuPopup();

      Gtk::Menu::MenuList& menulist = popupMenu.items();

      menulist.push_back(
			 Gtk::Menu_Helpers::MenuElem(
						     _("_Open Ibm"),
						     sigc::mem_fun(
								   *this,
								   &PluginIbmInsideGVLE::onLaunchIbm)));

    }

    /**
     * @brief Open the plugin window, the vpz selected and show the class list
     */
    void onLaunchIbm()
    {
        Glib::RefPtr< Gtk::Builder > mXml = Gtk::Builder::create();

        vle::utils::Package pack("vle.ibm");
        std::string glade = pack.getPluginGvleModelingFile("ibmGVLE.glade", vle::utils::PKG_BINARY);
        mXml->add_from_file(glade);

        mXml->get_widget("window1", mWindow);
        mWindow->show();

        if (openVpz())
        {
            mXml->get_widget("treeviewClassName", mClasses);
            mClassesListStore = Gtk::ListStore::create(mClassesColumns);
            mClasses->set_model(mClassesListStore);
            initClassesColumnName();
            mClassesCopy = mGVLE->getModeling()->vpz().project().classes();

            fillClasses();
            initMenuPopupClasses();

            mXml->get_widget("buttonApply", mButtonApply);
            mButtonApply->signal_clicked().connect(sigc::mem_fun(*this,
                                                     &PluginIbmInsideGVLE::onApply));
            mXml->get_widget("buttonCancel", mButtonApply);
            mButtonApply->signal_clicked().connect(sigc::mem_fun(*this,
                                                     &PluginIbmInsideGVLE::onCancel));
            mXml->get_widget("textviewScript", mTextViewScript);

	    //to replace
            //mXml->get_widget("listbox1", mListBox);
            mXml->get_widget("scriptNoteBook", mScriptEditor);
            mXml->get_widget("buttonMore", mButtonMore);
            mButtonMore->signal_clicked().connect(sigc::mem_fun(*this,
                                                     &PluginIbmInsideGVLE::onMoreScript));
            mXml->get_widget("buttonRemove", mButtonRemove);
            mButtonRemove->signal_clicked().connect(sigc::mem_fun(*this,
                                                     &PluginIbmInsideGVLE::onRemoveScript));
            mXml->get_widget("dialog1", mDialog);
            mXml->get_widget("buttonDialogApply", mButtonDialogApply);
            mButtonDialogApply->signal_clicked().connect(sigc::mem_fun(*this,
                                                     &PluginIbmInsideGVLE::onApplyNewName));
            mXml->get_widget("buttonDialogCancel", mButtonDialogCancel);
            mButtonDialogCancel->signal_clicked().connect(sigc::mem_fun(*this,
                                                     &PluginIbmInsideGVLE::onCancelNewName));
            mXml->get_widget("entryName", mEntryName);

            if (existConditions()) {
                fillTextViewScript();
                fillScripts();
            }
        }
    }

    /**
     * @brief Action when click on Valider
     */
    void onApply()
    {
        mGVLE->getModeling()->vpz().project().classes() = mClassesCopy;
        createControleurAndDyn();
        saveScripts();
        removePort();

        std::set<std::string>::const_iterator
            sit (mDeletedClasses.begin()),
            send(mDeletedClasses.end());
        for (;sit!=send;++sit) {
            if (mGVLE->getModeling()->vpz().project().dynamics().exist(*sit))
                mGVLE->getModeling()->vpz().project().dynamics().del(*sit);
std::string fileToRemove = mGVLE->currentPackage().getSrcFile(*sit + ".cpp", vle::utils::PKG_SOURCE);
            std::remove(fileToRemove.c_str());
        }

        clean();
        mGVLE->getModeling()->setModified(true);

        mWindow->hide();
        mGVLE->redrawModelClassBox();
        mGVLE->onSave();
    }

    void clean() {
        mDeletedClasses.clear();
        mClassesCopy.clear();

        mScriptsCopy.clear();
        mPortToRemove.clear();
    }

    /**
     * @brief On click on Cancel Button
     */
    void onCancel()
    {
        clean();
        mWindow->hide();
    }

    void onMoreScript() {
        mDialog->show_all();
        mDialog->run();
    }

    void onApplyNewName() {
        std::string eventName = mEntryName->get_text();
        if (isValidName(eventName) && mScriptsCopy.find(eventName) == mScriptsCopy.end() && std::find(mPortToRemove.begin(), mPortToRemove.end(), eventName) == mPortToRemove.end()) {
            mDialog->hide();
            createListViewRow(eventName, SCRIPT_DEFAULT);
        }
    }

    void onCancelNewName() {
        mDialog->hide();
    }

    void createListViewRow(std::string title, std::string content) {
        Glib::RefPtr<Gtk::TextBuffer> buffer = Gtk::TextBuffer::create();
        buffer->set_text(content);
        Gtk::TextView* textView =  new Gtk::TextView(buffer);
        Gtk::ScrolledWindow* scrolledWindow =  new Gtk::ScrolledWindow();
        scrolledWindow->add(*textView);

        mScriptEditor->append_page(*scrolledWindow, title);
        mScriptEditor->show_all_children();

        mScriptsCopy.insert(std::pair<std::string, Gtk::TextView*> (title, textView));
    }

    void fillScripts() {
        vpz::Condition& cond = mGVLE->getModeling()->experiment().conditions().get("cond_" + NAME_CONTROLER);
        for (std::map < std::string, value::Set* >::iterator it = cond.begin(); it!=cond.end(); it++) {
            if (it->first != "Script" && cond.firstValue(it->first).isXml()) {
                createListViewRow(it->first, cond.firstValue(it->first).toXml().value());
            }
        }
    }

    void onRemoveScript() {
        int i = mScriptEditor->get_current_page();

        if (i >= 0) {
            std::string port = mScriptEditor->get_tab_label_text(
                *(mScriptEditor->get_nth_page(i)));

            mScriptEditor->remove_page(i);
            //std::string port = toRemove->get_name();
            mPortToRemove.push_back(port);
            mScriptsCopy.erase(port);
        }
    }

    bool existPort(std::string port) {
        vpz::Condition& cond = mGVLE->getModeling()->experiment().conditions().get("cond_" + NAME_CONTROLER);
        std::map < std::string, value::Set* > it = cond.conditionvalues();
        if (it.find(port) == it.end()) {
            return false;
        }
        return true;
    }

    void saveScripts() {
        for(std::map<std::string, Gtk::TextView*>::iterator it = mScriptsCopy.begin(); it!= mScriptsCopy.end(); it++) {
            Glib::RefPtr<Gtk::TextBuffer> buffer = it->second->get_buffer();
            saveScriptsOnCondition(it->first, buffer->get_text());
        }
        saveScriptOnCondition();
    }

    void removePort() {
        vpz::Condition& cond = mGVLE->getModeling()->experiment().conditions().get("cond_" + NAME_CONTROLER);
        for (unsigned int i=0; i<mPortToRemove.size(); i++) {
            cond.del(mPortToRemove[i]);
        }
    }

    /**
     * @brief Open the Vpz selected on the TreeView
     *
     * @return bool if the Vpz is opened
     */
    bool openVpz()
    {
        FileTreeView* f = mGVLE->getFileTreeView();
        std::string vpz_file = f->getSelected();

        if (utils::Path::extension(vpz_file) == ".vpz")
        {
            mGVLE->parseXML(vpz_file);
            mGVLE->getEditor()->openTabVpz(mGVLE->getModeling()->getFileName(),
                                        mGVLE->getModeling()->getTopModel());
            mGVLE->redrawModelTreeBox();
            mGVLE->redrawModelClassBox();
            mGVLE->getMenu()->onOpenVpz();
            mGVLE->getModelTreeBox()->set_sensitive(true);
            mGVLE->getModelClassBox()->set_sensitive(true);
            return true;
        }
        return false;
    }

    /**
     * @brief Create Controler and associate dynamic if don't exist
     */
    void createControleurAndDyn() {
        if (!existControleur()) {
            createControleur();
        }
        if (!existDynControleur()) {
            createDynControleur();
        }
        if (!existConditions()) {
            createConditions();
        }
    }

    /**
     * @brief Test if the controler exist
     *
     * @return bool true if the controler already exist, false if not
     */
    bool existControleur() {
        return mGVLE->getModeling()->getTopModel()->exist(NAME_CONTROLER);
    }

    /**
     * @brief Create the controler called NAME_CONTROLER
     */
    void createControleur() {
        vpz::AtomicModel* atomicModel = new vpz::AtomicModel(NAME_CONTROLER, mGVLE->getModeling()->getTopModel());
        atomicModel->setPosition(50, 50);
        atomicModel->setSize(40, 20);

        if (existDynControleur()) {
            atomicModel->setDynamics("dyn" + NAME_CONTROLER);
        }
        mGVLE->redrawModelTreeBox();
    }

    /**
     * @brief Test if the controler's dynamic exist
     *
     * @return bool true if the dynamic exist, false if not
     */
    bool existDynControleur() {
        return mGVLE->getModeling()->vpz().project().dynamics().exist("dyn" + NAME_CONTROLER);
    }

    /**
     * @brief Create a dynamic for the controler
     */
    void createDynControleur() {
        vpz::Dynamic dyn("dyn" + NAME_CONTROLER);
        dyn.setLibrary(NAME_CONTROLER);
        dyn.setPackage("vle.ibm");
        mGVLE->getModeling()->vpz().project().dynamics().add(dyn);
        vpz::AtomicModel* atom = mGVLE->getModeling()->getTopModel()->getModel(NAME_CONTROLER)->toAtomic();
        atom->setDynamics(dyn.name());
    }

    /**
     * @brief Test if the condition of the controler exist
     *
     * @return bool true if the controler condition exist, false if not
     */
    bool existConditions() {
        return mGVLE->getModeling()->experiment().conditions().exist("cond_" + NAME_CONTROLER);
    }

    /**
     * @brief Create the controler conditions
     */
    void createConditions() {
        vpz::Condition* conditionControleur = new vpz::Condition("cond_" + NAME_CONTROLER);
        conditionControleur->add("Script");
        mGVLE->getModeling()->experiment().conditions().add(*conditionControleur);
        mGVLE->getModeling()->getTopModel()->getModel(NAME_CONTROLER)->toAtomic()->addCondition("cond_" + NAME_CONTROLER);
    }

    /**
     * @brief Fill the TextView with the script existing in the controler condition
     */
    void fillTextViewScript() {
        vpz::Condition& cond = mGVLE->getModeling()->experiment().conditions().get("cond_" + NAME_CONTROLER);
        Glib::RefPtr<Gtk::TextBuffer> buffer = mTextViewScript->get_buffer();
        buffer->set_text(cond.firstValue("Script").toXml().value());
    }

    /**
     * @brief Save the script of the textView in the controler condition
     */
    void saveScriptOnCondition() {
        Glib::RefPtr<Gtk::TextBuffer> buffer = mTextViewScript->get_buffer();
        vpz::Condition& c = mGVLE->getModeling()->experiment().conditions().get("cond_" + NAME_CONTROLER);
        c.setValueToPort("Script", *(new vle::value::Xml(buffer->get_text())));
    }

    void saveScriptsOnCondition(std::string param, std::string content) {
        vpz::Condition& c = mGVLE->getModeling()->experiment().conditions().get("cond_" + NAME_CONTROLER);
        if (existPort(param)) {
            c.setValueToPort(param, *(new vle::value::Xml(content)));
        } else
            c.addValueToPort(param, *(new vle::value::Xml(content)));
    }

    std::string showParameter(std::string className) {
        std::string s = "";
        vle::vpz::Conditions& cl = mGVLE->getModeling()->experiment().conditions();
	    vle::vpz::Condition& c = cl.get("cond_DTE_" + className);
	    std::map< std::string, value::Set* > listParameters = c.conditionvalues();
	    for (std::map< std::string, value::Set* >::iterator it = listParameters.begin(); it!=listParameters.end(); ++it) {
	        if (it->first != "variables") {
	            s = s + it->first + "\n";
	        }
	    }
	    return s.substr(0, s.length()-1);
    }

    /**
     * @brief Put the column name of the Class TreeViewList
     */
    void initClassesColumnName()
    {
        mColumnName = mClasses->append_column_editable(_("Name"),
                                                        mClassesColumns.mName);
        Gtk::TreeViewColumn* nameCol = mClasses->get_column(mColumnName - 1);
        nameCol->set_clickable(true);
        nameCol->set_resizable(true);

        mCellRenderer = dynamic_cast<Gtk::CellRendererText*>(
            mClasses->get_column_cell_renderer(mColumnName - 1));

        mCellRenderer->property_editable() = true;
        mList.push_back(mCellRenderer->signal_edited().connect(
                            sigc::mem_fun(*this, &PluginIbmInsideGVLE::onEdition)));

        mClasses->set_has_tooltip();
        mClasses->signal_query_tooltip().connect(
            sigc::mem_fun(*this, &PluginIbmInsideGVLE::onQueryTooltip));
    }

    bool onQueryTooltip(int wx,int wy, bool keyboard_tooltip,
        const Glib::RefPtr<Gtk::Tooltip>& tooltip)
    {
        Gtk::TreeModel::iterator iter;
        Glib::ustring card;

        if (mClasses->get_tooltip_context_iter(wx, wy, keyboard_tooltip, iter)) {
            Glib::ustring cond = (*iter).get_value(mClassesColumns.mInfo);
            card = mGVLE->getModeling()->getInfoCard(cond);
            tooltip->set_markup(card);
            mClasses->set_tooltip_row(tooltip, Gtk::TreePath(iter));
            return true;
        } else {
            return false;
        }
    }

    void onEditionStarted(Gtk::CellEditable* /*celleditable*/,
    const Glib::ustring& path)
    {
        Gtk::TreeModel::Path selectedpath(path);
        Gtk::TreeModel::iterator it = mClassesListStore->get_iter(selectedpath);

        Gtk::TreeModel::Row row = *it;
        if (row) {
            mOldName = row.get_value(mClassesColumns.mName);
        }
    }

    void onEdition(const Glib::ustring& /*pathstring*/,
    const Glib::ustring& newstring)
    {
        std::string newName = newstring.raw();
        boost::trim(newName);
        std::string oldName = mOldName.raw();

        if (isMakingClass) {
            onAddClasses(newName);
            isMakingClass = false;
        }
    }

    /**
     * @brief Fill the treeview's rows with the list of classes
     */
    void fillClasses()
    {
        Gtk::RadioAction::Group toolsGroup;

        std::map<std::string, vpz::Class> listClass = mClassesCopy.list();
        for (std::map<std::string, vpz::Class>::iterator it=listClass.begin() ; it!=listClass.end() ; ++it)
        {
            Gtk::TreeIter iter = mClassesListStore->append();
            if (iter)
            {
                Gtk::ListStore::Row row = *iter;
                row[mClassesColumns.mName] = it->first.c_str();
                row[mClassesColumns.mInfo] = showParameter(it->first.c_str());
            }
        }
        mIter = mClassesListStore->children().end();
    }

    /**
     * @brief Update the class list
     */
    void refreshClassesList()
    {
        mClassesListStore->clear();
        fillClasses();
    }

    /**
     * @brief Initialize right click menu
     */
    void initMenuPopupClasses()
    {
        mActionGroup = Gtk::ActionGroup::create("initMenuPopupClasses");
        mActionGroup->add(Gtk::Action::create("ClasBox_ContextMenu", _("Context Menu")));

        mActionGroup->add(Gtk::Action::create("ClasBox_ContextNew", _("_New")),
        sigc::mem_fun(*this, &PluginIbmInsideGVLE::onNewClasses));
        mActionGroup->add(Gtk::Action::create("ClasBox_ContextModify", _("_Modify")),
        sigc::mem_fun(*this, &PluginIbmInsideGVLE::onModifyClasses));
        mActionGroup->add(Gtk::Action::create("ClasBox_ContextRemove", _("_Remove")),
        sigc::mem_fun(*this, &PluginIbmInsideGVLE::onRemoveClass));


        mUIManager = Gtk::UIManager::create();
        mUIManager->insert_action_group(mActionGroup);

        Glib::ustring ui_info =
        "<ui>"
        "  <popup name='ClasBox_Popup'>"
        "      <menuitem action='ClasBox_ContextNew'/>"
        "      <menuitem action='ClasBox_ContextModify'/>"
        "      <menuitem action='ClasBox_ContextRemove'/>"
        "  </popup>"
        "</ui>";

        try {
            mUIManager->add_ui_from_string(ui_info);
            mMenu = (Gtk::Menu *) (
            mUIManager->get_widget("/ClasBox_Popup"));
        } catch(const Glib::Error& ex) {
            std::cerr << "building menus failed: ClasBox_Popup " <<  ex.what();
        }

        if (!mMenu)
            std::cerr << "menu not found : ClasBox_Popup \n";

        mList.push_back(mClasses->signal_button_release_event().connect(
                        sigc::mem_fun(*this,
                                      &PluginIbmInsideGVLE::onButtonRealeaseClasses)));
    }

    /**
     * @brief Executed by New to add a Class
     */
    void onNewClasses() {
        std::string modelName;

        Gtk::TreeIter iter = mClassesListStore->append();
        if (iter) {
            std::string modelName = generateClassName("newClasses");

            Gtk::ListStore::Row row = *iter;
            row[mClassesColumns.mName] = modelName;
            Gtk::TreeViewColumn* nameCol = mClasses->get_column(mColumnName - 1);
            isMakingClass = true;
            mClasses->set_cursor(mClassesListStore->get_path(iter),*nameCol,true);
        }
    }

    std::string generateClassName(std::string modelName) {
        std::string modelNameOrigin = modelName;
        int copyNumber = 1;
        std::string suffixe;
        while (mClassesCopy.exist(modelName) || mDeletedClasses.find(modelName) != mDeletedClasses.end() || !isValidName(modelName) || modelName.length() == 0) {
            suffixe = "_" + boost::lexical_cast < std::string >(copyNumber);
            modelName = modelNameOrigin + suffixe;

            copyNumber++;
        };
        return modelName;
    }

    /**
     * @brief Add a Class on the class list
     *
     * @param std::string name of the class
     */
    void onAddClasses(std::string mName)
    {
        mName = generateClassName(mName);
        const std::string pluginname = "vle.forrester/Forrester";
        ModelingPluginPtr plugin = mGVLE->pluginFactory().getModelingPlugin(pluginname, mGVLE->currentPackage().name());
        vpz::Conditions& cond = mGVLE->getModeling()->conditions();
        vpz::Dynamic dyn(mName);
        vpz::Observables& obs = mGVLE->getModeling()->observables();
        mAtomicModel = new vpz::AtomicModel(mName, NULL);

        const std::string namespace_ = mGVLE->currentPackage().name();
        if (plugin->create(*mAtomicModel, dyn, cond, obs, mName, namespace_))
        {
            const std::string& buffer = plugin->source();
            std::string filename = mGVLE->getPackageSrcFile(mName + ".cpp");

            vpz::Class& new_class = mClassesCopy.add(mName);
            new_class.setModel(mAtomicModel);
            dyn.setLibrary(new_class.name());
            dyn.setPackage(mGVLE->currentPackage().name());
            mGVLE->getModeling()->vpz().project().dynamics().add(dyn);
            mAtomicModel->setDynamics(dyn.name());
            try {
                std::ofstream f(filename.c_str());
                f.exceptions(std::ofstream::failbit | std::ofstream::badbit);
                f << buffer;
            } catch(const std::ios_base::failure& e) {
                throw utils::ArgError(fmt(
                        _("Cannot store buffer in file '%1%'")) % filename);
            }
            refreshClassesList();
            mName = "";
        } else {
            onRemoveClass();
        }
    }

    std::string getClassSelected() {
        std::string name = "";
        Glib::RefPtr < Gtk::TreeView::Selection > ref = mClasses->get_selection();
        if (ref) {
            Gtk::TreeModel::iterator iter = ref->get_selected();
            if (iter) {
                Gtk::TreeModel::Row row = *iter;
                name = row.get_value(mClassesColumns.mName);
            }
        }

        return name;
    }

    /**
     * @brief Modify the selected class
     */
    void onModifyClasses()
    {
        std::string name = getClassSelected();
        if (name != "") {

            vpz::Dynamic dyn = mGVLE->getModeling()->vpz().project().dynamics().get(name);
            vpz::Conditions& cond = mGVLE->getModeling()->experiment().conditions();
            vpz::Observables& obs = mGVLE->getModeling()->observables();
            vpz::Class& my_class = mClassesCopy.get(name);
            mAtomicModel = my_class.model()->toAtomic();

            std::string filename = mGVLE->getPackageSrcFile(name + ".cpp");

            std::string newTab = filename;
            try {
                std::string pluginname, packagename, conf;
                utils::Template tpl;
                tpl.open(newTab);
                tpl.tag(pluginname, packagename, conf);
                ModelingPluginPtr plugin =
                    mGVLE->pluginFactory().getModelingPlugin(packagename,
                                      pluginname,
                                      mGVLE->currentPackage().name());
                if (plugin->modify(*mAtomicModel, dyn, cond,
                                   obs, conf, tpl.buffer())) {
                    const std::string& buffer = plugin->source();
                    std::string filename = mGVLE->getPackageSrcFile(dyn.library() +
                        ".cpp");
                    try {
                        std::ofstream f(filename.c_str());
                        f.exceptions(std::ofstream::failbit |
                                     std::ofstream::badbit);
                        f << buffer;
                    } catch(const std::ios_base::failure& e) {
                        throw utils::ArgError(fmt(
                                _("Cannot store buffer in file '%1%'")) %
                            filename);
                    }
                }
            } catch(...) {
                std::cout << "error open Forrester" << std::endl;
            }
            refreshClassesList();
        }
    }

    /**
     * @brief Remove the selected class
     */
    void onRemoveClass()
    {
        Glib::RefPtr < Gtk::TreeView::Selection > ref = mClasses->get_selection();
        if (ref) {
            Gtk::TreeModel::iterator iter = ref->get_selected();
            if (iter) {
                Gtk::TreeModel::Row row = *iter;
                std::string name(row.get_value(mClassesColumns.mName));

                mClassesCopy.del(name);
                mClassesListStore->erase(iter);
                mDeletedClasses.insert(name);
            }
        }
    }

    /**
     * @brief Right click menu
     *
     * @param GdkEventButton*
     *
     * @return bool
     */
    bool onButtonRealeaseClasses(GdkEventButton* event)
    {
        if (event->button == 3) {
            mMenu->popup(event->button, event->time);
        }
        return true;
    }

    /**
     * @brief Structure of the treeview's column
     */
    struct ClassesModelColumns : public Gtk::TreeModel::ColumnRecord
    {
        ClassesModelColumns()
        { add(mName);
          add(mInfo);}

        Gtk::TreeModelColumn<Glib::ustring> mName;
        Gtk::TreeModelColumn<Glib::ustring> mInfo;

    } mClassesColumns;

    /**
     * @brief Check if the name is valid
     *
     * @param std::string name
     *
     * @return bool true if the name is valid, false if not
    */
    bool isValidName(std::string name)
    {
        size_t i = 0;
        while (i < name.length()) {
            if (!isalnum(name[i])) {
                if (name[i] != '_') {
                    return false;
                }
            }
            i++;
        }
        if (i == 0)
            return false;
        return true;
    }

    Glib::RefPtr< Gtk::Action >                 mAction;
    sigc::connection                            mConnection;

    Glib::RefPtr<Gtk::UIManager>                m_refUIManager;
    Glib::RefPtr<Gtk::ActionGroup>              m_refActionGroup;

    Gtk::UIManager::ui_merge_id                 mMergeId;

    Glib::RefPtr<Gtk::Builder>                  mXml;
    Gtk::Window*                                mWindow;
    Gtk::TreeView*                              mClasses;
    Gtk::TextView*                              mTextViewScript;
    Gtk::Menu*                                  mMenu;
    Gtk::Button*                                mButtonApply;
    Gtk::CellRendererText*                      mCellRenderer;

    Glib::RefPtr<Gtk::ListStore>                mClassesListStore;
    int                                         mColumnName;
    int                                         mColumnInfo;
    std::list < sigc::connection >              mList;
    Gtk::TreeModel::Children::iterator          mIter;

    Glib::RefPtr <Gtk::UIManager>               mUIManager;
    Glib::RefPtr <Gtk::ActionGroup>             mActionGroup;
    vpz::Classes                                mClassesCopy;
    vpz::AtomicModel*                           mAtomicModel;
    std::set <std::string>                      mDeletedClasses;
    Glib::ustring                               mOldName;

    bool                                        isMakingClass;

    Gtk::Button*     mButtonMore;

    Gtk::Notebook*                              mScriptEditor;
    std::map<std::string, Gtk::TextView*>       mScriptsCopy;
    std::vector<std::string>                    mPortToRemove;
    Gtk::Button*                                mButtonRemove;
    Gtk::Dialog*                                mDialog;
    Gtk::Button*                                mButtonDialogApply;
    Gtk::Button*                                mButtonDialogCancel;
    Gtk::Entry*                                 mEntryName;

    static const std::string                    NAME_CONTROLER;
};

const std::string PluginIbmInsideGVLE::NAME_CONTROLER = "Controleur";
}
}
}
}    // namespace vle gvle global rinsidegvle

DECLARE_GVLE_GLOBALPLUGIN(vle::gvle::global::ibminsidegvle::PluginIbmInsideGVLE)
