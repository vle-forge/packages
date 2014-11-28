/*
 * @file vle/gvle/modeling/decision/DecisionModel.hpp
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

#ifndef VLE_GVLE_MODELING_DECISION_DECISION_MODEL_HPP
#define VLE_GVLE_MODELING_DECISION_DECISION_MODEL_HPP

#include <vle/extension/decision/KnowledgeBase.hpp>
#include <vle/extension/decision/Plan.hpp>
#include <vle/utils/DateTime.hpp>
#include <vle/utils/Tools.hpp>
#include <vle/utils/Path.hpp>
#include <gtkmm/messagedialog.h>
#include <boost/algorithm/string.hpp>
#include <boost/lexical_cast.hpp>
#include <iostream>
#include <fstream>
#include <vector>
#include <map>

using namespace std;

namespace vle {
namespace gvle {
namespace modeling {
namespace decision {

typedef std::vector < std::string > strings_t;
typedef std::map < std::string, std::string > buffers_t;

struct point_t
{
    int x;
    int y;

/**
 * @brief point_t constructor
 * @param x value
 * @param y value
 */
    point_t(int x = -1, int y = -1) : x(x), y(y)
    {
    }

/**
 * @brief Check if point value is correct
 * @return a boolean, true if coordinates are correct or false
 */
    bool valid() const
    {
        return x != -1 and y != -1;
    }

/**
 * @brief Set the point to an invalid coordinate
 */
    void invalid()
    {
        x = -1;
        y = -1;
    }

/**
 * @brief Redefinate the comparison operator ==
 * @param a point_t to compare
 * @return true if the two point_t are equals or false
 */
    bool operator==(const point_t& pt) const
    {
        return pt.x == x and pt.y == y;
    }

/**
 * @brief Redefinate the comparison operator !=
 * @param a point_t to compare
 * @return true if the two point_t are differents or false
 */
    bool operator!=(const point_t& pt) const
    {
        return pt.x != x or pt.y != y;
    }
};


typedef std::vector < point_t > points_t;

/**
 * @class PrecedenceConstraintModel
 * @brief Define the precedence constraints
 */

class PrecedenceConstraintModel
{
public :
    PrecedenceConstraintModel(const std::string& src, const std::string& dst,
            const points_t& pts) :
        mSource(src), mDestination(dst), mPoints(pts)
    {
        mCType = "FS";
    }

    PrecedenceConstraintModel(const std::string& src, const std::string& dst,
            const points_t& pts, const std::string actTlMin,
            const std::string actTlMax) :
        mSource(src), mDestination(dst), mPoints(pts), mActTlMin(actTlMin),
        mActTlMax(actTlMax)
    {
        mCType = "FS";
    }

    PrecedenceConstraintModel(const std::string cType, const std::string& src,
            const std::string& dst, const points_t& pts,
            const std::string actTlMin, const std::string actTlMax) :
        mSource(src), mDestination(dst), mPoints(pts), mActTlMin(actTlMin),
        mActTlMax(actTlMax), mCType(cType)
    {
    }

    PrecedenceConstraintModel() {}

    void displaceSource(int deltax, int deltay)
    {
        mPoints[0].x += deltax;
        mPoints[0].y += deltay;
    }

    void displaceDestination(int deltax, int deltay)
    {
        mPoints[mPoints.size() - 1].x += deltax;
        mPoints[mPoints.size() - 1].y += deltay;
    }

    points_t& points()
    {
        return mPoints;
    }

    void points(points_t points)
    {
        mPoints = points;
    }

    const std::string& source() const
    {
        return mSource;
    }

    const std::string& destination() const
    {
        return mDestination;
    }

    std::string actTlMin()
    {
        return mActTlMin;
    }

    std::string actTlMax()
    {
        return mActTlMax;
    }

    void actTlMin(std::string actTlMin)
    {
        mActTlMin = actTlMin;
    }

    void actTlMax(std::string actTlMax)
    {
        mActTlMax = actTlMax;
    }

    std::string cType()
    {
        return mCType;
    }

    void cType(std::string cType)
    {
        mCType = cType;
    }

    void setDestination(std::string dest)
    {
        mDestination = dest;
    }

    void setSource(std::string src)
    {
        mSource = src;
    }

