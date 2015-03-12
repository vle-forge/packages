/*
 * @file vle/ibm/Controleur.hpp
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
#ifndef CONTROLEUR_HPP
#define CONTROLEUR_HPP

#include "ControleurProxy.hpp"
#include <vle/extension/mas/GenericAgent.hpp>
#include <vle/vpz/Conditions.hpp>
#include <map>
#include <sstream>
#include <iostream>
#include <fstream>
#include <boost/algorithm/string.hpp>
#include <boost/algorithm/string/replace.hpp>
#include <boost/lexical_cast.hpp>
#include <vle/vpz/ModelPortList.hpp>
#include <limits>
#include <algorithm>
#include <vector>
#include <vle/utils/Types.hpp>

namespace vd = vle::devs;
namespace vc = vle::vpz;
//namespace vv = vle::value;

namespace vle {
namespace gvle {
namespace global {
namespace ibminsidegvle {
/*
*@@tagdynamic@@
*@@tagdepends:@@endtagdepends
*/
class Controleur : public GenericAgent
{
public:
    Controleur(const vd::ExecutiveInit& mdl,
               const vd::InitEventList& events);

    ~Controleur();

    void addInstruction(int nb_clone, std::string className);

    void addModelWith(int nb_clone, std::string className,
                      std::map <std::string, vv::Value*> variableToModify);

    void delOneModel(std::string modelName);

    double getData(std::string modelName, std::string varName);
    double getData(std::string className, int n, std::string varName);

    std::string getModelNameFromClassNb(std::string className, int i);

    int countModelOfClass(std::string className);

    void setModelValue(std::string className, int n, std::string varName, double varValue);
    void setModelValue(std::string modelName, std::string varName, double varValue);

    /**
     * @brief Find the number associated to the variable
     *
     * @param std::string nb
     *
     * @return int
     */
    double readNumber(std::string nb);

    double getTime();

    void addEffectAt(double time, double frequency, std::string script);

    void execInit(std::string script);

protected:
    /** @brief Pure virtual agent functions. Modeler must override them */
    virtual void agent_dynamic();
    /** @brief Pure virtual agent functions. Modeler must override them */
    virtual void agent_init();
    /** @brief Pure virtual agent functions. Modeler must override them */
    virtual void agent_handleEvent(const Message&);

private:
    double time;
    vv::Map* mEvents;
	std::string mScript;
	std::string mScriptExec;
	std::map <std::string, std::map <std::string, vle::value::Value*> > mData;
	std::map <std::string, int> mNameNumber;
	int mIndexEffect;
	double ta;
	std::vector <vd::ExternalEvent*> mNextExternalEvent;
	std::vector <std::string> mDeadModel;

	lua_State *L;
    ControleurProxy mCP;

    vv::Value* observation(const vd::ObservationEvent& event) const;

    std::string addOneModel(std::string className);

    /**
     * @brief Find a name starting with className_ and a number doesn't used
     *
     * @param std::string className
     *
     * @return std::string
     */
    std::string findModelName(std::string className);

    /**
     * @brief Connect the model to the executive
     *
     * @param std::string modelName
     * @param const vpz::BaseModel* model
     */
    void connectionModelToExec(std::string modelName, const vpz::BaseModel* model);

    /**
     * @brief Connect the executive with the model
     *
     * @param std::string the model name
     */
    void connectionExecToModel(std::string modelName);
    /**
     * @brief Delete models
     *
     * @param std::vector<std::string> list of the model name to remove
     */
    void delInstruction(std::vector<std::string> words);



    /**
     * @brief Remove the input port of the executive associated to the model
     *
     * @param std::string model name
     */
    void removeInputPortExec(std::string modelName);

    /**
     * @brief Remove the output port of the executive associated
     * to pertub port of modelName
     *
     * @param std::string
     */
    void removeOutputPortExec(std::string modelName);

    /**
     * @brief Put in the data struture the parameters, if already exist update them.
     *
     */
    void putInStructure(std::string modelName, std::string variable,
                        vle::value::Value* value);

    /**
     * @brief find the model from the port name
     *
     * @param std::string port name
     *
     * @return std::string model name
     */
    std::string getModelNameFromPort(std::string s);

    /**
     * @brief update the data structure from the event list
     *
     * @param const vd::ExternalEventList& events
     */
    void updateData(const vd::ExternalEventList& events);

    std::map <std::string, vv::Value*> modifyParameter(std::string className,
                                                       std::map <std::string,
                                                       vv::Value*> variableToModify);

    /**
     * @brief Compare the model name and the class name,
     * if the model belongs to the class name, return true, false otherwise
     *
     * @param std::string modelName
     * @param std::string className
     *
     * @return bool
     */
    bool compareModelClass(std::string modelName, std::string className);

    void doScriptAt(const Effect& e);

    Effect doScriptEffectAt(double t, const std::string& source,
                            std::string functionName, double frequency);

    int PrintErrorMessageOrNothing(int ErrorCode);

    struct CompareTime {
        bool operator()( const std::pair<int, std::string> a,
                         const std::pair<int, std::string>  b ) const {
            return a.first > b.first;
        }
    };
};

}}}} // namespace vle gvle global ibminsidegvle
#endif
