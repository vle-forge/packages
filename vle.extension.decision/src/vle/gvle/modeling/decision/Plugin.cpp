/*
 * @file vle/gvle/modeling/decision/Plugin.cpp
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

#include <vle/gvle/modeling/decision/Plugin.hpp>
#include <vle/utils/Package.hpp>

namespace vle {
namespace gvle {
namespace modeling {
namespace decision {

// Template : source file
const std::string PluginDecision::TEMPLATE_DEFINITION =
    "/**\n"                                                             \
    " * @file {{classname}}.cpp\n"                                      \
    " * @author ...\n"                                                  \
    " * ...\n"                                                          \
    "  *@@tagdynamic@@\n"                                               \
    "  *@@tagdepends:vle.extension.decision@@endtagdepends\n"           \
    " * @@tag Decision@vle.extension.decision @@"                       \
    "namespace:{{namespace}};"                                          \
    "class:{{classname}};f:"                                            \
    "{{for i in facts}}"                                                \
    "{{facts^i}}|"                                                      \
    "{{end for}};p:"                                                    \
    "{{for i in predicates}}"                                           \
    "{{predicates^i}}|"                                                 \
    "{{end for}};of:"                                                   \
    "{{for i in outputFunctions}}"                                      \
    "{{outputFunctions^i}}|"                                            \
    "{{end for}};pf:{{planfilename}};ak:"                               \
    "{{for i in ackFunctions}}"                                         \
    "{{ackFunctions^i}}|"                                               \
    "{{end for}};pm:"                                                   \
    "{{for i in parameters}}"                                           \
    "{{parameters^i}}|"                                                 \
    "{{end for}}"                                                       \
    "@@end tag@@\n"                                                     \
    "  */\n\n"                                                          \
    "#include <vle/extension/decision/Agent.hpp>\n"                     \
    "#include <vle/extension/decision/Activity.hpp>\n"                  \
    "#include <vle/extension/decision/KnowledgeBase.hpp>\n"             \
    "#include <vle/utils/Package.hpp>\n"                                \
    "#include <sstream>\n"                                              \
    "#include <numeric>\n"                                              \
    "#include <fstream>\n"                                              \
    "//@@begin:include@@\n"                                             \
    "{{include}}"                                                       \
    "//@@end:include@@\n"                                               \
    "namespace vd = vle::devs;\n"                                       \
    "namespace vv = vle::value;\n"                                      \
    "namespace ved = vle::extension::decision;\n"                       \
    "namespace vu = vle::utils;\n"                                      \
    "namespace vmd = vle::extension::decision;\n"                       \
    "namespace {{namespace}} {\n\n"                                     \
    "class {{classname}} : public vmd::Agent\n"                         \
    "{\n"                                                               \
    "public:\n"                                                         \
    "    {{classname}}(\n"                                              \
    "        const vd::DynamicsInit& init,\n"                           \
    "        const vd::InitEventList& evts)\n"                          \
    "        : vmd::Agent(init, evts)\n"                                \
    "\n{{for i in factsInitList}}"                                      \
    "{{factsInitList^i}}\n"                                             \
    "{{end for}}"                                                       \
    "    {\n"                                                           \
    "//@@begin:customconstructor@@\n"                                   \
    "{{customconstructor}}"                                             \
    "//@@end:customconstructor@@\n"                                     \
    "        //Parameters managed by the plugin"                        \
    "//@@begin:constructor@@\n"                                         \
    "{{constructor}}"                                                   \
    "//@@end:constructor@@\n"                                           \
    "        //KnowledgeBase Inititialisation"                          \
    "{{for i in addFactsList}}"                                         \
    "{{addFactsList^i}}"                                                \
    "{{end for}}"                                                       \
    "{{for i in addPredicatesList}}"                                    \
    "{{addPredicatesList^i}}"                                           \
    "{{end for}}"                                                       \
    "{{for i in addOutputFunctionList}}"                                \
    "{{addOutputFunctionList^i}}"                                       \
    "{{end for}}"                                                       \
    "{{for i in addAckFunctionList}}"                                   \
    "{{addAckFunctionList^i}}"                                          \
    "{{end for}}\n"                                                     \
    "        //Plan initialisation\n"                                   \
    "        std::string dataPackageParam;\n"                           \
    "        if (evts.exist(\"PackageName\"))\n"                        \
    "          dataPackageParam = evts.getString(\"PackageName\");\n"   \
    "        else\n"                                                    \
    "          throw vle::utils::ModellingError(\n"                     \
    "             \"Package where to find the data is not set\");\n"    \
    "        vle::utils::Package mPack(dataPackageParam);\n"            \
    "        std::string filePath =\n"                                  \
    "        mPack.getDataFile(\"{{classname}}.txt\");\n"               \
    "        std::ifstream fileStream(filePath.c_str());\n"             \
    "        KnowledgeBase::plan().fill(fileStream);\n"                 \
    "    }\n"                                                           \
    "\n"                                                                \
    "    virtual ~{{classname}}()\n"                                    \
    "    {}\n\n"                                                        \
    "//@@begin:definition@@\n"                                          \
    "{{definition}}"                                                    \
    "//@@end:definition@@\n"                                            \
    "/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *"    \
    "\n * Predicates"                                                   \
    "\n * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */" \
    "\n{{for i in predicatesFunctionList}}"                             \
    "{{predicatesFunctionList^i}}\n\n"                                  \
    "{{end for}}"                                                       \
    "/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *"    \
    "\n * Facts"                                                        \
    "\n * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */" \
    "\n{{for i in factsFunctionList}}"                                  \
    "{{factsFunctionList^i}}\n\n"                                       \
    "{{end for}}"                                                       \
    "/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *"    \
    "\n * Output Functions"                                             \
    "\n * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */" \
    "\n{{for i in outputFunctionList}}"                                 \
    "{{outputFunctionList^i}}\n\n"                                      \
    "{{end for}}"                                                       \
    "/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *"    \
    "\n * Acknowledge functions"                                        \
    "\n * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */" \
    "\n{{for i in ackFunctionList}}"                                    \
    "{{ackFunctionList^i}}\n\n"                                         \
    "{{end for}}"                                                       \
    "private:\n"                                                        \
    "        //Custom members"                                          \
    "//@@begin:custommembers@@\n"                                       \
    "{{custommembers}}"                                                 \
    "//@@end:custommembers@@\n"                                         \
    "\n{{for i in factsAttributeList}}"                                 \
    "{{factsAttributeList^i}}\n\n"                                      \
    "{{end for}}"                                                       \
    "\n{{for i in hierPred}}"                                           \
    "{{hierPred^i}}\n\n"                                                \
    "{{end for}}\n"                                                     \
    "{{hierarchicalPreds}}"                                             \
    "\n};\n\n"                                                          \
    "} // namespace {{namespace}}\n\n"                                  \
    "DECLARE_DYNAMICS({{namespace}}::{{classname}})\n";

// Template : plan file
const std::string PluginDecision::PLAN_TEMPLATE_DEFINITION =
    "#/**\n"                                                            \
    "#  * @file {{classname}}.txt\n"                                    \
    "#  * @author ...\n"                                                \
    "#  * ...\n"                                                        \
    "#  * @@tag Decision@vle.extension.decision @@"                     \
    "namespace:{{namespace}};"                                          \
    "class:{{classname}};{{width}}|{{height}};a:"                       \
    "{{for i in activities}}"                                           \
    "{{activities^i}}|"                                                 \
    "{{end for}};r:"                                                    \
    "{{for i in ruleAndPred}}"                                          \
    "{{ruleAndPred^i}}|"                                                \
    "{{end for}};c:"                                                    \
    "{{for i in precedencesTag}}"                                       \
    "{{precedencesTag^i}}|"                                             \
    "{{end for}}"                                                       \
    "@@end tag@@\n"                                                     \
    "#  */\n\n"                                                         \
    "## Rules\n"                                                        \
    "{{rules}}"                                                         \
    "\n\n"                                                              \
    "## Activities\n"                                                   \
    "activities { # liste des activites\n"                              \
    "{{for i in activitiesList}}"                                       \
    "    activity {\n"                                                  \
    "        id = \"{{activitiesList^i}}\n"                             \
    "    }\n"                                                           \
    "{{end for}}"                                                       \
    "}\n\n## Precedences\n"                                             \
    "{{precedences}}";

const Glib::ustring PluginDecision::UI_DEFINITION =
    "<ui>"
    "    <toolbar name='Toolbar'>"
    "        <toolitem action='Select' />"
    "        <toolitem action='AddState' />"
    "        <toolitem action='AddConstraint' />"
    "        <toolitem action='Delete' />"
    "        <toolitem action='Help' />"
    "    </toolbar>"
    "</ui>";