    void replaceDestination(int ox, int oy, double ratiox, double ratioy)
    {
        mPoints[mPoints.size() - 1].x =
            ox + (mPoints[mPoints.size() - 1].x - ox) * ratiox;
        mPoints[mPoints.size() - 1].y =
            oy + (mPoints[mPoints.size() - 1].y - oy) * ratioy;
    }

    void replaceSource(int ox, int oy, double ratiox, double ratioy)
    {
        mPoints[0].x =
            ox + (mPoints[0].x - ox) * ratiox;
        mPoints[0].y =
             oy + (mPoints[0].y - oy) * ratioy;
    }

/**
 * @brief Return the PrecedenceConstraintModel card : the description displayed in a tooltip
 * @return the PrecedenceConstraintModel card
 */
    const Glib::ustring card()
    {
        Glib::ustring card = "Type: " + cType();
        card += "\n<b>Mintimelag:</b> " + actTlMin();
        card += "\n<b>Maxtimelag:</b> " + actTlMax();
        return card;
    }

private :
    std::string mSource;
    std::string mDestination;
    points_t mPoints;
    std::string mActTlMin;
    std::string mActTlMax;
    std::string mCType;
};

/**
 * @class ActivityModel
 * @brief Defines the activities model
 */

class ActivityModel
{
public:
/**
 * @brief ActivityModel constructor
 * @param name of the point
 * @param value of x coordinate
 * @param value of y coordinate
 * @param value of the height of the box
 * @param value of the width of the box
 */
    ActivityModel(const std::string& name,
          int x,
          int y,
          int w,
          int h) :
        mName(name), mX(x), mY(y), mWidth(w), mHeight(h), mMinstart(""),
        mMaxfinish("")
    {
        mIsHumanDate = true;
        mIsRelativeDate = false;
        computeAnchors();
    }

/**
 * @brief ActivityModel constructor
 * @param string with the ActivityModel configuration
 */
    ActivityModel(const std::string& conf);

/**
 * @brief Displace an ActivityModel (change coordinates)
 * @param deltax
 * @param deltay
 */
    void displace(int deltax, int deltay);

/**
 * @brief Return the height of the ActivityModel box
 * @return the height
 */
    int height() const
    {
        return mHeight;
    }

/**
 * @brief Set the ActivityModel height
 * @param the new height of the box
 */
    void height(int height)
    {
        mHeight = height;
        computeAnchors();
    }

/**
 * @brief Return the ActivityModel box width
 * @return the width
 */
    int width() const
    {
        return mWidth;
    }

/**
 * @brief Set the ActivityModel box width
 * @param the new width of the box
 * @return
 */
    void width(int width)
    {
        mWidth = width;
        computeAnchors();
    }

// Activities informations
/**
 * @brief Return the ActivityModel minstart
 * @return the ActivityModel minstart
 */
    const std::string& minstart() const
    {
        return mMinstart;
    }

/**
 * @brief Return the ActivityModel maxfinish
 * @return the ActivityModel maxfinish
 */
    const std::string& maxfinish() const
    {
        return mMaxfinish;
    }

/**
 * @brief Set the ActivityModel minstart
 */
    void minstart(std::string pMinstart)
    {
        mMinstart = pMinstart;
    }

/**
 * @brief Set the ActivityModel maxfinish
 */
    void maxfinish(std::string pMaxfinish)
    {
        mMaxfinish = pMaxfinish;
    }

/**
 * @brief Return the ActivityModel name
 * @return the ActivityModel name
 */
    const std::string& name() const
    {
        return mName;
    }

/**
 * @brief Set the name of the ActivityModel
 * @param the new name of the ActivityModel
 */
    void name(const std::string& name)
    {
        mName = name;
    }

/**
 * @brief Check if the ActivityModel box is selected
 * (mouse cursor is on the box)
 * @param x coordinate
 * @param y coordinate
 * @return true if the box is selected or false
 */
    bool select(int x, int y) const
    {
        return x >= mX and x <= mX + mWidth and y >= mY and y <= mY + mHeight;
    }

/**
 * @brief Check if the ActivityModel box is selected
 * (mouse cursor is on the box)
 * @param x coordinate up left corner
 * @param y coordinate up left corner
 * @param x coordinate down right corner
 * @param y coordinate down right corner
 * @return true if the box is selected or false
 */
    bool select(int ulx, int uly, int drx, int dry) const
    {
        return (ulx <= mX && uly <= mY && drx >= mX + mWidth &&
                dry >= mY + mHeight);
    }

/**
 * @brief Convert an ActivityModel configuration string into a table
 * @return a converted string
 */
    std::string toString() const;

/**
 * @brief Get the x coordinate of the class
 * @return the x coordinate of the class
 */
    int x() const
    {
        return mX;
    }

/**
 * @brief Get the y coordinate of the class
 * @return return the y coordinate of the class
 */
    int y() const
    {
        return mY;
    }

/**
 * @brief Get the rules vector of the activity
 * @return return the vector which contains the class activities
 */
    strings_t getRules() const
    {
        return mRules;
    }
/**
 * @brief Get the rules vector of the activity
 * @return return the vector which contains the class activities
 */
    strings_t& getRules()
    {
        return mRules;
    }

/**
 * @brief Get the rules vector of the activity
 * @return return the vector which contains the class activities
 */
   strings_t getOutputFunc() const
    {
        return mOutputFunc;
    }

