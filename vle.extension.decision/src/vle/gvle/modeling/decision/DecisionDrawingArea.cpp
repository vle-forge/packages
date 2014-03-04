/*
 * @file vle/gvle/modeling/decision/DecisionDrawingArea.cpp
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

#include <vle/gvle/modeling/decision/DecisionDrawingArea.hpp>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

namespace vle {
namespace gvle {
namespace modeling {
namespace decision {

const guint DecisionDrawingArea::OFFSET = 10;
const guint DecisionDrawingArea::HEADER_HEIGHT = 20;
const guint DecisionDrawingArea::ACTIVITY_NAME_HEIGHT = 20;
const guint DecisionDrawingArea::ACTIVITY_HEIGHT = 50;
const guint DecisionDrawingArea::ACTIVITY_WIDTH = 50;

DecisionDrawingArea::DecisionDrawingArea(
        BaseObjectType* cobject,
        const Glib::RefPtr < Gtk::Builder >& xml) :
    Gtk::DrawingArea(cobject),
    mXml(xml),
//    mIsRealized(false),
//    mNeedRedraw(true),
    mDecision(0),
    mState(SELECT_MODE),
    mHeight(300 + 2 * OFFSET),
    mWidth(400 + 2 * OFFSET),
    mMaxHeight(0),
    mMaxWidth(0),
    mPreviousX(-1),
    mPreviousY(-1),
    mDecisionResize(false)
{
    set_events(Gdk::POINTER_MOTION_MASK | Gdk::BUTTON_MOTION_MASK |
        Gdk::BUTTON1_MOTION_MASK | Gdk::BUTTON2_MOTION_MASK |
        Gdk::BUTTON3_MOTION_MASK | Gdk::BUTTON_PRESS_MASK |
        Gdk::BUTTON_RELEASE_MASK);
    mStartSelectRect.invalid();
    mEndSelectRect.invalid();
    initMenuPopupModels();

    set_has_tooltip();

    m_cntSignalQueryTooltip = signal_query_tooltip().connect(
        sigc::mem_fun(*this, &DecisionDrawingArea::onQueryTooltip));
}

void DecisionDrawingArea::initMenuPopupModels()
{
    Glib::RefPtr <Gtk::ActionGroup> mPopupActionGroup = Gtk::ActionGroup::create("DecisionDrawingArea");
    mPopupActionGroup->add(Gtk::Action::create("DDA_Copy", _("_Copy")), sigc::mem_fun(*this, &DecisionDrawingArea::copy));
    mPopupActionGroup->add(Gtk::Action::create("DDA_Paste", _("_Paste")), sigc::mem_fun(*this, &DecisionDrawingArea::paste));
    mPopupActionGroup->add(Gtk::Action::create("DDA_Delete", _("_Delete")), sigc::mem_fun(*this, &DecisionDrawingArea::deleteElmts));
    
    Glib::RefPtr <Gtk::UIManager> mUIManager = Gtk::UIManager::create();
    mUIManager->insert_action_group(mPopupActionGroup);
    
    Glib::ustring ui_info =
                "<ui>"
                "  <popup name='DDA_Popup'>"
                "    <menuitem action='DDA_Copy'/>"
                "    <menuitem action='DDA_Paste'/>"
                "    <menuitem action='DDA_Delete'/>"
                 "  </popup>"
                "</ui>";
    
    try {
      mUIManager->add_ui_from_string(ui_info);
      mMenuPopup = (Gtk::Menu *) (mUIManager->get_widget("/DDA_Popup"));
    } catch(const Glib::Error& ex) {
      std::cerr << "building menus failed: DDA_Popup " <<  ex.what();
    }
    
    if (!mMenuPopup)
      std::cerr << "not a menu : DDA_Popup\n";
/*
    Gtk::Menu::MenuList& menulist(mMenuPopup.items());
    menulist.push_back(Gtk::Menu_Helpers::MenuElem(_("_Copy"),
            sigc::mem_fun(*this, &DecisionDrawingArea::copy)));
    menulist.push_back(Gtk::Menu_Helpers::MenuElem(_("_Paste"),
            sigc::mem_fun(*this, &DecisionDrawingArea::paste)));
    menulist.push_back(Gtk::Menu_Helpers::MenuElem(_("_Delete"),
            sigc::mem_fun(*this, &DecisionDrawingArea::deleteElmts)));

    mMenuPopup.accelerate(*this);
*/
}

