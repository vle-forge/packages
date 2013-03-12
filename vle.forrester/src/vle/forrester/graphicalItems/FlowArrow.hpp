/*
 * @file vle/forrester/graphicalItems/FlowArrow.hpp
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

#ifndef VLE_GVLE_MODELING_FORRESTER_GRAPHICALITEMS_FLOWARROW_HPP
#define VLE_GVLE_MODELING_FORRESTER_GRAPHICALITEMS_FLOWARROW_HPP

#include <vle/forrester/graphicalItems/Arrow.hpp>
namespace vle {
namespace gvle {
namespace modeling {
namespace forrester {

/** @class FlowArrow
 *  @brief Flow arrow class
 */
class FlowArrow : public Arrow
{
public:
    FlowArrow():Arrow()
    { }

    FlowArrow(Anchor* _origin, Anchor* _destination)
    :Arrow(_origin,_destination)
    { }

    void draw(const Cairo::RefPtr<Cairo::Context>&) const;

    void drawAnchors(const Cairo::RefPtr<Cairo::Context>& ctx) const
    {
        mOrigin->draw(ctx);
        mDestination->draw(ctx);
    }

    void generateSource (utils::Template& tpl) const;

    std::string toString()const;

    bool select(int x, int y);

    bool valid();
};

}
}
}
} // namespace vle gvle modeling forrester

#endif
