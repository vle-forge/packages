/*
 * @file vle/forrester/utilities/ComputeParser.cpp
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
#include "ComputeParser.hpp"

namespace vle {
namespace gvle{
namespace modeling{
namespace forrester{
namespace utilities {

ComputeParser::ComputeParser() {
    //Delete default binary operators and default functions
    EnableBuiltInOprt(false);
    ClearFun();

    //Adding functions
    DefineFun("sin", sin);
    DefineFun("cos", cos);
    DefineFun("tan", tan);
    DefineFun("exp", exp);
    DefineFun("abs", abs);
    DefineFun("sqrt", sqrt);
    DefineFun("pow", pow);

    //Adding operators
    DefineOprt("+", addition, mu::prADD_SUB, mu::oaLEFT, true);
    DefineOprt("-", substraction, mu::prADD_SUB, mu::oaLEFT, true);
    DefineOprt("*", multi, mu::prMUL_DIV, mu::oaLEFT, true);
    DefineOprt("/", division, mu::prMUL_DIV, mu::oaLEFT, true);

    //Adding Logical operators
    DefineOprt("&&", logical_and, mu::prLOGIC, mu::oaLEFT, true);
    DefineOprt("||", logical_or, mu::prLOGIC, mu::oaLEFT, true);
    DefineOprt("<=", less_or_equal, mu::prCMP, mu::oaLEFT, true);
    DefineOprt(">=", greater_or_equal, mu::prCMP, mu::oaLEFT, true);
    DefineOprt("!=", not_equal, mu::prCMP, mu::oaLEFT, true);
    DefineOprt("==", equal, mu::prCMP, mu::oaLEFT, true);
    DefineOprt(">", greater_than, mu::prCMP, mu::oaLEFT, true);
    DefineOprt("<", less_than, mu::prCMP, mu::oaLEFT, true);
}


// Defining binary operators
mu::value_type ComputeParser::addition(mu::value_type a, mu::value_type b) {
    return a + b;
}
mu::value_type ComputeParser::substraction(mu::value_type a, mu::value_type b) {
    return a - b;
}
mu::value_type ComputeParser::multi(mu::value_type a, mu::value_type b) {
    return a * b;
}
mu::value_type ComputeParser::division(mu::value_type a, mu::value_type b) {
    return a / b;
}

//Defining logical operators
mu::value_type ComputeParser::logical_and(mu::value_type a, mu::value_type b) {
    return a && b;
}
mu::value_type ComputeParser::logical_or(mu::value_type a, mu::value_type b) {
    return a || b;
}
mu::value_type ComputeParser::less_or_equal(mu::value_type a, mu::value_type b) {
    return a <= b;
}
mu::value_type ComputeParser::greater_or_equal(mu::value_type a, mu::value_type b){
    return a >= b;
}
mu::value_type ComputeParser::not_equal(mu::value_type a, mu::value_type b){
    return a != b;
}
mu::value_type ComputeParser::equal(mu::value_type a, mu::value_type b){
    return a == b;
}
mu::value_type ComputeParser::greater_than(mu::value_type a, mu::value_type b){
    return a > b;
}
mu::value_type ComputeParser::less_than(mu::value_type a, mu::value_type b){
    return a < b;
}

//Defining parser functions
mu::value_type ComputeParser::sin(mu::value_type v){
    return std::sin(v);
}

mu::value_type ComputeParser::cos(mu::value_type v){
    return std::cos(v);
}

mu::value_type ComputeParser::tan(mu::value_type v){
    return std::tan(v);
}

mu::value_type ComputeParser::exp(mu::value_type v){
    return std::exp(v);
}

mu::value_type ComputeParser::abs(mu::value_type v){
    return std::abs(v);
}

mu::value_type ComputeParser::sqrt(mu::value_type v){
    return std::sqrt(v);
}

mu::value_type ComputeParser::power(mu::value_type a,
                                    mu::value_type b){
    return std::pow(a, b);
}

ComputeParser::~ComputeParser ()
{
    for(unsigned int i=0; i < mVariables.size(); i++)
        delete mVariables[i];
    mVariables.clear();
}
}}}}} // namespace vle gvle modeling forrester utils
