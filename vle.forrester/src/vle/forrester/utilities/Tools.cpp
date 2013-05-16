/*
 * @file vle/forrester/graphicalItems/Variable.cpp
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

#include <vle/forrester/utilities/Tools.hpp>
#include <vle/forrester/utilities/Tokenizer.hpp>
#include <vle/forrester/graphicalItems/ExogenousVariable.hpp>

#include <vector>

namespace vle {
namespace gvle {
namespace modeling {
namespace forrester {
namespace utilities {

void generateParenthesis(std::string& equation, const Forrester& forrester)
{
    std::vector<utilities::Tokenizer::token> elements;
    int insertedCaracters = 0;
    utilities::Tokenizer tokenizer(equation,"<>=&| +-*/()\t\n\r");

    while(tokenizer.nextToken())
        elements.push_back(tokenizer.getToken());

    for(std::vector<utilities::Tokenizer::token>::const_iterator it =
    elements.begin(); it != elements.end(); ++it) {

        if(dynamic_cast<ExogenousVariable*>(forrester.getItem(it->string))) {
            equation.insert(it->endPosition + insertedCaracters,"()");
            insertedCaracters += 2;
        }

        if(dynamic_cast<Compartment*>(forrester.getItem(it->string))) {
            equation.insert(it->endPosition + insertedCaracters,"()");
            insertedCaracters += 2;
        }
    }
}

void generateStdPrefix(std::string& equation)
{
    std::vector<utilities::Tokenizer::token> elements;
    int insertedCaracters = 0;
    utilities::Tokenizer tokenizer(equation," +-*/()\t\n\r");

    while(tokenizer.nextToken())
        elements.push_back(tokenizer.getToken());

    for(std::vector<utilities::Tokenizer::token>::const_iterator it =
    elements.begin(); it != elements.end(); ++it) {
        for(int i = 0; i < STL_FUNCTION_COUNT; i++){
            if(it->string == STL_FUNCTION[i]) {
                equation.insert(it->beginPosition + insertedCaracters,"std::");
                insertedCaracters += 5;
            }
        }
    }
}

}
}
}
}
} // namespace vle gvle modeling forrester utilities
