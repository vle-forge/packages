/*
 * @file vle/gvle/modeling/decision/Plugin.hpp
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

#ifndef VLE_GVLE_MODELING_DECISION_PLUGIN_HPP
#define VLE_GVLE_MODELING_DECISION_PLUGIN_HPP

#include <vle/gvle/ModelingPlugin.hpp>
#include <vle/vpz/AtomicModel.hpp>
#include <vle/vpz/Experiment.hpp>
#include <vle/vpz/Conditions.hpp>
#include <vle/vpz/Condition.hpp>

#include <vle/value/String.hpp>
#include <vle/value/Value.hpp>

#include <vle/utils/Template.hpp>
#include <vle/utils/Tools.hpp>
#include <vle/utils/Path.hpp>

#include <vle/gvle/modeling/decision/OutputFunctionDialog.hpp>
#include <vle/gvle/modeling/decision/DecisionDrawingArea.hpp>
#include <vle/gvle/modeling/decision/PredicateDialog.hpp>
#include <vle/gvle/modeling/decision/SourceDialog.hpp>
#include <vle/gvle/modeling/decision/FactDialog.hpp>
#include <vle/gvle/modeling/decision/RuleDialog.hpp>
#include <vle/gvle/modeling/decision/AckFunctionDialog.hpp>
#include <vle/gvle/modeling/decision/EditorDialog.hpp>
#include <vle/gvle/modeling/decision/Decision.hpp>

#include <vle/extension/decision/KnowledgeBase.hpp>
#include <vle/extension/decision/Activity.hpp>

#include <vle/gvle/Message.hpp>

#include <gtkmm/filechooserdialog.h>
#include <gtkmm/actiongroup.h>
#include <gtkmm/radioaction.h>
#include <gtkmm/uimanager.h>
#include <gtkmm/builder.h>
#include <gtkmm/toolbar.h>
#include <gtkmm/dialog.h>
#include <gtkmm/stock.h>

#include <boost/algorithm/string/classification.hpp>
#include <boost/algorithm/string/trim.hpp>
#include <boost/algorithm/string/split.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/filesystem.hpp>
#include <boost/regex.hpp>

#include <iostream>
#include <fstream>

namespace vle {
namespace gvle {
namespace modeling {
namespace decision {
typedef std::vector < std::string > strings_t;
typedef strings_t::iterator strings_it;
typedef std::map < std::string, strings_t > hierarchicalPred;
typedef std::map < std::string, strings_t > rules_t;
typedef rules_t::iterator rules_it;


/**
 * @class PluginDecision
 * @brief This class is the main class of the plugin.
 */

class PluginDecision : public ModelingPlugin
{
public:
/**
 * @brief PluginDecision parameterized constructor
 * @param package name
 * @param library name
 * @param current package name
 */
    PluginDecision(const std::string& package, const std::string& library,
            const std::string& curr_package);

/**
 * @brief PluginDecision destructor
 */
    virtual ~PluginDecision();

    void clear();

/**
 * @brief This fonction show the plugin window and wait for the user action
 * ("Valider" or "Annuler")
 * @param reference to the classname
 * @param reference to the namespace
 * @return return a boolean, true if the user clicked on "Valider" or false
 */
    virtual bool create(vpz::AtomicModel& model,
                        vpz::Dynamic& dynamic,
                        vpz::Conditions& conditions,
                        vpz::Observables& observables,
                        const std::string& classname,
                        const std::string& namespace_);

/**
 * @brief This fonction show the plugin window and wait
 * for the user action ( "Valider" or "Annuler" ).
 * It loads data form the buffer.
 * @param reference to the classname
 * @param reference to the namespace
 * @return return a boolean, true if the user clicked
 * on "Valider" or false
 */
    virtual bool modify(vpz::AtomicModel& model,
                        vpz::Dynamic& dynamic,
                        vpz::Conditions& conditions,
                        vpz::Observables& observables,
                        const std::string& conf,
                        const std::string& buffer);

/**
 * @brief Load the plan file into the model.
 * @param plan filename
 * @return true if the plan file is loaded correctly
 */
    bool loadPlanFile(std::string);

/**
 * @brief Write the plan file into the file.
 * @param plan filename
 */
    void writePlanFile(std::string);

/**
 * @brief Return if the plugin is started or not
 * @return return a boolean, true if the plugin is on
 */
    bool start(vpz::Condition& /*condition*/)
    {
        return true;
    }

/**
 * @brief Return if the plugin is started or not
 * @param reference to the condition
 * @param reference to a string
 * @return return a boolean, true if the plugin is on
 */
    virtual bool start(vpz::Condition&, const std::string&)
    {
        return true;
    }

private:

