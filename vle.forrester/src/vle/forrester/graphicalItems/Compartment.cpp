/*
 * @file vle/forrester/graphicalItems/Compartment.cpp
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

#include <vle/forrester/graphicalItems/Compartment.hpp>
#include <vle/forrester/graphicalItems/ForresterGI.hpp>
#include <vle/forrester/dialogs/CompartmentDialog.hpp>
#include <boost/math/special_functions/round.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/algorithm/string.hpp>
#include <vle/utils/i18n.hpp>

namespace vle {
namespace gvle {
namespace modeling {
namespace forrester {

const int Compartment::COMPARTMENT_HEIGHT = 25;
const int Compartment::COMPARTMENT_MINIMAL_WIDTH = 70;
const double Compartment::INITIAL_INITIALVALUE = 0.;
const double Compartment::INITIAL_TIMESTEP = 0.001;
const int Compartment::GLOBAL_OFFSET = 10;

Compartment::Compartment(const Forrester& f)
    :GraphicalItem(-1,-1,COMPARTMENT_MINIMAL_WIDTH,COMPARTMENT_HEIGHT,"",f),
    mTimeStep(INITIAL_TIMESTEP),mInitialValue(INITIAL_INITIALVALUE)
{
    computeAnchors();
}

Compartment::Compartment(const std::string& _name,
    int _x, int _y, int _width, int _height, const Forrester& f)
    :GraphicalItem(_x,_y,_width,_height,_name, f),
    mTimeStep(INITIAL_TIMESTEP),mInitialValue(INITIAL_INITIALVALUE)
{
    computeAnchors();
}

Compartment::Compartment(int _x, int _y, const Forrester& f)
:GraphicalItem(_x, _y, COMPARTMENT_MINIMAL_WIDTH, COMPARTMENT_HEIGHT, "",f),
    mTimeStep(INITIAL_TIMESTEP),mInitialValue(INITIAL_INITIALVALUE)
{
    computeAnchors();
}

Compartment::Compartment(const std::string& conf, const Forrester& f):
    GraphicalItem(f),mTimeStep(INITIAL_TIMESTEP),
    mInitialValue(INITIAL_INITIALVALUE)
{
    std::vector<std::string> compartment;
    boost::split(compartment,conf,boost::is_any_of(","));

    mName = compartment[0];
    mX = boost::lexical_cast < int > (compartment[1]);
    mY = boost::lexical_cast < int > (compartment[2]);
    mWidth = boost::lexical_cast < int > (compartment[3]);
    mHeight = boost::lexical_cast < int > (compartment[4]);
    computeAnchors();
}

bool Compartment::select(int _x, int _y) const
{
    return _x >= mX and _x <= mX + static_cast<int>(mWidth) and _y >= mY
        and _y <= mY + static_cast<int>(mHeight);
}

void Compartment::draw(const Cairo::RefPtr<Cairo::Context>& context)
{
    context->save();
    context->set_line_width(1);
    drawRectangle(context, mX, mY, mWidth, mHeight, 0.62745098, 0.780392157,
        0.933333333);
    context->stroke();
    drawName(context);
    context->restore();
}

void Compartment::checkSize(const Cairo::RefPtr<Cairo::Context>& context)
{
    Cairo::TextExtents textExtents;

    context->get_text_extents(mName, textExtents);
    if (mWidth < textExtents.width + 2 * ForresterGI::GLOBAL_OFFSET)
        resize(textExtents.width + 2 * ForresterGI::GLOBAL_OFFSET, mHeight);
    else if (mWidth > textExtents.width + 2 * ForresterGI::GLOBAL_OFFSET) {
        if (textExtents.width < COMPARTMENT_MINIMAL_WIDTH)
            resize(COMPARTMENT_MINIMAL_WIDTH + 2 * ForresterGI::GLOBAL_OFFSET,
                mHeight);
        else
            resize(textExtents.width + 2 * ForresterGI::GLOBAL_OFFSET, mHeight);
    }
}

void Compartment::drawName(const Cairo::RefPtr<Cairo::Context>& context)
{
    setColor(Settings::settings().getForegroundColor(), context);
    context->select_font_face(Settings::settings().getFont(),
        Cairo::FONT_SLANT_NORMAL,
        Cairo::FONT_WEIGHT_NORMAL);
    context->set_font_size(Settings::settings().getFontSize());

    Cairo::TextExtents textExtents;

    context->get_text_extents(mName, textExtents);
    context->move_to(mX + (mWidth - textExtents.width) / 2,
        mY + (mHeight + textExtents.height) / 2);
    context->show_text(mName);
    context->stroke();
}

std::string Compartment::toString() const
{
    return (fmt("%1%,%2%,%3%,%4%,%5%")
            % mName % mX % mY % mWidth % mHeight).str();
}
bool Compartment::launchCreationWindow(
    const Glib::RefPtr < Gtk::Builder >& xml)
{
    CompartmentDialog dialog(xml, mForrester);
    if (dialog.run() == Gtk::RESPONSE_ACCEPT) {
        mInitialValue = dialog.getInitialValue();
        mName = dialog.getName();
        if (mForrester.integrationScheme() == Forrester::QSS2)
            mTimeStep = dialog.getTimeStep();
        return true;
    }
    return false;
}

void Compartment::launchEditionWindow(
    const Glib::RefPtr < Gtk::Builder >& xml)
{
    CompartmentDialog dialog(xml,*this, mForrester);
    if (dialog.run() == Gtk::RESPONSE_ACCEPT) {
        mName = dialog.getName();
        mInitialValue = dialog.getInitialValue();
        if (mForrester.integrationScheme() == Forrester::QSS2) {
            mTimeStep = dialog.getTimeStep();
        }
    }
}

void Compartment::computeAnchors()
{
    mAnchors.push_back(new Anchor(boost::math::iround(0.25 * mWidth) + mX,
        mY));
    mAnchors.push_back(new Anchor(boost::math::iround(0.75 * mWidth) + mX,
        mY));
    mAnchors.push_back(new Anchor(boost::math::iround(0.25 * mWidth) + mX,
        mY + mHeight));
    mAnchors.push_back(new Anchor(boost::math::iround(0.75 * mWidth) + mX,
        mY + mHeight));

    mAnchors.push_back(new Anchor(mX,
        boost::math::iround(0.5 * mHeight) + mY));
    mAnchors.push_back(new Anchor(mX + mWidth,
        boost::math::iround(0.5 * mHeight) + mY));

    for (anchors_t::const_iterator it = mAnchors.begin();
    it != mAnchors.end(); ++it) {
        (*it)->setOwner(this);
    }
}

void Compartment::resize(int _width, int _height)
{
    mHeight = _height;
    mWidth = _width;
    if (!mAnchors.empty()) {
        mAnchors[0]->setX(boost::math::iround(0.25 * mWidth) + mX);
        mAnchors[0]->setY(mY);
        mAnchors[1]->setX(boost::math::iround(0.75 * mWidth) + mX);
        mAnchors[1]->setY(mY);

        mAnchors[2]->setX(boost::math::iround(0.25 * mWidth) + mX);
        mAnchors[2]->setY(mY + mHeight - 3);
        mAnchors[3]->setX(boost::math::iround(0.75 * mWidth) + mX);
        mAnchors[3]->setY(mY + mHeight - 3);

        mAnchors[4]->setX(mX);
        mAnchors[4]->setY(boost::math::iround(0.5 * mHeight) + mY);
        mAnchors[5]->setX(mX + mWidth);
        mAnchors[5]->setY(boost::math::iround(0.5 * mHeight) + mY);
    }
}

void Compartment::generatePorts (vpz::AtomicModel& model) const {
    if (not model.existOutputPort(mName))
        model.addOutputPort(mName);
}

void Compartment::generateObservable (vpz::Observable& obs) const {
    obs.add(mName);
}

void Compartment::generateSource (utils::Template& tpl_) const {
    tpl_.listSymbol().append("compartments");
    tpl_.listSymbol().append("compartments", toString());
    tpl_.listSymbol().append("createVar", mName + " = " +
                         "createVar(\"" + mName + "\");");
    tpl_.listSymbol().append("definitionVar",
                         "Var " + mName + ";");
}

std::string Compartment::tooltipText() {
    std::string tooltip;
    tooltip = "<b>Element : </b>Compartment\n<b>Name</b> : "+mName+"\n";
    tooltip += "<b>Initial value</b> : "+
        boost::lexical_cast<std::string>(mInitialValue)+"\n";
    return tooltip;
}

}
}
}
} // namespace vle gvle modeling forrester
