/*
 * @file vle/forrester/ForresterDrawingArea.cpp
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

#include <vle/forrester/ForresterDrawingArea.hpp>
#include <vle/forrester/graphicalItems/Compartment.hpp>
#include <vle/forrester/graphicalItems/Anchor.hpp>
#include <vle/forrester/graphicalItems/Flow.hpp>
#include <vle/forrester/graphicalItems/FlowArrow.hpp>
#include <vle/forrester/graphicalItems/Parameter.hpp>
#include <vle/forrester/graphicalItems/DependencyArrow.hpp>
#include <vle/forrester/graphicalItems/ExogenousVariable.hpp>
#include <vle/forrester/graphicalItems/Variable.hpp>


namespace vle {
namespace gvle {
namespace modeling {
namespace forrester {

ForresterDrawingArea::ForresterDrawingArea(BaseObjectType* cobject,
    const Glib::RefPtr < Gtk::Builder >& xml) :
    Gtk::DrawingArea(cobject),
    mXml(xml),
    mForrester(0),
//    mNeedRedraw(true),
//    mIsRealized(false),
    mPreviousX(-1),
    mPreviousY(-1),
    mState(ForresterDrawingArea::SELECT),
    focusItem(0),
    tooltipItem(0),
    mCurrentArrow(0)
{
    set_events(Gdk::POINTER_MOTION_MASK | Gdk::BUTTON_MOTION_MASK |
        Gdk::BUTTON1_MOTION_MASK | Gdk::BUTTON2_MOTION_MASK |
        Gdk::BUTTON3_MOTION_MASK | Gdk::BUTTON_PRESS_MASK |
        Gdk::BUTTON_RELEASE_MASK);
    
//    modify_bg(Gtk::STATE_NORMAL, Gdk::Color("#FFFFFF"));
    override_background_color (Gdk::RGBA("#FFFFFF00"), Gtk::STATE_FLAG_NORMAL);
    
    initMenuPopupModels();
    set_double_buffered(true);
}

bool ForresterDrawingArea::on_draw(const Cairo::RefPtr<Cairo::Context>& context)
{
    mContext = context;
    mContext->set_line_width(Settings::settings().getLineWidth());

    on_draw_old (context);

    draw();
    
    return true;
}

/*
bool ForresterDrawingArea::on_expose_event(GdkEventExpose*)
{
    if (mIsRealized) {
        if (!mBuffer) {
            updateBuffer();
        }
        if (mBuffer) {
            if (mNeedRedraw) {
                mContext = mBuffer->create_cairo_context();
                mContext->set_line_width(
                    Settings::settings().getLineWidth());
                draw();
                mNeedRedraw = false;
            }
            mWin->draw_drawable(mWingc, mBuffer, 0, 0, 0, 0, -1, -1);
        }
    }
    return true;
}
*/

void ForresterDrawingArea::on_realize()
{
    Gtk::DrawingArea::on_realize();
    mWin = get_window();
//    mWingc = Gdk::GC::create(mWin);
//    mIsRealized = true;
    queueRedraw();
}

bool ForresterDrawingArea::on_configure_event(GdkEventConfigure* ev)
{
    checkBuffer(ev->x, ev->y);
    return true;
}

bool ForresterDrawingArea::on_motion_notify_event(GdkEventMotion* ev)
{
    switch (mState) {
    case SELECT:
        on_motion_notify_event_SELECT(ev);
        break;
    case ADD_FLOW_ARROW:
        if(mCurrentArrow != 0) {
            mCurrentArrow->getDestination()->setX(ev->x);
            mCurrentArrow->getDestination()->setY(ev->y);
            queueRedraw();
        }
        break;
    case ADD_DEPENDENCY_ARROW:
        if(mCurrentArrow != 0) {
            mCurrentArrow->getDestination()->setX(ev->x);
            mCurrentArrow->getDestination()->setY(ev->y);
            dynamic_cast<DependencyArrow*>(mCurrentArrow)->
                computeControlPoint();
            queueRedraw();
        }
    break;
    default:
        break;
    }
    return true;
}