    void delOutputFunc()
    {
        mOutputFunc.clear();
    }

/**
 * @brief Get the rules vector of the activity
 * @return return the vector which contains the class activities
 */
   strings_t getAckFunc() const
    {
        return mAckFunc;
    }

    void delAckFunc()
    {
        mAckFunc.clear();
    }

/**
 * @brief Set the rules vector of the activity
 * @param a vector of activities
 */
    void setRules(strings_t rules) {
        mRules = rules;
    }

/**
 * @brief Set the rules vector of the activity
 * @param a vector of activities
 */
    void setOutputFunc(strings_t rules) {
        mOutputFunc = rules;
    }

    void setAckFunc(strings_t rule) {
        mAckFunc = rule;
    }

/**
 * @brief Add a rule to the rules vector of the activity
 * @param an activity name
 */
    void addRule(std::string rule) {
        mRules.push_back(rule);
    }

/**
 * @brief Get the anchors of the activity
 * @return the anchors of the activity
 */
    points_t anchors() const
    {
        return mAnchors;
    }

    void setRelativeDate(bool state)
    {
        mIsRelativeDate = state;
    }

    bool getRelativeDate() const
    {
        return mIsRelativeDate;
    }

    bool isHumanDate() const
    {
        return mIsHumanDate;
    }

    void setHumanDate(bool state)
    {
        mIsHumanDate = state;
    }

private:
    void computeAnchors();

    std::string mName;
    int mX;
    int mY;
    int mWidth;
    int mHeight;
    std::string mMinstart;
    std::string mMaxfinish;
    bool mIsRelativeDate;
    bool mIsHumanDate;

    points_t mAnchors;

    std::vector < std::string > mRules;
    std::vector < std::string > mOutputFunc;
    std::vector < std::string > mAckFunc;
};


typedef std::map < std::string, ActivityModel* > activitiesModel_t;
typedef activitiesModel_t::const_iterator activitiesModel_it;
typedef std::vector < PrecedenceConstraintModel* > precedenceConstraints_t;

/**
 * @class ActivityModel
 * @brief Defines the decision model
 */

class Decision
{
public:
/**
 * @brief Constructor of the Decision class
 * @param a string with the name of the Decision model
 */
    Decision(const std::string& name) :
        mName(name), mWidth(INITIAL_WIDTH), mHeight(INITIAL_HEIGHT)
    {
        mKnowledgeBase = new vle::extension::decision::KnowledgeBase();
    }

/**
 * @brief Decision destructor
 */
    virtual ~Decision()
    {
        for (activitiesModel_t::const_iterator it = activitiesModel().begin();
             it != activitiesModel().end(); ++it) {
            delete it->second;
        }
    }

/**
 * @brief Change the name of an activity model
 * @param oldName of the ActivityModel
 * @param newName of the ActivityModel
 */
    void changeActivityModelName (const std::string& oldName,
            const std::string& newName) {
        ActivityModel* activityModel = mActivitiesModel.find(oldName)->second;
        mActivitiesModel.erase(mActivitiesModel.find(oldName));
        mActivitiesModel[newName] = activityModel;

        changePrecName(oldName, newName);
    }

