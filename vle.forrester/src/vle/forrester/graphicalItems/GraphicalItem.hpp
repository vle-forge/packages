/*
 * @file vle/forrester/graphicalItems/GraphicalItem.hpp
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

#ifndef VLE_GVLE_MODELING_FORRESTER_GRAPHICALITEMS_GRAPHICAL_ITEM_HPP
#define VLE_GVLE_MODELING_FORRESTER_GRAPHICALITEMS_GRAPHICAL_ITEM_HPP

#include <string>
#include <list>
#include <gtkmm/builder.h>

#include <vle/forrester/graphicalItems/Anchor.hpp>
#include <vle/vpz/AtomicModel.hpp>
#include <vle/vpz/Condition.hpp>
#include <vle/gvle/ModelingPlugin.hpp>
#include <vle/utils/Template.hpp>
#include <vle/gvle/Settings.hpp>

namespace vle {
namespace gvle {
namespace modeling {
namespace forrester {

class GraphicalItem;
class Forrester;

typedef std::list<GraphicalItem *> graphical_items;

/**
 * @class GraphicalItem
 * @brief Class used to build graphical items
 *
 * This abstract class allows to define a graphical item which will be
 * used in the drawing area. Several methods for drawing et saving are
 * present, and must be define in inherited classes.
 */
class GraphicalItem
{
public:
    //Getters
    /** @return x position */
    virtual int getX() const
    { return mX; }

    /** @return y position */
    virtual int getY() const
    { return mY; }

    /** @return width of the graphical item */
    virtual int getWidth() const
    { return mWidth; }

    /** @return height of the graphical item */
    virtual int getHeight() const
    { return mHeight; }

    /** @return name of the graphical item */
    virtual const std::string& getName() const
    { return mName; }

    /** @return anchors of the graphical item */
    virtual anchors_t& getAnchors()
    { return mAnchors; }

    //Setters
    /** Set x position*/
    virtual void setX(int _x);

    /** Set y position*/
    virtual void setY(int _y);

    /** Set width of the graphical item*/
    virtual void setWidth(int _width)
    { mWidth = _width; }

    /** Set height of the graphical item*/
    virtual void setHeight(int _height)
    { mHeight = _height; }

    /** Set name of the graphical item*/
    virtual void setName(const std::string& _name)
    { mName = _name; }

    ////Virtual functions
    //Graphical functions
    /** Draw method. Used by the plugin to draw the item on the drawing area*/
    virtual void draw(const Cairo::RefPtr<Cairo::Context>&) = 0;

    /** Draw method. Draw anchors of the graphical item*/
    virtual void drawAnchors(const Cairo::RefPtr<Cairo::Context>&);

    /** @return true if the item is selected, false otherwise*/
    virtual bool select(int x, int y) const = 0;

    /**
     * Launch a window to buit new item.
     * @return true if the item must be built, false otherwise
     */
    virtual bool launchCreationWindow(
        const Glib::RefPtr < Gtk::Builder >&) = 0;

    /**
     * Launch a window to edit item.
     */
    virtual void launchEditionWindow(
        const Glib::RefPtr < Gtk::Builder >&) = 0;

    /** Check the size of the item, and resize it*/
    virtual void checkSize(const Cairo::RefPtr<Cairo::Context>&) = 0;

    /** @return top offset of the item*/
    virtual int getTopOffset() const
    { return 0; }

    /** @return left offset of the item*/
    virtual int getLeftOffset() const
    { return 0; }

    /** @return right offset of the item*/
    virtual int getRightOffset() const
    { return 0; }

    /** @return bottom offset of the item*/
    virtual int getBottomOffset() const
    { return 0; }

    /** Resize the item to a new width and height*/
    virtual void resize(int _width, int _height)
    {
        mWidth = _width;
        mHeight = _height;
    }

    /** @return tooltip text displayed when user select the item*/
    virtual std::string tooltipText();

    /** Displace the item*/
    virtual void displace(int deltax, int deltay);

    //Data storing functions
    /** @brief Save method.
     * @return a string which contains item's informations */
    virtual std::string toString() const = 0;

    /** @brief Save method.
     * Configure atomic model with informations of the item */
    virtual void generatePorts (vpz::AtomicModel&) const  = 0;

    /** @brief Save method.
     * Configure observable with informations of the item */
    virtual void generateObservable (vpz::Observable&) const  = 0;

    /** @brief Save method.
     * Gerate the c++ code for the item */
    virtual void generateSource (utils::Template&) const  = 0;


    //Anchors management
    /** @return the selected anchor, or null */
    virtual Anchor* selectAnchor(int x, int y);

    /** @return the index of the anchor */
    virtual int getIndex(const Anchor*)const;

    /** @return the anchor corresponding to the index */
    virtual Anchor* getAnchor(int);

    virtual ~GraphicalItem();

    friend bool operator==(const GraphicalItem& a,const GraphicalItem& b);

    //Constants
    static const int TEXT_OFFSET;

protected:
    GraphicalItem(const Forrester& f):mName(),mAnchors(),mForrester(f)
    { }

    GraphicalItem(int _x, int _y, int _width, int _height,
        const std::string& _name, const Forrester& f)
    :mX(_x), mY(_y), mWidth(_width),
    mHeight(_height), mName(_name),
    mAnchors(), mForrester(f)
    { }

    /** @brief Drawing method
     * Draw the name of the item */
    virtual void drawName(const Cairo::RefPtr<Cairo::Context>&) = 0;

    /** @brief Drawing method
     * Draw a rounded rectangle */
    void drawRoundedRectangle(const Cairo::RefPtr<Cairo::Context>&,
        int x, int y,
        int width, int height,
        double aspect,
        double corner_radius,
        double red, double green, double blue);

    /** @brief Drawing method
     * Draw a rectangle */
    void drawRectangle(const Cairo::RefPtr<Cairo::Context>&,
        int x, int y,
        int width, int height,
        double red, double green, double blue);

    /** Set the color of the graphical area */
    void setColor(const Gdk::Color&,
        const Cairo::RefPtr<Cairo::Context>&);

    int mX;                         /**< x postion*/
    int mY;                         /**< y postion*/
    int mWidth;                     /**< width of the item*/
    int mHeight;                    /**< height of the item*/
    std::string mName;               /**< name of the item*/
    anchors_t mAnchors;             /**< anchors of the item*/
    const Forrester& mForrester;   /**< Reference to the Forrester object*/
};


}
}
}
} // namespace vle gvle modeling forrester

#endif