void ForresterDrawingArea::resizeForrester(int oldx, int oldy,
    int newx, int newy, bool& xok, bool& yok)
{
    int deltax = newx - oldx;
    int deltay = newy - oldy;

    xok = true;
    yok = true;
    if (deltax != 0 or deltay != 0) {
        for (graphical_items::const_iterator
        it = mForrester->getItems().begin();
        it != mForrester->getItems().end(); ++it) {
            if ((int)((*it)->getX() + (*it)->getWidth() +
            ForresterGI::GLOBAL_OFFSET) >
            mForrester->getForresterGI().getWidth() + deltax) {
                deltax = 0;
                xok = false;
            }
            if ((int)((*it)->getY() + (*it)->getHeight() +
            ForresterGI::GLOBAL_OFFSET) >
            mForrester->getForresterGI().getHeight() + deltay) {
                deltay = 0;
                yok = false;
            }
        }
    }
    if (mForrester->getForresterGI().getWidth()
    + deltax < ForresterGI::MINIMAL_WIDTH) {
        deltax = 0;
        xok = false;
    }
    if (mForrester->getForresterGI().getHeight() + deltay <
    ForresterGI::MINIMAL_HEIGHT) {
        deltay = 0;
        yok = false;
    }
    if (deltax != 0 || deltay != 0) {
        mForrester->getForresterGI().resize(deltax +
            mForrester->getForresterGI().getWidth(),
            deltay + mForrester->getForresterGI().getHeight());
        set_size_request(deltax + mForrester->getForresterGI().getWidth(),
            deltay + mForrester->getForresterGI().getHeight());
    }
}

bool ForresterDrawingArea::on_button_press_event(GdkEventButton* ev)
{
    if (ev->button == 3) {
        mMenuPopup->popup(ev->button, ev->time);
    }
    switch (mState) {
    case SELECT:
        on_button_press_event_SELECT(ev);
        break;

    case ADD_COMPARTMENT:
        on_button_press_event_ADD_COMPARTMENT(ev);
        break;

    case DELETE:
        on_button_press_event_DELETE(ev);
        break;

    case ADD_FLOW:
        on_button_press_event_ADD_FLOW(ev);
        break;

    case ADD_PARAMETER:
        on_button_press_event_ADD_PARAMETER(ev);
        break;

    case ADD_EXOGENOUS_VARIABLE:
        on_button_press_event_ADD_EXOGENOUS_VARIABLE(ev);
        break;

    case ADD_VARIABLE:
        on_button_press_event_ADD_VARIABLE(ev);
        break;

    case ADD_FLOW_ARROW:
        on_button_press_event_ADD_FLOW_ARROW(ev);
        break;

    case ADD_DEPENDENCY_ARROW:
        on_button_press_event_ADD_DEPENDENCY_ARROW(ev);
        break;

    default:
        break;
    };
    
    return true;
}

bool ForresterDrawingArea::on_draw_old(
        const Cairo::RefPtr<Cairo::Context> &cr)
{
    cr->set_source_rgb(1, 1, 1);
    cr->rectangle(0, 0, get_width(), get_height());
    cr->fill();
    return true;
}