bool DecisionDrawingArea::addActivity(guint x, guint y)
{
    NewActivityDialog dialog(mXml, mDecision, *mDecision->getRule(),
            *mDecision->getOutputFunc(), *mDecision->getAckFunc());

    if (dialog.run() == Gtk::RESPONSE_ACCEPT) {

        mDecision->addActivityModel(dialog.name(),
                x, y, ACTIVITY_WIDTH, ACTIVITY_HEIGHT);
        ActivityModel* newActivity = mDecision->activityModel(dialog.name());
        newActivity->setRules(dialog.getActRule());
        newActivity->setOutputFunc(dialog.getActOut());
        newActivity->setAckFunc(dialog.getActAck());
        checkSize(newActivity, newActivity->name());

        if (!dialog.minstart().empty() && !dialog.maxfinish().empty()) {
            newActivity->minstart(dialog.minstart());
            newActivity->maxfinish(dialog.maxfinish());
        }
        newActivity->setRelativeDate(dialog.isRelativeDate());

        int newWidth = x + newActivity->width() + OFFSET;
        int newHeight = y + newActivity->height() + OFFSET;

        if (newWidth > mDecision->width() || newHeight > mDecision->height()) {
            mDecision->width(newWidth);
            mDecision->height(newHeight);
            mWidth = mDecision->width() + 2 * OFFSET;
            mHeight = mDecision->height() + 2 * OFFSET;
//            mBuffer = Gdk::Pixmap::create(mWin, mWidth, mHeight, -1);
        }

        return true;
    }

    return false;
}

int DecisionDrawingArea::checkSize(ActivityModel* activityModel,
        const std::string& str)
{
    Cairo::TextExtents textExtents;
    mContext->get_text_extents(str, textExtents);

    if ((activityModel->width() < textExtents.width + 2 * OFFSET) ||
                (activityModel->width() > textExtents.width + 2 * OFFSET)) {
        if ((textExtents.width + 2 * OFFSET) < ACTIVITY_WIDTH) {
            mDecision->resizeActivityModel(activityModel, ACTIVITY_WIDTH,
                    activityModel->height());
        }
        else {
            mDecision->resizeActivityModel(activityModel, textExtents.width +
                    2 * OFFSET, activityModel->height());
        }
    }
    return textExtents.height + OFFSET;
}

void DecisionDrawingArea::displaceActivitiesModel(int oldx, int oldy,
        int newx, int newy,
        bool& xok, bool& yok)
{
    int deltax = newx - oldx;
    int deltay = newy - oldy;

    xok = false;
    yok = false;
    if (deltax != 0 or deltay != 0) {
        for (std::vector < ActivityModel* >::const_iterator it =
                mCurrentActivitiesModel.begin();
                it != mCurrentActivitiesModel.end(); ++it) {
            int _deltax = deltax;
            int _deltay = deltay;

            if ((*it)->x() + deltax < (int)(2 * OFFSET)) {
                _deltax = 0;
            } else {
                xok = true;
            }
            if ((*it)->y() + deltay < (int)(2 * OFFSET + HEADER_HEIGHT)) {
                _deltay = 0;
            } else {
                yok = true;
            }

            mDecision->displace(*it, _deltax, _deltay);

            {
                int newWidth = (*it)->x() + (*it)->width();
                int newHeight = (*it)->y() + (*it)->height();

                if (newWidth > mDecision->width()) {
                    mDecision->width(newWidth);
                    mWidth = mDecision->width() + 2 * OFFSET;
                }
                if (newHeight > mDecision->height()) {
                    mDecision->height(newHeight);
                    mHeight = mDecision->height() + 2 * OFFSET;
                }
            }
        }
    } else {
        xok = true;
        yok = true;
    }
}

