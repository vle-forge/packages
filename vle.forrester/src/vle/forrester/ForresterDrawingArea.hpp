/*
 * @file vle/forrester/ForresterDrawingArea.hpp
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

#ifndef VLE_GVLE_MODELING_FORRESTER_FORRESTER_DRAWING_AREA_HPP
#define VLE_GVLE_MODELING_FORRESTER_FORRESTER_DRAWING_AREA_HPP

#ifndef M_PI
#define M_PI           3.14159265358979323846
#endif

#include <vle/forrester/Forrester.hpp>
#include <vle/gvle/Settings.hpp>
#include <vle/gvle/Message.hpp>
#include <gtkmm/filechooserdialog.h>
#include <gtkmm/scrolledwindow.h>
#include <gtkmm/drawingarea.h>
#include <gtkmm/filefilter.h>
#include <gdkmm/color.h>
#include <gtkmm/builder.h>
#include <gtkmm/menu.h>
#include <gtkmm/stock.h>
#include <gtkmm/uimanager.h>

namespace vle {
namespace gvle {
namespace modeling {
namespace forrester {

/*!
 * @class ForresterDrawingArea
 * @brief Forrester diagram area.
 * This class display a graphical view which allows to edit a Forrester diagram.
 */
class ForresterDrawingArea : public Gtk::DrawingArea
{
public:
    /*!
     * @enum tool_states
     * @brief States of the ForresterDrawingArea.
     */
    enum tool_states {
        SELECT,         /*!< Select mode : displace or modify element*/
        ADD_COMPARTMENT,/*!< Add mode : add a compartment to diagram*/
        DELETE,          /*!< Delete mode : remove a compartment from diagram*/
        ADD_FLOW,        /*!< Add mode : add a flow to diagram*/
        ADD_FLOW_ARROW,  /*!< Add mode : add a flow arrow to diagram*/
        ADD_DEPENDENCY_ARROW,/*!< Add mode : add a dependency arrow to diagram*/
        ADD_PARAMETER,  /*!< Add mode : add a parameter to diagram*/
        ADD_EXOGENOUS_VARIABLE,/*!< Add mode : add an exogenous variable to diagram*/
        ADD_VARIABLE    /*!< Add mode : add a variable to diagram*/
    };

    ForresterDrawingArea(BaseObjectType* cobject,
                          const Glib::RefPtr < Gtk::Builder >& xml);

    virtual ~ForresterDrawingArea()
    { }

    /*!
     * @brief Set the Forrester model used by the Forrester diagram area.
     */
    void setForrester(Forrester* forrester_)
    {
        mForrester = forrester_;
        set_size_request(mForrester->getForresterGI().getWidth(),
                         mForrester->getForresterGI().getHeight());
//        updateBuffer();
        queueRedraw();
    }

    /*!
     * @brief Modify the current state of the Forrester drawing area.
     * @param state
     * @see tool_states
     */
    void setState(int state)
    {
        mState = state;
        mCurrentArrow = 0;
        queueRedraw();
    }

private:
    ForresterDrawingArea(const ForresterDrawingArea&);
    ForresterDrawingArea& operator= (const ForresterDrawingArea&);

    /*!
     * @brief Check if the buffer size is correct
     */
    void checkBuffer(int width, int height);
    /*!
     * @brief Export diagram to a PNG picture.
     * @param filename filename of the png
     */
    void exportPng(const std::string& filename);

    /*!
     * @brief Export diagram to a PDF file.
     * @param filename filename of the pdf
     */
    void exportPdf(const std::string& filename);

    /*!
     * @brief Export diagram to a SVG picture.
     * @param filename filename of the svg
     */
    void exportSvg(const std::string& filename);

    /*!
     * @brief Launch window to save the digram with a specified file type.
     */
    void exportGraphic();

    /*!
     * @brief Initilization of the popup which is accessible by a right click.
     */
    void initMenuPopupModels();

    /*!
     * @brief GTKmm event.
     * Called to redraw the drawing area
     */
//    virtual bool on_expose_event(GdkEventExpose*);

    /*!
     * @brief GTKmm event.
     */
    virtual void on_realize();