PluginDecision::PluginDecision(const std::string& package,
                               const std::string& library,
                               const std::string& curr_package)
    : ModelingPlugin(package, library, curr_package), mDialog(0), mDecision(0)
{
    vle::utils::Package pack(getPackage());

    std::string glade =
        pack.getPluginGvleModelingFile("decision.glade",
                vle::utils::PKG_BINARY);

    mXml = Gtk::Builder::create();
    mXml->add_from_file(glade.c_str());

    mXml->get_widget("DecisionDialog", mDialog);

    mXml->get_widget_derived("StatechartDrawingArea", mView);

    mXml->get_widget("IncludeSourceButton", mIncludeButton);
    mXml->get_widget("UserFactButton", mFactButton);
    mXml->get_widget("PredicateButton", mPredicateButton);
    mXml->get_widget("RuleButton", mRuleButton);
    mXml->get_widget("OutputButton", mOutputButton);
    mXml->get_widget("AckButton", mAckButton);
    mXml->get_widget("HierarchicalPredButton", mHierPredsButton);
    // Menu bar
    mXml->get_widget("imagemenuitem2", mOpenMenu);
    mXml->get_widget("MenuItemChooseFileName", mChooseFileNameMenu);
    mXml->get_widget("imagemenuitem4", mSaveAsMenu);

    mList.push_back(mIncludeButton->signal_clicked().connect(
            sigc::mem_fun(*this,
                &PluginDecision::onIncludeSource)));
    mList.push_back(mFactButton->signal_clicked().connect(
            sigc::mem_fun(*this,
                &PluginDecision::onUserFact)));
    mList.push_back(mPredicateButton->signal_clicked().connect(
            sigc::mem_fun(*this,
                &PluginDecision::onUserPredicate)));
    mList.push_back(mRuleButton->signal_clicked().connect(
            sigc::mem_fun(*this,
                &PluginDecision::onUserRule)));
    mList.push_back(mOutputButton->signal_clicked().connect(
            sigc::mem_fun(*this,
                &PluginDecision::onUserOutputFunction)));
    mList.push_back(mAckButton->signal_clicked().connect(
            sigc::mem_fun(*this,
                &PluginDecision::onUserAckFunction)));
    mList.push_back(mHierPredsButton->signal_clicked().connect(
            sigc::mem_fun(*this,
                &PluginDecision::onUserHierPredicate)));
    // Menu bar
    mList.push_back(mOpenMenu->signal_activate().connect(
            sigc::mem_fun(*this,
                &PluginDecision::onUserOpenMenu)));
    mList.push_back(mChooseFileNameMenu->signal_activate().connect(
            sigc::mem_fun(*this, &PluginDecision::onChooseFileName)));
    mList.push_back(mSaveAsMenu->signal_activate().connect(
            sigc::mem_fun(*this, &PluginDecision::onUserSaveAsMenu)));
    {
        Gtk::Box* hbox;
        mXml->get_widget("StatechartHBox", hbox);
        mActionGroup = Gtk::ActionGroup::create();
        mUIManager = Gtk::UIManager::create();
        createActions();
        mUIManager->insert_action_group(mActionGroup);
        mDialog->add_accel_group(mUIManager->get_accel_group());
        createUI();
        mToolbar = dynamic_cast < Gtk::Toolbar* > (
                mUIManager->get_widget("/Toolbar"));
        mToolbar->set_toolbar_style(Gtk::TOOLBAR_BOTH);
        //mToolbar->set_orientation(Gtk::ORIENTATION_VERTICAL); change in GTK3
        mToolbar->set_property("orientation", Gtk::ORIENTATION_VERTICAL);
        mToolbar->set_size_request(100, 50);
        hbox->pack_start(*mToolbar, false, false);
    }
}

PluginDecision::~PluginDecision()
{
    destroy();
    for (std::list < sigc::connection >::iterator it = mList.begin();
        it != mList.end(); ++it) {
        it->disconnect();
    }
}

void PluginDecision::createUI()
{
#ifdef GLIBMM_EXCEPTIONS_ENABLED
    try {
        mUIManager->add_ui_from_string(UI_DEFINITION);
    } catch (const Glib::Error& ex) {
        throw utils::InternalError(fmt(
                _("Decision modeling plugin building menus failed: %1%")) %
            ex.what());
    }
#else
    std::auto_ptr < Glib::Error > ex;
    mUIManager->add_ui_from_string(UI_DEFINITION, ex);
    if (ex.get()) {
        throw utils::InternalError(fmt(
                _("Decision modeling plugin building menus failed: %1%")) %
            ex->what());
    }
#endif
}

void PluginDecision::destroy()
{
    delete mDecision;
    mDecision = NULL;
}

void PluginDecision::onAddActivity()
{
    mView->setState(DecisionDrawingArea::ADD_ACTIVITY_MODE);
}

void PluginDecision::onDelete()
{
    mView->setState(DecisionDrawingArea::DELETE_MODE);
}

void PluginDecision::onSelect()
{
    mView->setState(DecisionDrawingArea::SELECT_MODE);
}

void PluginDecision::onAddConstraint()
{
    mView->setState(DecisionDrawingArea::ADD_CONSTRAINT_MODE);
}

bool PluginDecision::create(vpz::AtomicModel& model,
                            vpz::Dynamic& dynamic,
                            vpz::Conditions& conditions,
                            vpz::Observables& observables,
                            const std::string& classname,
                            const std::string& namespace_)
{
    mView->clearSelections();

    vle::utils::Package pack(getCurrPackage());
    mPlanFileName = pack.getDataDir(vle::utils::PKG_SOURCE) + "/" +
            classname + ".txt";
    mPlanFile = classname;
    mClassname = classname;
    mNamespace = namespace_;

    mDialog->set_title("Decision - Plugin : " + mPlanFile + ".txt");

    mDecision = new Decision(mClassname);

    mView->setDecision(mDecision);
    mDecision->setRule(&mRule);
    mDecision->setAckFunc(&mAckFunctionName);
    mDecision->setOutputFunc(&mOutputFunctionName);

    if (mDialog->run() == Gtk::RESPONSE_ACCEPT) {
        generateSource(mClassname, mNamespace);
        writePlanFile(mPlanFileName);
        generateObservables(model, observables);
        generateConditions(model, conditions);
        generatePorts(model);
        model.setDynamics(dynamic.name());
        mDialog->hide();
        destroy();
        return true;
    }
    else {
        mDialog->hide();
        destroy();
        return false;
    }
}

void PluginDecision::createActions()
{
    Gtk::RadioAction::Group toolsGroup;

    mActionGroup->add(
        Gtk::RadioAction::create(toolsGroup, "Select", Gtk::Stock::INDEX,
            _("Select"),
            _("Select activity (F1)")),
        Gtk::AccelKey("F1"), sigc::mem_fun(this, &PluginDecision::onSelect));
    mActionGroup->add(
        Gtk::RadioAction::create(toolsGroup, "AddState", Gtk::Stock::ADD,
            _("Add activity"),
            _("Add activity (F2)")),
        Gtk::AccelKey("F2"),
        sigc::mem_fun(this, &PluginDecision::onAddActivity));
    mActionGroup->add(
        Gtk::RadioAction::create(toolsGroup, "AddConstraint",
            Gtk::Stock::DISCONNECT,
            _("Add constraint"),
            _("Add constraint (F3)")),
        Gtk::AccelKey("F3"),
        sigc::mem_fun(this, &PluginDecision::onAddConstraint));
    mActionGroup->add(
        Gtk::RadioAction::create(toolsGroup, "Delete", Gtk::Stock::DELETE,
            _("Delete"),
            _("Delete activity (F4)")),
        Gtk::AccelKey("F4"),
        sigc::mem_fun(this, &PluginDecision::onDelete));
}

