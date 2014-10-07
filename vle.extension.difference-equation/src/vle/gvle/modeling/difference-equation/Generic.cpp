/*
 * @file vle/gvle/modeling/difference-equation/Generic.cpp
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


#include <vle/gvle/modeling/difference-equation/Generic.hpp>
#include <vle/utils/Package.hpp>

namespace vle { namespace gvle { namespace modeling { namespace de {

Generic::Generic(const std::string& package,
                 const std::string& library,
                 const std::string& curr_package) :
    Plugin(package, library, curr_package), m_dialog(0), m_buttonSource(0)
{
}

Generic::~Generic()
{
}

void Generic::build(bool modeling)
{
    Gtk::VBox* vbox;

    vle::utils::Package pack(getPackage());

    std::string glade = pack.getPluginGvleModelingFile(
            "DifferenceEquation.glade", vle::utils::PKG_BINARY);

    mXml = Gtk::Builder::create();
    mXml->add_from_file(glade.c_str());

    mXml->get_widget("DialogPluginGenericBox", m_dialog);
    m_dialog->set_title("DifferenceEquation - Generic");
    mXml->get_widget("GenericPluginVBox", vbox);

    vbox->pack_start(mNameValue.build(mXml));
    vbox->pack_start(mTimeStep.build(mXml));
    vbox->pack_start(mMapping.build(mXml));

    if (modeling) {
        Gtk::VBox* vbox;
        mXml->get_widget("GenericPluginVBox", vbox);
        vbox->pack_start(mParameters.buildParameters(mXml));

        m_buttonSource = Gtk::manage(
            new Gtk::Button("Includes / Compute / InitValue / User section"));
        m_buttonSource->show();
        vbox->pack_start(*m_buttonSource);
        mList.push_back(m_buttonSource->signal_clicked().connect(
                sigc::mem_fun(*this, &Plugin::onSource)));
    }
}

bool Generic::create(vpz::AtomicModel& model,
                     vpz::Dynamic& dynamic,
                     vpz::Conditions& conditions,
                     vpz::Observables& observables,
                     const std::string& classname,
                     const std::string& namespace_)
{
    std::string conditionName((fmt("cond_DE_%1%") % model.getName()).str());

    build(true);

    if (not conditions.exist(conditionName)) {
        vpz::Condition condition(conditionName);

        Generic::fillFields(condition);
    } else {
        Generic::fillFields(conditions.get(conditionName));
    }

    mIncludes = "";
    mComputeFunction =
        "virtual double compute(const vd::Time& /*time*/)\n"        \
        "{ return 0; }\n";
    mInitValueFunction =
        "virtual double initValue(const vd::Time& /*time*/)\n"      \
        "{ return 0; }\n";
    mUserFunctions = "";
    mUserConstructor =
        "// available constructor parameters :\n"     \
        "//  * const vd::DynamicsInit& atom\n"        \
        "//  * const vd::InitEventList& evts";

    if (m_dialog->run() == Gtk::RESPONSE_ACCEPT) {
        Plugin::generate(model, dynamic, conditions, observables,
                         classname, namespace_, true);
        m_dialog->hide_all();
        destroy();
        return true;
    }
    m_dialog->hide_all();
    destroy();
    return false;
}

void Generic::destroy()
{
    if (m_buttonSource) {
        Gtk::VBox* vbox;

        mXml->get_widget("GenericPluginVBox", vbox);
        vbox->remove(*m_buttonSource);
    }

    for (std::list < sigc::connection >::iterator it = mList.begin();
         it != mList.end(); ++it) {
        it->disconnect();
    }
}

void Generic::fillFields(const vpz::Condition& condition)
{
    mNameValue.fillFields(condition);
    mTimeStep.fillFields(condition);
}

void Generic::generateCondition(vpz::AtomicModel& model,
                               vpz::Conditions& conditions)
{
    std::string conditionName((fmt("cond_DE_%1%") % model.getName()).str());
    if (conditions.exist(conditionName)) {
        vpz::Condition& condition(conditions.get(conditionName));

        mNameValue.deletePorts(condition);
        mTimeStep.deletePorts(condition);
        mParameters.deletePorts(condition);

        mNameValue.assign(condition);
        mTimeStep.assign(condition);
        mParameters.assign(condition);
    } else {
        vpz::Condition condition(conditionName);

        mNameValue.assign(condition);
        mTimeStep.assign(condition);
        mParameters.assign(condition);
        conditions.add(condition);
    }

    std::vector < std::string > cond(model.conditions());
    if (std::find(cond.begin(), cond.end(), conditionName) == cond.end()) {
        cond.push_back(conditionName);
        model.setConditions(cond);
    }
}

void Generic::generateObservables(vpz::AtomicModel& model,
                                  vpz::Observables& observables)
{
    std::string observableName((fmt("obs_DE_%1%") % model.getName()).str());

    if (observables.exist(observableName)) {
        vpz::Observable& observable(observables.get(observableName));

        if (not observable.exist(mNameValue.getVariableName())) {
            observable.add(mNameValue.getVariableName());
        }
    } else {
        vpz::Observable observable(observableName);

        observable.add(mNameValue.getVariableName());
        observables.add(observable);
    }
    if (model.observables().empty()) {
        model.setObservables(observableName);
    }
}