bool ForresterDrawingArea::on_button_release_event(
        GdkEventButton*  ev)
{
    bool found;

    switch (mState) {
    case SELECT:
        mPreviousX = -1;
        mPreviousY = -1;
        mForresterResize = false;
        focusItem = 0;
        break;

    case ADD_FLOW_ARROW:
        found = false;
        if(mCurrentArrow !=0) {
            for (graphical_items::const_iterator it =
            mForrester->getItems().begin();
            it != mForrester->getItems().end() && !found; ++it) {
                for(anchors_t::iterator itA = (*it)->getAnchors().begin();
                itA != (*it)->getAnchors().end() && !found; ++itA) {
                    if((*itA)->select(ev->x,ev->y)) {
                        delete(mCurrentArrow->getDestination());
                        mCurrentArrow->setDestination(*itA);
                        mForrester->add(mCurrentArrow);
                        found = true;
                    }
                }
            }
        }
        if(!found)
            delete mCurrentArrow;

        mCurrentArrow = 0;

        queueRedraw();
        break;
    case ADD_DEPENDENCY_ARROW:
        found = false;
        if(mCurrentArrow !=0) {
            for (graphical_items::const_iterator it = mForrester->getItems().begin();
            it != mForrester->getItems().end() && !found; ++it) {
                if(dynamic_cast<Flow*>(*it) || dynamic_cast<Variable*>(*it)) {
                    for(anchors_t::iterator itA = (*it)->getAnchors().begin();
                    itA != (*it)->getAnchors().end() && !found; ++itA) {
                        if((*itA)->select(ev->x,ev->y)) {
                            delete(mCurrentArrow->getDestination());
                            mCurrentArrow->setDestination(*itA);
                            mForrester->add(mCurrentArrow);
                            found = true;
                        }
                    }
                }
            }
        }
        if(!found)
            delete mCurrentArrow;

        mCurrentArrow = 0;

        queueRedraw();
        break;
    case ADD_FLOW:
        on_button_release_event_ADD_FLOW(ev);
        break;
    default:
        break;
    }

    return true;
}

void ForresterDrawingArea::on_button_release_event_ADD_FLOW(GdkEventButton* ev) {
    GraphicalItem* lastItem = 0;
    Flow* newFlow = 0;

    lastItem = mForrester->getSelectedItem(ev->x, ev->y);

    if(!focusItem)
        newFlow  = new Flow(mPreviousX, mPreviousY,*mForrester);
    else if(!lastItem)
        newFlow  = new Flow(ev->x, ev->y,*mForrester);
    else
        newFlow  = new Flow((ev->x + mPreviousX)/2., (ev->y+mPreviousY)/2.,*mForrester);


    if(newFlow->launchCreationWindow(mXml)) {
        newFlow->checkSize(mContext);
        mForrester->add(newFlow);
        checkBuffer(
            newFlow->getX() + newFlow->getWidth() + newFlow->getRightOffset(),
            newFlow->getY() + newFlow->getHeight() + newFlow->getBottomOffset());
        queueRedraw();
    } else {
        delete newFlow;
        newFlow = 0;
    }


    if(newFlow) {
        mForrester->linkFlowCompartments(
        newFlow,
        dynamic_cast<Compartment*>(focusItem),
        dynamic_cast<Compartment*>(lastItem));
    }

    focusItem = 0;
    mPreviousX = -1;
    mPreviousY = -1;
}



void ForresterDrawingArea::draw(){
    if (mContext) {
        mContext->save();
        mContext->set_line_width(Settings::settings().getLineWidth());

        drawBackground();
        mForrester->getForresterGI().draw(mContext);

        for (arrows::const_iterator it =
        mForrester->getArrows().begin();
        it != mForrester->getArrows().end(); ++it) {
            (*it)->draw(mContext);
        }

        if (mCurrentArrow != 0 &&
        (mState == SELECT || mState == ADD_DEPENDENCY_ARROW ||
        mState == ADD_FLOW_ARROW || mState == DELETE)) {
            mCurrentArrow->draw(mContext);
            if(mState != ADD_FLOW_ARROW)
                mCurrentArrow->drawAnchors(mContext);
        }

        for (graphical_items::const_iterator it =
        mForrester->getItems().begin();
        it != mForrester->getItems().end(); ++it) {
            (*it)->draw(mContext);
            if (mState == ADD_FLOW_ARROW || mState == ADD_DEPENDENCY_ARROW)
                (*it)->drawAnchors(mContext);
        }

        mContext->restore();
    }


}


void ForresterDrawingArea::drawBackground()
{
    setColor(Settings::settings().getBackgroundColor());
    mContext->rectangle(0, 0, mForrester->getForresterGI().getWidth() + 10,
        mForrester->getForresterGI().getHeight() + 10);
    mContext->fill();
    mContext->stroke();
}

