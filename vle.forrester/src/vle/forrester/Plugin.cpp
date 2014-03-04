/*
 * @file vle/forrester/Plugin.cpp
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


#include <vle/forrester/Plugin.hpp>
#include <vle/forrester/dialogs/SourceDialog.hpp>
#include <vle/forrester/dialogs/TimeStepDialog.hpp>
#include <vle/utils/Template.hpp>
#include <vle/utils/Package.hpp>
#include <vle/gvle/Message.hpp>
#include <gtkmm/radioaction.h>
#include <gtkmm/stock.h>
#include <boost/regex.hpp>
#include <boost/algorithm/string/trim.hpp>
#include <boost/algorithm/string/split.hpp>
#include <boost/algorithm/string/classification.hpp>
#include <boost/lexical_cast.hpp>
#include <vle/value/String.hpp>
#include <vle/value/Map.hpp>
#include <vle/vpz/Condition.hpp>
#include <vle/forrester/graphicalItems/Parameter.hpp>
#include <vle/forrester/graphicalItems/Flow.hpp>
#include <vle/forrester/graphicalItems/DependencyArrow.hpp>
#include <vle/forrester/graphicalItems/ExogenousVariable.hpp>
#include <vle/forrester/graphicalItems/Variable.hpp>

#include <vle/forrester/utilities/Tools.hpp>

namespace vle {
namespace gvle {
namespace modeling {
namespace forrester {

const std::string PluginForrester::TEMPLATE_DEFINITION =
    "/**\n"                                                             \
    "  * @file {{classname}}.cpp\n"                                     \
    "  * @author ...\n"                                                 \
    "  * ...\n"                                                         \
    "  *@@tagdynamic@@\n"                                               \
    "  *@@tagdepends:vle.extension.differential-equation@@endtagdepends\n"\
    "  * @@tag Forrester@vle.forrester "                                \
    "@@namespace:{{namespace}};"                                        \
    "class:{{classname}};{{width}}|{{height}};"                         \
    "c:{{for i in compartments}}"                                       \
    "{{compartments^i}}|"                                               \
    "{{end for}};"                                                      \
    "f:{{for i in flows}}"                                              \
    "{{flows^i}}|"                                                      \
    "{{end for}};"                                                      \
    "fa:{{for i in flowsArrows}}"                                       \
    "{{flowsArrows^i}}|"                                                \
    "{{end for}};"                                                      \
    "p:{{for i in parameters}}"                                         \
    "{{parameters^i}}|"                                                 \
    "{{end for}};"                                                      \
    "pa:{{for i in parametersArrow}}"                                   \
    "{{parametersArrow^i}}|"                                            \
    "{{end for}};"                                                      \
    "ev:{{for i in exogenousVariables}}"                                \
    "{{exogenousVariables^i}}|"                                         \
    "{{end for}};"                                                      \
    "v:{{for i in Variables}}"                                          \
    "{{Variables^i}}|"                                                  \
    "{{end for}};"                                                      \
    "@@end tag@@\n"                                                     \
    "  */\n\n"                                                          \
    "#include <vle/extension/differential-equation"                      \
    "/DifferentialEquation.hpp>\n\n"                                    \
    "//@@begin:include@@\n"                                             \
    "{{include}}\n"                                                     \
    "//@@end:include@@\n"                                               \
    "namespace vd = vle::devs;\n"                                       \
    "namespace ve = vle::extension::differential_equation;\n"            \
    "namespace vv = vle::value;\n\n"                                    \
    "namespace {{namespace}} {\n\n"                                     \
    "class {{classname}} : public ve::DifferentialEquation\n"           \
    "{\n"                                                               \
    "public:\n"                                                         \
    "    {{classname}}(\n"                                              \
    "       const vd::DynamicsInit& init,\n"                            \
    "       const vd::InitEventList& events)\n"                         \
    "    : ve::DifferentialEquation(init, events)\n"                    \
    "    {\n"                                                           \
    "{{for i in createVar}}"                                            \
    "        {{createVar^i}}\n"                                         \
    "{{end for}}\n"                                                     \
    "{{for i in createVarExo}}"                                         \
    "        {{createVarExo^i}}\n"                                      \
    "{{end for}}\n"                                                     \
    "{{for i in parametersInitialization}}"                             \
    "        {{parametersInitialization^i}}\n\n"                        \
    "{{end for}}"                                                       \
    "    }\n\n"                                                         \
    "    virtual void compute(const vd::Time& t)\n"                     \
    "    {\n"                                                           \
    "{{for i in variableCompute}} "                                     \
    "       {{variableCompute^i}}\n"                                    \
    "{{end for}}"                                                       \
    "{{for i in conditionalFlowVariableCompute}} "                      \
    "       {{conditionalFlowVariableCompute^i}}\n"                     \
    "{{end for}}"                                                       \
    "{{for i in FlowVariableCompute}} "                                 \
    "       {{FlowVariableCompute^i}}\n"                                \
    "{{end for}}"                                                       \
    "{{for i in equations}} "                                           \
    "       {{equations^i}}\n"                                          \
    "{{end for}}"                                                       \
    "    }\n\n"                                                         \
    "    virtual ~{{classname}}()\n"                                    \
    "    { }\n\n"                                                       \
    "private:\n"                                                        \
    "{{for i in definitionVar}}"                                        \
    "    {{definitionVar^i}}\n"                                         \
    "{{end for}}\n"                                                     \
    "{{for i in definitionParameters}}"                                 \
    "    {{definitionParameters^i}}\n"                                  \
    "{{end for}}"                                                       \
    "{{for i in definitionExoVariables}}"                               \
    "    {{definitionExoVariables^i}}\n"                                \
    "{{end for}}"                                                       \
    "{{for i in variableDeclaration}}"                                  \
    "    {{variableDeclaration^i}}\n"                                   \
    "{{end for}}"                                                       \
    "{{for i in FlowVariableDeclaration}}"                              \
    "    {{FlowVariableDeclaration^i}}\n"                               \
    "{{end for}}"                                                       \
    "{{for i in conditionalFlowVariableDeclaration}}"                   \
    "    {{conditionalFlowVariableDeclaration^i}}\n"                    \
    "{{end for}}"                                                       \
    "virtual vv::Value* observation(\n"                                 \
    "    const vd::ObservationEvent& event) const\n"                    \
    "{\n"                                                               \
    "   const std::string& port = event.getPortName();\n"               \
    "{{for i in flowObservation}}"                                      \
    "    {{flowObservation^i}}\n"                                       \
    "{{end for}}"                                                       \
    "{{for i in variableObservation}}"                                  \
    "    {{variableObservation^i}}\n"                                   \
    "{{end for}}"                                                       \
    "   return ve::DifferentialEquation::observation(event);\n"         \
    "}\n"                                                               \
    "};\n\n"                                                            \
    "} // namespace {{namespace}}\n\n"                                  \
    "DECLARE_DYNAMICS({{namespace}}::{{classname}})\n\n";

