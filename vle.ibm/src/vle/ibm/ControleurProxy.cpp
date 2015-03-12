/*
 * @file vle/ibm/ControleurProxy.cpp
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

namespace vle {
namespace gvle {
namespace global {
namespace ibminsidegvle {

const char ControleurProxy::className[] = "ControleurProxy";

Lunar<ControleurProxy>::RegType ControleurProxy::methods[] = {
    LUNAR_DECLARE_METHOD(ControleurProxy, addModel),
    LUNAR_DECLARE_METHOD(ControleurProxy, addModelWithParam),
    LUNAR_DECLARE_METHOD(ControleurProxy, delModel),
    LUNAR_DECLARE_METHOD(ControleurProxy, getModelValue),
    LUNAR_DECLARE_METHOD(ControleurProxy, getNumber),
    LUNAR_DECLARE_METHOD(ControleurProxy, setModelValue),
    LUNAR_DECLARE_METHOD(ControleurProxy, getModelName),
    LUNAR_DECLARE_METHOD(ControleurProxy, getTime),
    LUNAR_DECLARE_METHOD(ControleurProxy, addEvent),
    LUNAR_DECLARE_METHOD(ControleurProxy, getParam),
    {0,0} };


void ControleurProxy::setControleur ( Controleur* controleur) {mControleur = controleur;}

ControleurProxy::~ControleurProxy() { printf("deleted GodProxy (%p)\n", this); }

int ControleurProxy::addModel(lua_State *L) {
    int nbi = getValueFromParam(L, 1);
    std::string si (luaL_checkstring(L, 2));
    mControleur->addInstruction(nbi, si);
    return 0;
}

int ControleurProxy::addModelWithParam(lua_State *L) {
    std::map<std::string, vv::Value*> variableToModify;
    int top = lua_gettop(L);
    std::string varName = "";

    for (int i = 3; i <= top; i++) {
        if (i%2 != 0) {
            varName = lua_tostring(L, i);
        } else {
            vv::Value* varValue = new vv::Double(getValueFromParam(L, i));
            variableToModify.insert(std::pair<std::string, vle::value::Value* >(varName, varValue));
        }
    }
    int nbi = getValueFromParam(L, 1);
    mControleur->addModelWith(nbi, luaL_checkstring(L, 2), variableToModify);
    return 0;
}

int ControleurProxy::delModel(lua_State *L) {

    for (int i=1; i<=lua_gettop(L); i++) {
        mControleur->delOneModel(lua_tostring(L, i));
    }

    return 0;
}

int ControleurProxy::getModelValue(lua_State *L) {
    std::string modelName (luaL_checkstring(L, 1));

    double d = 0;
    if (lua_gettop(L) == 2) {
        std::string varName (luaL_checkstring(L, 2));
        d = mControleur->getData(modelName, varName);
    } else {
        int nb = luaL_checknumber(L, 2);
        std::string varName (luaL_checkstring(L, 3));
        d = mControleur->getData(modelName, nb, varName);
    }
    lua_pushnumber(L, d);
    return 1;
}

int ControleurProxy::getNumber(lua_State *L) {
    std::string className = luaL_checkstring(L, 1);
    int d = mControleur->countModelOfClass(className);
    lua_pushnumber(L, d);
    return 1;
}

int ControleurProxy::setModelValue(lua_State *L) {
    if (lua_gettop(L) == 4) {
        std::string className (luaL_checkstring(L, 1));
        int i = luaL_checknumber(L, 2);
        std::string varName (luaL_checkstring(L, 3));
        double varValue = getValueFromParam(L, 4);
        mControleur->setModelValue(className, i, varName, varValue);
    } else {
        std::string modelName (luaL_checkstring(L, 1));
        std::string varName (luaL_checkstring(L, 2));
        double varValue = getValueFromParam(L, 3);
        mControleur->setModelValue(modelName, varName, varValue);
    }
    return 0;
}

int ControleurProxy::getModelName(lua_State *L) {
    std::string className (luaL_checkstring(L, 1));
    int i = luaL_checknumber(L, 2);
    std::string name = mControleur->getModelNameFromClassNb(className, i);
    lua_pushstring(L, name.c_str());
    return 1;
}

int ControleurProxy::getTime(lua_State *L) {
    lua_pushnumber(L, mControleur->getTime());
    return 1;
}

int ControleurProxy::addEvent(lua_State *L) {
    std::string type = luaL_checkstring(L, 1);
    if (type == "INIT") {
        std::string script (luaL_checkstring(L, 2));
        script += "()";
        mControleur->execInit(script);
    } else if (type == "SINGLE") {
        mControleur->addEffectAt(luaL_checknumber(L, 2), vd::infinity, luaL_checkstring(L, 3));
    } else if (type == "RECUR") {
        mControleur->addEffectAt(luaL_checknumber(L, 2), luaL_checknumber(L, 3), luaL_checkstring(L, 4));
    }
    return 0;
}

int ControleurProxy::getParam(lua_State *L) {
    double d = mControleur->readNumber(luaL_checkstring(L, 1));
    lua_pushnumber(L, d);
    return 1;
}

double ControleurProxy::getValueFromParam(lua_State *L, int i) {
    double d = 0;
    if (lua_type(L, i) == LUA_TNUMBER){
        d = lua_tonumber(L, i);
    } else {
        d = mControleur->readNumber(luaL_checkstring(L, i));
    }
    return d;
}

}}}} // namespace vle gvle global ibminsidegvle
