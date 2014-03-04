/*
 * @file vle/gvle/modeling/decision/DecisionDrawingArea.hpp
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

#ifndef VLE_GVLE_MODELING_DECISION_DECISION_DRAWING_AREA_HPP
#define VLE_GVLE_MODELING_DECISION_DECISION_DRAWING_AREA_HPP

#include <vle/gvle/modeling/decision/PrecedenceConstraintDialog.hpp>
#include <vle/gvle/modeling/decision/ActivityDialog.hpp>
#include <vle/gvle/modeling/decision/Decision.hpp>
#include <vle/gvle/Message.hpp>
#include <vle/gvle/Settings.hpp>
#include <vle/utils/i18n.hpp>
#include <boost/lexical_cast.hpp>
#include <gtkmm.h>
#include <iostream>
#include <limits>

namespace vle {
namespace gvle {
namespace modeling {
namespace decision {

/**
 * @class DecisionDrawingArea
 * @brief Constains the methods used to draw the model.
 */

class DecisionDrawingArea : public Gtk::DrawingArea
{
public:
    enum tool_states {
        SELECT_MODE,
        ADD_ACTIVITY_MODE,
        ADD_CONSTRAINT_MODE,
        DELETE_MODE};

/**
 * @brief DecisionDrawingArea class constructor
 * @param a pointer to the cobject
 * @param a reference to the gtk builder
 */
    DecisionDrawingArea(BaseObjectType* cobject,
                        const Glib::RefPtr < Gtk::Builder >& xml);

/**
 * @brief class destructor
 */
    virtual ~DecisionDrawingArea() {}

/**
 * @brief set the mouse cursor state
 * @param state of the cursor
 */
    void setState(int state)
    {
        mState = state;
        mCurrentActivitiesModel.clear();
    }

/**
 * @brief This function set the Decision model of the class
 * @param a decision model
 */
    void setDecision(Decision* decision)
    {
        mDecision = decision;
        mWidth = mDecision->width() + 2 * OFFSET;
        mHeight = mDecision->height() + 2 * OFFSET;
        queueRedraw();
    }

/**
 * @brief to clear the selection.
 */
    void clearSelections()
    {
        mCopiedActivitiesModel.clear();
        mCurrentActivitiesModel.clear();
        mCurrentPrecedenceConstraints.clear();
        queueRedraw();
    }

private:
/**
 * @brief This function add an activity to the model
 * @param the coordinates of the new activity
 * @return a boolean, true if an activity was added or false
 */
    bool addActivity(guint x, guint y);

/**
 * @brief This function check the size of the activity (text length for
 * example)
 * @param the activity
 * @param the name of the activity
 * @return return the new height of the activity
 */
    int checkSize(ActivityModel* activityModel, const std::string& str);

/**
 * @brief This function displace an activity
 * @param the ancient x
 * @param the ancient y
 * @param the new x
 * @param the new y
 * @param valid x
 * @param valid y
 */
    void displaceActivitiesModel(int oldx, int oldy, int newx, int newy,
            bool& xok, bool& yok);

/**
 * @brief This function draw all the diagram
 */
    void draw();

/**
 * @brief This function draw the background of the diagram
 */
    void drawBackground();

/**
 * @brief This function draw the name of the used plan file
 */
    void drawName();

/**
 * @brief This function draw the diagram rectangle
 */
    void drawRectangle();

/**
 * @brief this function draw a rounded rectangle
 * @param the x position of the upper left corner
 * @param the y position of the upper left corner
 * @param the width of the diagram
 * @param the height of the diagram
 * @param the apsect of the diagram
 * @param the radius of the round corners
 * @param the value of red
 * @param the value of green
 * @param the value of blue
 */
    void drawRoundedRectangle(guint x, guint y, guint width, guint height,
            double aspect, double radius, double red,
            double green, double blue);

/**
 * @brief draw an activity
 * @param a pointer to an activity
 */
    void drawActivityModel(ActivityModel* activityModel);

/**
 * @brief draw the name of an activity
 * @param the activity
 */
    void drawActivityModelName(ActivityModel* activityModel);

/**
 * @brief Draw the activities contains into the class mDecision.
 */
    void drawActivitiesModel();

/**
 * @brief Draw the precedenceConstraints contains into the class mDecision.
 */
    void drawPrecedenceConstraints();

/**
 * @brief Draw a precedenceConstraint.
 * @param a pointer to the constraint to build
 */
    void drawPrecedenceConstraint(
            PrecedenceConstraintModel* precedenceConstraint);

/**
 * @brief This function search the anchors where the precedence
 * can be fixed
 * @param the activity
 * @param the x position of the precedence constraint
 * @param the y position of the precedence constraint
 * @return return the points where the precedence constraint can be fixed
 */
    point_t searchAnchor(const ActivityModel* activityModel, guint x, guint y);

/**
 * @brief Draw the anchors of an activity.
 * @param the activity
 */
    void drawAnchors(ActivityModel* activityModel);

/**
 * @brief This function allow the user to modifie the current
 * selected activity.
 * @return return true if the activity have been modified or false
 */
    bool modifyCurrentActivityModel();

/**
 * @brief This function allow the user to modify the current precedence
 * constraint.
 * @return true if the precedence constraint have been modified or false
 */
    bool modifyCurrentPrecedenceConstraint();

/**
 * @brief This function is called when an event is detected.
 * @param the event
 * @return return a boolean
 */
    bool on_button_press_event(GdkEventButton* event);

/**
 * @brief This function is called when an event is detected.
 * @param the event
 * @return return a boolean
 */
    bool on_button_release_event(GdkEventButton* event);

/**
 * @brief If the cursor is above an activity, return the name of the activity,
 * otherwise return an empty string.
 * @param The x position of the cursor
 * @param The y position of the cursor
 * @return return the name of the activity
 */
 //   std::string aboveActivity(int x, int y);

/**
 * @brief This function is called when an event is detected.
 * @param the event
 * @return return a boolean
 */
    bool on_configure_event(GdkEventConfigure* event);

/**
 * @brief This function is called when an event is detected.
 * @param the event
 * @return return a boolean
 */
//    bool on_expose_event(GdkEventExpose* event);
    bool on_draw(const Cairo::RefPtr<Cairo::Context>& context);
    