const Glib::ustring PluginForrester::UI_DEFINITION =
    "<ui>"
    "    <toolbar name='Toolbar'>"
    "        <toolitem action='Select' />"
    "        <toolitem action='AddCompartment' />"
    "        <toolitem action='AddFlow' />"
    "        <toolitem action='AddParameter' />"
    "        <toolitem action='AddExogenousVariable' />"
    "        <toolitem action='AddVariable' />"
    "        <toolitem action='AddFlowArrow' />"
    "        <toolitem action='AddDependencyArrow' />"
    "        <toolitem action='Delete' />"
    "    </toolbar>"
    "</ui>";

PluginForrester::PluginForrester(const std::string& package,
                                 const std::string& library,
                                 const std::string& curr_package)
    : ModelingPlugin(package, library, curr_package),
      mDialog(0),
      mForrester(0),
      mTimeStep(0.001)
{
    vle::utils::Package pack(package);
    std::string glade = pack.getPluginGvleModelingFile(
            "Forrester.glade", vle::utils::PKG_BINARY);
    mXml = Gtk::Builder::create();
    mXml->add_from_file(glade.c_str());
    mXml->get_widget("ForresterDialog", mDialog);
    mDialog->set_title("Forrester Dialog");
    mXml->get_widget("IncludeSourceButton", mIncludeButton);
    mXml->get_widget("TimeStepButton", mTimeStepButton);
    mXml->get_widget("eulerButton", mEulerButton);
    mXml->get_widget("rk4Button", mRk4Button);
    mXml->get_widget("qss2Button", mQss2Button);
    mXml->get_widget_derived("ForresterDrawingArea", mView);
    mList.push_back(mIncludeButton->signal_clicked().connect(
                        sigc::mem_fun(*this,
                                      &PluginForrester::onIncludeSource)));
    mList.push_back(mTimeStepButton->signal_clicked().connect(
                        sigc::mem_fun(*this,
                                      &PluginForrester::onTimeStep)));
    mList.push_back(mQss2Button->signal_clicked().connect(
                        sigc::mem_fun(*this,
                                      &PluginForrester::onQss2)));
    mList.push_back(mEulerButton->signal_clicked().connect(
                        sigc::mem_fun(*this,
                                      &PluginForrester::onEuler)));
    mList.push_back(mRk4Button->signal_clicked().connect(
                        sigc::mem_fun(*this,
                                      &PluginForrester::onRk4)));
    {
        Gtk::Box* hbox;
        mXml->get_widget("ForresterHBox", hbox);
        mActionGroup = Gtk::ActionGroup::create();
        mUIManager = Gtk::UIManager::create();
        createActions();
        mUIManager->insert_action_group(mActionGroup);
        mDialog->add_accel_group(mUIManager->get_accel_group());
        createUI();
        mToolbar = dynamic_cast < Gtk::Toolbar* >(
                       mUIManager->get_widget("/Toolbar"));
        //mToolbar->set_orientation(Gtk::ORIENTATION_VERTICAL); Not in GTK3
        mToolbar->set_property("orientation", Gtk::ORIENTATION_VERTICAL);
        hbox->pack_start(*mToolbar, false, false);
    }
}

