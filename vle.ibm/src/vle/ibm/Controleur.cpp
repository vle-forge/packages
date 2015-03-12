/*
 * @file vle/ibm/Controleur.cpp
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
#include "ControleurProxy.hpp"
#include "Controleur.hpp"
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

namespace vle {
namespace gvle {
namespace global {
namespace ibminsidegvle {
/*
*@@tagdynamic@@
*@@tagdepends:@@endtagdepends
*/

Controleur::Controleur(const vd::ExecutiveInit& mdl,
           const vd::InitEventList& events)
: GenericAgent(mdl, events), mIndexEffect(0)
{
    mEvents = new vv::Map(events);
    L = luaL_newstate();
    luaL_openlibs(L);

    Lunar<ControleurProxy>::Register(L);

    mCP.setControleur(this);

    lua_settop(L,0);

    Lunar<ControleurProxy>::push(L, &mCP);
    lua_setglobal(L, "ibm");
    mScript = "";
    for (vv::MapValue::const_iterator it = events.begin(); it!=events.end(); it++) {
        if (it->first != "Script" && it->second->isXml()) {
            mScript += "function " + it->first + " ()\n" + it->second->toXml().value() + "\nend\n";
        }
    }


    if (events.exist("Script"))
        mScript += events.getXml("Script");
    else
        throw vle::utils::ModellingError("Text Script not found");

    addEffect("doScriptAt",boost::bind(&Controleur::doScriptAt,this,_1));
}

Controleur::~Controleur() {}


void Controleur::agent_init() {
    execInit(mScript.c_str());
}

void Controleur::agent_dynamic() {
    Effect nextEffect = mScheduler.nextEffect();
    applyEffect(nextEffect.getName(),nextEffect);
}

void Controleur::agent_handleEvent(const Message& message) {
    if (std::find(mDeadModel.begin(), mDeadModel.end(), getModelNameFromPort(message.getSender())) == mDeadModel.end())
        putInStructure(getModelNameFromPort(message.getSender()), std::string(message.get("name")->toString().value()), new vv::Double(message.get("value")->toDouble()));
}

Effect Controleur::doScriptEffectAt(double t,const std::string& source, std::string functionToExec, double frequency)
{
    Effect effect(t,"doScriptAt",source);
    effect.add("script", vv::String::create(functionToExec));
    effect.add("frequency", vv::Double::create(frequency));
    return effect;
}

void Controleur::doScriptAt(const Effect& e) {
    execInit(e.get("script")->toString().value());
    double t = e.get("frequency")->toDouble().value();
    if (t != vd::infinity) {
        t += mCurrentTime;
    }
    Effect update = doScriptEffectAt(t, e.getOrigin(), e.get("script")->toString().value(), e.get("frequency")->toDouble().value());

    mScheduler.update(update);
}

void Controleur::addEffectAt(double time, double frequency, std::string functionName) {
    if (time == 0)
        time = std::numeric_limits<double>::epsilon();
    if (frequency <= 0)
        throw utils::ArgError("The frequency have to be more than 0");
    std::string idEffect = "Effect_"+ std::to_string(mIndexEffect);
    mIndexEffect++;
    std::string functionToExec = functionName + "()";
    Effect effect = doScriptEffectAt(time, idEffect, functionToExec, frequency);

    mScheduler.addEffect(effect);
}

void Controleur::execInit(std::string script) {
    int errorCode = luaL_dostring(L, script.c_str());
    PrintErrorMessageOrNothing(errorCode);
}

vv::Value* Controleur::observation(const vd::ObservationEvent& event) const {
    lua_getglobal(L, event.getPortName().c_str());
    if (!lua_isnumber(L, -1)) {
        lua_settop(L,0);
        return 0;
    }
    double nb = lua_tonumber(L, -1);
    lua_settop(L,0);
    return new vv::Double(nb);
}

/**
 * @brief Create nb_clone modelwith the className Class
 *
 * @param int nb_clone
 * @param std::string className
 */
void Controleur::addInstruction(int nb_clone, std::string className) {
    for (int i=0; i<nb_clone; i++){
		addOneModel(className);
	}
}