    std::string toHumanDate(double date);
/**
 * @brief Create actions for the butons group like "Select",
 * "Add Activity"...
 */
    void createActions();

/**
 * @brief Create the menu in the window
 */
    void createUI();

 /**
 * @brief Destroy the decision model
 */
    void destroy();

/**
 * @brief Generate source file
 * @param reference to the classname
 * @param reference to the namespace
 */
    void generateSource(const std::string& classname,
                        const std::string& namespace_);

/**
 * @brief Generate the observables
 * @param reference to the model of the vpz
 * @param reference to the observables of the vpz
 */
    void generateObservables(vpz::AtomicModel& mModel,
                             vpz::Observables& mObservables);

    void generateConditions(vpz::AtomicModel& model,
                            vpz::Conditions& pConditions);
/**
 * @brief Generate the ports
 * @param reference to the model of the vpz
 */
    void generatePorts(vpz::AtomicModel& model);

/**
 * @brief Set the DecisionDrawingArea in add mode.
 */
    void onAddActivity();

/**
 * @brief Set the DecisionDrawingArea in delete mode.
 */
    void onDelete();

/**
 * @brief Set the DecisionDrawingArea in select mode.
 */
    void onSelect();

/**
 * @brief Set the DecisionDrawingArea in add constraint mode.
 */
    void onAddConstraint();

/**
 * @brief This function start the include dialog.
 */
    void onIncludeSource();

/**
 * @brief This function start the fact dialog.
 */
    void onUserFact();

/**
 * @brief This function start the predicate dialog.
 */
    void onUserPredicate();

/**
 * @brief This function start the rule dialog.
 */
    void onUserRule();

/**
 * @brief This function start the output function dialog.
 */
    void onUserOutputFunction();

/**
 * @brief This function start the file chooser dialog.
 */
    void onUserOpenMenu();

    void onChooseFileName();
    void onUserSaveAsMenu();
    void onUserHierPredicate();

/**
 * @brief This function start the ack function dialog.
 */
    void onUserAckFunction();

/**
 * @brief Parse the namespace and the classname in the buffer.
 * @param buffer
 * @param classname
 * @param namespace
 */
    void parseConf(const strings_t& lst, std::string& classname,
                   std::string& namespace_);

/**
 * @brief Parse function into the buffer
 * @param reference to the buffer
 * @param reference to the begin tag
 * @param reference to the end tag
 * @param vector of tags name
 * @param reference to the returned vector containing values
 * @return Return the values between param 2 and 3 with the tag in param 4
 * into a string vector
 */
    void parseFunction(const std::string& buffer,
                       const std::string& begin,
                       const std::string& end,
                       strings_t& names,
                       strings_t& buffers);

/**
 * @brief Parse function into the buffer
 * @param reference to the buffer
 * @param reference to the begin tag
 * @param reference to the end tag
 * @param name of the tag
 * @throw Throw an exception if no begin or end tag and if the tag in param 4
 * doesn't appear into the buffer.
 * @return Return the values between param 2 and 3 with the tag in param 4.
 */
    std::string parseFunction(const std::string& buffer,
                              const std::string& begin,
                              const std::string& end,
                              const std::string& name);

/**
 * @brief Parse activities into the buffer.
 * @param activities list
 */
    void parseActivities(const strings_t& lst);

/**
 * @brief Parse facts into the buffer.
 * @param facts list
 */
    void parseFacts(const strings_t& lst);

/**
 * @brief Parse predicates into the buffer.
 * @param predicates list
 */
    void parsePredicates(const strings_t& lst);

/**
 * @brief Parse the RulesAndPred into the buffer.
 * @param rules list
 */
    void parseRulesAndPred(const strings_t& lst);

/**
 * @brief Parse the PrecedenceConstraints into the buffer.
 * @param constraints list
 */
    void parsePrecedenceConstraints(const strings_t& lst);

/**
 * @brief Parse output functions into the buffer.
 * @param output functions list
 */
    void parseOutputFunctions(const strings_t& lst);

/**
 * @brief Parse acknowledge functions into the buffer.
 * @param acknowledge functions list
 */
    void parseAckFunctions(const strings_t& lst);

    void parsePlanFileName(const strings_t& lst);

    void parseParameters(vpz::AtomicModel& model,
                         const strings_t& lst,
                         vpz::Conditions& pConditions);

    void decodePredicates(std::string&);
/**
 * @brief A dumy predicate...
 */
    bool dumyPredicate() const
    {
        return true;
    }