    bool onQueryTooltip(int wx,int wy, bool /* keyboard_tooltip */,
                const Glib::RefPtr<Gtk::Tooltip>& tooltip);
/**
 * @brief This function is called when an event is detected.
 * @param the event
 * @return return a boolean
 */
    bool on_motion_notify_event(GdkEventMotion* event);

/**
 * @brief This function create the window.
 */
    void on_realize();

/**
 * @brief This function indicate that the diagram need to be redraw.
 */
    void queueRedraw()
    {
//        mNeedRedraw = true;
        queue_draw();
    }

/**
 * @brief Set the color of an element
 * @param a color.
 */
    void setColor(const Gdk::Color& color);

/**
 * @brief This function fill a vector with the activities selected by the user.
 * @param the x position of the mouse cursor
 * @param the y position of the mouse cursor
 * @param indicates if the ctrl key is pressed
 * @return return true if activity(ies) have been selected
 */
    bool selectActivityModel(guint x, guint y, bool ctrl);

/**
 * @brief This function indicates to the user if a precedence constraint
 * is selected.
 * @param the x position of the cursor
 * @param the y position of the cursor
 * @return a boolean, true if a transition is selected
 * or false.
 */
    bool selectPrecedenceConstraint(guint mx, guint my);

/**
 * @brief This function initialize the popup menu.
 */
    void initMenuPopupModels();

/**
 * @brief This function fill a vector with copied activities.
 */
    void copy()
    {
        if (!mCurrentActivitiesModel.empty())
        {
            mCopiedActivitiesModel = mCurrentActivitiesModel;
        }
    }

/**
 * @brief This function paste the copied activities.
 */
    void paste()
    {
        std::map < std::string, std::string > generatedNames;
        for (std::vector < ActivityModel* >::iterator it =
                mCopiedActivitiesModel.begin();
                it != mCopiedActivitiesModel.end(); ++it) {
            int number = 1;
            std::ostringstream oss;
            oss << number;
            std::string sNb = oss.str();
            while (mDecision->existActivityModel((*it)->name() +
                    /*"_" +*/ sNb)) {
                number++;
                oss.str("");
                oss << number;
                sNb = oss.str();
            }

            generatedNames[(*it)->name()] = (*it)->name() + /*"_" +*/ sNb;

            mDecision->addActivityModel((*it)->name() + /*"_" +*/ sNb,
                    (*it)->x() + OFFSET, (*it)->y() + OFFSET, ACTIVITY_WIDTH,
                    ACTIVITY_HEIGHT);
            ActivityModel* newActivity = mDecision->activityModel((*it)->name()
                    + /*"_" +*/ sNb);
            newActivity->setRules((*it)->getRules());
            newActivity->setOutputFunc((*it)->getOutputFunc());
            newActivity->setAckFunc((*it)->getAckFunc());
            checkSize(newActivity, newActivity->name());

            newActivity->minstart((*it)->minstart());
            newActivity->maxfinish((*it)->maxfinish());

            int newWidth = (*it)->x() + OFFSET + newActivity->width() + OFFSET;
            int newHeight = (*it)->y() + OFFSET + newActivity->height() +OFFSET;

            if (newWidth > mDecision->width() ||
                    newHeight > mDecision->height()) {
                mDecision->width(newWidth);
                mDecision->height(newHeight);
                mWidth = mDecision->width() + 2 * OFFSET;
                mHeight = mDecision->height() + 2 * OFFSET;
//                mBuffer = Gdk::Pixmap::create(mWin, mWidth, mHeight, -1);
                set_size_request(mWidth, mHeight);
            }
        }

        for (std::vector < ActivityModel* >::iterator it =
                mCopiedActivitiesModel.begin();
                it != mCopiedActivitiesModel.end(); ++it) {
            // Precedence constraints management
            for (std::vector < ActivityModel* >::iterator it2 =
                    mCopiedActivitiesModel.begin();
                    it2 != mCopiedActivitiesModel.end(); ++it2) {
                PrecedenceConstraintModel* tmpConstraint =
                        new PrecedenceConstraintModel();
                if (mDecision->getPrecedenceConstraint((*it)->name(),
                        (*it2)->name(), tmpConstraint)) {
                    // Change precedence constraints coordinates, use anchors
                    tmpConstraint->displaceSource(OFFSET, OFFSET);
                    tmpConstraint->displaceDestination(OFFSET, OFFSET);

                    points_t pts;
                    point_t newAnchorSource = searchAnchor(
                            mDecision->activityModel(
                                    generatedNames[tmpConstraint->source()]),
                                    tmpConstraint->points().at(0).x,
                                    tmpConstraint->points().at(0).y);
                    pts.push_back(newAnchorSource);

                    point_t newAnchorDest = searchAnchor(
                            mDecision->activityModel(
                                generatedNames[tmpConstraint->destination()]),
                                tmpConstraint->points().at(1).x,
                                tmpConstraint->points().at(1).y);
                    pts.push_back(newAnchorDest);
                    tmpConstraint->points(pts);

                    tmpConstraint->setSource(
                            generatedNames[tmpConstraint->source()]);
                    tmpConstraint->setDestination(
                            generatedNames[ tmpConstraint->destination()]);
                    mDecision->addPrecedenceConstraint(tmpConstraint);
                }
                else {
                    delete tmpConstraint;
                }
            }
        }

        mCopiedActivitiesModel.clear();
        queueRedraw();
    }

/**
 * @brief This function delete the selected activities.
 */
    void deleteElmts()
    {
        if (!mCurrentActivitiesModel.empty()) {
            std::string question = boost::lexical_cast<std::string>
                    ((int)mCurrentActivitiesModel.size());

            if (mCurrentActivitiesModel.size() == 1 && Question(_(
                    "Are you sure you want to delete this activity?"))) {
                mDecision->removeActivityModel(
                        *mCurrentActivitiesModel.begin());
                mCopiedActivitiesModel.clear();
                mCurrentActivitiesModel.clear();
                mCurrentPrecedenceConstraints.clear();
                queueRedraw();
            }
            else if (mCurrentActivitiesModel.size() > 1 &&
                    Question(_("Are you sure you want to delete these ")
                    + question + _(" activities?"))) {
                for (std::vector < ActivityModel* >::iterator it =
                        mCurrentActivitiesModel.begin();
                        it != mCurrentActivitiesModel.end(); ++it) {
                    mDecision->removeActivityModel(*it);
                }
                mCopiedActivitiesModel.clear();
                mCurrentActivitiesModel.clear();
                mCurrentPrecedenceConstraints.clear();
                queueRedraw();
            }
        }
    }