void DecisionDrawingArea::draw()
{
    if (mContext) {
        mContext->save();
        mContext->set_line_width(Settings::settings().getLineWidth());

        drawBackground();
        drawRectangle();
        drawName();
        drawActivitiesModel();
        drawPrecedenceConstraints();

        if (mMouse.valid() and mBegin.valid()) {
            mContext->set_source_rgb(1., 0., 0.);
            mContext->rectangle(mBegin.x - 2, mBegin.y - 2, 5, 5);
            mContext->fill();

            mContext->set_source_rgb(0., 0., 0.);
            mContext->move_to(mBegin.x, mBegin.y);
            mContext->line_to(mMouse.x, mMouse.y);
            mContext->stroke();
        }

        if (mStartSelectRect.valid() && mEndSelectRect.valid()) {
            mContext->set_source_rgb(0., 0., 0.);
            mContext->move_to(0, 0);
            mContext->set_line_width(2.0);
            mContext->rectangle(mStartSelectRect.x, mStartSelectRect.y,
                    mEndSelectRect.x - mStartSelectRect.x,
                    mEndSelectRect.y - mStartSelectRect.y);
            mContext->stroke();
        }

        mContext->restore();
        set_size_request(mWidth, mHeight);
    }
}

void DecisionDrawingArea::drawBackground()
{
    setColor(Settings::settings().getBackgroundColor());
    mContext->rectangle(0, 0, mWidth, mHeight);
    mContext->fill();
    mContext->stroke();
}

void DecisionDrawingArea::drawName()
{
    mContext->select_font_face(Settings::settings().getFont(),
        Cairo::FONT_SLANT_NORMAL,
        Cairo::FONT_WEIGHT_NORMAL);
    mContext->set_font_size(Settings::settings().getFontSize());

    Cairo::TextExtents textExtents;
    mContext->get_text_extents(mDecision->name(), textExtents);

    mContext->move_to(
        (mDecision->width() - textExtents.width) / 2 + OFFSET,
        OFFSET + (HEADER_HEIGHT + textExtents.height) / 2);
    mContext->show_text(mDecision->name());
    mContext->stroke();
}

void DecisionDrawingArea::drawRectangle()
{
    setColor(Settings::settings().getForegroundColor());

    float maxWidth, maxHeight;
    maxWidth = maxHeight = 0;
    for (activitiesModel_t::const_iterator it =
            mDecision->activitiesModel().begin();
            it != mDecision->activitiesModel().end(); ++it) {
        if ((it->second->x() + it->second->width()) > maxWidth) {
            maxWidth = it->second->x() + it->second->width();
        }
        if ((it->second->y() + it->second->height()) > maxHeight) {
            maxHeight = it->second->y() + it->second->height();
        }
    }

    if (maxWidth != 0 && (maxWidth != mDecision->width()) &&
                (mDecision->width() > Decision::MINIMAL_WIDTH)) {
        mDecision->width(maxWidth);
        mWidth = mDecision->width() + 2 * OFFSET;
    }
    else if (mDecision->width() < Decision::MINIMAL_WIDTH) {
        mDecision->width(Decision::MINIMAL_WIDTH);
        mWidth = Decision::MINIMAL_WIDTH + 2 * OFFSET;
    }

    if (maxHeight != 0 && (maxHeight != mDecision->height()) &&
                (mDecision->height() > Decision::MINIMAL_HEIGHT)) {
        mDecision->height(maxHeight);
        mHeight = maxHeight + 2 * OFFSET;
    }
    else if (mDecision->height() < Decision::MINIMAL_HEIGHT) {
        mDecision->height(Decision::MINIMAL_HEIGHT);
        mHeight = Decision::MINIMAL_HEIGHT + 2 * OFFSET;
    }

    drawRoundedRectangle(OFFSET, OFFSET, mDecision->width(),
            mDecision->height(), 1.0, 20, 1., 1., 1.);

    mContext->move_to(OFFSET, OFFSET + HEADER_HEIGHT);
    mContext->line_to(
            mDecision->width() + OFFSET, OFFSET + HEADER_HEIGHT);
    mContext->stroke();
}

void DecisionDrawingArea::drawRoundedRectangle(guint x, guint y,
    guint width, guint height,
    double aspect,
    double corner_radius,
    double red,
    double green,
    double blue)
{
    double radius = corner_radius / aspect;
    double degrees = M_PI / 180.0;

    mContext->set_line_width(Settings::settings().getLineWidth());
    mContext->begin_new_sub_path();
    mContext->arc(x + width - radius, y + radius, radius,
        -90 * degrees, 0 * degrees);
    mContext->arc(x + width - radius, y + height - radius,
        radius, 0 * degrees, 90 * degrees);
    mContext->arc(x + radius, y + height - radius, radius,
        90 * degrees, 180 * degrees);
    mContext->arc(x + radius, y + radius, radius,
        180 * degrees, 270 * degrees);
    mContext->close_path();

    mContext->set_source_rgb(red, green, blue);
    mContext->fill_preserve();
    setColor(Settings::settings().getForegroundColor());
    mContext->stroke();
}

