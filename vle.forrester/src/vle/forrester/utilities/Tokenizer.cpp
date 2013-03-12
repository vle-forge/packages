/*
 * @file vle/forrester/utilities/Tokenizer.cpp
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
#include "Tokenizer.hpp"

namespace vle {
namespace gvle{
namespace modeling{
namespace forrester{
namespace utilities {

const std::string Tokenizer::DEFAULT_DELIMITERS(" \t\n\r");

Tokenizer::Tokenizer(const std::string& s) :
    mOffset(0),
    mString(s),
    mDelimiters(DEFAULT_DELIMITERS) {}

Tokenizer::Tokenizer(const std::string& s,
                        const std::string& delimiters) :
    mOffset(0),
    mString(s),
    mDelimiters(delimiters) {}

bool Tokenizer::nextToken(const std::string& delimiters)
{
    size_t i = mString.find_first_not_of(delimiters, mOffset);
    if (std::string::npos == i) {
        mOffset = mString.length();
        return false;
    }

    mToken.beginPosition = i;

    size_t j = mString.find_first_of(delimiters, i);
    if (std::string::npos == j) {
        mToken.string = mString.substr(i);
        mToken.endPosition = i + mToken.string.length();
        mOffset = mString.length();
        return true;
    }

    mToken.string = mString.substr(i, j - i);
    mToken.endPosition = j;
    mOffset = j;
    return true;
}

}}}}} // namespace vle gvle modeling forrester utils