    void renamePredicate(std::string old, std::string news);
    void renameRule(std::string old, std::string news);
    void renameOutput(std::string old, std::string news);
    void renameAck(std::string old, std::string news);
    void renameHierachicalPredItem(std::string old, std::string news);

strings_t::iterator* generatePred(std::string *pPred,
        strings_t::iterator *it,
        strings_t *vPred, int pRowNumber, std::string pLog) {
    if (*it == vPred->end()) {
        return it;
    }

    bool firstRound = true;
    do {
        strings_t parameters;
        boost::split(parameters, *(*it), boost::is_any_of("|"));

        Glib::ustring pred = parameters.at(0);
        std::string log = parameters.at(1);
        Glib::ustring fact = parameters.at(2);
        std::string op = parameters.at(3);
        Glib::ustring par = parameters.at(4);

        std::istringstream iss(parameters.at(5));
        int rowType;
        iss >> rowType;
        Glib::ustring predName = parameters.at(6);

        std::istringstream iss2(parameters.at(7));
        int rowNumber;
        iss2 >> rowNumber;

        if (!log.empty() && rowNumber > pRowNumber) {
            if (*it != vPred->end()) {
                ++(*it);
            }
            std::string tmpLog("");
            if (pLog == "OR") {
                tmpLog = " || ";
            }
            else if (pLog == "AND") {
                tmpLog = " && ";
            }

            if (firstRound == true) {
                *pPred += "(";
                generatePred(pPred, it, vPred, rowNumber, log);
                *pPred += ")";
                firstRound = false;
            }
            else {
                *pPred += tmpLog + "(";
                generatePred(pPred, it, vPred, rowNumber, log);
                *pPred += ")";
            }
        }
        else if (!log.empty()) {
            return it;
        }

        if (log.empty() && rowNumber > pRowNumber) {
            if (firstRound == true) {
                *pPred += "var" + fact + op + par;

                if (*it != vPred->end()) {
                    ++(*it);
                }
                firstRound = false;
            }
            else {
                if (pLog == "OR") {
                    *pPred += " || var" + fact + op + par;
                }
                else if (pLog == "AND") {
                    *pPred += " && var" + fact + op + par;
                }
                else {
                    *pPred += "var" + fact + op + par;
                }
                if (*it != vPred->end()) {
                    ++(*it);
                }
            }
        }
        else if (log.empty()) {
            return it;
        }

    } while (*it != vPred->end());

    return it;
}

    static const Glib::ustring UI_DEFINITION;
    static const std::string TEMPLATE_DEFINITION;
    static const std::string PLAN_TEMPLATE_DEFINITION;

    Glib::RefPtr < Gtk::Builder > mXml;

    Gtk::Dialog* mDialog;
    DecisionDrawingArea* mView;

    std::string mPlanFileName;
    std::string mPlanFile;
    std::string mClassname;
    std::string mNamespace;

    Gtk::Toolbar* mToolbar;
    Glib::RefPtr < Gtk::UIManager > mUIManager;
    Glib::RefPtr < Gtk::ActionGroup > mActionGroup;

    Gtk::Button* mIncludeButton;
    Gtk::Button* mPredicateButton;
    Gtk::Button* mFactButton;
    Gtk::Button* mRuleButton;
    Gtk::Button* mOutputButton;
    Gtk::Button* mAckButton;
    Gtk::Button* mHierPredsButton;

    std::list < sigc::connection > mList;

// Menu bar
    Gtk::MenuItem* mOpenMenu;
    Gtk::MenuItem* mChooseFileNameMenu;
    Gtk::MenuItem* mSaveAsMenu;

// Link to the decision diagram
    Decision* mDecision;
    std::string mInclude;
    std::string mCustomConstructor;
    std::string mMembers;
    std::string mConstructor;
    std::string mDefinition;

// Store the Facts
    strings_t mFactName;
    std::map < std::string, std::string > mFactFunction;
    std::map < std::string, std::string > mFactAttribute;
    std::map < std::string, std::string > mFactInit;

// Store the Predicates
    strings_t mPredicateName;
    std::map < std::string, std::string > mPredicateFunction;

// Store the Rules
    rules_t mRule;

// Store the Output functions
    strings_t mOutputFunctionName;
    std::map < std::string, std::string > mOutputFunction;

// Store the Acknowledge functions
    strings_t mAckFunctionName;
    std::map < std::string, std::string > mAckFunction;

// Store the Parameters
    std::map < std::string, std::string > mParam;

// Store the hierarchical predicates
    hierarchicalPred mPred;
};

}
}
}
}    // namespace vle gvle modeling decision
#endif