PluginForrester::~PluginForrester()
{
    destroy();

    for (std::list < sigc::connection >::iterator it = mList.begin();
    it != mList.end(); ++it) {
        it->disconnect();
    }

    delete mForrester;
    delete mView;
}
void PluginForrester::destroy()
{
    for (std::list < sigc::connection >::iterator it = mList.begin();
    it != mList.end(); ++it) {
        it->disconnect();
    }
}

void PluginForrester::onIncludeSource()
{
    SourceDialog dialog(mXml);
    dialog.add("Include", mInclude);

    if (dialog.run() == Gtk::RESPONSE_ACCEPT) {
        std::string includes = dialog.buffer("Include");
        boost::trim(includes);
        mInclude.assign(includes);
    }
}

bool PluginForrester::create(vpz::AtomicModel& model,
                             vpz::Dynamic& dynamic,
                             vpz::Conditions& conditions,
                             vpz::Observables& observables,
                             const std::string& classname,
                             const std::string& namespace_)
{
    std::string conditionName((fmt("cond_DTE_%1%") % model.getName()).str());
    mForrester = new Forrester(classname);
    mView->setForrester(mForrester);
    //create conditions
    // if (not conditions.exist(conditionName))
    vpz::Condition condition(conditionName);
    mForrester->integrationScheme(Forrester::EULER);

    if (mDialog->run() == Gtk::RESPONSE_ACCEPT) {
        generateSource(classname, namespace_);
        generateCondition(conditions, model);
        generateExperimentalConditions(conditions, model);
        generateObservable(model,observables);
        generatePorts(model);
        model.setDynamics(dynamic.name());
        mDialog->hide();
        destroy();
        return true;
    } else {
        mDialog->hide();
        destroy();
        return false;
    }
}

void PluginForrester::generateSource(const std::string& classname,
                                     const std::string& namespace_)
{
    boost::algorithm::trim(mInclude);
    utils::Template tpl_(TEMPLATE_DEFINITION);
    tpl_.stringSymbol().append("namespace", namespace_);
    tpl_.stringSymbol().append("classname", classname);
    tpl_.stringSymbol().append("include", mInclude);
    mForrester->getForresterGI().generateSource(tpl_);

    for (graphical_items::const_iterator it = mForrester->getItems().begin();
    it != mForrester->getItems().end(); ++it) {
        if (Compartment* item = dynamic_cast<Compartment*>(*it))
            tpl_.listSymbol().append(
                "equations", "grad(" + item->getName() + ") = " +
                generateEquation(item) + ";");
        (*it)->generateSource(tpl_);
    }

    for (arrows::const_iterator it = mForrester->getArrows().begin();
    it != mForrester->getArrows().end(); ++it) {
        (*it)->generateSource(tpl_);
    }
    tpl_.listSymbol().append("createVarExo");
    tpl_.listSymbol().append("compartments");
    tpl_.listSymbol().append("flows");
    tpl_.listSymbol().append("flowsArrows");
    tpl_.listSymbol().append("parameters");
    tpl_.listSymbol().append("parametersArrow");
    tpl_.listSymbol().append("exogenousVariables");
    tpl_.listSymbol().append("createVar");
    tpl_.listSymbol().append("parametersInitialization");
    tpl_.listSymbol().append("equations");
    tpl_.listSymbol().append("definitionVar");
    tpl_.listSymbol().append("definitionParameters");
    tpl_.listSymbol().append("definitionExoVariables");
    tpl_.listSymbol().append("FlowVariableDeclaration");
    tpl_.listSymbol().append("conditionalFlowVariableDeclaration");
    tpl_.listSymbol().append("conditionalFlowVariableCompute");
    tpl_.listSymbol().append("FlowVariableCompute");
    tpl_.listSymbol().append("Variables");
    tpl_.listSymbol().append("variableCompute");
    tpl_.listSymbol().append("variableDeclaration");
    tpl_.listSymbol().append("flowObservation");
    tpl_.listSymbol().append("variableObservation");

    std::ostringstream out;
    tpl_.process(out);
    mSource = out.str();
}