std::map <std::string, vv::Value*> Controleur::modifyParameter(std::string className, std::map <std::string, vv::Value*> variableToModify)
{
    std::map <std::string, vv::Value*> variableModified;
    vc::Conditions& cl = conditions();
	vc::Condition& c = cl.get("cond_DTE_" + className);
	for (std::map <std::string, vv::Value*>::iterator it = variableToModify.begin(); it != variableToModify.end(); ++it) {
	    vle::value::Value* d = c.firstValue(it->first).clone();
	    variableModified.insert(std::pair<std::string, vv::Value*>(it->first, d));
	    c.setValueToPort(it->first, *(it->second));
	}

	return variableModified;
}

void Controleur::addModelWith(int nb_clone, std::string className, std::map <std::string, vv::Value*> variableToModify) {
    std::map <std::string, vv::Value*> variableModified = modifyParameter(className, variableToModify);
    addInstruction(nb_clone, className);
    modifyParameter(className, variableModified);
}

void Controleur::delOneModel(std::string modelName) {
    delModel(modelName);
    removeInputPortExec(modelName);
    removeOutputPortExec(modelName);
    mData.erase(modelName);
    mDeadModel.push_back(modelName);
}

/**
 * @brief Find the number associated to the variable
 *
 * @param std::string nb
 *
 * @return int
 */
double Controleur::readNumber(std::string nb) {
    if (mEvents->exist(nb)) {
        switch(mEvents->get(nb)->getType()) {
            case vv::Value::INTEGER :
                return static_cast<double>(mEvents->getInt(nb));
                break;
            case vv::Value::DOUBLE :
                return mEvents->getDouble(nb);
                break;
            default :
                throw utils::ArgError(fmt("Type of variable `%1%' not an int or double") % nb);
        }

    }

    throw utils::ArgError(fmt("Variable `%1%' not found ") % nb);
}

std::string Controleur::addOneModel(std::string className) {
    std::string modelName = findModelName(className);
	const vpz::BaseModel* newModel = createModelFromClass(className, modelName);
	connectionModelToExec(modelName, newModel);
	connectionExecToModel(modelName);

	return modelName;
}

/**
 * @brief Find a name starting with className_ and a number doesn't used
 *
 * @param std::string className
 *
 * @return std::string
 */
std::string Controleur::findModelName(std::string className) {
    int i = 0;
    std::string modelName = className + "_";
    std::map<std::string, int>::iterator it = mNameNumber.find(className);
    if (it == mNameNumber.end()) {
        mNameNumber.insert(std::pair<std::string, int> (className,i));
	} else {
	    i = it->second;
	}
	std::stringstream ss;
    ss << i;
    modelName += ss.str();
    i++;
    mNameNumber[className] = i;
    return modelName;
}

/**
 * @brief Connect the model to the executive
 *
 * @param std::string modelName
 * @param const vpz::BaseModel* model
 */
void Controleur::connectionModelToExec(std::string modelName, const vpz::BaseModel* model) {
    std::map<std::string, vpz::ModelPortList> portList = model->getOutputPortList();
    for (std::map<std::string, vpz::ModelPortList>::iterator it=portList.begin(); it!=portList.end(); ++it){
        std::string outputPortName = it->first;
        std::string inputPort = modelName + "_" + outputPortName;
        addInputPort(getModelName(), inputPort);
        addConnection(modelName, outputPortName, getModelName(), inputPort);
    }
}

/**
 * @brief Connect the executive with the model
 *
 * @param std::string the model name
 */
void Controleur::connectionExecToModel(std::string modelName) {
    std::string outputPortName = modelName + "_toPerturb";
    addOutputPort(getModelName(), outputPortName);
    addConnection(getModelName(), outputPortName, modelName, "perturb");
}

/**
 * @brief Delete models
 *
 * @param std::vector<std::string> list of the model name to remove
 */
void Controleur::delInstruction(std::vector<std::string> words) {
    for (unsigned int i=1; i<words.size(); i++) {
        delOneModel(words[i]);
    }
}

/**
 * @brief Remove the input port of the executive associated to the model
 *
 * @param std::string model name
 */
void Controleur::removeInputPortExec(std::string modelName) {
    std::vector<std::string> toRemove;
    std::map<std::string, vpz::ModelPortList> portList = getModel().getInputPortList();
    for (std::map<std::string, vpz::ModelPortList>::iterator it=portList.begin(); it!=portList.end(); ++it){
        std::string temp = getModelNameFromPort(it->first);
        if (temp == modelName) {
            toRemove.push_back(it->first);
        }
    }
    for (unsigned int i=0; i<toRemove.size(); i++) {
        removeInputPort(getModelName(), toRemove[i]);
    }

}

