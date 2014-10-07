/*
 * @file vle/gvle/modeling/difference-equation/Multiple.cpp
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


#include <vle/gvle/modeling/difference-equation/Multiple.hpp>
#include <vle/utils/Package.hpp>

namespace vle { namespace gvle { namespace modeling { namespace de {

Multiple::Multiple(const std::string& package, const std::string& library,
        const std::string& curr_package)
    : Plugin(package, library, curr_package), m_dialog(0), m_buttonSource(0)
{
}

Multiple::~Multiple()
{
}

void Multiple::build(bool modeling)
{
    Gtk::VBox* vbox;


    vle::utils::Package pack(getPackage());

    std::string glade = pack.getPluginGvleModelingFile(
            "DifferenceEquation.glade", vle::utils::PKG_BINARY);

    mXml = Gtk::Builder::create();
    mXml->add_from_file(glade.c_str());

    mXml->get_widget("DialogPluginMultipleBox", m_dialog);
    m_dialog->set_title("DifferenceEquation - Multiple");
    mXml->get_widget("MultiplePluginVBox", vbox);

    vbox->pack_start(mVariables.build(mXml));
    vbox->pack_start(mTimeStep.build(mXml));
    if (modeling) {
        vbox->pack_start(mParameters.build(mXml));

        {
            m_buttonSource = Gtk::manage(
                new Gtk::Button("Includes / Compute / InitValue / User section"));
            m_buttonSource->show();
            vbox->pack_start(*m_buttonSource);
            mList.push_back(m_buttonSource->signal_clicked().connect(
                                sigc::mem_fun(*this, &Plugin::onSource)));
        }
    }
    vbox->pack_start(mMapping.build(mXml));
}

bool Multiple::create(vpz::AtomicModel& model,
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
        Multiple::fillFields(condition);
    } else {
        Multiple::fillFields(conditions.get(conditionName));
    }

    mIncludes = "";
    mComputeFunction =
        "virtual void compute(const vd::Time& /*time*/)\n"        \
        "{ }\n";
    mInitValueFunction =
        "virtual void initValue(const vd::Time& /*time*/)\n"      \
        "{ }\n";
    mUserFunctions = "";
    mUserConstructor =
        "// available constructor parameters :\n"            \
        "//  * const vd::DynamicsInit& atom\n"               \
        "//  * const vd::InitEventList& evts";

    if (m_dialog->run() == Gtk::RESPONSE_ACCEPT) {
        generate(model, dynamic, conditions, observables, classname,
                 namespace_);
        m_dialog->hide_all();
        destroy();
        return true;
    }
    m_dialog->hide_all();
    destroy();
    return false;
}

void Multiple::destroy()
{
    if (m_buttonSource) {
        Gtk::VBox* vbox;

        mXml->get_widget("MultiplePluginVBox", vbox);
        vbox->remove(*m_buttonSource);
    }
    for (std::list < sigc::connection >::iterator it = mList.begin();
         it != mList.end(); ++it) {
        it->disconnect();
    }
}

void Multiple::fillFields(const vpz::Condition& condition)
{
    mVariables.fillFields(condition);
    mMapping.fillFields(condition);
}

void Multiple::generateCondition(vpz::AtomicModel& model,
                               vpz::Conditions& conditions)
{
    std::string conditionName((fmt("cond_DE_%1%") % model.getName()).str());
    if (conditions.exist(conditionName)) {
        vpz::Condition& condition(conditions.get(conditionName));

	mTimeStep.deletePorts(condition);
        mVariables.deletePorts(condition);
        mMapping.deletePorts(condition);
        mParameters.deletePorts(condition);

	mTimeStep.assign(condition);
        mVariables.assign(condition);
        mMapping.assign(condition);
	mParameters.assign(condition);
    } else {
        vpz::Condition condition(conditionName);

	mTimeStep.assign(condition);
	mVariables.assign(condition);
        mMapping.assign(condition);
	mParameters.assign(condition);
        conditions.add(condition);
    }

    std::vector < std::string > cond(model.conditions());
    if (std::find(cond.begin(), cond.end(), conditionName) == cond.end()) {
        cond.push_back(conditionName);
        model.setConditions(cond);
    }
}

void Multiple::generateObservables(vpz::AtomicModel& model,
                                   vpz::Observables& observables)
{
    std::string observableName((fmt("obs_DE_%1%") % model.getName()).str());

    if (observables.exist(observableName)) {
        vpz::Observable& observable(observables.get(observableName));
        Variables::Variables_t variables = mVariables.getVariables();

        for (Variables::Variables_t::const_iterator it = variables.begin();
             it != variables.end(); ++it) {
            if (not observable.exist(*it)) {
                observable.add(*it);
            }
        }
    } else {
        vpz::Observable observable(observableName);
        Variables::Variables_t variables = mVariables.getVariables();

        for (Variables::Variables_t::const_iterator it = variables.begin();
             it != variables.end(); ++it) {
            observable.add(*it);
        }
        observables.add(observable);
    }
    if (model.observables().empty()) {
        model.setObservables(observableName);
    }
}