std::string PluginForrester::generateEquation(const Compartment * comp)
{
    std::string positive, negative, final;

    for (arrows::const_iterator it = mForrester->getArrows().begin();
    it != mForrester->getArrows().end(); ++it) {
        if(dynamic_cast<FlowArrow*>(*it)) {
            if ((*it)->getOrigin()->getOwner() == comp) {
                Flow* item = dynamic_cast<Flow*>(
                    (*it)->getDestination()->getOwner());
                std::string finalExpression(item->getValue());
                utilities::generateParenthesis(finalExpression, *mForrester);
                utilities::generateStdPrefix(finalExpression);

                if (negative != "")
                    negative += " + ";

                negative += item->getName();
            }

            if ((*it)->getDestination()->getOwner() == comp) {
                Flow* item = dynamic_cast<Flow*>(
                    (*it)->getOrigin()->getOwner());
                std::string finalExpression(item->getValue());
                utilities::generateParenthesis(finalExpression, *mForrester);
                utilities::generateStdPrefix(finalExpression);

                if (positive != "")
                    positive += " + ";

                positive += item->getName();
            }
        }
    }

    if (positive == "" && negative == "") {
        final = "0";
    } else {
        if (positive != "") {
            final = "(" + positive + ")";
        } else {
            final = "0";
        }

        if (negative != "") {
            final += " - (" + negative + ")";
        }
    }
    return final;
}


bool PluginForrester::modify(vpz::AtomicModel& model,
                             vpz::Dynamic& dynamic,
                             vpz::Conditions& conditions,
                             vpz::Observables& observables,
                             const std::string& conf,
                             const std::string& buffer)
{
    std::string namespace_;
    std::string classname;
    strings_t lst;
    //Class name & namespace
    boost::split(lst, conf, boost::is_any_of(";"));
    parseConf(lst, classname, namespace_);
    mForrester = new Forrester(classname);
    //Width & Heigth
    strings_t wh;
    boost::split(wh, lst[2], boost::is_any_of("|"));
    mForrester->getForresterGI().setWidth(
        boost::lexical_cast < unsigned int > (wh[0]));
    mForrester->getForresterGI().setHeight(
        boost::lexical_cast < unsigned int > (wh[1]));

    loadConditions(lst[3]);
    loadFlows(lst[4]);
    loadParameters(lst[6]);
    loadExogenousVariables(lst[8]);
    loadVariables(lst[9]);
    loadDependencyArrow(lst[7]);
    loadFlowsArrow(lst[5]);

    //Includes
    mInclude.assign(parseFunction(buffer, "//@@begin:include@@",
                                  "//@@end:include@@", "include"));
    //Condition
    setIntegrationModeFromCondition(conditions,
        (fmt("cond_DTE_%1%_integration") % model.getName()).str());

    setIntegrationTimeStep(conditions,
        (fmt("cond_DTE_%1%_integration") % model.getName()).str());
    loadInitialValues(conditions, generateConditionName(model));
    mView->setForrester(mForrester);
    if (mDialog->run() == Gtk::RESPONSE_ACCEPT) {
        generateSource(classname, namespace_);
        generateCondition(conditions, model);
        generateExperimentalConditions(conditions, model);
        generateObservable(model,observables);
        generatePorts(model);
        model.setDynamics(dynamic.name());
        mDialog->hide();
        destroy();
        return true;
    } else {
        mDialog->hide();
        destroy();
        return false;
    }

    return true;
}

void PluginForrester::parseConf(const strings_t& lst,
                                std::string& classname,
                                std::string& namespace_)
{
    namespace_ = std::string(lst[0], 10, lst[0].size() - 10);
    classname = std::string(lst[1], 6, lst[1].size() - 6);
}

std::string PluginForrester::parseFunction(const std::string& buffer,
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
        throw utils::ArgError(fmt(_("Forrester plugin error, " \
                                    "no begin or end tag (%1%)")) % name);
    }

    if ((*it) [0].second >= (*jt) [0].first) {
        throw utils::ArgError(fmt(_("Forrester plugin error, " \
                                    "bad tag (%1%)")) % name);
    }

    return std::string((*it) [0].second + 1, (*jt) [0].first);
}