void Generic::generateOutputPorts(vpz::AtomicModel& model)
{
    if (not model.existOutputPort("update")) {
        model.addOutputPort("update");
    }
}

void Generic::generateVariables(utils::Template& tpl_)
{
    tpl_.stringSymbol().append("varname", mNameValue.getVariableName());
}

std::string Generic::getTemplate() const
{
    return
    "/**\n"                                                             \
    "  * @file {{classname}}.cpp\n"                                     \
    "  * @author ...\n"                                                 \
    "  * ...\n"                                                         \
    "  * @@tag DifferenceEquationGeneric@vle.extension.difference-equation @@"\
    "namespace:{{namespace}};"                                          \
    "class:{{classname}};par:"                                          \
    "{{for i in par}}"                                                  \
    "{{par^i}},{{val^i}}|"                                              \
    "{{end for}}"                                                       \
    "@@end tag@@\n"                                                     \
    "  */\n\n"                                                          \
    "#include <vle/extension/DifferenceEquation.hpp>\n\n"               \
    "//@@begin:includes@@\n"                                            \
    "{{includes}}"                                                      \
    "//@@end:includes@@\n\n"                                            \
    "namespace vd = vle::devs;\n"                                       \
    "namespace ve = vle::extension;\n"                                  \
    "namespace vv = vle::value;\n\n"                                    \
    "namespace {{namespace}} {\n\n"                                     \
    "class {{classname}} : public ve::DifferenceEquation::Generic\n"    \
    "{\n"                                                               \
    "public:\n"                                                         \
    "    {{classname}}(\n"                                              \
    "       const vd::DynamicsInit& atom,\n"                            \
    "       const vd::InitEventList& evts)\n"                           \
    "        : ve::DifferenceEquation::Generic(atom, evts)\n"           \
    "    {\n"                                                           \
    "//@@begin:construct@@\n"                                           \
    "{{construct}}"                                                     \
    "//@@end:construct@@\n\n"                                           \
    "{{for i in par}}"                                                  \
    "        {{par^i}} = vv::toDouble(evts.get(\"{{par^i}}\"));\n"      \
    "{{end for}}"                                                       \
    "        {{varname}} = createVar(\"{{varname}}\");\n"               \
    "    }\n"                                                           \
    "\n"                                                                \
    "    virtual ~{{classname}}()\n"                                    \
    "    {}\n"                                                          \
    "\n"                                                                \
    "//@@begin:compute@@\n"                                             \
    "{{compute}}"                                                       \
    "//@@end:compute@@\n\n"                                             \
    "//@@begin:initValue@@\n"                                           \
    "{{initValue}}"                                                     \
    "//@@end:initValue@@\n\n"                                           \
    "private:\n"                                                        \
    "//@@begin:user@@\n"                                                \
    "{{userFunctions}}"                                                 \
    "//@@end:user@@\n\n"                                                \
    "{{for i in par}}"                                                  \
    "    double {{par^i}};\n"                                           \
    "{{end for}}"                                                       \
    "    Var {{varname}};\n"                                            \
    "};\n\n"                                                            \
    "} // namespace {{namespace}}\n\n"                                  \
    "DECLARE_DYNAMICS({{namespace}}::{{classname}})\n\n";
}

bool Generic::modify(vpz::AtomicModel& model,
                    vpz::Dynamic& dynamic,
                    vpz::Conditions& conditions,
                    vpz::Observables& observables,
                    const std::string& conf,
                    const std::string& buffer)
{
    std::string namespace_;
    std::string classname;
    Parameters::Parameters_t parameters;
    Parameters::ExternalVariables_t externalVariables;

    parseConf(conf, classname, namespace_, parameters, externalVariables);
    parseFunctions(buffer);
    std::string conditionName((fmt("cond_DE_%1%") % model.getName()).str());

    build(true);

    if (not conditions.exist(conditionName)) {
        vpz::Condition condition(conditionName);

        Generic::fillFields(condition);
	mParameters.fillFields(parameters);
    } else {
        Generic::fillFields(conditions.get(conditionName));
	mParameters.fillFields(conditions.get(conditionName),
                               parameters);
    }

    backup();

    if (m_dialog->run() == Gtk::RESPONSE_ACCEPT) {
        generate(model, dynamic, conditions, observables, classname,
                 namespace_, true);
        m_dialog->hide_all();
        destroy();
        return true;
    }
    m_dialog->hide_all();
    destroy();
    return false;
}

bool Generic::start(vpz::Condition& condition)
{
    build(false);
    fillFields(condition);

    if (m_dialog->run() == Gtk::RESPONSE_ACCEPT) {
        Generic::assign(condition);
    }
    m_dialog->hide();
    destroy();
    return true;
}

void Generic::assign(vpz::Condition& condition)
{
    mNameValue.deletePorts(condition);
    mTimeStep.deletePorts(condition);

    mNameValue.assign(condition);
    mTimeStep.assign(condition);
}

}}}} // namespace vle gvle modeling de

DECLARE_GVLE_MODELINGPLUGIN(vle::gvle::modeling::de::Generic)