void PluginDecision::generateSource(const std::string& classname,
    const std::string& namespace_)
{
    utils::Template tpl_(TEMPLATE_DEFINITION);

    tpl_.stringSymbol().append("namespace", namespace_);
    tpl_.stringSymbol().append("classname", classname);
    tpl_.stringSymbol().append("planfilename", mPlanFile);

    // User parameters
    tpl_.stringSymbol().append("include", mInclude);

    // Custom Constructor
    tpl_.stringSymbol().append("customconstructor", mCustomConstructor);

    // Personalised parameters
    std::string constructor;
    for (std::map < std::string, std::string > ::iterator it = mParam.begin();
            it != mParam.end();
            ++it) {
        // Implements the parameterized constructor
        constructor += "if (evts.exist(\"" + it->first + "\"))\n";
        constructor += it->first + " = " + "evts.getDouble(\"" + it->first
                + "\");\n";
        constructor += "else\n";
        constructor += "throw vle::utils::ModellingError(\"Parameter" +
                it->first + "not found\");\n\n";
    }
    tpl_.stringSymbol().append("constructor", constructor);

    std::string definition;
    for (std::map < std::string, std::string > ::iterator it = mParam.begin();
            it != mParam.end();
            ++it) {
        definition += "double " + it->first + ";\n";
    }
    tpl_.stringSymbol().append("definition", definition);

    // facts
    tpl_.listSymbol().append("facts");
    for (std::vector < std::string > ::const_iterator it = mFactName.begin();
            it != mFactName.end(); ++it) {
        tpl_.listSymbol().append("facts", *it);
    }

    // facts functions (specials tags)
    tpl_.listSymbol().append("factsFunctionList");
    for (std::vector < std::string > ::const_iterator it = mFactName.begin();
            it != mFactName.end(); ++it) {
        tpl_.listSymbol().append("factsFunctionList", "{{factsFunctionTags"
                + *it + "}}" );
    }

    // facts attributes (specials tags)
    tpl_.listSymbol().append("factsAttributeList");
    for (std::vector < std::string > ::const_iterator it = mFactName.begin();
            it != mFactName.end(); ++it) {
        tpl_.listSymbol().append("factsAttributeList", "{{factsAttributeTags"
                + *it + "}}" );
    }

    // Facts init list
    tpl_.listSymbol().append("factsInitList");
    for (std::vector < std::string > ::const_iterator it = mFactName.begin();
         it != mFactName.end(); ++it) {
        std::string factinit;
        if ( mFactInit.find(*it) != mFactInit.end() ) {
            factinit = ",\n";
            factinit += "//@@begin:factsInit" + *it + "@@\n";
            factinit += mFactInit[*it];
            factinit += "\n";
            factinit += "//@@end:factsInit" + *it + "@@\n";
        }
        tpl_.listSymbol().append("factsInitList", factinit);
    }

    // addFacts list
    tpl_.listSymbol().append("addFactsList");
    for (std::vector < std::string > ::const_iterator it = mFactName.begin();
            it != mFactName.end(); ++it) {
        std::string esp8 = "        ";
        std::string esp12 = esp8 + "    ";
        if (it == mFactName.begin() && it == (mFactName.end() - 1)) {
        tpl_.listSymbol().append("addFactsList", "\n" + esp8 +
            "addFacts(this) +=\n" + esp12 + ""\
            "F(\"" + *it + "\", &" + classname + "::" + *it + ");\n");
        }
        else if (it == mFactName.begin()) {
            tpl_.listSymbol().append("addFactsList", "\n" + esp8 +
            "addFacts(this) +=\n" + esp12 + ""\
            "F(\"" + *it + "\", &" + classname + "::" + *it + "),\n");
        }
        else if (it == (mFactName.end() - 1) ) {
            tpl_.listSymbol().append("addFactsList", esp12 + "F(\"" + *it +
            "\", &" + classname + "::" + *it + ");\n" );
        }
        else {
            tpl_.listSymbol().append("addFactsList", esp12 + "F(\"" + *it +
                "\", &" + classname + "::" + *it + "),\n" );
        }
    }

    // output functions
    tpl_.listSymbol().append("outputFunctions");
    for (std::vector < std::string > ::const_iterator it =
            mOutputFunctionName.begin();
            it != mOutputFunctionName.end(); ++it) {
        tpl_.listSymbol().append("outputFunctions", *it);
    }

    // output functions (specials tags)
    tpl_.listSymbol().append("outputFunctionList");
    for (std::vector < std::string > ::const_iterator it =
            mOutputFunctionName.begin();
            it != mOutputFunctionName.end(); ++it) {
        tpl_.listSymbol().append("outputFunctionList", "{{outputFunctionTags"
                + *it + "}}");
    }

    // add output functions list
    tpl_.listSymbol().append("addOutputFunctionList");
    for (std::vector < std::string > ::const_iterator it =
            mOutputFunctionName.begin();
            it != mOutputFunctionName.end(); ++it) {
        std::string esp8 = "        ";
        std::string esp12 = esp8 + "    ";
        if (it == mOutputFunctionName.begin() && it ==
                (mOutputFunctionName.end() - 1)) {
        tpl_.listSymbol().append("addOutputFunctionList", "\n" + esp8 +
            "addOutputFunctions(this) +=\n" + esp12 + ""\
            "O(\"" + *it + "\", &" + classname + "::" + *it + ");\n");
        }
        else if (it == mOutputFunctionName.begin()) {
            tpl_.listSymbol().append("addOutputFunctionList", "\n" + esp8 +
            "addOutputFunctions(this) +=\n" + esp12 + ""\
            "O(\"" + *it + "\", &" + classname + "::" + *it + "),\n");
        }
        else if (it == (mOutputFunctionName.end() - 1) ) {
            tpl_.listSymbol().append("addOutputFunctionList", esp12 + "O(\""
            + *it + "\", &" + classname + "::" + *it + ");\n");
        }
        else {
            tpl_.listSymbol().append("addOutputFunctionList", esp12 + "O(\""
            + *it + "\", &" + classname + "::" + *it + "),\n");
        }
    }

    // acknowledge functions
    tpl_.listSymbol().append("ackFunctions");
    for (std::vector < std::string > ::const_iterator it =
            mAckFunctionName.begin();
            it != mAckFunctionName.end(); ++it) {
        tpl_.listSymbol().append("ackFunctions", *it);
    }

    // acknowledge functions (specials tags)
    tpl_.listSymbol().append("ackFunctionList");
    for (std::vector < std::string > ::const_iterator it =
            mAckFunctionName.begin();
            it != mAckFunctionName.end(); ++it) {
        tpl_.listSymbol().append("ackFunctionList", "{{ackFunctionTags"
                + *it + "}}");
    }

    // add acknowledge functions list
    tpl_.listSymbol().append("addAckFunctionList");
    for (std::vector < std::string > ::const_iterator it =
            mAckFunctionName.begin();
            it != mAckFunctionName.end(); ++it) {
        std::string esp8 = "        ";
        std::string esp12 = esp8 + "    ";
        if (it == mAckFunctionName.begin() && it ==
                (mAckFunctionName.end() - 1)) {
        tpl_.listSymbol().append("addAckFunctionList", "\n" + esp8 +
            "addAcknowledgeFunctions(this) +=\n" + esp12 + ""\
            "A(\"" + *it + "\", &" + classname + "::" + *it + ");\n");
        }
        else if (it == mAckFunctionName.begin()) {
            tpl_.listSymbol().append("addAckFunctionList", "\n" + esp8 +
            "addAcknowledgeFunctions(this) +=\n" + esp12 + ""\
            "A(\"" + *it + "\", &" + classname + "::" + *it + "),\n");
        }
        else if (it == (mAckFunctionName.end() - 1) ) {
            tpl_.listSymbol().append("addAckFunctionList", esp12 + "A(\""
            + *it + "\", &" + classname + "::" + *it + ");\n");
        }
        else {
            tpl_.listSymbol().append("addAckFunctionList", esp12 + "A(\""
            + *it + "\", &" + classname + "::" + *it + "),\n");
        }
    }

    // predicates
    tpl_.listSymbol().append("predicates");
    for (std::vector < std::string > ::const_iterator it =
            mPredicateName.begin();
            it != mPredicateName.end(); ++it) {
        tpl_.listSymbol().append("predicates", *it);
    }

    // predicates functions (specials tags)
    tpl_.listSymbol().append("predicatesFunctionList");
    for (std::vector < std::string > ::const_iterator it =
            mPredicateName.begin();
            it != mPredicateName.end(); ++it) {
        tpl_.listSymbol().append("predicatesFunctionList",
                "{{predicatesFunctionTags" + *it + "}}" );
    }

    // addPredicates list
    tpl_.listSymbol().append("addPredicatesList");
    if (!mPred.empty() || !mPredicateName.empty()) {
        for (std::vector < std::string > ::const_iterator it =
                mPredicateName.begin();
                it != mPredicateName.end(); ++it) {
            std::string esp8 = "        ";
            std::string esp12 = esp8 + "    ";
            if (it == mPredicateName.begin()) {
                tpl_.listSymbol().append("addPredicatesList", "\n" + esp8 +
                "addPredicates(this) +=\n" + esp12
                + "P(\"" + *it + "\", &" + classname +
                "::" + *it + ")");
            }
            else {
                tpl_.listSymbol().append("addPredicatesList", ",\n" + esp12
                + "P(\"" + *it
                + "\", &" + classname + "::" + *it + ")" );
            }
        }
        if (mPred.empty()) {
            tpl_.listSymbol().append("addPredicatesList", ";");
        }
        else {
            for (hierarchicalPred::iterator it = mPred.begin();
                    it != mPred.end(); ++it) {
                std::string esp8 = "        ";
                std::string esp12 = esp8 + "    ";
                if (it == mPred.begin() && mPredicateName.empty()) {
                    tpl_.listSymbol().append("addPredicatesList", "\n" + esp8 +
                    "addPredicates(this) +=\n" + esp12
                    + "P(\"" + it->first + "\", &" + classname +
                    "::" + it->first + ")");
                }
                else {
                    tpl_.listSymbol().append("addPredicatesList", ",\n" +
                    esp12 + "P(\"" + it->first
                    + "\", &" + classname + "::" + it->first + ")" );
                }
            }
            tpl_.listSymbol().append("addPredicatesList", ";");
        }
    }

    // parameters
    tpl_.listSymbol().append("parameters");
    for (std::map < std::string, std::string > ::const_iterator it =
            mParam.begin();
            it != mParam.end(); ++it) {
        tpl_.listSymbol().append("parameters", it->first +
                "," + "double" + "," + it->second);
    }

    // hierPred
    tpl_.listSymbol().append("hierPred");
    for (hierarchicalPred::iterator it = mPred.begin(); it != mPred.end();
            ++it) {
        if (it->second.size() > 1) {
            strings_t::iterator itParPred = it->second.begin();
            itParPred++;
            std::string pred;
            generatePred(&pred, &itParPred, &it->second, 0, "");
            pred = "bool " + it->first + "() const\n{\n    return (" + pred;
            pred += ");\n}";
            tpl_.listSymbol().append("hierPred", pred);
        }
    }

    // hierarchical predicates
    std::string hierarchicalPred("/*@@begin:hierarchicalPreds@@\n");
    for (hierarchicalPred::iterator it = mPred.begin(); it != mPred.end();
            ++it) {
        for (strings_t::iterator it2 = it->second.begin();
                it2 != it->second.end();
                it2++) {
            hierarchicalPred += *it2 + ";";
        }
    }
    hierarchicalPred += "@@end:hierarchicalPreds@@*/";
    tpl_.stringSymbol().append("hierarchicalPreds", hierarchicalPred);

    // Custom members
    tpl_.stringSymbol().append("custommembers", mMembers);

    std::ostringstream out;
    tpl_.process(out);

    utils::Template tpl1(out.str());
    tpl_ = tpl1;

    // facts functions (function)
    for (std::vector < std::string > ::const_iterator it = mFactName.begin();
            it != mFactName.end(); ++it) {
        std::string function;
        if ( mFactFunction.find(*it) != mFactFunction.end() ) {
            function = "void " + *it + "(const vv::Value& value)\n{";
            function += "//@@begin:factsFunction" + *it + "@@\n";
            function += mFactFunction[*it];
            function += "//@@end:factsFunction" + *it + "@@\n}";
        }
        else {
            function = "void " + *it + "(const vv::Value& value)\n{";
            function = "//@@begin:factsFunction" + *it + "@@ ";
            function += "//@@end:factsFunction" + *it + "@@\n}";
        }

        std::string tag = "factsFunctionTags" + *it;
        tpl_.stringSymbol().append(tag, function);
    }

    // facts attributes (function)
    for (std::vector < std::string > ::const_iterator it = mFactName.begin();
            it != mFactName.end(); ++it) {
        std::string function;
        if ( mFactAttribute.find(*it) != mFactAttribute.end() ) {
            function = "//@@begin:factsAttribute" + *it + "@@\n";
            function += mFactAttribute[*it];
            function += "//@@end:factsAttribute" + *it + "@@";
        }
        else {
            function = "//@@begin:factsAttribute" + *it + "@@ ";
            function += "//@@end:factsAttribute" + *it + "@@";
        }

        std::string tag = "factsAttributeTags" + *it;
        tpl_.stringSymbol().append(tag, function);
    }

    // output functions (specials tags)
    for (std::vector < std::string > ::const_iterator it =
            mOutputFunctionName.begin();
            it != mOutputFunctionName.end(); ++it) {
        std::string tag = "outputFunctionTags" + *it;

        std::string function = "void " + *it + "(\n"\
                "const std::string& name,\nconst ved::Activity& activity,\n"\
                "vd::ExternalEventList& output) {";
        function += "//@@begin:outputFunction" + *it +
                "@@\n" + mOutputFunction[*it] + "//@@end:outputFunction" +
                *it + "@@\n}";
        tpl_.stringSymbol().append(tag, function);
    }

    // acknowledge functions (specials tags)
    for (std::vector < std::string > ::const_iterator it =
            mAckFunctionName.begin();
            it != mAckFunctionName.end(); ++it) {
        std::string tag = "ackFunctionTags" + *it;

        std::string function = "void " + *it + "(const std::string&"\
                    "activityname, const ved::Activity& activity) {";
        function += "//@@begin:ackFunction" + *it +
                "@@\n" + mAckFunction[*it] + "//@@end:ackFunction" +
                *it + "@@\n}";
        tpl_.stringSymbol().append(tag, function);
    }

    // predicates functions (function)
    for (std::vector < std::string > ::const_iterator it =
            mPredicateName.begin();
            it != mPredicateName.end(); ++it) {
        std::string function;
        if ( mPredicateFunction.find(*it) != mPredicateFunction.end() ) {
            function = "bool " + *it + "() const\n{";
            function += "//@@begin:predicatesFunction" + *it + "@@\n";
            function += mPredicateFunction[*it];
            function += "//@@end:predicatesFunction" + *it + "@@\n}";
        }
        else {
            function = "bool " + *it + "() const\n{";
            function = "//@@begin:predicatesFunction" + *it + "@@ ";
            function += "//@@end:predicatesFunction" + *it + "@@\n}";
        }

        std::string tag = "predicatesFunctionTags" + *it;
        tpl_.stringSymbol().append(tag, function);
    }

    std::ostringstream out1;
    tpl_.process(out1);

    mSource = out1.str();
}