void PluginForrester::createActions()
{
    Gtk::RadioAction::Group toolsGroup;
    mActionGroup->add(
        Gtk::RadioAction::create(toolsGroup, "Select", Gtk::Stock::INDEX,
                                 _("Select"),
                                 _("Select state or transition (F1)")),
        Gtk::AccelKey("F1"), sigc::mem_fun(this, &PluginForrester::onSelect));

    mActionGroup->add(
        Gtk::RadioAction::create(toolsGroup, "AddCompartment", Gtk::Stock::ADD,
                                 _("Compartment"),
                                 _("Add a compartment (F2)")),
        Gtk::AccelKey("F2"),
        sigc::mem_fun(this, &PluginForrester::onAddCompartment));

    mActionGroup->add(
        Gtk::RadioAction::create(toolsGroup, "AddFlow", Gtk::Stock::ADD,
                                 _("Material Flow"),
                                 _("Add a material flow (F3)")),
        Gtk::AccelKey("F3"),
        sigc::mem_fun(this, &PluginForrester::onAddFlow));

    mActionGroup->add(
        Gtk::RadioAction::create(toolsGroup, "AddParameter", Gtk::Stock::ADD,
                                 _("Parameter"),
                                 _("Add a parameter (F4)")),
        Gtk::AccelKey("F4"),
        sigc::mem_fun(this, &PluginForrester::onAddParameter));
    mActionGroup->add(
        Gtk::RadioAction::create(toolsGroup, "AddExogenousVariable", Gtk::Stock::ADD,
                                 _("Exogenous variable"),
                                 _("Add a exogenous variable (F9)")),
        Gtk::AccelKey("F9"),
        sigc::mem_fun(this, &PluginForrester::onAddExogenousVariable));

    mActionGroup->add(
        Gtk::RadioAction::create(toolsGroup, "AddVariable", Gtk::Stock::ADD,
                                 _("Auxiliary Variable"),
                                 _("Add a auxiliary variable (F10)")),
        Gtk::AccelKey("F10"),
        sigc::mem_fun(this, &PluginForrester::onAddVariable));

    mActionGroup->add(
        Gtk::RadioAction::create(toolsGroup,
                                 "AddFlowArrow", Gtk::Stock::DISCONNECT,
                                 _("Hardware link"),
                                 _("Add a harware link (F6)")),
        Gtk::AccelKey("F6"),
        sigc::mem_fun(this, &PluginForrester::onAddFlowArrow));
    mActionGroup->add(
        Gtk::RadioAction::create(toolsGroup,
                                 "AddDependencyArrow", Gtk::Stock::DISCONNECT,
                                 _("Information flow"),
                                 _("Add a information flow (F7)")),
        Gtk::AccelKey("F7"),
        sigc::mem_fun(this, &PluginForrester::onAddParameterArrow));

    mActionGroup->add(
        Gtk::RadioAction::create(toolsGroup, "Delete", Gtk::Stock::DELETE,
                                 _("Remove"),
                                 _("Remove a item (F8)")),
        Gtk::AccelKey("F8"),
        sigc::mem_fun(this, &PluginForrester::onRemoveCompartment));

}

void PluginForrester::createUI()
{
#ifdef GLIBMM_EXCEPTIONS_ENABLED

    try
    {
        mUIManager->add_ui_from_string(UI_DEFINITION);
    }
    catch (const Glib::Error& ex)
    {
        throw utils::InternalError(fmt(
            _("Forrester modeling plugin building menus failed: %1%")) %
            ex.what());
    }

#else
    std::auto_ptr < Glib::Error > ex;
    mUIManager->add_ui_from_string(UI_DEFINITION, ex);

    if (ex.get())
    {
        throw utils::InternalError(fmt(
            _("Forrester modeling plugin building menus failed: %1%")) %
            ex->what());
    }

#endif
}

bool PluginForrester::start(vpz::Condition& /*condition*/)
{
    return true;
}

void PluginForrester::onAddFlowArrow()
{
    mView->setState(ForresterDrawingArea::ADD_FLOW_ARROW);
}

void PluginForrester::onSelect()
{
    mView->setState(ForresterDrawingArea::SELECT);
}

void PluginForrester::onAddCompartment()
{
    mView->setState(ForresterDrawingArea::ADD_COMPARTMENT);
}

void PluginForrester::onRemoveCompartment()
{
    mView->setState(ForresterDrawingArea::DELETE);
}

void PluginForrester::onAddFlow()
{
    mView->setState(ForresterDrawingArea::ADD_FLOW);
}

void PluginForrester::onAddParameter()
{
    mView->setState(ForresterDrawingArea::ADD_PARAMETER);
}

void PluginForrester::onAddParameterArrow()
{
    mView->setState(ForresterDrawingArea::ADD_DEPENDENCY_ARROW);
}

void PluginForrester::onAddExogenousVariable()
{
    mView->setState(ForresterDrawingArea::ADD_EXOGENOUS_VARIABLE);
}

void PluginForrester::onAddVariable() {
    mView->setState(ForresterDrawingArea::ADD_VARIABLE);
}


void PluginForrester::onTimeStep()
{
    TimeStepDialog dialog(mXml);

    if (dialog.run(mTimeStep) == Gtk::RESPONSE_ACCEPT) {
        mTimeStep = dialog.timeStep();
    }
}