void ForresterDrawingArea::setColor(const Gdk::Color& color)
{
    mContext->set_source_rgb(color.get_red_p(),
        color.get_green_p(),
        color.get_blue_p());
}

void ForresterDrawingArea::initMenuPopupModels()
{
    Glib::RefPtr <Gtk::ActionGroup> mPopupActionGroup = Gtk::ActionGroup::create("ForresterDrawingArea");
    
    mPopupActionGroup->add(Gtk::Action::create("FDA_ExportGraphic", _("_Export Graphic")), sigc::mem_fun(*this, &ForresterDrawingArea::exportGraphic));
    
    Glib::RefPtr <Gtk::UIManager> mUIManager = Gtk::UIManager::create();
    mUIManager->insert_action_group(mPopupActionGroup);
    
    Glib::ustring ui_info =
                "<ui>"
                "  <popup name='FDA_Popup'>"
                "    <menuitem action='FDA_ExportGraphic'/>"
                "  </popup>"
                "</ui>";
    
    try {
      mUIManager->add_ui_from_string(ui_info);
      mMenuPopup = (Gtk::Menu *) (mUIManager->get_widget("/FDA_Popup"));
    } catch(const Glib::Error& ex) {
      std::cerr << "building menus failed: FDA_Popup " <<  ex.what();
    }
    
    if (!mMenuPopup)
      std::cerr << "not a menu : FDA_Popup\n";
}

void ForresterDrawingArea::exportPng(const std::string& filename)
{
    Cairo::RefPtr < Cairo::ImageSurface > surface =
        Cairo::ImageSurface::create(Cairo::FORMAT_ARGB32,
        mForrester->getForresterGI().getWidth(),
        mForrester->getForresterGI().getHeight());
    mContext = Cairo::Context::create(surface);
    mContext->set_line_width(Settings::settings().getLineWidth());
    mContext->select_font_face(Settings::settings().getFont(),
        Cairo::FONT_SLANT_NORMAL,
        Cairo::FONT_WEIGHT_NORMAL);
    mContext->set_font_size(Settings::settings().getFontSize());
    draw();
    surface->write_to_png(filename + ".png");
}

void ForresterDrawingArea::exportPdf(const std::string& filename)
{
    Cairo::RefPtr < Cairo::PdfSurface > surface =
        Cairo::PdfSurface::create(filename + ".pdf",
        mForrester->getForresterGI().getWidth(),
        mForrester->getForresterGI().getHeight());
    mContext = Cairo::Context::create(surface);
    mContext->set_line_width(Settings::settings().getLineWidth());
    mContext->select_font_face(Settings::settings().getFont(),
        Cairo::FONT_SLANT_NORMAL,
        Cairo::FONT_WEIGHT_NORMAL);
    mContext->set_font_size(Settings::settings().getFontSize());
    draw();
    mContext->show_page();
    surface->finish();
}