bool PluginDecision::modify(vpz::AtomicModel& model,
                            vpz::Dynamic& dynamic,
                            vpz::Conditions& conditions,
                            vpz::Observables& observables,
                            const std::string& conf,
                            const std::string& buffer)
{
    mView->clearSelections();

    std::string conditionPlanName((fmt("condDecisionPlan_%1%") %
                                   model.getName()).str());

    strings_t lst;

// Buffer : contains *.c file, only used to
// read classname, namespace and includes
    boost::split(lst, conf, boost::is_any_of(";"));
    parseConf(lst, mClassname, mNamespace);
    // Parse the plan filename
    parsePlanFileName(lst);
    // If conditions have an other plan file
    // name, it uses the one who is in the conditions

    if (conditions.exist(conditionPlanName)) {
        mPlanFile =
        conditions.get(conditionPlanName).firstValue("planFile").toString().value();
    }

    mDialog->set_title("Decision - Plugin : " + mPlanFile + ".txt");

    boost::trim(mClassname);
    parseFacts(lst);
    parsePredicates(lst);
    parseOutputFunctions(lst);
    parseAckFunctions(lst);
    parseParameters(model, lst, conditions);

    // parse the hierarchical predicates informations
    std::string hierP = parseFunction(buffer, "@@begin:hierarchicalPreds@@",
            "@@end:hierarchicalPreds@@", "hierarchicalPreds");
    decodePredicates(hierP);

    vle::utils::Package pack(getCurrPackage());

    mPlanFileName = pack.getDataDir(vle::utils::PKG_SOURCE) + "/" +
            mPlanFile + ".txt";

    mInclude.assign(parseFunction(buffer,
                                  "//@@begin:include@@",
                                  "//@@end:include@@",
                                  "include"));
    mCustomConstructor.assign(parseFunction(buffer,
                                            "//@@begin:customconstructor@@",
                                            "//@@end:customconstructor@@",
                                            "customconstructeur"));
    mConstructor.assign(parseFunction(buffer,
                                      "//@@begin:constructor@@",
                                      "//@@end:constructor@@",
                                      "constructor"));
    mDefinition.assign(parseFunction(buffer,
                                     "//@@begin:definition@@",
                                     "//@@end:definition@@",
                                     "definition"));

    // parse the facts functions informations
    for (std::vector < std::string > ::const_iterator it = mFactName.begin();
            it != mFactName.end(); ++it) {
        mFactFunction[*it] = parseFunction(buffer, "//@@begin:factsFunction"
            + *it + "@@", "//@@end:factsFunction" + *it + "@@", "factsFunction"
            + *it);
    }

    mMembers.assign(parseFunction(buffer,
                                 "//@@begin:custommembers@@",
                                 "//@@end:custommembers@@",
                                 "custommembers"));

    // parse the facts attributes informations
    for (std::vector < std::string > ::const_iterator it = mFactName.begin();
            it != mFactName.end(); ++it) {
        mFactAttribute[*it] = parseFunction(buffer, "//@@begin:factsAttribute"
            + *it + "@@", "//@@end:factsAttribute" + *it + "@@",
            "factsAttribute" + *it);
    }

    // parse the facts Init informations
    for (std::vector < std::string > ::const_iterator it = mFactName.begin();
         it != mFactName.end(); ++it) {
        mFactInit[*it] = parseFunction(buffer, "//@@begin:factsInit"
                                       + *it + "@@", "//@@end:factsInit" + *it + "@@",
                                       "factsInit" + *it);
    }

    // parse the predicates functions informations
    for (std::vector < std::string > ::const_iterator it =
            mPredicateName.begin();
            it != mPredicateName.end(); ++it) {
        mPredicateFunction[*it] = parseFunction(buffer,
            "//@@begin:predicatesFunction" + *it + "@@",
            "//@@end:predicatesFunction" + *it + "@@", "predicatesFunction"
            + *it);
    }

    // parse the output functions informations
    for (std::vector < std::string > ::const_iterator it =
            mOutputFunctionName.begin();
            it != mOutputFunctionName.end(); ++it) {
        mOutputFunction[*it] = parseFunction(buffer, "//@@begin:outputFunction"
            + *it + "@@", "//@@end:outputFunction" + *it +
            "@@", "outputFunction" + *it);
    }

    // parse the acknowledge functions informations
    for (std::vector < std::string > ::const_iterator it =
            mAckFunctionName.begin();
            it != mAckFunctionName.end(); ++it) {
        mAckFunction[*it] = parseFunction(buffer, "//@@begin:ackFunction"
            + *it + "@@", "//@@end:ackFunction" + *it +
            "@@", "ackFunction" + *it);
    }

    mDecision = new Decision(mClassname);
    mDecision->setRule(&mRule);
    mDecision->setOutputFunc(&mOutputFunctionName);
    mDecision->setAckFunc(&mAckFunctionName);

    if (!loadPlanFile(mPlanFileName)) {
        Gtk::MessageDialog errorDial("Error filling knowledge"\
                "base!\n"\
                "Check correspondence between predicates, acknowledge"\
                "functions and output functions with the C++ class...",
                false,
                Gtk::MESSAGE_ERROR,
                Gtk::BUTTONS_OK,
                true);
        errorDial.set_title("Error !");
        errorDial.run();
        return false;
    }

    if (mDialog->run() == Gtk::RESPONSE_ACCEPT) {
        generateSource(mClassname, mNamespace);
        generateObservables(model, observables);
        generateConditions(model, conditions);
        generatePorts(model);
        model.setDynamics(dynamic.name());
        writePlanFile(mPlanFileName);
        mDialog->hide();
        clear();
        destroy();
        return true;
    }
    else {
        mDialog->hide();
        clear();
        destroy();
        return false;
    }
}

