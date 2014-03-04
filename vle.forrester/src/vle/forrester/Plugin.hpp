/*
 * @file vle/forrester/Plugin.hpp
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


#ifndef VLE_GVLE_MODELING_FORRESTER_PLUGIN_HPP
#define VLE_GVLE_MODELING_FORRESTER_PLUGIN_HPP

#include <vle/forrester/Forrester.hpp>
#include <vle/forrester/ForresterDrawingArea.hpp>
#include <vle/gvle/ModelingPlugin.hpp>
#include <vle/utils/Path.hpp>
#include <gtkmm/actiongroup.h>
#include <gtkmm/dialog.h>
#include <gtkmm/toolbar.h>
#include <gtkmm/uimanager.h>
#include <gtkmm/builder.h>
#include <gtkmm/radiobutton.h>
#include <vle/forrester/graphicalItems/Compartment.hpp>
#include <vle/vpz/AtomicModel.hpp>
#include <vle/vpz/Condition.hpp>


namespace vle {
namespace gvle {
namespace modeling {
namespace forrester {

typedef std::vector < std::string > strings_t;

/**
 * @class PluginForrester
 * @brief Forrester diagram editor plugin class
 *
 * Differential equation plugin class. Contains ModelingPlugin inherited
 * methods. Graphical user interface has a drawing area. This allow to draw
 * Forrester diagrams, and finaly convert them in c++ class.
 */
class PluginForrester : public ModelingPlugin
{
public:


    /**
     * Class constructor.
     *
     * @param[in] package containing the plugin
     * @param[in] library
     * @param[in] name of the current package
     */
    PluginForrester(const std::string& package,
                    const std::string& library,
                    const std::string& curr_package);

    /**
     * Class destructor.
     */
    virtual ~PluginForrester();

    /**
     * Create a new model based on Differential equation class.
     * @param[in] classname The name of the generated class.
     * @param[in] namespace_ The namespace of the generated class.
     */
    virtual bool create(vpz::AtomicModel& model,
                        vpz::Dynamic& dynamic,
                        vpz::Conditions& conditions,
                        vpz::Observables& observables,
                        const std::string& classname,
                        const std::string& namespace_);

    /**
     * Modify an existant model based on differential equation class.
     * @param[in] conf The tag string of the generated class.
     * @param[in] buffer All the text of the class.
     */
    virtual bool modify(vpz::AtomicModel& model,
                        vpz::Dynamic& dynamic,
                        vpz::Conditions& conditions,
                        vpz::Observables& observables,
                        const std::string& conf,
                        const std::string& buffer);

    virtual bool start(vpz::Condition& condition);

    /**
     * Start.
     * @return Always return true.
     */
    virtual bool start(vpz::Condition&, const std::string&)
    {
        return true;
    }



private:
    PluginForrester (const PluginForrester&);
    PluginForrester& operator= (const PluginForrester&);

    /**
     * Delete all gtkmm signal connections.
     */
    void destroy();

    /**
     * Extract the string between 2 beacons.
     * @param[in] buffer Parsed text.
     * @param[in] begin The beginning beacon
     * @param[in] end The ending beacon
     * @param[in] name The name of the beacon
     * @return The final string.
     * @throw There are none beginning and ending beacon
     * @throw Empty tag.
     */
    std::string parseFunction(const std::string& buffer,
    const std::string& begin,
    const std::string& end,
    const std::string& name);

    /**
     * Generate the ports corresponding to the model
     */
    void generatePorts(vpz::AtomicModel& model);

    /**
     * Generate the c++ class text.
     * @param[in] classname The name of the class.
     * @param[in] namespace_ The name of the namespace.
     */
    void generateSource(const std::string& classname,
                        const std::string& namespace_);

    /**
     * Generate conditions corresponding to the model
     */
    void generateCondition(vle::vpz::Conditions&, vpz::AtomicModel&);

    /**
     * Generate the name of the conditions
     */
    std::string generateConditionName(const vpz::AtomicModel& model) const
    { return (fmt("cond_DTE_%1%") % model.getName()).str(); }

    void generateExperimentalConditions(vle::vpz::Conditions&,
                                            vpz::AtomicModel&);
    /**
     * Generate observables corresponding to the model
     */
    void generateObservable(vpz::AtomicModel& model,vpz::Observables& obs);