void ForresterDrawingArea::exportSvg(const std::string& filename)
{
    Cairo::RefPtr < Cairo::SvgSurface > surface =
        Cairo::SvgSurface::create(filename + ".svg",
        mForrester->getForresterGI().getWidth(),
        mForrester->getForresterGI().getHeight());
    mContext = Cairo::Context::create(surface);
    mContext->set_line_width(Settings::settings().getLineWidth());
    mContext->select_font_face(Settings::settings().getFont(),
        Cairo::FONT_SLANT_NORMAL,
        Cairo::FONT_WEIGHT_NORMAL);
    mContext->set_font_size(Settings::settings().getFontSize());
    draw();
    mContext->show_page();
    surface->finish();
}
void ForresterDrawingArea::exportGraphic()
{
    Gtk::FileChooserDialog file(_(
        "Image file"),
                                Gtk::FILE_CHOOSER_ACTION_SAVE);
    file.add_button(Gtk::Stock::CANCEL, Gtk::RESPONSE_CANCEL);
    file.add_button(Gtk::Stock::OK, Gtk::RESPONSE_OK);
    Glib::RefPtr<Gtk::FileFilter> filterAuto = Gtk::FileFilter::create ();
    Glib::RefPtr<Gtk::FileFilter> filterPng = Gtk::FileFilter::create ();
    Glib::RefPtr<Gtk::FileFilter> filterPdf = Gtk::FileFilter::create ();
    Glib::RefPtr<Gtk::FileFilter> filterSvg = Gtk::FileFilter::create ();
    filterAuto->set_name(_("Guess type from file name"));
    filterAuto->add_pattern("*");
    filterPng->set_name(_("Portable Newtork Graphics (.png)"));
    filterPng->add_pattern("*.png");
    filterPdf->set_name(_("Portable Format Document (.pdf)"));
    filterPdf->add_pattern("*.pdf");
    filterSvg->set_name(_("Scalable Vector Graphics (.svg)"));
    filterSvg->add_pattern("*.svg");
    file.add_filter(filterAuto);
    file.add_filter(filterPng);
    file.add_filter(filterPdf);
    file.add_filter(filterSvg);


    if (file.run() == Gtk::RESPONSE_OK) {
        std::string filename(file.get_filename());
        std::string extension(file.get_filter()->get_name());

        if (extension == _("Guess type from file name")) {
            size_t ext_pos = filename.find_last_of('.');
            if (ext_pos != std::string::npos) {
                std::string type(filename, ext_pos + 1);
                filename.resize(ext_pos);
                if (type == "png") {
                    exportPng(filename);
                } else if (type == "pdf") {
                    exportPdf(filename);
                } else if (type == "svg") {
                    exportSvg(filename);
                } else {
                    Error(_("Unsupported file format"));
                }
            }
        } else if (extension == _("Portable Newtork Graphics (.png)")) {
            exportPng(filename);
        } else if (extension == _("Portable Format Document (.pdf)")) {
            exportPdf(filename);
        } else if (extension == _("Scalable Vector Graphics (.svg)")) {
            exportSvg(filename);
        }
    }
}

void ForresterDrawingArea::displace(GraphicalItem* item, int newx,
    int newy, bool& xok, bool& yok)
{
    mPreviousX = mPreviousX == -1 ? newx : mPreviousX;
    mPreviousY = mPreviousY == -1 ? newy : mPreviousY;

    int deltax = newx - mPreviousX;
    int deltay = newy - mPreviousY;

    if (deltax != 0 || deltay != 0 ) {
        if (item->getX() + deltax < item->getLeftOffset())
            deltax = 0;
        else
            xok = true;

        if ((item->getY() + deltay < item->getTopOffset()
            + ForresterGI::HEADER_HEIGHT)  || newy < 0 )
            deltay = 0;
        else
            yok = true;

        item->displace(deltax, deltay);

        if (mForrester->checkDiagramSize()) {
            set_size_request(mForrester->getForresterGI().getWidth(),
                mForrester->getForresterGI().getHeight());
        }
    }
}

void ForresterDrawingArea::checkBuffer(int width, int height) {
    bool change = false;

    if (width > mForrester->getForresterGI().getWidth()) {
        change = true;
        mForrester->getForresterGI().setWidth(width);
    }
    if (height > mForrester->getForresterGI().getHeight()) {
        change = true;
        mForrester->getForresterGI().setHeight(height);
    }
    if (change) {
       set_size_request(mForrester->getForresterGI().getWidth() +10,
            mForrester->getForresterGI().getHeight() + 10);
        queueRedraw();
    }
}

void ForresterDrawingArea::on_button_press_event_SELECT(
        GdkEventButton* ev) {
    if (ev->type == GDK_BUTTON_PRESS and ev->button == 1) {
        if ((focusItem = mForrester->getSelectedItem(ev->x, ev->y))
        == 0) {
            int x = ev->x - ForresterGI::GLOBAL_OFFSET;
            int y = ev->y - ForresterGI::GLOBAL_OFFSET;
            int forresterHeight = mForrester->getForresterGI().getHeight();
            int forresterWidth = mForrester->getForresterGI().getWidth();

            mForresterResize = (x >= forresterWidth - 20) and
                x <= forresterWidth and
                y >= forresterHeight - 20 and
                y <= forresterHeight;
        }
    } else if (ev->type == GDK_2BUTTON_PRESS and ev->button == 1) {
        GraphicalItem* item;
        if((item = mForrester->getSelectedItem(ev->x, ev->y))
        != 0) {
            item->launchEditionWindow(mXml);
            item->checkSize(mContext);
            queueRedraw();
        }
    }
}

