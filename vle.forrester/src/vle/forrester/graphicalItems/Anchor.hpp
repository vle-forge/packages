/*
 * @file vle/forrester/graphicalItems/Anchor.hpp
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


#ifndef VLE_GVLE_MODELING_FORRESTER_GRAPHICALITEMS_ANCHOR_HPP
#define VLE_GVLE_MODELING_FORRESTER_GRAPHICALITEMS_ANCHOR_HPP

namespace vle {
namespace gvle {
namespace modeling {
namespace forrester {

class GraphicalItem;
class Anchor;

typedef std::vector<Anchor *> anchors_t;

/**
 * @brief Anchor class
 *
 * Anchors are graphical elements which can be used to handle an object.
 * They have an owner, a GraphicalItem.
 */
class Anchor
{
public:
    Anchor()
        :mX(-1), mY(-1), mOwner(0)
    { }
    Anchor(int _x, int _y)
        :mX(_x), mY(_y), mOwner(0)
    { }

    ~Anchor()
    { }

    /** @return true if the anchor is selected, false otherwise */
    bool select(int x, int y) const;

    /** Draw method */
    void draw(const Cairo::RefPtr<Cairo::Context>&) const;

    //Getters
    int getX() const
    { return mX; }

    int getY() const
    { return mY; }

    GraphicalItem* getOwner() const
    { return mOwner; }

    //Setters
    void setX(int _x)
    { mX = _x; }

    void setY(int _y)
    { mY = _y; }

    void setOwner(GraphicalItem* _owner)
    { mOwner = _owner; }

    /** @return Return the closest anchor of the list of anchors*/
    static Anchor* closestAnchor(const Anchor&, anchors_t&);

    static const int ANCHOR_HEIGHT;
    static const int ANCHOR_WIDTH;
private:
    int mX;
    int mY;
    GraphicalItem* mOwner;
};


}
}
}
}    // namespace vle gvle modeling forrester

#endif

