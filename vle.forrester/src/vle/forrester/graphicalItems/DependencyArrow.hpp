/*
 * @file vle/forrester/graphicalItems/DependencyArrow.hpp
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

#ifndef VLE_GVLE_MODELING_FORRESTER_GRAPHICALITEMS_DEPENDENCYARROW_HPP
#define VLE_GVLE_MODELING_FORRESTER_GRAPHICALITEMS_DEPENDENCYARROW_HPP

#include <vle/forrester/graphicalItems/Arrow.hpp>

namespace vle {
namespace gvle {
namespace modeling {
namespace forrester {

/** @class DependencyArrow
 * @brief Dependency arrow class
 *
 * This arrow is drawn with a bezier curve
 */
class DependencyArrow : public Arrow
{
public:
    DependencyArrow():Arrow(),mControlPoint()
    { }

    DependencyArrow(Anchor* _origin, Anchor* _destination,int cPX, int cPY)
    :Arrow(_origin,_destination),mControlPoint(cPX,cPY)
    { }

    void draw(const Cairo::RefPtr<Cairo::Context>&) const;

    void drawAnchors(const Cairo::RefPtr<Cairo::Context>& ctx) const
    {
        mControlPoint.draw(ctx);
    }

    std::string toString()const;

    bool select(int x, int y);

    void setDestination(Anchor* destination);

    void generateSource (utils::Template& _tpl) const;

    void computeControlPoint();

    Anchor& getControlPoint()
    { return mControlPoint; }

    bool valid();

private:
    Anchor mControlPoint;
};


}
}
}
} // namespace vle gvle modeling forrester

#endif
