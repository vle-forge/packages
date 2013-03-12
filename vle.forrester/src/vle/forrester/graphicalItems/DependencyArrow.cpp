/*
 * @file vle/forrester/graphicalItems/DependencyArrow.cpp
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
#include <vle/forrester/graphicalItems/DependencyArrow.hpp>
#include <boost/algorithm/string.hpp>
#include <boost/lexical_cast.hpp>
#include <vle/utils/i18n.hpp>
namespace vle {
namespace gvle {
namespace modeling {
namespace forrester {

void DependencyArrow::draw(const Cairo::RefPtr<Cairo::Context>& context) const
{
    // the way to compute the (tcx, tcy) single control point of the
    // quadratic
    double dX = mControlPoint.getX() - mOrigin->getX();
    double dY = mControlPoint.getY() - mOrigin->getY();
    double d1 = std::sqrt(dX * dX + dY * dY);
    double d = d1;

    dX = mDestination->getX() - mControlPoint.getX();
    dY = mDestination->getY() - mControlPoint.getY();
    d += std::sqrt(dX * dX + dY * dY);
    double t = d1/d;

    double t1 = 1.0 - t;
    double tSq = t * t;
    double denom = 2.0 * t * t1;

    double tcx = (mControlPoint.getX() - t1 * t1 * mOrigin->getX() -
        tSq * mDestination->getX()) / denom;
    double tcy = (mControlPoint.getY() - t1 * t1 * mOrigin->getY() -
        tSq * mDestination->getY()) / denom;

    // from the single point of the quadratic to the both of the cubic
    double tcxq1 = mOrigin->getX() + 2. * (tcx - mOrigin->getX()) / 3.;
    double tcyq1 = mOrigin->getY() + 2. * (tcy - mOrigin->getY()) / 3.;
    double tcxq2 = mDestination->getX() +
        2. * (tcx - mDestination->getX()) / 3.;
    double tcyq2 = mDestination->getY() +
        2. * (tcy - mDestination->getY()) / 3.;

    // and now to draw,
    std::valarray< double > dashes(2);
    double angle = atan2 (mDestination->getY() - tcyq2,
        mDestination->getX() - tcxq2) + M_PI;
    double x1 = mDestination->getX() + 9 * std::cos(angle - 0.35);
    double y1 = mDestination->getY() + 9 * std::sin(angle - 0.35);
    double x2 = mDestination->getX() + 9 * std::cos(angle + 0.35);
    double y2 = mDestination->getY() + 9 * std::sin(angle + 0.35);
    dashes[0] = 8.0;
    dashes[1] = 3.0;

    context->save();
    context->set_line_width(1);
    context->move_to(mDestination->getX(), mDestination->getY());
    context->line_to(x1,y1);
    context->line_to(x2,y2);
    context->line_to(mDestination->getX(), mDestination->getY());
    context->fill();

    context->set_dash(dashes,0.);
    context->move_to(mOrigin->getX(), mOrigin->getY());
    context->curve_to(tcxq1, tcyq1, tcxq2, tcyq2, mDestination->getX(),
        mDestination->getY());
    context->stroke();
    context->restore();
}

std::string DependencyArrow::toString()const
{
    return (fmt("%1%,%2%,%3%,%4%,%5%,%6%")
            % mOrigin->getOwner()->getName()
            % mDestination->getOwner()->getName()
            % mOrigin->getOwner()->getIndex(mOrigin)
            % mDestination->getOwner()->getIndex(mDestination)
            % mControlPoint.getX()
            % mControlPoint.getY()).str();
}

bool DependencyArrow::select(int x, int y)
{
    if (mOrigin->select(x,y)
        || mDestination->select(x,y)
        || mControlPoint.select(x,y))
        return true;

    if(x > mControlPoint.getX() - 15 && x < mControlPoint.getX() + 15
       && y > mControlPoint.getY() - 15 && y < mControlPoint.getY() + 15)
       return true;

    return false;
}

void DependencyArrow::setDestination(Anchor* destination)
{
    mDestination = destination;
    if (mOrigin != 0)
        computeControlPoint();
}

void DependencyArrow::computeControlPoint()
{
    mControlPoint.setX((mOrigin->getX() + mDestination->getX())/2.);
    mControlPoint.setY((mOrigin->getY() + mDestination->getY())/2.);
}

bool DependencyArrow::valid() {
    if(mOrigin == 0 && mDestination == 0)
        return true;
    if(dynamic_cast<Flow*>(mOrigin->getOwner()) ||
    !dynamic_cast<Flow*>(mDestination->getOwner()) )
            return false;
    return true;
}

void DependencyArrow::generateSource (utils::Template& tpl) const {
    tpl.listSymbol().append("parametersArrow");
    tpl.listSymbol().append("parametersArrow", toString());
}


}
}
}
} // namespace vle gvle modeling forrester
