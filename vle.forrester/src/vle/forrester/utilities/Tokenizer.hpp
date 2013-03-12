/*
 * @file vle/forrester/utilities/Tokenizer.hpp
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
#ifndef TOKENIZER_HPP
#define TOKENIZER_HPP

#include <string>

namespace vle {
namespace gvle{
namespace modeling{
namespace forrester{
namespace utilities {
/**
 * @class Tokenizer
 * @brief String splitter
 */
class Tokenizer
{
public:
    typedef struct {
        std::string string;  /**< The extracted string */
        size_t beginPosition; /**< The begining position in original string */
        size_t endPosition;  /**< The ending position in original string */
    } token;

    /** @param str The parsed string */
    Tokenizer(const std::string&);

    /** @param str The parsed string
     @param delimiters Delimiters caracters */
    Tokenizer(const std::string&, const std::string&);

    /** @param delimiter Delimiter caracters
    @return true if another token is available
    @return false if all tokens have been treated */
    bool nextToken(const std::string&);

    /** @return true if another token is available
    @return false if all tokens have been treated */
    inline bool nextToken()
    { return nextToken(mDelimiters); }

    /** @return Return the last built token */
    inline const token& getToken() const
    { return mToken; }

    static const std::string DEFAULT_DELIMITERS;
private:
    Tokenizer() {}
    size_t mOffset;

    const std::string mString;
    token mToken;
    std::string mDelimiters;
};

}}}}} // namespace vle gvle modeling forrester utils

#endif
