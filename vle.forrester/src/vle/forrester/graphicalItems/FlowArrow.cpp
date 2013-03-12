/*
 * @file vle/forrester/graphicalItems/FlowArrow.cpp
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
#include <vle/forrester/graphicalItems/Flow.hpp>
#include <vle/forrester/graphicalItems/Compartment.hpp>
#include <vle/forrester/graphicalItems/FlowArrow.hpp>
#include <boost/algorithm/string.hpp>
#include <boost/lexical_cast.hpp>
#include <vle/utils/i18n.hpp>
namespace vle {
namespace gvle {
namespace modeling {
namespace forrester {

void FlowArrow::draw(const Cairo::RefPtr<Cairo::Context>& context) const
{
    context->save();
    context->set_line_width(1);

    double angle = atan2 (mDestination->getY() - mOrigin->getY(),
        mDestination->getX() - mOrigin->getX()) + M_PI;
    double x1 = mDestination->getX() + 9 * std::cos(angle - 0.35);
    double y1 = mDestination->getY() + 9 * std::sin(angle - 0.35);
    double x2 = mDestination->getX() + 9 * std::cos(angle + 0.35);
    double y2 = mDestination->getY() + 9 * std::sin(angle + 0.35);
    context->move_to(mDestination->getX(), mDestination->getY());
    context->line_to(x1,y1);
    context->line_to(x2,y2);
    context->line_to(mDestination->getX(), mDestination->getY());
    context->fill();

    context->move_to(mOrigin->getX(), mOrigin->getY());
    context->line_to(mDestination->getX(), mDestination->getY());

    context->stroke();
    context->restore();
}

std::string FlowArrow::toString()const
{
    std::string oriIndex, destIndex;

return (fmt("%1%,%2%,%3%,%4%")
            % mOrigin->getOwner()->getName()
            % mDestination->getOwner()->getName()
            % mOrigin->getOwner()->getIndex(mOrigin)
            % mDestination->getOwner()->getIndex(mDestination) ).str();
}

bool FlowArrow::select(int x, int y)
{
    if (mOrigin->select(x,y) or mDestination->select(x,y))
        return true;

    double x1, x2, y1, y2;
    double a, b;

    x1 = static_cast<double>(mOrigin->getX());
    x2 = static_cast<double>(mDestination->getX());
    y1 = static_cast<double>(mOrigin->getY());
    y2 = static_cast<double>(mDestination->getY());

    double aff;
    if (x2 - x1 != 0) {
        a = ((y2 - y1) / (x2 - x1));
        b = ((x2*y1)-(x1*y2))/(x2-x1);
        aff = ((a * x) + b - y);
    } else {
        aff = 0;
    }

    if ((aff <= 40) and (aff >= -40) and (x > std::min(x1,x2) - 3)
    and (y > std::min(y1,y2) - 3) and (x < std::max(x1,x2) + 3)
    and (y < std::max(y1,y2) + 3))
        return true;

    return false;
}

bool FlowArrow::valid() {
    if(mOrigin == 0 && mDestination == 0)
        return true;
    if(dynamic_cast<Compartment*>(mOrigin->getOwner())) {
        if (dynamic_cast<Flow*>(mDestination->getOwner()) ||
        mDestination->getOwner() == 0)
            return true;
        else
            return false;
    }
    if(dynamic_cast<Flow*>(mOrigin->getOwner())) {
        if (dynamic_cast<Compartment*>(mDestination->getOwner()) ||
        mDestination->getOwner() == 0)
            return true;
        else
            return false;
    }
    return false;
}

void FlowArrow::generateSource (utils::Template& tpl) const {
    tpl.listSymbol().append("flowsArrows");
    tpl.listSymbol().append("flowsArrows", toString());
}

}
}
}
} // namespace vle gvle modeling forrester