void ForresterDrawingArea::on_button_press_event_DELETE(
        GdkEventButton* ev) {
    if((focusItem = mForrester->getSelectedItem(ev->x, ev->y))
    != 0) {
        if (Question(_("Are you sure to delete this element ?"))) {
            mForrester->remove(focusItem);
        }
        queueRedraw();
    } else {
        Arrow *ArrowtoDel;
        if ((ArrowtoDel =
        mForrester->getSelectedArrow(ev->x,ev->y)) != 0) {
            if (Question(_("Are you sure to delete this arrow ?")))
                mForrester->remove(ArrowtoDel);

            queueRedraw();
        }
    }
}
void ForresterDrawingArea::on_button_press_event_ADD_COMPARTMENT(
        GdkEventButton* ev) {
    if (ev->type == GDK_BUTTON_PRESS and ev->button == 1) {
        Compartment* newCompartment =
            new Compartment(ev->x, ev->y,*mForrester);
        if(newCompartment->launchCreationWindow(mXml)) {
            newCompartment->checkSize(mContext);
            mForrester->add(newCompartment);
            checkBuffer(
                newCompartment->getX() + newCompartment->getWidth() + newCompartment->getRightOffset(),
                newCompartment->getY() + newCompartment->getHeight() + newCompartment->getBottomOffset());
            queueRedraw();
        } else {
            delete newCompartment;
        }
    }
}

void ForresterDrawingArea::on_button_press_event_ADD_FLOW(
        GdkEventButton* ev) {
    if (ev->type == GDK_BUTTON_PRESS and ev->button == 1) {
        focusItem = mForrester->getSelectedItem(ev->x, ev->y);
        mPreviousX = ev->x;
        mPreviousY = ev->y;
    }
}

void ForresterDrawingArea::on_button_press_event_ADD_PARAMETER(
        GdkEventButton* ev) {
    if (ev->type == GDK_BUTTON_PRESS and ev->button == 1) {
        Parameter *newParameter =
            new Parameter(ev->x, ev->y,*mForrester);
        if(newParameter->launchCreationWindow(mXml)) {
            newParameter->checkSize(mContext);
            mForrester->add(newParameter);
            checkBuffer(
                newParameter->getX() + newParameter->getWidth() + newParameter->getRightOffset(),
                newParameter->getY() + newParameter->getHeight() + newParameter->getBottomOffset());
            queueRedraw();
        } else {
            delete newParameter;
        }
    }
}

void ForresterDrawingArea::on_button_press_event_ADD_EXOGENOUS_VARIABLE(
        GdkEventButton* ev) {
    if (ev->type == GDK_BUTTON_PRESS and ev->button == 1) {
        ExogenousVariable* newVar =
            new ExogenousVariable(ev->x, ev->y,*mForrester);
        if(newVar->launchCreationWindow(mXml)) {
            newVar->checkSize(mContext);
            mForrester->add(newVar);
            checkBuffer(
                newVar->getX() + newVar->getWidth() + newVar->getRightOffset(),
                newVar->getY() + newVar->getHeight() + newVar->getBottomOffset());
            queueRedraw();
        } else {
            delete newVar;
        }
    }
}

void ForresterDrawingArea::on_button_press_event_ADD_VARIABLE(
        GdkEventButton* ev) {
    if (ev->type == GDK_BUTTON_PRESS and ev->button == 1) {
        Variable* newVar =
            new Variable(ev->x, ev->y,*mForrester);
        if(newVar->launchCreationWindow(mXml)) {
            newVar->checkSize(mContext);
            mForrester->add(newVar);
            checkBuffer(
                newVar->getX() + newVar->getWidth() + newVar->getRightOffset(),
                newVar->getY() + newVar->getHeight() + newVar->getBottomOffset());
            queueRedraw();
        } else {
            delete newVar;
        }
    }
}