void DecisionDrawingArea::drawActivityModel(ActivityModel* activityModel)
{
    setColor(Settings::settings().getSelectedColor());

    if (std::find(mCurrentActivitiesModel.begin(),
            mCurrentActivitiesModel.end(), activityModel) !=
            mCurrentActivitiesModel.end()) {
        drawRoundedRectangle(activityModel->x(), activityModel->y(),
                activityModel->width(), activityModel->height(),
                1.0, 10, 0.44, 0.86, 0.58);
    }
    else {
        drawRoundedRectangle(activityModel->x(), activityModel->y(),
                activityModel->width(), activityModel->height(),
                1.0, 10, 1., 1., 0.75);
    }
    mContext->stroke();

    drawActivityModelName(activityModel);

    int pos = activityModel->y() + ACTIVITY_NAME_HEIGHT + OFFSET;

    mContext->move_to(activityModel->x() + OFFSET, pos);

    if (mState == ADD_CONSTRAINT_MODE && !mCurrentActivitiesModel.empty() &&
            activityModel == *mCurrentActivitiesModel.begin()) {
        drawAnchors(activityModel);
    }
}

void DecisionDrawingArea::drawActivityModelName(ActivityModel* activityModel)
{
    setColor(Settings::settings().getForegroundColor());
    mContext->select_font_face(Settings::settings().getFont(),
        Cairo::FONT_SLANT_NORMAL,
        Cairo::FONT_WEIGHT_NORMAL);
    mContext->set_font_size(Settings::settings().getFontSize());

    Cairo::TextExtents textExtents;

    mContext->get_text_extents(activityModel->name(), textExtents);
    mContext->move_to(
        activityModel->x() + (activityModel->width() - textExtents.width) / 2,
        activityModel->y() + (ACTIVITY_NAME_HEIGHT +
                      textExtents.height) / 2);
    mContext->show_text(activityModel->name());
    mContext->stroke();
}

void DecisionDrawingArea::drawActivitiesModel()
{
    if (!mCurrentActivitiesModel.empty()) {
        ActivityModel* actSelect = NULL;
        std::string nomSelect = mCurrentActivitiesModel.at(0)->name();
        for (activitiesModel_t::const_iterator it =
                mDecision->activitiesModel().begin();
                it != mDecision->activitiesModel().end(); ++it) {
            if (it->second->name() == nomSelect) {
                actSelect = it->second;
            }
            else {
                drawActivityModel(it->second);
            }
        }

        if (actSelect != NULL) {
            drawActivityModel(actSelect);
        }
    }
    else {
        for (activitiesModel_t::const_iterator it =
                mDecision->activitiesModel().begin();
                it != mDecision->activitiesModel().end(); ++it) {
            drawActivityModel(it->second);
        }
    }
}

void DecisionDrawingArea::drawPrecedenceConstraints()
{
    mContext->set_source_rgb(0., 0., 0.);
    for (precedenceConstraints_t::const_iterator
            it = mDecision->precedenceConstraints().begin();
            it != mDecision->precedenceConstraints().end(); ++it) {
        drawPrecedenceConstraint(*it);
    }
}