/**
 * @brief Remove the output port of the executive associated to pertub port of modelName
 *
 * @param std::string
 */
void Controleur::removeOutputPortExec(std::string modelName) {
    removeOutputPort(getModelName(), modelName + "_toPerturb");
}

void Controleur::putInStructure(std::string modelName, std::string variable, vle::value::Value* value) {
    std::map<std::string,std::map <std::string, vle::value::Value*> >::iterator it = mData.find(modelName);
    if (it == mData.end()){
        std::map <std::string, vle::value::Value*> secondMap;
        secondMap.insert(std::pair<std::string,vle::value::Value*>(variable, value->clone()));
        mData.insert(std::pair<std::string,std::map <std::string, vle::value::Value*> >(modelName, secondMap));

    } else {
        std::map <std::string, vle::value::Value*>& temp = mData.find(modelName)->second;
        if (!temp.insert(std::pair<std::string,vle::value::Value*>(variable, value->clone())).second) {
            temp[variable] = value->clone();
        }
    }

}

void Controleur::updateData(const vd::ExternalEventList& events) {
    for (unsigned int i=0; i<events.size(); i++) {
        std::string s = events[i]->getPortName();
        std::string variable = events[i]->getAttributes().get("name")->toString().value();

        putInStructure(getModelNameFromPort(s), variable, events[i]->getAttributes().get("value"));
    }
}

double Controleur::getData(std::string modelName, std::string varName) {
std::map<std::string,std::map <std::string, vle::value::Value*> >::iterator it = mData.find(modelName);
    if (it == mData.end())
        throw utils::ArgError(fmt("getData, invalid parameters `%1%' ") % modelName);
    std::map <std::string, vle::value::Value*>::iterator it2 = it->second.find(varName);
    if (it2 == it->second.end())
        throw utils::ArgError(fmt("No varName `%1%' ") % varName);
    return it2->second->toDouble().value();
}

double Controleur::getData(std::string className, int n, std::string varName) {
    return getData(getModelNameFromClassNb(className,n),varName);
}

void Controleur::setModelValue(std::string className, int n, std::string varName, double varValue) {
    setModelValue(getModelNameFromClassNb(className,n), varName, varValue);
}

void Controleur::setModelValue(std::string modelName, std::string varName, double varValue) {
    Message m(getModelName(),modelName,"");
    m.add("name",vv::String::create(varName));
    m.add("value",vv::Double::create(varValue));
    sendMessage(m);
}

std::string Controleur::getModelNameFromClassNb(std::string className, int n) {
    int i = 0;
    for (std::map <std::string, std::map <std::string, vle::value::Value*> >::iterator it = mData.begin(); it!=mData.end(); ++it) {
        if (compareModelClass(it->first, className)) {
            i++;
            if (i == n) {
                return it->first;
            }
        }
    }
    throw utils::ArgError(fmt("Not enough `%1%' ") % className);
}

bool Controleur::compareModelClass(std::string modelName, std::string className) {
    if (modelName.substr(0, className.size()) == className) {
        return true;
    }
    return false;
}

std::string Controleur::getModelNameFromPort(std::string s) {
    unsigned i = s.find_last_of("_");
    return s.substr(0, i);
}

int Controleur::countModelOfClass(std::string className) {
    int i = 0;
    for (std::map <std::string, std::map <std::string, vle::value::Value*> >::iterator it = mData.begin(); it!=mData.end(); ++it) {
        if (compareModelClass(it->first, className)) {
            i++;
        }
    }
    return i;
}

double Controleur::getTime() {
    return mCurrentTime;
}

int Controleur::PrintErrorMessageOrNothing(int errorCode)
{
   if (errorCode != 0) {
		throw utils::ArgError(fmt("Lua Error Code: `%1%' ") % lua_tostring(L, -1));
	}
   return errorCode;
}

}}}} // namespace vle gvle global ibminsidegvle

DECLARE_EXECUTIVE(vle::gvle::global::ibminsidegvle::Controleur)
