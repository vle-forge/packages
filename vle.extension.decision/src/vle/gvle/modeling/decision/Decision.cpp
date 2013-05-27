/*
 * @file vle/gvle/modeling/decision/DecisionModel.cpp
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

#include <vle/gvle/modeling/decision/Decision.hpp>

namespace vle {
namespace gvle {
namespace modeling {
namespace decision {

const int Decision::INITIAL_HEIGHT = 400;
const int Decision::INITIAL_WIDTH = 400;
const int Decision::MINIMAL_HEIGHT = 100;
const int Decision::MINIMAL_WIDTH = 100;

ActivityModel::ActivityModel(const std::string& conf):
        mMinstart(utils::toScientificString(utils::DateTime::toJulianDayNumber
                ("1400-01-01"))),
        mMaxfinish(utils::toScientificString(utils::DateTime::toJulianDayNumber
                ("1401-01-01")))
{
    strings_t activityModel;

    boost::split(activityModel, conf, boost::is_any_of(","));
    mName = activityModel[0];
    mX = boost::lexical_cast < int > (activityModel[1]);
    mY = boost::lexical_cast < int > (activityModel[2]);
    mWidth = boost::lexical_cast < int > (activityModel[3]);
    mHeight = boost::lexical_cast < int > (activityModel[4]);

    if (!activityModel[5].empty()) {
        mOutputFunc.push_back(activityModel[5]);
    }

    if (!activityModel[6].empty()) {
        mAckFunc.push_back(activityModel[6]);
    }

    if (activityModel[7] == "R") {
        mIsRelativeDate = true;
    }
    else {
        mIsRelativeDate = false;
    }

    computeAnchors();
}

void ActivityModel::displace(int deltax, int deltay)
{
    mX += deltax;
    mY += deltay;

    for (points_t::iterator it = mAnchors.begin();
            it != mAnchors.end(); ++it) {
        it->x += deltax;
        it->y += deltay;
    }
}

std::string ActivityModel::toString() const
{
    return (fmt("%1%,%2%,%3%,%4%,%5%")
            % mName % mX % mY % mWidth % mHeight).str();
}

void Decision::displace(ActivityModel* activityModel, int deltax, int deltay)
{
    activityModel->displace(deltax, deltay);
    for (std::vector < PrecedenceConstraintModel* >::iterator it =
            mPrecedenceConstraint.begin();
            it != mPrecedenceConstraint.end(); ++it) {
        if ((*it)->source() == activityModel->name()) {
            (*it)->displaceSource(deltax, deltay);
        }
        if ((*it)->source() != (*it)->destination() and
            (*it)->destination() == activityModel->name()) {
            (*it)->displaceDestination(deltax, deltay);
        }
    }
}

void Decision::removeActivityModel(ActivityModel* activityModel)
{
    for (std::vector < PrecedenceConstraintModel* >::iterator it =
            mPrecedenceConstraint.begin();
            it != mPrecedenceConstraint.end(); ) {
        if ((*it)->source() == activityModel->name() ||
                (*it)->destination() == activityModel->name()) {
            it = mPrecedenceConstraint.erase(it);
        }
        else {
            ++it;
        }
    }
    mActivitiesModel.erase(activityModel->name());
}

void Decision::resizeActivityModel(ActivityModel* activityModel,
    int width, int height)
{
    int deltax = width - activityModel->width();
    int deltay = height - activityModel->height();
    double ratiox = (double)width / activityModel->width();
    double ratioy = (double)height / activityModel->height();

    for (std::vector < PrecedenceConstraintModel* >::iterator it =
            mPrecedenceConstraint.begin();
            it != mPrecedenceConstraint.end(); ++it) {
        if ((*it)->source() == activityModel->name()) {
            (*it)->replaceSource(activityModel->x(), activityModel->y(),
                                 ratiox, ratioy);
        }
        if ((*it)->destination() == activityModel->name()) {
            (*it)->replaceDestination(activityModel->x(), activityModel->y(),
                                      ratiox, ratioy);
        }
    }

    if (deltax != 0) {
        activityModel->width(width);
    }
    if (deltay != 0) {
        activityModel->height(height);
    }
}

void ActivityModel::computeAnchors()
{
    mAnchors.clear();

    // top
    mAnchors.push_back(point_t(mX + mWidth / 3, mY));
    mAnchors.push_back(point_t(mX + 2 * mWidth / 3, mY));

    // bottom
    mAnchors.push_back(point_t(mX + mWidth / 3, mY + mHeight));
    mAnchors.push_back(point_t(mX + 2 * mWidth / 3, mY + mHeight));

    // left
    mAnchors.push_back(point_t(mX, mY + mHeight / 3));
    mAnchors.push_back(point_t(mX, mY + 2 * mHeight / 3));

    // right
    mAnchors.push_back(point_t(mX + mWidth, mY + mHeight / 3));
    mAnchors.push_back(point_t(mX + mWidth, mY + 2 * mHeight / 3));
}

}
}
}
}    // namespace vle gvle modeling decision
