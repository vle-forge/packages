/*
 * @file vle/forrester/graphicalItems/Arrow.hpp
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

#ifndef VLE_GVLE_MODELING_FORRESTER_GRAPHICALITEMS_ARROW_HPP
#define VLE_GVLE_MODELING_FORRESTER_GRAPHICALITEMS_ARROW_HPP

#include <vle/forrester/graphicalItems/GraphicalItem.hpp>
#include <vle/utils/Template.hpp>

namespace vle {
namespace gvle {
namespace modeling {
namespace forrester {

class Arrow;

typedef std::list<Arrow*> arrows;

/**
 * @class Arrow
 * @brief This abstract class provide a generic interface to build arrows
 *
 * An arrow is a graphical object which have two anchors : an origin and
 * a destination.
 */
class Arrow
{
public:
    Anchor* getOrigin() const
    { return mOrigin; }

    Anchor* getDestination() const
    { return mDestination; }

    virtual void setOrigin(Anchor* origin)
    { mOrigin = origin; }

    virtual void setDestination(Anchor* destination)
    { mDestination = destination; }

    virtual void draw(const Cairo::RefPtr<Cairo::Context>&) const = 0;

    virtual void drawAnchors(const Cairo::RefPtr<Cairo::Context>&) const = 0;

    /** Save function.
     * @return a string which contains all informations about the arrow */
    virtual std::string toString()const = 0;

    /** @return True if the arrow is selected, false otherwise */
    virtual bool select(int x, int y) = 0;

    /** @return True if the arrow is valid, false otherwise */
    virtual bool valid() = 0;

    virtual ~Arrow()
    { }

    friend bool operator== (const Arrow &a1, const Arrow &a2);

    /** @return True if the arrows aro opposed, false otherwise */
    virtual bool isOpposed (const Arrow &a2)
    {
        return (mOrigin->getOwner() == a2.getDestination()->getOwner()) &&
                (mDestination->getOwner() == a2.getOrigin()->getOwner());
    }

    /** Generate the c++ source code corresponding to the arrow */
    virtual void generateSource (utils::Template& _tpl) const  = 0;

protected:
    Arrow():mOrigin(0), mDestination(0)
    { }
    Arrow(Anchor* origin, Anchor* destination)
    :mOrigin(origin), mDestination(destination)
    { }

    Anchor* mOrigin;
    Anchor* mDestination;
};





}
}
}
} // namespace vle gvle modeling forrester

#endif