void Multiple::generateOutputPorts(vpz::AtomicModel& model)
{
    Variables::Variables_t variables = mVariables.getVariables();

    for (Variables::Variables_t::const_iterator it = variables.begin();
         it != variables.end(); ++it) {
        if (not model.existOutputPort(*it)) {
            model.addOutputPort(*it);
        }
    }
}

void Multiple::generateVariables(utils::Template& tpl_)
{
    tpl_.listSymbol().append("var");

    Variables::Variables_t variables = mVariables.getVariables();

    for (Variables::Variables_t::const_iterator it = variables.begin();
         it != variables.end(); ++it) {
        tpl_.listSymbol().append("var", *it);
    }
}

std::string Multiple::getTemplate() const
{
    return
    "/**\n"                                                             \
    "  * @file {{classname}}.cpp\n"                                     \
    "  * @author ...\n"                                                 \
    "  * ...\n"                                                         \
    "  * @@tag DifferenceEquationMultiple@vle.extension.difference-equation @@"\
    "namespace:{{namespace}};"                                          \
    "class:{{classname}};par:"                                          \
    "{{for i in par}}"                                                  \
    "{{par^i}},{{val^i}}|"                                              \
    "{{end for}}"                                                       \
    ";sync:"                                                            \
    "{{for i in sync}}"                                                 \
    "{{sync^i}}|"                                                       \
    "{{end for}}"                                                       \
    ";nosync:"                                                          \
    "{{for i in nosync}}"                                               \
    "{{nosync^i}}|"                                                     \
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
    "class {{classname}} : public ve::DifferenceEquation::Multiple\n"   \
    "{\n"                                                               \
    "public:\n"                                                         \
    "    {{classname}}(\n"                                              \
    "       const vd::DynamicsInit& atom,\n"                            \
    "       const vd::InitEventList& evts)\n"                           \
    "        : ve::DifferenceEquation::Multiple(atom, evts)\n"          \
    "    {\n"                                                           \
    "//@@begin:construct@@\n"                                           \
    "{{construct}}"                                                     \
    "//@@end:construct@@\n\n"                                           \
    "{{for i in par}}"                                                  \
    "        {{par^i}} = vv::toDouble(evts.get(\"{{par^i}}\"));\n"      \
    "{{end for}}"                                                       \
    "{{for i in var}}"                                                  \
    "        {{var^i}} = createVar(\"{{var^i}}\");\n"                   \
    "{{end for}}"                                                       \
    "{{for i in sync}}"                                                 \
    "        {{sync^i}} = createSync(\"{{sync^i}}\");\n"                \
    "{{end for}}"                                                       \
    "{{for i in nosync}}"                                               \
    "        {{nosync^i}} = createNosync(\"{{nosync^i}}\");\n"          \
    "{{end for}}"                                                       \
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
    "{{for i in var}}"                                                  \
    "    Var {{var^i}};\n"                                              \
    "{{end for}}"                                                       \
    "{{for i in sync}}"                                                 \
    "    Sync {{sync^i}};\n"                                            \
    "{{end for}}"                                                       \
    "{{for i in nosync}}"                                               \
    "    Nosync {{nosync^i}};\n"                                        \
    "{{end for}}"                                                       \
    "};\n\n"                                                            \
    "} // namespace {{namespace}}\n\n"                                  \
    "DECLARE_DYNAMICS({{namespace}}::{{classname}})\n\n";
}

bool Multiple::modify(vpz::AtomicModel& model,
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

        mTimeStep.fillFields(condition);
        Multiple::fillFields(condition);
	mParameters.fillFields(parameters, externalVariables);
    } else {
        mTimeStep.fillFields(conditions.get(conditionName));
        Multiple::fillFields(conditions.get(conditionName));
	mParameters.fillFields(conditions.get(conditionName),
                               parameters,
                               externalVariables);
    }

    backup();

    if (m_dialog->run() == Gtk::RESPONSE_ACCEPT) {
        generate(model, dynamic, conditions, observables, classname,
                 namespace_);
        m_dialog->hide_all();
        destroy();
        return true;
    }
    m_dialog->hide_all();
    destroy();
    return false;
}

bool Multiple::start(vpz::Condition& condition)
{
    build(false);
    Multiple::fillFields(condition);
    if (m_dialog->run() == Gtk::RESPONSE_ACCEPT) {
	Multiple::assign(condition);
    }
    m_dialog->hide();
    destroy();
    return true;
}

void Multiple::assign(vpz::Condition& condition)
{
    mMapping.deletePorts(condition);
    mTimeStep.deletePorts(condition);
    mVariables.deletePorts(condition);

    mMapping.assign(condition);
    mTimeStep.assign(condition);
    mVariables.assign(condition);
}

}}}} // namespace vle gvle modeling de

DECLARE_GVLE_MODELINGPLUGIN(vle::gvle::modeling::de::Multiple)