void DecisionDrawingArea::drawPrecedenceConstraint(
            PrecedenceConstraintModel* precedenceConstraint) {
    points_t::const_iterator it = precedenceConstraint->points().begin();
    double xs = it->x;
    double ys = it->y;
    double xd = (++it)->x;
    double yd = it->y;

    mContext->move_to(xs, ys);
    while (it != precedenceConstraint->points().end()) {
        mContext->line_to(xd, yd);
        ++it;
        if (it != precedenceConstraint->points().end()) {
            xs = xd;
            ys = yd;
            xd = it->x;
            yd = it->y;
        }
    }

    double d = std::sqrt((xd - xs) * (xd - xs) + (yd - ys) * (yd - ys));
    double angle = std::acos((xd - xs) / d);

    if (ys > yd) {
        angle *= -1;
    }

    if (xd != xs || yd != ys) {
        mContext->set_identity_matrix();
        mContext->move_to(xd, yd);
        mContext->rotate(angle + 0.3);
        mContext->rel_line_to(-8, 0);

        mContext->set_identity_matrix();
        mContext->move_to(xd, yd);
        mContext->rotate(angle - 0.3);
        mContext->rel_line_to(-8, 0);
        mContext->set_identity_matrix();
    }

    double xmh = (xs + xd) / 2;
    double ymh = (ys + yd) / 2;

    if (angle <= ((-3 * M_PI) / 4)) {
        mContext->move_to(xmh + 4, ymh - 4);
    }
    else if (angle <= ((- 2 * M_PI) / 4)) {
        mContext->move_to(xmh + 4, ymh);
    }
    else if (angle <= (-M_PI / 4)) {
        mContext->move_to(xmh + 4, ymh + 10);
    }
    else if (angle <= 0) {
        mContext->move_to(xmh - 10, ymh - 4);
    }
    else if (angle <= (M_PI / 4)) {
        mContext->move_to(xmh, ymh - 4);
    }
    else if (angle <= ((2 * M_PI) / 4)) {
        mContext->move_to(xmh + 4, ymh);
    }
    else if (angle <= ((3 * M_PI) / 4)) {
        mContext->move_to(xmh + 10, ymh);
    }
    else if (angle <= M_PI) {
        mContext->move_to(xmh - 20, ymh - 4);
    }
    else {
        mContext->move_to(xmh, ymh);
    }

    mContext->show_text(precedenceConstraint->cType());

    mContext->stroke();
}

bool DecisionDrawingArea::modifyCurrentActivityModel()
{
    if (!mCurrentActivitiesModel.empty()) {
        ActivityModel* activityModel = *mCurrentActivitiesModel.begin();
        std::string oldName = activityModel->name();
        ActivityDialog dialog(mXml, mDecision, activityModel,
                *mDecision->getRule(),*mDecision->getOutputFunc(),
                *mDecision->getAckFunc());

        if (dialog.run() == Gtk::RESPONSE_ACCEPT) {
            activityModel->name(dialog.name());
            activityModel->setRules(dialog.getActRule());
            activityModel->setOutputFunc(dialog.getActOut());
            activityModel->setAckFunc(dialog.getActAck());
            if (!dialog.minstart().empty() && !dialog.maxfinish().empty()) {
                activityModel->minstart(dialog.minstart());
                activityModel->maxfinish(dialog.maxfinish());
            }
            if (oldName != dialog.name()) {
                mDecision->changeActivityModelName(oldName,
                                             dialog.name());
            }
            activityModel->setRelativeDate(dialog.isRelativeDate());
            checkSize(activityModel, activityModel->name());

            int newWidth = activityModel->x() + activityModel->width() + OFFSET;
            int newHeight = activityModel->y() +
                    activityModel->height() + OFFSET;

            if (newWidth > mDecision->width() ||
                    newHeight > mDecision->height()) {
                if (newWidth > mDecision->width()) {
                    mDecision->width(newWidth);
                    mWidth = mDecision->width() + 2 * OFFSET;
                }

                if (newHeight > mDecision->height()) {
                    mDecision->height(newHeight);
                    mHeight = mDecision->height() + 2 * OFFSET;
                }
            }

            return true;
        }
    }
    return false;
}

bool DecisionDrawingArea::modifyCurrentPrecedenceConstraint()
{
    if (mCurrentPrecedenceConstraint != 0) {
        PrecedenceConstraintDialog dialog(mXml, *mCurrentPrecedenceConstraint);
        if (dialog.run() == Gtk::RESPONSE_ACCEPT) {
            mDecision->updatePrecedenceConstraint(
                    dialog.getPrecedenceConstraint());
            return true;
        }
    }
    return false;
}

point_t DecisionDrawingArea::searchAnchor(const ActivityModel* activityModel,
        guint x, guint y)
{
    const points_t& anchors = activityModel->anchors();
    point_t anchor;
    double min = std::numeric_limits < double >::max();

    for (points_t::const_iterator it = anchors.begin(); it != anchors.end();
            ++it) {
        double deltax = (double)x - it->x;
        double deltay = (double)y - it->y;
        double dst = std::sqrt(deltax * deltax + deltay * deltay);

        if (dst < min) {
            anchor = *it;
            min = dst;
        }
    }
    return anchor;
}