bool PluginDecision::loadPlanFile(std::string filename)
{
// planBuffer : contains the entire generated *.txt plan file
    std::string planBuffer;
    std::string planPluginName;
    std::string planPackageName;
    std::string planConfig;

    utils::Template planTemplate;
    planTemplate.open(filename);
    planTemplate.tag(planPluginName, planPackageName, planConfig);

    strings_t planArgsList;
    boost::split(planArgsList, planConfig, boost::is_any_of(";"));
    // width & height
    strings_t wh;

    boost::split(wh, planArgsList[2], boost::is_any_of("|"));
    mDecision->width(boost::lexical_cast < int > (wh[0]));
    mDecision->height(boost::lexical_cast < int > (wh[1]));

    // activities
    parseActivities(planArgsList);

    // rules
    parseRulesAndPred(planArgsList);

    // precedenceConstraints
    parsePrecedenceConstraints(planArgsList);

    //KnowledgeBase Inititialisation
    mDecision->resetKnowledgeBase();
    for (std::vector < std::string > ::const_iterator it =
            mPredicateName.begin();
            it != mPredicateName.end(); ++it) {
        mDecision->getKnowledgeBase()->predicates().add(*it,
                boost::bind(&PluginDecision::dumyPredicate, this));
    }
    for (hierarchicalPred::const_iterator it =
            mPred.begin();
            it != mPred.end(); ++it) {
        mDecision->getKnowledgeBase()->predicates().add(it->first,
                boost::bind(&PluginDecision::dumyPredicate, this));
    }

    for (std::vector < std::string > ::const_iterator it =
            mOutputFunctionName.begin();
            it != mOutputFunctionName.end(); ++it) {
        mDecision->getKnowledgeBase()->outputFunctions().add(*it,
                boost::bind(&PluginDecision::dumyPredicate, this));
    }

    for (std::vector < std::string > ::const_iterator it =
            mAckFunctionName.begin();
            it != mAckFunctionName.end(); ++it) {
        mDecision->getKnowledgeBase()->acknowledgeFunctions().add(*it,
                boost::bind(&PluginDecision::dumyPredicate, this));
    }

    // Fill KB with plan
    if (!mDecision->fillKnowledgeBase(mPlanFileName)) {
        return false;
    }

    // Get the Activities dates in the KB
    for (vle::extension::decision::Activities::const_iterator it =
        mDecision->getKnowledgeBase()->activities().begin();
        it != mDecision->getKnowledgeBase()->activities().end(); ++it)
    {
        mDecision->activityModel(it->first)->minstart(
                utils::toScientificString(it->second.minstart()));
        mDecision->activityModel(it->first)->maxfinish(
                utils::toScientificString(it->second.maxfinish()));
    }

    // Get the Rules atached to an activity in the KB
    for (vle::extension::decision::Activities::const_iterator it =
        mDecision->getKnowledgeBase()->activities().begin();
        it != mDecision->getKnowledgeBase()->activities().end(); ++it) {
        for (vle::extension::decision::Rules::const_iterator it2 =
                it->second.rules().begin();
                it2 != it->second.rules().end(); ++it2) {
            mDecision->activityModel(it->first)->addRule(it2->first);
        }
    }

    mView->setDecision(mDecision);

    return true;
}