void PluginForrester::generatePorts(vpz::AtomicModel& model)
{
    std::vector<std::string> conditions;

    for (vpz::ConnectionList::const_iterator it =
    model.getOutputPortList().begin();
    it != model.getOutputPortList().end(); ++it) {
       conditions.push_back(it->first);
    }

    for (std::vector<std::string>::const_iterator it = conditions.begin();
    it != conditions.end(); ++it) {
        model.delOutputPort(*it);
    }
    conditions.clear();

    for (vpz::ConnectionList::const_iterator it =
    model.getInputPortList().begin();
    it != model.getInputPortList().end(); ++it) {
       conditions.push_back(it->first);
    }

    for (std::vector<std::string>::const_iterator it = conditions.begin();
    it != conditions.end(); ++it) {
        model.delInputPort(*it);
    }

    for (graphical_items::const_iterator it = mForrester->getItems().begin();
    it != mForrester->getItems().end(); ++it) {
            (*it)->generatePorts(model);
    }

    if (not model.existInputPort("perturb")) {
        model.addInputPort("perturb");
    }
}

void PluginForrester::generateCondition(vle::vpz::Conditions& conditions,
                                        vpz::AtomicModel& model)
{
    vle::value::String method;
    vle::value::Map methodParameters;

    if (mForrester->integrationScheme() == Forrester::QSS2) {
        method.set("qss2");
        vle::value::Map deltaQ;

        for (graphical_items::const_iterator it =
        mForrester->getItems().begin();
        it != mForrester->getItems().end(); ++it) {
            if (Compartment* item = dynamic_cast<Compartment*>(*it))
                deltaQ.add(item->getName(),
                    vle::value::Double(item->getTimeStep()));
        }

        methodParameters.add("DeltaQ", deltaQ);
    } else {
        if (mForrester->integrationScheme() == Forrester::EULER)
            method.set("euler");
        else if (mForrester->integrationScheme() == Forrester::RK4)
            method.set("rk4");

        methodParameters.add("timestep",
                             vle::value::Double(mTimeStep));
    }

    std::string conditionName(
        (fmt("cond_DTE_%1%_integration") % model.getName()).str());

    if (conditions.exist(conditionName)) {
        vpz::Condition& condition(conditions.get(conditionName));
        condition.del("method");
        condition.del("method-parameters");
        condition.addValueToPort("method", method);
        condition.addValueToPort("method-parameters", methodParameters);
    } else {
        vpz::Condition condition(conditionName);
        condition.addValueToPort("method", method);
        condition.addValueToPort("method-parameters", methodParameters);
        conditions.add(condition);
    }

    std::vector < std::string > cond(model.conditions());

    if (std::find(cond.begin(), cond.end(), conditionName) == cond.end()) {
        cond.push_back(conditionName);
        model.setConditions(cond);
    }

//Creating parameters
    std::string parametersConditionName(
        (fmt("cond_DTE_%1%") % model.getName()).str());

    if (conditions.exist(parametersConditionName)) {
        conditions.del(parametersConditionName);
    }

    vpz::Condition conditionParameter(parametersConditionName);
    for (graphical_items::const_iterator it =
    mForrester->getItems().begin();
    it != mForrester->getItems().end(); ++it) {
        if (Parameter* item = dynamic_cast<Parameter*>(*it))
        conditionParameter.addValueToPort(item->getName(),
            vle::value::Double(item->getValue()));
    }
    conditions.add(conditionParameter);

    cond = model.conditions();

    if (std::find(cond.begin(), cond.end(), parametersConditionName)
        == cond.end()) {
        cond.push_back(parametersConditionName);
        model.setConditions(cond);
    }
}

void PluginForrester::generateExperimentalConditions(
    vle::vpz::Conditions& conditions, vpz::AtomicModel& model)
{
    vle::value::Map variables;

    for (graphical_items::const_iterator it = mForrester->getItems().begin();
         it != mForrester->getItems().end(); ++it) {
        if (Compartment* item = dynamic_cast<Compartment*>(*it))
            variables.add(item->getName(),
                          vle::value::Double(item->getInitialValue()));
    }

    std::string conditionName(generateConditionName(model));

    vpz::Condition& condition(conditions.get(conditionName));
    condition.del("variables");
    condition.addValueToPort("variables", variables);
    for (graphical_items::const_iterator it =
             mForrester->getItems().begin();
         it != mForrester->getItems().end(); ++it) {
        if (Compartment* item = dynamic_cast<Compartment*>(*it))
            condition.addValueToPort(item->getName(),
                                     vle::value::Double(item->getInitialValue()));
    }

    std::vector < std::string > cond(model.conditions());

    if (std::find(cond.begin(), cond.end(), conditionName) == cond.end()) {
        cond.push_back(conditionName);
        model.setConditions(cond);
    }
}

