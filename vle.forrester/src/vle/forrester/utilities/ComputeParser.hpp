/*
 * @file vle/forrester/utilities/ComputeParser.hpp
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
#ifndef COMPUTE_PARSER_HPP
#define COMPUTE_PARSER_HPP
#include <muParser.h>
#include <map>

namespace vle {
namespace gvle{
namespace modeling{
namespace forrester{
namespace utilities {

/**
 * @class ComputeParser
 * @brief Mathematical parser
 */
class ComputeParser : public mu::Parser {
public:
    ComputeParser();

    /** Add a variable to parser */
    void addVariable(const std::string& name, double initialValue = 1) {
        double *tmp = new double(initialValue);
        mVariables.push_back(tmp);
        DefineVar(name, tmp);
    }

    /** Add a constant to parser */
    void addConstant(const std::string& name, double value = 1) {
        DefineConst(name, value);
    }

    virtual ~ComputeParser ();
protected:
    //Functions
    static mu::value_type  sin(mu::value_type);
    static mu::value_type  cos(mu::value_type);
    static mu::value_type  tan(mu::value_type);
    static mu::value_type  exp(mu::value_type);
    static mu::value_type  abs(mu::value_type);
    static mu::value_type  sqrt(mu::value_type);
    static mu::value_type power(mu::value_type, mu::value_type);

    //Operators
    static mu::value_type addition(mu::value_type, mu::value_type);
    static mu::value_type substraction(mu::value_type, mu::value_type);
    static mu::value_type multi(mu::value_type, mu::value_type);
    static mu::value_type division(mu::value_type, mu::value_type);

    //Logical Operators
    static mu::value_type logical_and(mu::value_type, mu::value_type);
    static mu::value_type logical_or(mu::value_type, mu::value_type);
    static mu::value_type less_or_equal(mu::value_type, mu::value_type);
    static mu::value_type greater_or_equal(mu::value_type, mu::value_type);
    static mu::value_type not_equal(mu::value_type, mu::value_type);
    static mu::value_type equal(mu::value_type, mu::value_type);
    static mu::value_type greater_than(mu::value_type, mu::value_type);
    static mu::value_type less_than(mu::value_type, mu::value_type);

private:
    std::vector<double*> mVariables;
};

}}}}} // namespace vle gvle modeling forrester utils
#endif