bool DecisionDrawingArea::on_button_press_event(GdkEventButton* event)
{
    if (event->button == 3) {
        mMenuPopup->popup(event->button, event->time);
    }
    else {
        switch (mState)
        {
        case SELECT_MODE:
            if (event->type == GDK_BUTTON_PRESS and event->button == 1) {
                if (selectActivityModel(event->x, event->y,
                        event->state & GDK_CONTROL_MASK)) {
                    queueRedraw();
                }
            }
            else if (event->type == GDK_2BUTTON_PRESS and event->button == 1) {
                if (selectActivityModel(event->x, event->y, false)) {
                    if (modifyCurrentActivityModel()) {
                        queueRedraw();
                    }
                }
                else if (selectPrecedenceConstraint(event->x, event->y)) {
                    if (modifyCurrentPrecedenceConstraint()) {
                        queueRedraw();
                    }
                }
            }
            break;
        case ADD_ACTIVITY_MODE:
            if (event->type == GDK_BUTTON_PRESS and event->button == 1) {
                if (addActivity(event->x, event->y)) {
                    queueRedraw();
                }
            }
            break;
        case DELETE_MODE:
            if (selectActivityModel(event->x, event->y, false)) {
                if (Question(
                    _("Are you sure you want to delete this activity?"))) {
                    mDecision->removeActivityModel(
                    *mCurrentActivitiesModel.begin());
                    mCurrentActivitiesModel.clear();
                    mCurrentPrecedenceConstraints.clear();
                    mCopiedActivitiesModel.clear();
                    queueRedraw();
                }
            }
            else if (selectPrecedenceConstraint(event->x, event->y)) {
                if (Question(
                    _("Are you sure you want to delete this constraint?"))) {
                    mDecision->removePrecedenceConstraint(
                            *mCurrentPrecedenceConstraint);
                    mCurrentPrecedenceConstraints.clear();
                    queueRedraw();
                }
            }
            break;
        case ADD_CONSTRAINT_MODE:
            if (event->type == GDK_BUTTON_PRESS and event->button == 1) {
                if (not (event->state & GDK_CONTROL_MASK) and
                        selectActivityModel(event->x, event->y, false)) {
                    mBegin = searchAnchor(*mCurrentActivitiesModel.begin(),
                            event->x, event->y);
                    mStartPoint.x = event->x;
                    mStartPoint.y = event->y;
                    mStartActivity = mCurrentActivitiesModel.front();
                }
            }
            break;
        default:
            break;
        };
    }

    return true;
}

bool DecisionDrawingArea::on_button_release_event(GdkEventButton* event)
{
    switch (mState) {
    case SELECT_MODE:
        mPreviousX = -1;
        mPreviousY = -1;
        mDecisionResize = false;

        if (mStartSelectRect.valid()) {

            mCurrentActivitiesModel.clear();

            if (mStartSelectRect.x > mEndSelectRect.x && mStartSelectRect.y
                    < mEndSelectRect.y) {
                for (activitiesModel_t::const_iterator it = mDecision->
                        activitiesModel().begin();
                        it != mDecision->activitiesModel().end(); ++it) {
                    if (it->second->select(mEndSelectRect.x, mStartSelectRect.y,
                        mStartSelectRect.x, mEndSelectRect.y)) {
                        mCurrentActivitiesModel.push_back(it->second);
                    }
                }
            }
            else if (mStartSelectRect.x > mEndSelectRect.x && mStartSelectRect.y
                    > mEndSelectRect.y) {
                for (activitiesModel_t::const_iterator it = mDecision->
                        activitiesModel().begin();
                        it != mDecision->activitiesModel().end(); ++it) {
                    if (it->second->select(mEndSelectRect.x, mEndSelectRect.y,
                        mStartSelectRect.x, mStartSelectRect.y)) {
                        mCurrentActivitiesModel.push_back(it->second);
                    }
                }
            }
            else if (mStartSelectRect.x < mEndSelectRect.x && mStartSelectRect.y
                    > mEndSelectRect.y) {
                for (activitiesModel_t::const_iterator it = mDecision->
                        activitiesModel().begin();
                        it != mDecision->activitiesModel().end(); ++it) {
                    if (it->second->select(mStartSelectRect.x, mEndSelectRect.y,
                        mEndSelectRect.x, mStartSelectRect.y)) {
                        mCurrentActivitiesModel.push_back(it->second);
                    }
                }
            }
            else {
                for (activitiesModel_t::const_iterator it = mDecision->
                        activitiesModel().begin();
                        it != mDecision->activitiesModel().end(); ++it) {
                    if (it->second->select(mStartSelectRect.x,
                            mStartSelectRect.y,
                            mEndSelectRect.x, mEndSelectRect.y)) {
                        mCurrentActivitiesModel.push_back(it->second);
                    }
                }
            }

            mStartSelectRect.invalid();
            queueRedraw();
        }
        break;
    case ADD_CONSTRAINT_MODE:
        if (event->button == 1) {
            if (not mCurrentActivitiesModel.empty() and mStartPoint.x !=
                       event->x
                       and mStartPoint.y != event->y) {
                if (selectActivityModel(event->x, event->y, false)) {
                    if (mStartActivity->name() !=
                                mCurrentActivitiesModel.at(0)->name()) {
                        points_t pts;
                        pts.push_back(mBegin);
                        point_t anchor = searchAnchor(
                                mCurrentActivitiesModel.at(0),
                                event->x,
                                event->y);
                        pts.push_back(anchor);

                        mDecision->addPrecedenceConstraint(
                                mStartActivity->name(),
                                mCurrentActivitiesModel.at(0)->name(), pts);
                    }
                }
            }

            mMouse.invalid();
            mBegin.invalid();
            mStartPoint.invalid();
            mCurrentActivitiesModel.clear();
            mStartActivity = NULL;
            queueRedraw();
        }
        break;
    default:
        break;
    }
    return true;
}

