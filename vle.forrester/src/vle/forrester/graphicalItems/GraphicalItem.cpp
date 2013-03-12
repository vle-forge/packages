/*
 * @file vle/forrester/graphicalItems/GraphicalItem.cpp
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
#include <vle/forrester/Forrester.hpp>

namespace vle {
namespace gvle {
namespace modeling {
namespace forrester {

const int GraphicalItem::TEXT_OFFSET = 10;


void GraphicalItem::drawRoundedRectangle(
    const Cairo::RefPtr<Cairo::Context>& context,
    int x, int y,
    int width, int height,
    double aspect,
    double corner_radius,
    double red, double green, double blue)
{
    double radius = corner_radius / aspect;
    double degrees = M_PI / 180.0;

    context->begin_new_sub_path();
    context->arc(x + width - radius, y + radius, radius,
        -90 * degrees, 0 * degrees);
    context->arc(x + width - radius, y + height - radius,
        radius, 0 * degrees, 90 * degrees);
    context->arc(x + radius, y + height - radius, radius,
        90 * degrees, 180 * degrees);
    context->arc(x + radius, y + radius, radius,
        180 * degrees, 270 * degrees);
    context->close_path();

    context->set_source_rgb(red, green, blue);
    context->fill_preserve();
    setColor(Settings::settings().getForegroundColor(), context);
    context->stroke();
}

void GraphicalItem::drawRectangle(const Cairo::RefPtr<Cairo::Context>& context,
    int x, int y,
    int width, int height,
    double red, double green, double blue)
{
    context->begin_new_sub_path();

    context->move_to(x, y);
    context->line_to(x + width, y);
    context->line_to(x + width, y + height);
    context->line_to(x, y + height);
    context->line_to(x, y);

    context->close_path();

    context->set_source_rgb(red, green, blue);
    context->fill_preserve();
    setColor(Settings::settings().getForegroundColor(), context);
    context->stroke();
}

void GraphicalItem::drawAnchors(const Cairo::RefPtr<Cairo::Context>& context)
{
    for (anchors_t::const_iterator it = mAnchors.begin();
    it != mAnchors.end(); ++it) {
        (*it)->draw(context);
    }
}

Anchor* GraphicalItem::selectAnchor(int x, int y)
{
    for (anchors_t::const_iterator it = mAnchors.begin();
        it != mAnchors.end(); ++it)
    {
        if((*it)->select(x, y))
            return *it;
    }
    return 0;
}

void GraphicalItem::displace(int deltax, int deltay)
{
    mX += deltax;
    mY += deltay;

    for (anchors_t::const_iterator it = mAnchors.begin();
    it != mAnchors.end(); ++it) {
        (*it)->setX((*it)->getX()+deltax);
        (*it)->setY((*it)->getY()+deltay);
    }
}

bool operator==(const GraphicalItem& a, const GraphicalItem& b)
{ return a.mName == b.mName; }

void GraphicalItem::setX(int _x)
{
    for (anchors_t::const_iterator it = mAnchors.begin();
    it != mAnchors.end(); ++it) {
        (*it)->setX((*it)->getX()+(_x - mX));
    }
    mX = _x;
}

void GraphicalItem::setY(int _y)
{
    for (anchors_t::const_iterator it = mAnchors.begin();
    it != mAnchors.end(); ++it) {
        (*it)->setY((*it)->getY()+(_y - mY));
    }
    mY = _y;
}

int GraphicalItem::getIndex(const Anchor* anchor)const
{
    int i = 0;
    for (anchors_t::const_iterator it = mAnchors.begin();
    it != mAnchors.end(); ++it) {
        if (anchor == *it)
            return i;
        ++i;
    }
    return -1;
}

Anchor* GraphicalItem::getAnchor(int i)
{
    return mAnchors[i];
}

std::string GraphicalItem::tooltipText() {
    std::string tooltipText;

    tooltipText = "<b>Element : </b>Graphical Item\n";
    tooltipText += "<b>Name</b> : "+mName;
    return tooltipText;
}

void GraphicalItem::setColor(const Gdk::Color& color,
    const Cairo::RefPtr<Cairo::Context>& context)
{
        context->set_source_rgb(color.get_red_p(),
        color.get_green_p(),
        color.get_blue_p());
}

GraphicalItem::~GraphicalItem()
{
    for(anchors_t::iterator it = mAnchors.begin();
    it != mAnchors.end(); ++it)
        delete *it;
    mAnchors.clear();
}

}
}
}
}
