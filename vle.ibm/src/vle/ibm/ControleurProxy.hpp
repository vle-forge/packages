/*
 * @file vle/ibm/ControleurProxy.hpp
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
#ifndef CONTROLEURPROXY_HPP
#define CONTROLEURPROXY_HPP

#include <boost/lexical_cast.hpp>

#include <vle/value/Value.hpp>
#include <vle/devs/Executive.hpp>
#include <vle/value/Map.hpp>
#include <vle/utils/Rand.hpp>

#include<math.h>

extern "C" {
#include "lua.h"
#include "lauxlib.h"
#include "lualib.h"
}

#include "lunar.h"

namespace vd = vle::devs;
namespace vp = vle::vpz;
namespace vu = vle::utils;

namespace vle {
namespace gvle {
namespace global {
namespace ibminsidegvle {

class Controleur;

class ControleurProxy {
    enum functionType { INIT, SINGLE, RECUR};

    Controleur* mControleur;

public:
    static const char className[];
    static Lunar<ControleurProxy>::RegType methods[];

    ControleurProxy(lua_State */*L*/) {}
    ControleurProxy() {}

    void setControleur ( Controleur* controleur);

    int addModel(lua_State *L);
    int addModelWithParam(lua_State *L);
    int delModel(lua_State *L);
    int getModelValue(lua_State *L);
    int getNumber(lua_State *L);
    int setModelValue(lua_State *L);
    int addGlobalVar(lua_State *L);
    int getModelName(lua_State *L);
    int getTime(lua_State *L);
    int addEvent(lua_State *L);
    int getParam(lua_State *L);
    double getValueFromParam(lua_State *L, int i);

    ~ControleurProxy();
};

}}}} // namespace vle gvle global ibminsidegvle

#endif