void PluginDecision::writePlanFile(std::string filename)
{
    // Write PLAN file
    utils::Template templateSave(PLAN_TEMPLATE_DEFINITION);
    templateSave.stringSymbol().append("namespace", mNamespace);
    templateSave.stringSymbol().append("classname", mClassname);

    // width & height
    templateSave.stringSymbol().append("width",
        boost::lexical_cast < std::string > (
            mDecision->width()));
    templateSave.stringSymbol().append("height",
        boost::lexical_cast < std::string > (
            mDecision->height()));

    // activities
    templateSave.listSymbol().append("activities");
    for (activitiesModel_t::const_iterator it = mDecision->
            activitiesModel().begin(); it !=
            mDecision->activitiesModel().end(); ++it) {
        std::string outputFunc = "";
        std::string ackFunc = "";
        std::string relDate = "";
        if (!it->second->getOutputFunc().empty()) {
            outputFunc = it->second->getOutputFunc().at(0);
        }
        if (!it->second->getAckFunc().empty()) {
            ackFunc = it->second->getAckFunc().at(0);
        }
        if (it->second->getRelativeDate()) {
            relDate = "R";
        }
        templateSave.listSymbol().append("activities", it->second->toString() +
                "," + outputFunc + "," + ackFunc +  "," + relDate);
    }

    //ruleAndPred
    templateSave.listSymbol().append("ruleAndPred");
    for (std::map < std::string, strings_t > ::const_iterator it =
            mRule.begin(); it != mRule.end(); ++it ) {
        std::string writeRule;
        writeRule = it->first;
        for (strings_t::const_iterator it2 = it->second.begin() ;
                it2 != it->second.end() ; ++it2) {
            writeRule += "," + *it2;
        }
        templateSave.listSymbol().append("ruleAndPred", writeRule);
    }

    // rulesTag
    if ( mRule.size() != 0 ) {
        std::string rulesList = "rules { # liste des règles\n"          \
        "{{for i in rulesList}}"                                        \
        "    rule {\n"                                                  \
        "        id = \"{{rulesList^i}}\n"                              \
        "    }\n"                                                       \
        "{{end for}}}";
        templateSave.stringSymbol().append("rules", rulesList);

        // rulesList
        templateSave.listSymbol().append("rulesList");
        for (std::map < std::string, strings_t > ::const_iterator it =
                mRule.begin(); it != mRule.end(); ++it ) {
            std::string writeRule;
            if ( !mRule[it->first].empty() ) {
                writeRule = it->first + "\";\n        predicates = ";
            }
            else {
                writeRule = it->first + "\";";
            }

            for (strings_t::const_iterator it2 = it->second.begin() ;
                    it2 != it->second.end() ; ++it2) {
                if (it2 == (it->second.end() - 1)) {
                    writeRule += "\"" + *it2 + "\";";
                }
                else {
                    writeRule += "\"" + *it2 + "\", ";
                }
            }

            templateSave.listSymbol().append("rulesList", writeRule);
        }
    }
    else {
        templateSave.stringSymbol().append("rules", "# Pas de règles.");
    }

    // activitiesList
    templateSave.listSymbol().append("activitiesList");
    for (activitiesModel_t::const_iterator it = mDecision->
             activitiesModel().begin(); it != mDecision->
             activitiesModel().end(); ++it) {
        std::string esp16 = "                ";
        std::string actListElem;
        if (it->second->getRelativeDate()) {
            actListElem = it->second->name() +
                "\";\n        temporal {\n" + esp16 + "minstart = +" +
                it->second->minstart() + ";\n" + esp16 + "maxfinish = +" +
                it->second->maxfinish() + ";\n" + "        }\n";
        }
        else {
            actListElem = it->second->name() +
                "\";\n        temporal {\n" + esp16 + "minstart = " +
                it->second->minstart() + ";\n" + esp16 + "maxfinish = " +
                it->second->maxfinish() + ";\n" + "        }\n";
        }
        //activity rules
        strings_t mVect = it->second->getRules();

        for (strings_t::const_iterator it2 = mVect.begin();
                it2 != mVect.end(); ++it2) {
            if (it2 == mVect.begin() && it2 == (mVect.end() - 1)) {
                actListElem += "        rules = \"" + *it2 + "\";";
            }
            else if (it2 == mVect.begin()) {
                actListElem += "        rules = \"" + *it2 + "\"";
            }
            else if (it2 == (mVect.end() - 1)) {
                actListElem += ", \"" + *it2 + "\";";
            }
            else {
                actListElem += ", \"" + *it2 + "\"";
            }
        }

        if (it->second->getAckFunc().size() != 0) {
            actListElem += "\n        ack = \"" +
                    it->second->getAckFunc().at(0) + "\";";
        }

        if (it->second->getOutputFunc().size() != 0) {
            actListElem += "\n        output = \"" +
                    it->second->getOutputFunc().at(0) + "\";";
        }

        templateSave.listSymbol().append("activitiesList", actListElem);
    }

    // add PrecedenceConstraint tags
    templateSave.listSymbol().append("precedencesTag");
    for (precedenceConstraints_t::const_iterator it =
            mDecision->precedenceConstraints().begin();
            it != mDecision->precedenceConstraints().end(); ++it) {
        std::string tmpPre = (*it)->cType() + "," + (*it)->source() + "," +
                (*it)->destination() + "," + (*it)->actTlMin() + "," +
                (*it)->actTlMax() + ",";
        std::ostringstream oss;
        oss << (*it)->points().at(0).x << "," << (*it)->points().at(0).y <<
                "," << (*it)->points().at(1).x << "," <<
                (*it)->points().at(1).y;
        tmpPre += oss.str();
        templateSave.listSymbol().append("precedencesTag", tmpPre);
    }

    // precedences
    if (mDecision->precedenceConstraints().size() != 0) {
        std::string precedenceList = "precedences {\n"                  \
        "{{for i in precedencesList}}"                                  \
        "    precedence {\n"                                            \
        "        type = {{precedencesList^i}}\n"                        \
        "    }\n"                                                       \
        "{{end for}}}";
        templateSave.stringSymbol().append("precedences", precedenceList);

        // precedencesList
        templateSave.listSymbol().append("precedencesList");
        for (precedenceConstraints_t::const_iterator it =
                mDecision->precedenceConstraints().begin();
                it != mDecision->precedenceConstraints().end(); ++it ) {
            std::string writeConstraint;
            writeConstraint = (*it)->cType() + ";\n        first = \"" +
                (*it)->source() + "\";\n";
            writeConstraint += "        second = \"" + (*it)->destination();
            writeConstraint += "\";";

            if (!(*it)->actTlMin().empty()) {
                writeConstraint += "\n        mintimelag = " +
                    (*it)->actTlMin() + ";";
            }
            if (!(*it)->actTlMax().empty()) {
                writeConstraint += "\n        maxtimelag = " +
                    (*it)->actTlMax() + ";";
            }

            templateSave.listSymbol().append("precedencesList",
                    writeConstraint);
        }
    }
    else {
        templateSave.stringSymbol().append("precedences",
        "# Pas de contraintes de précédence.");
    }

    std::ostringstream outPlan;
    templateSave.process(outPlan);

    ofstream fichier(filename.c_str(), ios::out);

    if(fichier) {
        fichier << outPlan.str() << endl;
        fichier.close();
    }
}

void PluginDecision::generateConditions(vpz::AtomicModel& model,
                                        vpz::Conditions& pConditions)
{
    std::string conditionPlanName((fmt("condDecisionPlan_%1%") %
                                   model.getName()).str());
    std::string conditionParametersName((fmt("condDecisionParameters_%1%") %
                                         model.getName()).str());

    if (pConditions.exist(conditionPlanName)) {
        pConditions.del(conditionPlanName);
    }

    if (pConditions.exist(conditionParametersName)) {
        pConditions.del(conditionParametersName);
    }

    // Plan file name
    vpz::Condition planFileName(conditionPlanName);
    value::String planFile(mPlanFile);
    value::String packageName(getCurrPackage());
    planFileName.addValueToPort("planFile", planFile);
    planFileName.addValueToPort("PackageName", packageName);
    // Parameters
    vpz::Condition parameters(conditionParametersName);
    for (std::map < std::string, std::string > ::const_iterator it =
             mParam.begin();
            it != mParam.end(); ++it) {
        std::istringstream iss(it->second);
        double nb;
        iss >> nb;
        value::Double value(nb);
        parameters.addValueToPort(it->first, value);
    }

    pConditions.add(planFileName);
    pConditions.add(parameters);

    model.addCondition(conditionPlanName);
    model.addCondition(conditionParametersName);
}

void PluginDecision::generateObservables(vpz::AtomicModel& model,
                                    vpz::Observables& observables) {
    std::string observableName((fmt("obs_Decision_%1%") %
            model.getName()).str());

    if (observables.exist(observableName)) {
        vpz::Observable& observable(observables.get(observableName));

        // suppresses the activities and rules deleted
        for (vpz::Observable::iterator it = observable.begin();
                it != observable.end();
                ++it) {
            bool found = false;

            for (activitiesModel_t::const_iterator it2 = mDecision->
                    activitiesModel().begin(); it2 !=
                    mDecision->activitiesModel().end() && found == false;
                    ++it2) {
                if (it->first == "Activity_" + it2->second->name()) {
                    found = true;
                }
            }

            for (std::map < std::string, strings_t > ::const_iterator it2 =
                    mRule.begin()
                    ; it2 != mRule.end() && found == false; ++it2) {
                if ("Rules_" + it2->first == it->first) {
                    found = true;
                }
            }

            if (found == false) {
                observable.del(it->first);
            }
        }

        // check all Activities (Activity_Name)
        for (activitiesModel_t::const_iterator it = mDecision->
                activitiesModel().begin(); it !=
                mDecision->activitiesModel().end();
                ++it) {
            if (!observable.exist("Activity_" + it->second->name())) {
                observable.add("Activity_" + it->second->name());
            }
        }

        // check all Rules (Rules_Name)
        for (std::map < std::string, strings_t > ::const_iterator it =
                mRule.begin()
                ; it != mRule.end(); ++it) {
            if (!observable.exist("Rules_" + it->first)) {
                observable.add("Rules_" + it->first);
            }
        }

        if (!observable.exist("Activities")) {
            observable.add("Activities");
        }
        if (!observable.exist("KnowledgeBase")) {
            observable.add("KnowledgeBase");
        }
    }
    else {
        vpz::Observable observable(observableName);
        // add all Activities (Activity_Name)
        for (activitiesModel_t::const_iterator it = mDecision->
                activitiesModel().begin(); it !=
                mDecision->activitiesModel().end();
                ++it) {
            observable.add("Activity_" + it->second->name());
        }

        // add all Rules (Rules_Name)
        for (std::map < std::string, strings_t > ::const_iterator it =
                mRule.begin()
                ; it != mRule.end(); ++it) {
            observable.add("Rules_" + it->first);
        }

        observable.add("Activities");
        observable.add("KnowledgeBase");

        observables.add(observable);
        model.setObservables(observableName);
    }
}

 void PluginDecision::generatePorts(vpz::AtomicModel& model)
 {
    model.getInputPortList().clear();
    model.getOutputPortList().clear();
    model.addInputPort("ack");

    // facts
    for (std::vector < std::string > ::const_iterator it = mFactName.begin();
            it != mFactName.end(); ++it) {
        model.addInputPort(*it);
    }

    // output functions
    for (std::vector < std::string > ::const_iterator it =
            mOutputFunctionName.begin();
            it != mOutputFunctionName.end(); ++it) {
        model.addOutputPort(*it);
    }
}

void PluginDecision::parseConf(const strings_t& lst,
    std::string& classname,
    std::string& namespace_)
{
// namespace
    namespace_ = std::string(lst[0], 10, lst[0].size() - 10);

// classname
    classname = std::string(lst[1], 6, lst[1].size() - 6);
}

void PluginDecision::onIncludeSource()
{
    SourceDialog dialog(mXml,
                        mParam,
                        mInclude,
                        mCustomConstructor,
                        mMembers);

    if (dialog.run() == Gtk::RESPONSE_ACCEPT) {
        mInclude.assign(dialog.getIncludes());
        mCustomConstructor.assign(dialog.getCustomConstructor());
        mMembers.assign(dialog.getMembers());
        mParam = dialog.getParam();
    }
}