    void changePrecName(
            const std::string& oldName,
            const std::string& newName)
    {
        precedenceConstraints_t::iterator it = mPrecedenceConstraint.begin();

        while (it != mPrecedenceConstraint.end()) {
            if ((*it)->source() == oldName) {
                (*it)->setSource(newName);
            }
            if ((*it)->destination() == oldName) {
                (*it)->setDestination(newName);
            }
            ++it;
        }
    }

/**
 * @brief Add an ActivityModel into the Decision class
 * @param an ActivityModel configuration string
 */
    void addActivityModel(const std::string& conf)
    {
        ActivityModel* activityModel = new ActivityModel(conf);
        mActivitiesModel[activityModel->name()] = activityModel;
    }

/**
 * @brief Add an ActivityModel into the Decision class
 * @param name of the new ActivityModel
 * @param x coordinate
 * @param y coordinate
 * @param width of the box
 * @param height of the box
 */
    void addActivityModel(const std::string& name,
        int x, int y, int w, int h)
    {
        mActivitiesModel[name] = new ActivityModel(name, x, y, w, h);
    }

/**
 * @brief Add an ActivityModel into the Decision class
 * @param pointer to the ActivityModel
 */
    void addActivityModel(ActivityModel* activityModel)
    {
        mActivitiesModel[activityModel->name()] = activityModel;
    }

/**
 * @brief Displace an ActivityModel (update coordinates)
 * @param an ActivityModel to displace
 * @param deltax
 * @param deltay
 */
    void displace(ActivityModel* activityModel, int deltax, int deltay);

/**
 * @brief Get the height of the Decision draw rectangle
 * @return the height of the draw rectangle
 */
    int height() const
    {
        return mHeight;
    }

/**
 * @brief Set the height of the Decision draw rectangle
 * @param new height of the rectangle
 */
    void height(int height)
    {
        mHeight = height;
    }

/**
 * @brief Get the name of the Decision class
 * @return the name of the Decision class
 */
    const std::string& name() const
    {
        return mName;
    }

    ActivityModel* getActivityByName(string activityName) const;

/**
 * @brief Convert a double to a Human Date
 * @param Numeric date
 * @return Human date
 */
    const std::string toHumanRelativeDate(double numericDate) const;

/**
 * @brief Return the ActivityModel card : the description displayed in a tooltip
 * @param decision to display rules
 * @return the ActivityModel card
 */
    const std::string getActivityCard(string activityName) const;

    const strings_t getPredicates(string ruleName) const;
/**
 * @brief Remove an ActivityModel into the Decision class
 * @param ActivityModel to remove
 */
    void removeActivityModel(ActivityModel* activityModel);

/**
 * @brief Resize the Decision draw rectangle
 * @param deltax
 * @param deltay
 */
    void resize(int deltax, int deltay)
    {
        mWidth += deltax;
        mHeight += deltay;
    }

/**
 * @brief Resize an ActivityModel box into the Decision class
 * @param new width
 * @param new height
 */
    void resizeActivityModel(ActivityModel* activityModel, int widht,
            int height);

/**
 * @brief Check if an ActivityModel exists
 * @param name of the ActivityModel
 * @return true if the ActivityModel exists or false
 */
    bool existActivityModel(const std::string& name) const
    {
        return mActivitiesModel.find(name) != mActivitiesModel.end();
    }

/**
 * @brief Get an ActivityModel into the Decision class
 * @param the name of the ActivityModel
 * @return the ActivityModel
 */
    ActivityModel* activityModel(const std::string& name) const
    {
        return mActivitiesModel.find(name)->second;
    }

/**
 * @brief Get the activities model map
 * @return return the activities model map
 */
    const activitiesModel_t& activitiesModel() const
    {
        return mActivitiesModel;
    }

/**
 * @brief Get the width of the Decision draw rectangle
 * @return the width of the rectangle
 */
    int width() const
    {
        return mWidth;
    }

/**
 * @brief Set the width of the rectangle
 * @param the new width of the rectangle
 */
    void width(int width)
    {
        mWidth = width;
    }

/**
 * @brief Fill the KnowledgeBase of the class with the content of the plan file
 * @param the name of the plan file
 * @return true if the file exists and the plan is charged on the
 * mKnowledgeBase or false if an error occured
 */

    bool fillKnowledgeBase(std::string fileName) {
        if (mActivitiesModel.size() > 0) {
            std::ifstream fileStream(fileName.c_str(), ios::in);

            if (fileStream.is_open()) {
                mKnowledgeBase->plan().fill(fileStream);
                return true;
            }
            else {
                return false;
            }
        }
        else {
            return true;
        }
    }

/**
 * @brief Return the KnowledgeBase of the class decision.
 * @return the KnowledgeBase
 */
    vle::extension::decision::KnowledgeBase* getKnowledgeBase() {
        return mKnowledgeBase;
    }