void PluginForrester::onQss2()
{
    mTimeStepButton->set_sensitive(false);
    mForrester->integrationScheme(Forrester::QSS2);
}

void PluginForrester::onEuler()
{
    mTimeStepButton->set_sensitive(true);
    mForrester->integrationScheme(Forrester::EULER);
}

void PluginForrester::onRk4()
{
    mTimeStepButton->set_sensitive(true);
    mForrester->integrationScheme(Forrester::RK4);
}

void PluginForrester::setIntegrationModeFromCondition(
    const vpz::Conditions& conditions,
    std::string conditionName)
{
    if (conditions.exist(conditionName)) {
        std::string method = conditions.get(conditionName).firstValue("method")
                             .toString().value();

        if (method == "euler") {
            mEulerButton->set_active(true);
            mForrester->integrationScheme(Forrester::EULER);
        } else if (method == "rk4") {
            mRk4Button->set_active(true);
            mForrester->integrationScheme(Forrester::RK4);
        } else if (method == "qss2") {
            mQss2Button->set_active(true);
            mForrester->integrationScheme(Forrester::QSS2);
        }
    } else {
        throw vle::utils::ModellingError("error condition not found");
    }
}

void PluginForrester::setIntegrationTimeStep(const vpz::Conditions& conditions,
                                             std::string conditionName)
{
    if (!conditions.exist(conditionName))
        throw vle::utils::ModellingError("error condition not found");

    vle::vpz::Condition mCondition(conditions.get(conditionName));
    vle::value::Map ports = mCondition.firstValue("method-parameters").toMap();

    if (mForrester->integrationScheme() != Forrester::QSS2) {
        mTimeStep = ports.get("timestep")->toDouble().value();
    } else {
        vle::value::Map variablesMap = ports.getMap("DeltaQ");

        for (graphical_items::const_iterator it =
        mForrester->getItems().begin();
        it != mForrester->getItems().end(); ++it) {
            if(Compartment* item = dynamic_cast<Compartment*>(*it)) {
                if (variablesMap.exist(item->getName())) {
                    item->setTimeStep(variablesMap.getDouble(item->getName()));
                } else {
                    throw vle::utils::ModellingError(
                        vle::fmt("[%1%] Time step "
                                 "of variable '%2' missing")
                        % conditionName % item->getName());
                }
            }
        }
    }
}

void PluginForrester::loadInitialValues(const vpz::Conditions& conditions,
                                        std::string conditionName)
{
    if (!conditions.exist(conditionName))
        throw vle::utils::ModellingError("error condition not found");

    vle::vpz::Condition mCondition(conditions.get(conditionName));
    vle::value::Map variables = mCondition.firstValue("variables").toMap();

    for (graphical_items::const_iterator it =
    mForrester->getItems().begin();
    it != mForrester->getItems().end(); ++it) {
        if (Compartment* item = dynamic_cast<Compartment*>(*it)) {
            if (variables.exist(item->getName())) {
                item->setInitialValue(variables.getDouble(
                                             item->getName()));
            } else {
                throw vle::utils::ModellingError(
                    vle::fmt("[%1%] Initilization value "
                             "of variable '%2' missing")
                    % conditionName % item->getName());
            }
        }

        if (Parameter* item = dynamic_cast<Parameter*>(*it)) {
            std::list< std::string > portName;
            mCondition.portnames(portName);

            if(std::find(portName.begin(), portName.end(),
            item->getName())!=portName.end())
                item->setValue(mCondition.firstValue(item->getName())
                    .toDouble().value());
        }
    }
}

void PluginForrester::loadConditions(const std::string& conf)
{
    strings_t compartments;
    std::string compartments_lst(conf, 2, conf.size() - 2);
    boost::split(compartments, compartments_lst, boost::is_any_of("|"));

    for (strings_t::const_iterator it = compartments.begin();
    it != compartments.end(); ++it) {
        if (not it->empty()) {
            mForrester->add(new Compartment(*it,*mForrester));
        }
    }
}

void PluginForrester::loadExogenousVariables(const std::string& conf)
{
    strings_t exos;
    std::string exos_lst(conf, 3, conf.size() - 3);
    boost::split(exos, exos_lst, boost::is_any_of("|"));

    for (strings_t::const_iterator it = exos.begin();
    it != exos.end(); ++it) {
        if (not it->empty()) {
            mForrester->add(new ExogenousVariable(*it,*mForrester));
        }
    }
}

void PluginForrester::loadFlows(const std::string& conf)
{
    strings_t flows;
    std::string flows_list(conf, 2, conf.size() - 2);
    boost::split(flows, flows_list, boost::is_any_of("|"));

    for (strings_t::const_iterator it = flows.begin();
    it != flows.end(); ++it) {
        if (not it->empty()) {
            mForrester->add(new Flow(*it,*mForrester));
        }
    }
}

