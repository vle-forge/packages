/*
 * @file vle/forrester/graphicalItems/Anchor.cpp
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
#include <vle/forrester/graphicalItems/GraphicalItem.hpp>
#include <vle/forrester/graphicalItems/Anchor.hpp>
#include <vle/utils/i18n.hpp>


namespace vle {
namespace gvle {
namespace modeling {
namespace forrester {

const int Anchor::ANCHOR_HEIGHT = 5;
const int Anchor::ANCHOR_WIDTH = 5;

bool Anchor::select(int _x, int _y) const
{
        return (_x <= mX + ANCHOR_WIDTH + 2) &&
        (_y <= mY + ANCHOR_HEIGHT + 1) &&
        (_x > mX - 2) && (_y > mY - 2);
}

void Anchor::draw(const Cairo::RefPtr<Cairo::Context>& context) const
{
        context->save();
        context->move_to(mX,mY);
        context->set_source_rgb(0., 1., 0.);
        context->rectangle(mX, mY, ANCHOR_WIDTH, ANCHOR_HEIGHT);
        context->fill();
        context->restore();
}

Anchor* Anchor::closestAnchor(const Anchor& origin, anchors_t& destinations) {
    double minimalDistance;
    int index = 0;

    minimalDistance = std::sqrt(
        std::pow(destinations[index]->getX() - origin.getX(),2) +
        std::pow(destinations[index]->getY() - origin.getY(),2));

    for (int i = 1; i < (int)destinations.size(); i++) {
        double distance = std::sqrt(
        std::pow(destinations[i]->getX() - origin.getX(),2) +
        std::pow(destinations[i]->getY() - origin.getY(),2));

        if(distance < minimalDistance) {
            index = i;
            minimalDistance = distance;
        }
    }
    return destinations[index];
}

}
}
}
} // namespace vle gvle modeling forrester