    void resetKnowledgeBase()
    {
        delete mKnowledgeBase;
        mKnowledgeBase = new vle::extension::decision::KnowledgeBase();
    }

    void setRule (std::map < std::string, strings_t >* rule) {
        mRule = rule;
    }

    std::map < std::string, strings_t >* getRule() const {
        return mRule;
    }

    void setOutputFunc(strings_t* rule) {
        mOutputFunc = rule;
    }

    strings_t* getOutputFunc() const {
        return mOutputFunc;
    }

    void setAckFunc(strings_t* rule) {
        mAckFunc = rule;
    }

    strings_t* getAckFunc() const {
        return mAckFunc;
    }

    const precedenceConstraints_t& precedenceConstraints() const
    {
        return mPrecedenceConstraint;
    }

    void addPrecedenceConstraint(PrecedenceConstraintModel *precCons) {
        mPrecedenceConstraint.push_back(precCons);
    }

    void addPrecedenceConstraint(std::string src, std::string dest,
            points_t pts) {
        mPrecedenceConstraint.push_back(new PrecedenceConstraintModel(
                src, dest, pts));
    }

    void addPrecedenceConstraint(std::string cType, std::string source,
            std::string destination, points_t points,
            std::string actTlMin, std::string actTlMax)
    {
        mPrecedenceConstraint.push_back(new PrecedenceConstraintModel(cType,
            source, destination, points, actTlMin, actTlMax));
    }

    void updatePrecedenceConstraint(PrecedenceConstraintModel
            precedenceConstraint)
    {
        for (precedenceConstraints_t::iterator
            it = mPrecedenceConstraint.begin();
            it != mPrecedenceConstraint.end(); ) {
            if ((*it)->source() == precedenceConstraint.source() &&
                    precedenceConstraint.destination() ==
                    (*it)->destination()) {
                it = mPrecedenceConstraint.erase(it);
            }
            else {
                ++it;
            }
        }
        mPrecedenceConstraint.push_back(new PrecedenceConstraintModel(
                precedenceConstraint.cType(),
                precedenceConstraint.source(),
                precedenceConstraint.destination(),
                precedenceConstraint.points(),
                precedenceConstraint.actTlMin(),
                precedenceConstraint.actTlMax()));
    }

    void removePrecedenceConstraint(
            PrecedenceConstraintModel precedenceConstraint)
    {
        for (precedenceConstraints_t::iterator
            it = mPrecedenceConstraint.begin();
            it != mPrecedenceConstraint.end(); ) {
            if ((*it)->source() == precedenceConstraint.source() &&
                    precedenceConstraint.destination() ==
                    (*it)->destination()) {
                it = mPrecedenceConstraint.erase(it);
            }
            else {
                ++it;
            }
        }
    }

    void resetDecision()
    {
        mPrecedenceConstraint.clear();
        for (activitiesModel_t::iterator it = mActivitiesModel.begin();
                it != mActivitiesModel.end(); ++it) {
            delete it->second;
        }
        mActivitiesModel.clear();

        mRule->clear();
    }

    bool getPrecedenceConstraint(std::string source, std::string destination,
            PrecedenceConstraintModel* precCons)
    {
        for (precedenceConstraints_t::iterator
            it = mPrecedenceConstraint.begin();
            it != mPrecedenceConstraint.end(); ++it) {
            if ((*it)->source() == source &&
                    (*it)->destination() == destination) {
                *precCons = *(*it);
                return true;
            }
        }
        return false;
    }

    static const int INITIAL_HEIGHT;
    static const int INITIAL_WIDTH;
    static const int MINIMAL_HEIGHT;
    static const int MINIMAL_WIDTH;

private:
    std::string mName;
    int mWidth;
    int mHeight;
    activitiesModel_t mActivitiesModel;

    // Knowledge base to store activities
    vle::extension::decision::KnowledgeBase *mKnowledgeBase;
    // Plan
    vle::extension::decision::Plan *mPlan;
    // Pointer to the Rules
    std::map < std::string, strings_t >* mRule;
    // Vector of precedence constraints
    precedenceConstraints_t mPrecedenceConstraint;
    // Pointer to the Output Functions
    strings_t* mOutputFunc;
    // Pointer to the acknowledge Functions
    strings_t* mAckFunc;
};

}
}
}
}    // namespace vle gvle modeling decision

#endif