bool DecisionDrawingArea::on_configure_event(GdkEventConfigure* event)
{
    bool change = false;

    if (event->width > mWidth) {
        change = true;
        mWidth = event->width;
    }
    if (event->height > mHeight) {
        change = true;
        mHeight = event->height;
    }
    if (change) {
        set_size_request(mWidth, mHeight);
//        mBuffer = Gdk::Pixmap::create(mWin, mWidth, mHeight, -1);
        queueRedraw();
    }
    return true;
}

bool DecisionDrawingArea::on_draw(const Cairo::RefPtr<Cairo::Context>& context)
{
    mContext = context;
    mContext->set_line_width(Settings::settings().getLineWidth());
    draw();
    
    return true;
}

/*
bool DecisionDrawingArea::on_expose_event(GdkEventExpose*)
{
    if (mIsRealized) {
        if (!mBuffer) {
            mBuffer = Gdk::Pixmap::create(mWin, mWidth, mHeight, -1);
        }
        if (mBuffer) {
            if (mNeedRedraw) {
                mContext = mBuffer->create_cairo_context();
                mContext->set_line_width(Settings::settings().getLineWidth());
                draw();
                mNeedRedraw = false;
            }
            mWin->draw_drawable(mWingc, mBuffer, 0, 0, 0, 0, -1, -1);
        }
    }
    return true;
}
*/

bool DecisionDrawingArea::onQueryTooltip(int wx,int wy, bool,
            const Glib::RefPtr<Gtk::Tooltip>& tooltip)
{
    std::string card;
    bool found = false;
    activitiesModel_t::const_reverse_iterator it;
    it = mDecision->activitiesModel().rbegin();

    while (it != mDecision->activitiesModel().rend() && found == false) {
        found = it->second->select(wx, wy);
        if (!found) {
            ++it;
        }
    }

    if (found) {
        card = mDecision->getActivityCard(it->second->name());
        tooltip->set_markup(card);
        return true;
    }
    else if (selectPrecedenceConstraint(wx, wy)) {
        card = mCurrentPrecedenceConstraint->card();
        tooltip->set_markup(card);
        return true;
    }
    else {
        return false;
    }

}

//std::string DecisionDrawingArea::aboveActivity(int x, int y) {}