    /**
     * Generate the equation for a compartment
     * @return equation
     */
    std::string generateEquation(const Compartment *);

    /**
     * Gtkmm callback function. Get or set the c++ include string.
     */
    void onIncludeSource();

    /**
     * Gtkmm callback function. Time step button.
     */
    void onTimeStep();

    /**
     * Gtkmm callback function. Qss2 radiobutton.
     */
    void onQss2();

    /**
     * Gtkmm callback function. Euler radiobutton.
     */
    void onEuler();

    /**
     * Gtkmm callback function. Rk4 radiobutton.
     */
    void onRk4();

    /**
     * Gtkmm callback method. Change the drawing area mode.
     * Allow users to delete a compartment.
     */
    void onRemoveCompartment();

    /**
     * Gtkmm callback method. Change the drawing area mode.
     * Allow users to move a compartment on the drawing area.
     */
    void onSelect();

    /**
     * Gtkmm callback method. Change the drawing area mode.
     * Allow users to add a compartment.
     */
    void onAddCompartment();

    /**
     * Gtkmm callback method. Change the drawing area mode.
     * Allow users to add a compartment.
     */
    void onAddFlow();

    /**
     * Gtkmm callback method. Change the drawing area mode.
     * Allow users to add a flow.
     */
    void onAddFlowArrow();

    /**
     * Gtkmm callback method. Change the drawing area mode.
     * Allow users to add a flow arrow.
     */
    void onAddParameter();

    /**
     * Gtkmm callback method. Change the drawing area mode.
     * Allow users to add a parameter.
     */
    void onAddParameterArrow();

    /**
     * Gtkmm callback method. Change the drawing area mode.
     * Allow users to add an exogenous variable.
     */
    void onAddExogenousVariable();

    /**
     * Gtkmm callback method. Change the drawing area mode.
     * Allow users to add a variable.
     */
    void onAddVariable();

    /**
     * Parse the tag string to get classname and namespace of the generated
     * class.
     * @param[in] lst The list of elements of the tag string.
     * @param[out] classname The name of the class.
     * @param[out] namespace_ The name of the namespace.
     */
    void parseConf(const strings_t& lst, std::string& classname,
                   std::string& namespace_);

    void createActions();
    void createUI();

    /**
     * Load integration mode from experimental conditions
     */
    void setIntegrationModeFromCondition(const vpz::Conditions& conditions,
        std::string conditionName);

    /**
     * Save the integration timestep into experimental conditions
     */
    void setIntegrationTimeStep(const vpz::Conditions& conditions,
        std::string conditionName);
    /**
     * Load conditions mode from experimental conditions
     */
    void loadConditions(const std::string&);

    /**
     * Load flows from tags
     */
    void loadFlows(const std::string&);

    /**
     * Load flow arrows from tags
     */
    void loadFlowsArrow(const std::string&);

    /**
     * Load parameters from tags
     */
    void loadParameters(const std::string&);

    /**
     * Load dependency arrows from tags
     */
    void loadDependencyArrow(const std::string&);

    /**
     * Load initial values from experimental conditions
     */
    void loadInitialValues(const vpz::Conditions& conditions,
        std::string conditionName);

    /**
     * Load exogenous variables from tags
     */
    void loadExogenousVariables(const std::string& conf);

    /**
     * Load variables from tags
     */
    void loadVariables(const std::string& conf);

    //variables
    static const std::string TEMPLATE_DEFINITION;
    static const Glib::ustring UI_DEFINITION;

    Glib::RefPtr < Gtk::Builder > mXml;
    Gtk::Dialog* mDialog;
    Gtk::Button* mIncludeButton;
    Gtk::Button* mTimeStepButton;
    Gtk::RadioButton* mEulerButton;
    Gtk::RadioButton* mRk4Button;
    Gtk::RadioButton* mQss2Button;

    std::list < sigc::connection > mList;
    std::string mInclude;

    Gtk::Toolbar* mToolbar;
    Glib::RefPtr < Gtk::UIManager >    mUIManager;
    Glib::RefPtr < Gtk::ActionGroup >  mActionGroup;

    Forrester* mForrester;
    double mTimeStep;
    ForresterDrawingArea* mView;
 };

}
}
}
}    // namespace vle gvle modeling forrester

#endif