void ForresterDrawingArea::on_button_press_event_ADD_FLOW_ARROW(
        GdkEventButton* ev) {
    if (ev->type == GDK_BUTTON_PRESS and ev->button == 1) {
        bool found = false;
        for (graphical_items::const_iterator it =
        mForrester->getItems().begin();
        it != mForrester->getItems().end() && !found; ++it) {
            for(anchors_t::iterator itA = (*it)->getAnchors().begin();
            itA != (*it)->getAnchors().end() && !found; ++itA) {
                if((*itA)->select(ev->x,ev->y)) {
                    mCurrentArrow = new FlowArrow();
                    mCurrentArrow->setOrigin(*itA);
                    mCurrentArrow->setDestination(new Anchor(
                        ev->x,ev->y));
                    found = true;
                }
            }
        }
    }
}

void ForresterDrawingArea::on_button_press_event_ADD_DEPENDENCY_ARROW(
        GdkEventButton* ev) {
    if (ev->type == GDK_BUTTON_PRESS and ev->button == 1) {
        bool found = false;
        for (graphical_items::const_iterator it = mForrester->getItems().begin();
        it != mForrester->getItems().end() && !found; ++it) {
            if (!dynamic_cast<Flow*>(*it)) {
                for(anchors_t::iterator itA = (*it)->getAnchors().begin();
                itA != (*it)->getAnchors().end() && !found; ++itA) {
                    if((*itA)->select(ev->x,ev->y)) {
                        mCurrentArrow = new DependencyArrow();
                        mCurrentArrow->setOrigin(*itA);
                        mCurrentArrow->setDestination(
                            new Anchor(ev->x,ev->y));
                        found = true;
                    }
                }
            }
        }
    }
}

void ForresterDrawingArea::on_motion_notify_event_SELECT(
        GdkEventMotion* ev) {
    int button;

    if (ev->state & GDK_BUTTON1_MASK)
        button = 1;
    else if (ev->state & GDK_BUTTON2_MASK)
        button = 2;
    else
        button = 0;

    switch (button) {
    case 0:
        Arrow *tmp;
        if ((tmp = mForrester->getSelectedArrow(ev->x, ev->y))) {
            if(tmp != mCurrentArrow)
                queueRedraw();
        } else {
            if(mCurrentArrow != 0)
                queueRedraw();
        }
        mCurrentArrow = tmp;

        if ((mCurrentArrow =
            mForrester->getSelectedArrow(ev->x, ev->y))) {
            queueRedraw();
        }

        if(tooltipItem == 0) {
            if((tooltipItem = mForrester->getSelectedItem(ev->x, ev->y)) != 0)
                set_tooltip_markup(tooltipItem->tooltipText());
        } else {
            if(not tooltipItem->select(ev->x,ev->y)) {
                set_tooltip_markup("");
                tooltipItem = 0;
            }
        }

        break;
    case 1:
        if (focusItem != 0) {
            bool xok = false, yok = false;
            displace(focusItem,
                ev->x, ev->y, xok, yok);
            mForrester->checkDiagramSize();
            checkBuffer(ev->x, ev->y);
            queueRedraw();
            if(xok)
                mPreviousX += ev->x - mPreviousX;
            if(yok)
                mPreviousY += ev->y - mPreviousY;
        } else {
            DependencyArrow *ar;
            if((ar = dynamic_cast<DependencyArrow*>(mCurrentArrow)))
            {
                if (ar->getDestination()->getOwner() != 0) {
                    ar->getControlPoint().setX(ev->x);
                    ar->getControlPoint().setY(ev->y);
                } else {
                    ar->getDestination()->setX(ev->x);
                    ar->getDestination()->setY(ev->y);
                    ar->computeControlPoint();
                }
                queueRedraw();
            }
        }
        break;
    }
}

}
}
}
}