    /*!
     * @brief GTKmm event.
     */
    virtual bool on_configure_event(GdkEventConfigure* event);

    /*!
     * @brief GTKmm event.
     */
    virtual bool on_button_press_event(GdkEventButton* event);

    /*!
     * @brief GTKmm event.
     */
    virtual bool on_motion_notify_event(GdkEventMotion* event);

    /*!
     * @brief GTKmm event.
     */
    virtual bool on_button_release_event(GdkEventButton* event);


    virtual bool on_draw(const Cairo::RefPtr<Cairo::Context> &context);
    virtual bool on_draw_old(const Cairo::RefPtr<Cairo::Context> &cr);

    /*!
     * @brief Main method to draw all Forrester diagramm.
     * @see drawBackground drawRectangle drawName drawCompartment
     */
    void draw();

    /*!
     * @brief Draw a background
     */
    void drawBackground();

    /*!
     * @brief Update drawing area
     */
    void queueRedraw()
    {
//        mNeedRedraw = true;
        queue_draw();
    }

    /*!
     * @brief Displace a graphical item on the drawing area
     */
    void displace(GraphicalItem* item, int newx,
        int newy, bool& xok, bool& yok);

    /*!
     * @brief Update the pixmap size
     */
/*    void updateBuffer()
    {
        mBuffer = Gdk::Pixmap::create(mWin,
            mForrester->getForresterGI().getWidth() + 10,
            mForrester->getForresterGI().getHeight() + 10, -1);

    }
*/

    /*!
     * @brief Update a part of drawing area
     */
    void queueRedraw(int x, int y, int w, int h)
    {
//        mNeedRedraw = true;
        queue_draw_area(x, y, w, h);
    }

    /*!
     * @brief Set the current color of the drawing handler
     */
    void setColor(const Gdk::Color& color);

    /*!
     * @brief Resize the Forrester drawing area.
     * @param oldx Old x coordinate
     * @param oldy Old y coordinate
     * @param newx New x coordinate
     * @param newy New y coordinate
     * @param[out] xok Indicate if the x coordinate is correct or not.
     * @param[out] yok Indicate if the y coordinate is correct or not.
     */
    void resizeForrester(int oldx, int oldy, int newx,
        int newy, bool& xok, bool& yok);

    void on_button_press_event_SELECT(GdkEventButton* ev);
    void on_button_press_event_DELETE(GdkEventButton* ev);
    void on_button_press_event_ADD_COMPARTMENT(GdkEventButton* ev);
    void on_button_press_event_ADD_FLOW(GdkEventButton* ev);
    void on_button_press_event_ADD_PARAMETER(GdkEventButton* ev);
    void on_button_press_event_ADD_EXOGENOUS_VARIABLE(GdkEventButton* ev);
    void on_button_press_event_ADD_VARIABLE(GdkEventButton* ev);
    void on_button_press_event_ADD_FLOW_ARROW(GdkEventButton* ev);
    void on_button_press_event_ADD_DEPENDENCY_ARROW(GdkEventButton* ev);

    void on_motion_notify_event_SELECT(GdkEventMotion* ev);
    void on_button_release_event_ADD_FLOW(GdkEventButton* event);

    Gtk::Menu* mMenuPopup;
    Glib::RefPtr < Gtk::Builder > mXml;/*!< @brief Glade xml file. */
//    Glib::RefPtr < Gdk::Pixmap > mBuffer;
    Glib::RefPtr < Gdk::Window > mWin;
//    Glib::RefPtr < Gdk::GC > mWingc;
    Cairo::RefPtr < Cairo::Context > mContext;/*!< @brief Cairo context */

    Forrester* mForrester;/*!< @brief Forrester data model */

    /*! @brief Redraw flag.
     *   true if the drawing area needs to be redrawn, false otherwise
     */
//    bool mNeedRedraw;
//    bool mIsRealized;/*!< @brief Width of the drawing area */
    bool mForresterResize;
    int mPreviousX, mPreviousY;
    int mState;/*!< @brief Drawing area current state
                *   @see tool_states
                */
    int mCurrentX, mCurrentY;
    GraphicalItem *focusItem, *tooltipItem;
    Arrow *mCurrentArrow;
};

}
}
}
}
#endif