    static const guint OFFSET;
    static const guint HEADER_HEIGHT;
    static const guint ACTIVITY_NAME_HEIGHT;
    static const guint ACTIVITY_HEIGHT;
    static const guint ACTIVITY_WIDTH;

    Glib::RefPtr < Gtk::Builder > mXml;

//    Glib::RefPtr < Gdk::Pixmap > mBuffer;
    Cairo::RefPtr < Cairo::Context > mContext;
    Glib::RefPtr < Gdk::Window > mWin;
//    Glib::RefPtr < Gdk::GC > mWingc;
//    bool mIsRealized;
//    bool mNeedRedraw;

    Decision* mDecision;
    int mState;

    int mHeight;
    int mWidth;
    float mMaxHeight;
    float mMaxWidth;

    sigc::connection m_cntSignalQueryTooltip;

    std::vector < ActivityModel* > mCurrentActivitiesModel;
    std::vector < PrecedenceConstraintModel* > mCurrentPrecedenceConstraints;

    std::vector < ActivityModel* > mCopiedActivitiesModel;

    int mPreviousX;
    int mPreviousY;

    point_t mMouse;
    point_t mBegin;
    point_t mEnd;
    point_t mStartPoint;
    point_t* mBreakpoint;
    point_t* mFirstBreakpoint;
    point_t* mLastBreakpoint;

    point_t mStartSelectRect;
    point_t mEndSelectRect;

    Gtk::Menu *mMenuPopup;

    ActivityModel *mStartActivity;
    PrecedenceConstraintModel* mCurrentPrecedenceConstraint;
    bool mDecisionResize;
};

}
}
}
}    // namespace vle gvle modeling decision

#endif
