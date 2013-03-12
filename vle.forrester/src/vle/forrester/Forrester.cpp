/*
 * @file vle/forrester/Forrester.cpp
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

#include <vle/forrester/Forrester.hpp>
#include <vle/forrester/graphicalItems/Parameter.hpp>
#include <vle/forrester/graphicalItems/Compartment.hpp>
#include <vle/forrester/graphicalItems/Flow.hpp>
#include <vle/forrester/graphicalItems/DependencyArrow.hpp>
#include <vle/forrester/graphicalItems/ExogenousVariable.hpp>
#include <vle/forrester/Plugin.hpp>
#include <vle/utils/i18n.hpp>
#include <boost/algorithm/string.hpp>
#include <boost/lexical_cast.hpp>



#include "graphicalItems/Variable.hpp"

namespace vle {
namespace gvle{
namespace modeling{
namespace forrester{

Forrester::Forrester(const std::string& name):
mIntegrationScheme(Forrester::EULER),
mForresterGI(name,*this)
{}

void Forrester::add(GraphicalItem* item)
{
    mGraphicalItems.push_back(item);
}

void Forrester::remove(GraphicalItem* item)
{
    for(arrows::iterator it = mArrows.begin();
    it != mArrows.end() ;) {
        bool found = false;

        for(anchors_t::iterator itA = item->getAnchors().begin();
        itA != item->getAnchors().end() && !found; ++itA) {
            if ((*it)->getOrigin() == *itA ||
            (*it)->getDestination() == *itA) {
                found = true;
            }
        }
        if (found)
            it = mArrows.erase(it);
        else
            ++it;
    }

    mGraphicalItems.remove(item);
}

bool Forrester::exist(const std::string& name) const
{
    for (graphical_items::const_iterator it = mGraphicalItems.begin();
    it != mGraphicalItems.end(); ++it) {
        if ((*it)->getName() == name)
            return true;
    }

    return false;
}

GraphicalItem* Forrester::getSelectedItem(int _x, int _y)
{
    bool found = false;
    graphical_items::iterator it = mGraphicalItems.end();

    while ((it != mGraphicalItems.begin()) && (!found)) {
        --it;
        found = (*it)->select(_x, _y);
    }

    if (found) {
        mGraphicalItems.splice(mGraphicalItems.end(), mGraphicalItems, it);
        return *it;
    }
    else
        return 0;

}

GraphicalItem* Forrester::getSelectedItem()
{
    graphical_items::iterator it = mGraphicalItems.end();

    if(!mGraphicalItems.empty()) {
        --it;
        return *it;
    } else {
        return 0;
    }
}

bool Forrester::checkDiagramSize()
{
    int maxX = ForresterGI::MINIMAL_WIDTH;
    int maxY = ForresterGI::MINIMAL_HEIGHT;

    for (graphical_items::const_iterator it = mGraphicalItems.begin();
    it != mGraphicalItems.end(); ++it) {
        if((*it)->getX() + (*it)->getWidth() +
                (*it)->getRightOffset() > maxX)
            maxX = (*it)->getX() + (*it)->getWidth() +
                (*it)->getRightOffset();

        if((*it)->getY() + (*it)->getHeight() +
                (*it)->getBottomOffset() > maxY)
            maxY = (*it)->getY() + (*it)->getHeight() +
                (*it)->getBottomOffset();
    }

    if (maxX != mForresterGI.getWidth() ||
    maxY != mForresterGI.getHeight()) {
        mForresterGI.resize(maxX, maxY);
        return true;
    } else {
        return false;
    }
}

bool Forrester::add(Arrow* arrow)
{
    if (arrow->valid()) {
        for (arrows::iterator it = mArrows.begin();
        it != mArrows.end(); ++it) {
            if (dynamic_cast<FlowArrow*>(arrow) &&
            dynamic_cast<FlowArrow*>(*it)) {
                if ((arrow->isOpposed(**it) || *arrow == **it))
                    return false;
            }
            if (dynamic_cast<DependencyArrow*>(arrow) &&
            dynamic_cast<DependencyArrow*>(*it)) {
                if (*arrow == **it)
                    return false;
            }
        }

        mArrows.push_back(arrow);
        return true;
    }
    return false;
}

void Forrester::remove(Arrow* arrow)
{
    mArrows.remove(arrow);
}

GraphicalItem* Forrester::getItem(const std::string& name) const
{
    for (graphical_items::const_iterator it = mGraphicalItems.begin();
    it != mGraphicalItems.end(); ++it) {
        if(name == (*it)->getName())
            return *it;
    }
    return 0;
}


Arrow* Forrester::getSelectedArrow(int x, int y) const
{
    for (arrows::const_iterator it = mArrows.begin();
    it != mArrows.end(); ++it) {
        if ((*it)->select(x, y))
            return *it;
    }
    return 0;
}

bool Forrester::isValidName(const std::string& name)const
{
    size_t i = 0;
    while (i < name.length()) {
        if (!isalnum(name[i])) {
            return false;
        }
        i++;
    }

    try {
        boost::lexical_cast <double>(name);
        return false;
    } catch (boost::bad_lexical_cast e) {}

    if (name == "")
        return false;
    else
        return true;
}

void Forrester::linkFlowCompartments (Flow* flow,
        Compartment* origin,
        Compartment* dest)
{
    Anchor* tmpAnchorOrigin = 0;
    Anchor* tmpAnchorDest = 0;

    if(origin) {
        tmpAnchorDest = flow->getAnchors()[0];
        tmpAnchorOrigin = Anchor::closestAnchor(
            *tmpAnchorDest,origin->getAnchors());

        add(new FlowArrow(tmpAnchorOrigin,tmpAnchorDest));
    }
    if(dest) {
        tmpAnchorOrigin = flow->getAnchors()[0];
        tmpAnchorDest = Anchor::closestAnchor(
            *tmpAnchorOrigin,dest->getAnchors());

        add(new FlowArrow(tmpAnchorOrigin,tmpAnchorDest));
    }
}

Forrester::~Forrester() {
    for(graphical_items::iterator it = mGraphicalItems.begin();
    it != mGraphicalItems.end(); ++it)
        delete *it;
    mGraphicalItems.clear();

    for(arrows::iterator it = mArrows.begin();
    it != mArrows.end(); ++it)
        delete *it;
    mArrows.clear();
}

}
}
}
}