void PluginForrester::loadParameters(const std::string& conf)
{
    strings_t parameters_list;
    std::string parameters(conf, 2, conf.size() - 2);
    boost::split(parameters_list, parameters, boost::is_any_of("|"));
    for (strings_t::const_iterator it = parameters_list.begin();
    it != parameters_list.end(); ++it) {
        if (not it->empty()) {
            mForrester->add(new Parameter(*it,*mForrester));
        }
    }
}

void PluginForrester::loadDependencyArrow(const std::string& conf)
{
    strings_t parameterArrows_list;
    std::string parameterArrows(conf, 3, conf.size() - 3);
    boost::split(parameterArrows_list, parameterArrows, boost::is_any_of("|"));
    for (strings_t::const_iterator it = parameterArrows_list.begin();
    it != parameterArrows_list.end(); ++it) {
        if (not it->empty()) {
            GraphicalItem *ori, *dest;
            std::vector<std::string> parameterArrow;
            boost::split(parameterArrow,*it,boost::is_any_of(","));
            int originIndex = boost::lexical_cast<int>(parameterArrow[2]);
            int destinIndex = boost::lexical_cast<int>(parameterArrow[3]);
            int cPointX = boost::lexical_cast<int>(parameterArrow[4]);
            int cPointY = boost::lexical_cast<int>(parameterArrow[5]);
            ori = mForrester->getItem(parameterArrow[0]);
            dest = mForrester->getItem(parameterArrow[1]);
            mForrester->add(new DependencyArrow(
                ori->getAnchor(originIndex),dest->getAnchor(destinIndex),
                cPointX,cPointY));
        }
    }
}


void PluginForrester::loadFlowsArrow(const std::string& conf)
{
    strings_t flowsArrows_list;
    std::string flowsArrows(conf, 3, conf.size() - 3);
    boost::split(flowsArrows_list, flowsArrows, boost::is_any_of("|"));
    for (strings_t::const_iterator it = flowsArrows_list.begin();
    it != flowsArrows_list.end(); ++it) {
        if (not it->empty()) {
            GraphicalItem *ori, *dest;
            std::vector<std::string> flowArrow;
            boost::split(flowArrow,*it,boost::is_any_of(","));
            int originIndex = boost::lexical_cast<int>(flowArrow[2]);
            int destinIndex = boost::lexical_cast<int>(flowArrow[3]);
            ori = mForrester->getItem(flowArrow[0]);
            dest = mForrester->getItem(flowArrow[1]);
            mForrester->add(new FlowArrow(
                ori->getAnchor(originIndex),dest->getAnchor(destinIndex)));
        }
    }
}

void PluginForrester::loadVariables(const std::string& conf) {
    strings_t variables_list;
    std::string variables(conf, 2, conf.size() - 2);
    boost::split(variables_list, variables, boost::is_any_of("|"));
    for (strings_t::const_iterator it = variables_list.begin();
    it != variables_list.end(); ++it) {
        if (not it->empty()) {
            mForrester->add(new Variable(*it,*mForrester));
        }
    }
}


void PluginForrester::generateObservable(
    vpz::AtomicModel& model,vpz::Observables& observables)
{
    std::string observableName((fmt("obs_DTE_%1%") % model.getName()).str());

    if (observables.exist(observableName)) {
        vpz::Observable& observable(observables.get(observableName));

        for (vpz::Observable::iterator it = observable.begin();
             it != observable.end();
             ++it) {

            //vpz::Observable observable(observableName);
            graphical_items::const_iterator jt;
            for (jt = mForrester->getItems().begin();
                 jt != mForrester->getItems().end(); ++jt) {
                if ((*jt)->getName() == it->first)
                    break;
            }
            if (jt == mForrester->getItems().end()) {
                observable.del(it->first);
            }
        }

        for (graphical_items::const_iterator it =
                 mForrester->getItems().begin();
             it != mForrester->getItems().end(); ++it) {

            if (!observable.exist((*it)->getName())) {
                (*it)->generateObservable(observable);
            }
        }
    } else {
        vpz::Observable observable(observableName);

        for (graphical_items::const_iterator it =
                 mForrester->getItems().begin();
             it != mForrester->getItems().end(); ++it) {
            (*it)->generateObservable(observable);
        }
        observables.add(observable);

        if (model.observables().empty()) {
            model.setObservables(observableName);
        }
    }
}


}
}
}
}    // namespace vle gvle modeling forrester

DECLARE_GVLE_MODELINGPLUGIN(vle::gvle::modeling::forrester::PluginForrester)