bool DecisionDrawingArea::on_motion_notify_event(GdkEventMotion* event)
{
    int button;

    if (event->state & GDK_BUTTON1_MASK) {
        button = 1;
    } else if (event->state & GDK_BUTTON2_MASK) {
        button = 2;
    } else {
        button = 0;
    }

    switch (mState) {
    case SELECT_MODE:
        if (button == 1) {
            if (not mCurrentActivitiesModel.empty()) {

                bool xok, yok;

                displaceActivitiesModel(
                        mPreviousX == -1 ? event->x : mPreviousX,
                        mPreviousY == -1 ? event->y : mPreviousY,
                        event->x, event->y, xok, yok);
                if (xok) {
                    mPreviousX = event->x;
                }
                if (yok) {
                    mPreviousY = event->y;
                }
                queueRedraw();
            }
            else {
                point_t tmp(event->x, event->y);

                if (mStartSelectRect.valid()) {
                    mEndSelectRect = tmp;
                    queueRedraw();
                }
                else {
                    mStartSelectRect = tmp;
                }
            }
        }
        break;
    case ADD_CONSTRAINT_MODE:
        if (button == 1) {
            if (mBegin.valid()) {
                if (selectActivityModel(event->x, event->y, false)) {
                    mEnd = searchAnchor(mCurrentActivitiesModel.front(),
                            event->x, event->y);
                }
            }
        }
        if (button == 0) {
            mCurrentActivitiesModel.clear();
        }
        mMouse.x = event->x;
        mMouse.y = event->y;
        queueRedraw();
        break;
    default:
        break;
    }
    return true;
}

void DecisionDrawingArea::on_realize()
{
    Gtk::DrawingArea::on_realize();
    mWin = get_window();
//    mWingc = Gdk::GC::create(mWin);
//    mIsRealized = true;
    queueRedraw();
}

void DecisionDrawingArea::drawAnchors(ActivityModel* activityModel)
{
    const points_t& points = activityModel->anchors();

    mContext->set_source_rgb(0., 0., 1.);
    for (points_t::const_iterator it = points.begin(); it != points.end();
         ++it) {
        mContext->rectangle(it->x - 2, it->y - 2, 5, 5);
        mContext->fill();
    }
}

bool DecisionDrawingArea::selectActivityModel(guint x, guint y, bool ctrl)
{
    bool found = false;
    activitiesModel_t::const_reverse_iterator it;

    for (it = mDecision->activitiesModel().rbegin();
            it != mDecision->activitiesModel().rend() && found == false;) {
        found = it->second->select(x,y);
        if (!found) {
            ++it;
        }
    }

    if (found) {
        if (not ctrl) {
            mCurrentActivitiesModel.clear();
        }

        if (std::find(mCurrentActivitiesModel.begin(),
                mCurrentActivitiesModel.end(),
                it->second) == mCurrentActivitiesModel.end()) {
            mCurrentActivitiesModel.push_back(it->second);
        }
    } else {
        mCurrentActivitiesModel.clear();
        queueRedraw();
    }
    return found;
}

bool DecisionDrawingArea::selectPrecedenceConstraint(guint mx, guint my)
{
    std::vector < PrecedenceConstraintModel* > ::const_iterator itt =
            mDecision->precedenceConstraints().begin();
    bool found = false;

    while (itt != mDecision->precedenceConstraints().end() and not found) {
        int xs2, ys2;
        points_t::iterator it = (*itt)->points().begin();

        xs2 = it->x;
        ys2 = it->y;
        ++it;
        while (it != (*itt)->points().end() and not found) {
            int xs, ys, xd, yd;

            if (xs2 == it->x) {
                xs = xs2 - 5;
                xd = it->x + 5;
            } else {
                xs = xs2;
                xd = it->x;
            }

            if (ys2 == it->y) {
                ys = ys2 - 5;
                yd = it->y + 5;
            } else {
                ys = ys2;
                yd = it->y;
            }

            double h = -1;
            if (std::min(xs,
                    xd) <= (int)mx and (int)mx <= std::max(xs, xd)
                and std::min(ys, yd) <= (int)my
                and (int)my <= std::max(ys, yd)) {
                const double a = (ys - yd) / (double)(xs - xd);
                const double b = ys - a * xs;
                h = std::abs((my - (a * mx) - b) / std::sqrt(1 + a * a));
                if (h <= 10) {
                    mCurrentPrecedenceConstraint = (*itt);
                    found = true;
                }
            }
            if (not found) {
                xs2 = it->x;
                ys2 = it->y;
                ++it;
            }
        }
        if (not found) {
            ++itt;
        }
    }
    return found;
}

void DecisionDrawingArea::setColor(const Gdk::Color& color)
{
    mContext->set_source_rgb(color.get_red_p(),
        color.get_green_p(),
        color.get_blue_p());
}

}
}
}
}    // namespace vle gvle modeling decision
