/*
 * @file vle/forrester/graphicalItems/ForresterGI.cpp
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

#include <vle/forrester/graphicalItems/ForresterGI.hpp>
#include <vle/utils/i18n.hpp>
#include <boost/algorithm/string.hpp>
#include <boost/lexical_cast.hpp>

namespace vle {
namespace gvle {
namespace modeling {
namespace forrester {

const int ForresterGI::INITIAL_HEIGHT = 400;
const int ForresterGI::INITIAL_WIDTH = 400;
const int ForresterGI::MINIMAL_HEIGHT = 400;
const int ForresterGI::MINIMAL_WIDTH = 400;
const int ForresterGI::GLOBAL_OFFSET = 10;
const int ForresterGI::HEADER_HEIGHT = 20;

bool ForresterGI::select(int _x, int _y) const
{
    int x = _x - GLOBAL_OFFSET;
    int y = _y - GLOBAL_OFFSET;

    return ((x >= static_cast<int>(mWidth) - 20) and
        x <= static_cast<int>(mWidth) and
        y >= static_cast<int>(mHeight) - 20 and
        y <= static_cast<int>(mHeight));
}

void ForresterGI::draw(const Cairo::RefPtr<Cairo::Context>& context)
{
    setColor(Settings::settings().getForegroundColor(), context);

    drawRoundedRectangle(context, GLOBAL_OFFSET, GLOBAL_OFFSET,
    mWidth - GLOBAL_OFFSET, mHeight - GLOBAL_OFFSET, 1.0, 20, 1., 1., 1.);

    context->move_to(GLOBAL_OFFSET, GLOBAL_OFFSET + HEADER_HEIGHT);
    context->line_to(
        mWidth, GLOBAL_OFFSET + HEADER_HEIGHT);
    context->stroke();

    drawName(context);
}

void ForresterGI::checkSize(const Cairo::RefPtr<Cairo::Context>&)
{ }

void ForresterGI::drawName(const Cairo::RefPtr<Cairo::Context>& context)
{
    setColor(Settings::settings().getForegroundColor(), context);
    context->select_font_face(Settings::settings().getFont(),
        Cairo::FONT_SLANT_NORMAL,
        Cairo::FONT_WEIGHT_NORMAL);
    context->set_font_size(Settings::settings().getFontSize());

    Cairo::TextExtents textExtents;
    context->get_text_extents(mName, textExtents);

    context->move_to(
        (mWidth - textExtents.width) / 2 + GLOBAL_OFFSET,
        GLOBAL_OFFSET + (HEADER_HEIGHT + textExtents.height) / 2);
    context->show_text(mName);
    context->stroke();
}

void ForresterGI::generateSource (utils::Template& tpl_) const {
    tpl_.stringSymbol().append("width",
        boost::lexical_cast < std::string > (mWidth));
    tpl_.stringSymbol().append("height",
        boost::lexical_cast < std::string > (mHeight));
}

}
}
}
} // namespace vle gvle modeling forrester