void PluginDecision::onUserFact()
{
    FactDialog dialog(mXml, mFactName, mFactFunction, mFactAttribute, mFactInit);
    int state = dialog.run();

    if (state == Gtk::RESPONSE_ACCEPT) {
        mFactName = dialog.getFactName();
        mFactAttribute = dialog.getFactAttribute();
        mFactFunction = dialog.getFactFunction();
        mFactInit = dialog.getFactInit();
    }
}

void PluginDecision::onUserPredicate()
{
    PredicateDialog dialog(mXml, mPredicateName, mPredicateFunction, mPred);

    if (dialog.run() == Gtk::RESPONSE_ACCEPT) {
        mPredicateName = dialog.getPredicateName();
        mPredicateFunction = dialog.getPredicateFunction();

// Delete the deleted predicates in the Rule vector
        for (std::map < std::string, strings_t > ::const_iterator it =
                mRule.begin()
                ; it != mRule.end(); ++it) {
            for (strings_t::iterator it2 = mRule[it->first].begin();
                    it2 != mRule[it->first].end(); ) {
                if (mPred.find(*it2) == mPred.end() &&
                        find(mPredicateName.begin(), mPredicateName.end(),
                        *it2) == mPredicateName.end()) {
                    it2 = mRule[it->first].erase(it2);
                }
                else {
                    ++it2;
                }
            }
        }
    }
}

void PluginDecision::onUserHierPredicate() {
    std::vector < std::string > vectorParam;
    for (std::map < std::string, std::string > ::iterator it =
            mParam.begin(); it != mParam.end(); ++it) {
        vectorParam.push_back(it->first);
    }
    EditorDialog dialog(mXml,
                                mPred,
                                mFactName,
                                vectorParam,
                                mPredicateName);
    if (dialog.run() == Gtk::RESPONSE_ACCEPT) {
        mPred = dialog.getPred();
// Delete the deleted predicates in the Rule vector
        for (std::map < std::string, strings_t > ::const_iterator it =
                mRule.begin()
                ; it != mRule.end(); ++it) {
            for (strings_t::iterator it2 = mRule[it->first].begin();
                    it2 != it->second.end(); ) {
                if (mPred.find(*it2) == mPred.end() &&
                        find(mPredicateName.begin(), mPredicateName.end(),
                        *it2) == mPredicateName.end()) {
                    it2 = mRule[it->first].erase(it2);
                }
                else {
                    ++it2;
                }
            }
        }
    }
}

void PluginDecision::onUserRule()
{
    RuleDialog *dialog = new RuleDialog(mXml, mRule, mPredicateName, mPred);
    int response = dialog->run();
    if (response == Gtk::RESPONSE_ACCEPT) {
        mRule = dialog->getRule();

        // Delete the deleted rules in the activities
        for (activitiesModel_t::const_iterator it = mDecision->
                activitiesModel().begin(); it != mDecision->
                activitiesModel().end(); ++it) {
            //activity rules
            strings_t mVect = it->second->getRules();

            // Check in the activities rule vector if all rules exist
            for (strings_t::iterator it2 = mVect.begin();
                    it2 != mVect.end(); ) {
                if (mRule.find(*it2) == mRule.end()) {
                    it2 = mVect.erase(it2);
                }
                else {
                    ++it2;
                }
            }
        }
    }
    delete dialog;
}

void PluginDecision::onUserOutputFunction()
{
    OutputFunctionDialog dialog(mXml, mOutputFunctionName, mOutputFunction);
    int state = dialog.run();

    if (state == Gtk::RESPONSE_ACCEPT) {
        mOutputFunctionName = dialog.getOFName();
        mOutputFunction = dialog.getOFFunction();

        // Check for deleted output functions in activities
        for (activitiesModel_t::const_iterator it = mDecision->
                activitiesModel().begin(); it !=
                mDecision->activitiesModel().end(); ++it) {
            if (!it->second->getOutputFunc().empty()) {
                bool find = false;
                for (strings_t::const_iterator it2 =
                        mOutputFunctionName.begin()
                        ; it2 != mOutputFunctionName.end() && find == false;
                        ++it2) {
                    if (it->second->getOutputFunc().at(0) == *it2) {
                        find = true;
                    }
                }
                if (find == false) {
                    it->second->delOutputFunc();
                }
            }
        }
    }
}

void PluginDecision::onUserAckFunction()
{
    AckFunctionDialog dialog(mXml, mAckFunctionName, mAckFunction);
    int state = dialog.run();

    if (state == Gtk::RESPONSE_ACCEPT) {
        mAckFunctionName = dialog.getAckName();
        mAckFunction = dialog.getAckFunction();

        // Check for deleted acknowledge functions in activities
        for (activitiesModel_t::const_iterator it = mDecision->
                activitiesModel().begin(); it !=
                mDecision->activitiesModel().end(); ++it) {
            if (!it->second->getAckFunc().empty()) {
                bool find = false;
                for (strings_t::const_iterator it2 =
                        mAckFunctionName.begin()
                        ; it2 != mAckFunctionName.end() && find == false;
                        ++it2) {
                    if (it->second->getAckFunc().at(0) == *it2) {
                        find = true;
                    }
                }
                if (find == false) {
                    it->second->delAckFunc();
                }
            }
        }
    }
}

std::string PluginDecision::parseFunction(const std::string& buffer,
    const std::string& begin,
    const std::string& end,
    const std::string& name)
{
    boost::regex tagbegin(begin, boost::regex::grep);
    boost::regex tagend(end, boost::regex::grep);

    boost::sregex_iterator it(buffer.begin(), buffer.end(), tagbegin);
    boost::sregex_iterator jt(buffer.begin(), buffer.end(), tagend);
    boost::sregex_iterator itend;

    if (it == itend or jt == itend) {
        throw utils::ArgError(fmt(_("Decision plugin error, " \
                    "no begin or end tag (%1%)")) % name);
    }

    if ((*it)[0].second >= (*jt)[0].first) {
        throw utils::ArgError(fmt(_("Decision plugin error, " \
                    "bad tag (%1%)")) % name);
    }

    return std::string((*it)[0].second + 1, (*jt)[0].first);
}

void PluginDecision::parseFunction(const std::string& buffer,
    const std::string& begin,
    const std::string& end,
    strings_t& names,
    strings_t& buffers)
{
    boost::regex tagbegin(begin, boost::regex::grep);
    boost::regex tagend(end, boost::regex::grep);

    boost::sregex_iterator it(buffer.begin(), buffer.end(), tagbegin);
    boost::sregex_iterator jt(buffer.begin(), buffer.end(), tagend);
    boost::sregex_iterator itend;

    while (it != itend and jt != itend) {
        boost::regex nameend(")", boost::regex::grep);
        boost::sregex_iterator kt((*it)[0].second, (*jt)[0].first, nameend);

        names.push_back(std::string((*it)[0].second, (*kt)[0].first));
        buffers.push_back(std::string((*kt)[0].second + 3, (*jt)[0].first));
        ++it;
        ++jt;
    }
}

void PluginDecision::parseActivities(const strings_t& lst)
{
    std::string activities_lst(lst[3], 2, lst[3].size() - 2);
    strings_t activities;

    boost::trim(activities_lst);
    if ( !activities_lst.empty() ) {
        boost::split(activities, activities_lst, boost::is_any_of("|"));
        for (strings_t::const_iterator it = activities.begin();
             it != activities.end(); ++it) {
            if (not it->empty()) {
                mDecision->addActivityModel(*it);
            }
        }
    }
}

void PluginDecision::parseFacts(const strings_t& lst)
{
    mFactName.clear();

    std::string facts_lst(lst[2], 2, lst[2].size() - 2);
    strings_t facts;

    boost::trim(facts_lst);
    if ( !facts_lst.empty() ) {
        boost::split(facts, facts_lst, boost::is_any_of("|"));
        for (strings_t::const_iterator it = facts.begin();
                it != facts.end(); ++it) {
            if (not it->empty()) {
                mFactName.push_back(*it);
            }
        }
    }
}

void PluginDecision::parsePredicates(const strings_t& lst)
{
    mPredicateName.clear();

    std::string facts_lst(lst[3], 2, lst[3].size() - 2);
    strings_t facts;

    boost::trim(facts_lst);
    if ( !facts_lst.empty() ) {
        boost::split(facts, facts_lst, boost::is_any_of("|"));
        for (strings_t::const_iterator it = facts.begin();
                it != facts.end(); ++it) {
            if (not it->empty()) {
                mPredicateName.push_back(*it);
            }
        }
    }
}

void PluginDecision::parseOutputFunctions(const strings_t& lst)
{
    mOutputFunctionName.clear();

    std::string facts_lst(lst[4], 3, lst[4].size() - 3);
    strings_t facts;

    boost::trim(facts_lst);
    if ( !facts_lst.empty() ) {
        boost::split(facts, facts_lst, boost::is_any_of("|"));
        for (strings_t::const_iterator it = facts.begin();
                it != facts.end(); ++it) {
            if (not it->empty()) {
                mOutputFunctionName.push_back(*it);
            }
        }
    }
}

void PluginDecision::parseAckFunctions(const strings_t& lst)
{
    mAckFunctionName.clear();

    std::string facts_lst(lst[6], 3, lst[6].size() - 3);
    strings_t facts;

    boost::trim(facts_lst);
    if ( !facts_lst.empty() ) {
        boost::split(facts, facts_lst, boost::is_any_of("|"));
        for (strings_t::const_iterator it = facts.begin();
                it != facts.end(); ++it) {
            if (not it->empty()) {
                mAckFunctionName.push_back(*it);
            }
        }
    }
}

void PluginDecision::parseParameters(vpz::AtomicModel& model,
                                     const strings_t& lst,
                                     vpz::Conditions& pConditions)
{
    std::string conditionParametersName((fmt("condDecisionParameters_%1%") %
                                         model.getName()).str());

    std::string rules_lst(lst[7], 3, lst[7].size() - 3);
    strings_t rules;

    boost::trim(rules_lst);
    if ( !rules_lst.empty() ) {
        boost::split(rules, rules_lst, boost::is_any_of("|"));
        for (strings_t::const_iterator it = rules.begin();
                it != rules.end(); ++it) {
            if (!(*it).empty()) {
                strings_t rule;
                boost::split(rule, *it, boost::is_any_of(","));
                mParam[*rule.begin()] = rule.at(2);
            }
        }
    }
    // Update the tagged parameters with the values of the conditions
    if (pConditions.exist(conditionParametersName)) {
        for (vpz::ConditionValues::iterator it = pConditions.get(conditionParametersName).begin();
                it != pConditions.get(conditionParametersName).end();
                ++it) {
            if (mParam.find(it->first) != mParam.end()) {
                mParam.erase(it->first);
                std::ostringstream oss;
                oss << it->second->getDouble(0);
                mParam[it->first] = oss.str();
            }
        }
    }
}

void PluginDecision::parseRulesAndPred(const strings_t& lst)
{
    std::string rules_lst(lst[4], 2, lst[4].size() - 2);
    strings_t rules;

    boost::trim(rules_lst);
    if ( !rules_lst.empty() ) {
        boost::split(rules, rules_lst, boost::is_any_of("|"));
        for (strings_t::const_iterator it = rules.begin();
                it != rules.end(); ++it) {
            if (*it != "") {
                strings_t rule;
                boost::split(rule, *it, boost::is_any_of(","));
                std::vector < std::string > vectPred;
                for (strings_t::const_iterator it2 = (rule.begin()+1);
                        it2 != rule.end(); ++it2) {
                    vectPred.push_back(*it2);
                }
                mRule[*rule.begin()] = vectPred;
            }
        }
    }
}

void PluginDecision::parsePrecedenceConstraints(const strings_t& lst)
{
    std::string constraints_lst(lst[5], 2, lst[5].size() - 2);
    strings_t constraints;

    boost::trim(constraints_lst);
    if ( !constraints_lst.empty() ) {
        boost::split(constraints, constraints_lst, boost::is_any_of("|"));
        for (strings_t::const_iterator it = constraints.begin();
                it != constraints.end(); ++it) {
            if (*it != "") {
                strings_t constraint;
                boost::split(constraint, *it, boost::is_any_of(","));

                points_t points;
                point_t point;
                std::istringstream iss(constraint.at(5));
                iss >> point.x;
                std::istringstream iss2(constraint.at(6));
                iss2 >> point.y;
                points.push_back(point);
                std::istringstream iss3(constraint.at(7));
                iss3 >> point.x;
                std::istringstream iss4(constraint.at(8));
                iss4 >> point.y;
                points.push_back(point);

                mDecision->addPrecedenceConstraint(constraint.at(0),
                        constraint.at(1), constraint.at(2), points,
                        constraint.at(3), constraint.at(4));
            }
        }
    }
}

void PluginDecision::parsePlanFileName(const strings_t& lst)
{
    std::string rules_lst(lst[5], 3, lst[5].size() - 3);
    mPlanFile = rules_lst;
}

void PluginDecision::decodePredicates(std::string& pPred)
{
    strings_t preds;
    boost::trim(pPred);

    if (!pPred.empty()) {
        boost::split(preds, pPred, boost::is_any_of(";"));
        for (strings_t::iterator it = preds.begin(); it != preds.end();
                ++it) {
            if (!(*it).empty()) {
                strings_t parameters;
                boost::split(parameters, *it, boost::is_any_of("|"));
                std::string predName = parameters.at(6) + "";
                mPred[predName].push_back(*it);
            }
        }
    }
}

void PluginDecision::onUserOpenMenu()
{
    Gtk::FileChooserDialog fileChooserDial(*mDialog, "Select a plan file",
            Gtk::FILE_CHOOSER_ACTION_OPEN);

    //Add response buttons the the dialog:
    fileChooserDial.add_button(Gtk::Stock::CANCEL, Gtk::RESPONSE_CANCEL);
    fileChooserDial.add_button("Open", Gtk::RESPONSE_OK);
    fileChooserDial.set_current_folder(
            vle::utils::Package(getCurrPackage()).getDataDir(
                    vle::utils::PKG_SOURCE));
    fileChooserDial.set_local_only(true);
    fileChooserDial.show_all_children();

    // Change the plan file
    if (fileChooserDial.run() == Gtk::RESPONSE_OK) {
        mPlanFileName = fileChooserDial.get_filename();
        mPlanFile = fileChooserDial.get_filename();
        // Convert path in filename
        boost::filesystem::path my_path(mPlanFile);
        mPlanFile = my_path.filename().string();
        if (mPlanFile.find_last_of(".") != std::string::npos) {
            mPlanFile = mPlanFile.erase(mPlanFile.find_last_of("."),
                    mPlanFile.substr(mPlanFile.find_last_of(".")).size());
        }

        mDecision->resetDecision();
        loadPlanFile(fileChooserDial.get_filename());

        mDialog->set_title("Decision - Plugin : " + mPlanFile + ".txt");
    }
}

void PluginDecision::onUserSaveAsMenu()
{
    Gtk::FileChooserDialog fileChooserDial(*mDialog, "Save a plan file as :",
            Gtk::FILE_CHOOSER_ACTION_SAVE);

    //Add response buttons the the dialog:
    fileChooserDial.add_button(Gtk::Stock::CANCEL, Gtk::RESPONSE_CANCEL);
    fileChooserDial.add_button("Save", Gtk::RESPONSE_OK);
    fileChooserDial.set_current_folder(
            vle::utils::Package(getCurrPackage()).getDataDir(
                    vle::utils::PKG_SOURCE));
    fileChooserDial.set_do_overwrite_confirmation(true);
    fileChooserDial.set_local_only(true);
    fileChooserDial.set_current_name(mPlanFile + ".txt");

    fileChooserDial.show_all_children();

    // Change the plan file
    if (fileChooserDial.run() == Gtk::RESPONSE_OK) {
        writePlanFile(fileChooserDial.get_filename());
        mPlanFileName = fileChooserDial.get_filename();
        mPlanFile = fileChooserDial.get_filename();

        // Convert path in filename
        boost::filesystem::path my_path(mPlanFile);
        mPlanFile = my_path.filename().string();
        if (mPlanFile.find_last_of(".") != std::string::npos) {
            mPlanFile = mPlanFile.erase(mPlanFile.find_last_of("."),
                    mPlanFile.substr(mPlanFile.find_last_of(".")).size());
        }

        mDialog->set_title("Decision - Plugin : " + mPlanFile + ".txt");
    }
}

void PluginDecision::onChooseFileName()
{
    SimpleTypeBox box(("Plan file name (without extension)?"), "");
    int w, h;
    int &width = w;
    int &height = h;
    box.get_size(width, height);
    box.resize(400, h);
    std::string name = boost::trim_copy(box.run());
    if (box.valid() and !name.empty()) {
        mPlanFileName =
            vle::utils::Package(getCurrPackage()).getDataDir(
                    vle::utils::PKG_SOURCE) +
            "/" + name + ".txt";
        mPlanFile = name;
        mDialog->set_title("Decision - Plugin : " + mPlanFile + ".txt");
    }
}

void PluginDecision::clear()
{
    mInclude.clear();
    mCustomConstructor.clear();
    mMembers.clear();
    mConstructor.clear();
    mDefinition.clear();

    mFactName.clear();
    mFactFunction.clear();
    mFactAttribute.clear();
    mFactInit.clear();

    mPredicateName.clear();
    mPredicateFunction.clear();

    mRule.clear();

    mOutputFunctionName.clear();
    mOutputFunction.clear();

    mAckFunctionName.clear();
    mAckFunction.clear();

    mParam.clear();

    mPred.clear();
}

}
}
}
}    // namespace vle gvle modeling decision

DECLARE_GVLE_MODELINGPLUGIN(vle::gvle::modeling::decision::PluginDecision)
