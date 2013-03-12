/*
 * @file vle/forrester/graphicalItems/ExogenousVariable.cpp
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
#include <vle/forrester/graphicalItems/ExogenousVariable.hpp>
#include <vle/forrester/dialogs/ExogenousVarDialog.hpp>
#include <boost/algorithm/string.hpp>
#include <vle/utils/i18n.hpp>


namespace vle {
namespace gvle {
namespace modeling {
namespace forrester {

const int ExogenousVariable::EV_WIDTH = 22;
const int ExogenousVariable::EV_HEIGHT = 22;
const int ExogenousVariable::EV_LARGE_CIRCLE_RADIUS = 11;
const int ExogenousVariable::EV_TINY_CIRCLE_RADIUS = 6;
const int ExogenousVariable::EV_ANCHOR_GAP = 4;

ExogenousVariable::ExogenousVariable(int x, int y, const Forrester& f)
:GraphicalItem(x,y,EV_WIDTH,EV_HEIGHT,"",f)
{ computeAnchors(); }

ExogenousVariable::ExogenousVariable(int x, int y, const std::string &name, const Forrester& f)
:GraphicalItem(x,y,EV_WIDTH,EV_HEIGHT,name,f)
{ computeAnchors(); }

ExogenousVariable::ExogenousVariable(const std::string &conf, const Forrester& f)
:GraphicalItem(f)
{
    std::vector<std::string> parameter;
    boost::split(parameter,conf,boost::is_any_of(","));

    mName = parameter[0];
    mX = boost::lexical_cast < int > (parameter[1]);
    mY = boost::lexical_cast < int > (parameter[2]);
    mWidth = boost::lexical_cast < int > (parameter[3]);
    mHeight = boost::lexical_cast < int > (parameter[4]);
    computeAnchors();
}

void ExogenousVariable::draw(const Cairo::RefPtr<Cairo::Context>& context)
{
    context->save();
    context->set_line_width(1.);
    context->move_to(mX + 0.5 * mWidth, mY + 0.5 * mHeight);
    context->arc (mX + 0.5 * mWidth, mY + 0.5 * mHeight,
        EV_LARGE_CIRCLE_RADIUS, 0.0, 2 * M_PI);
    setColor(Gdk::Color("white"),context);
    context->fill();
    context->arc (mX + 0.5 * mWidth, mY + 0.5 * mHeight,
        EV_LARGE_CIRCLE_RADIUS, 0.0, 2 * M_PI);
    setColor(Gdk::Color("black"),context);
    context->stroke();
    context->arc(mX + 0.5 * mWidth, mY + 0.5 * mHeight,
        EV_TINY_CIRCLE_RADIUS, 0.0, 2 * M_PI);
    context->stroke();
    context->restore();
    drawName(context);
}

bool ExogenousVariable::select(int x, int y) const
{
    return (x <= mX + mWidth && x >= mX
        && y <= mY + mHeight && y >= mY);
}

bool ExogenousVariable::launchCreationWindow(
    const Glib::RefPtr < Gtk::Builder > &xml)
{
    ExogenousVarDialog box (xml, mForrester);
    if(box.run() ==  Gtk::RESPONSE_ACCEPT) {
        mName = box.getName();
        return true;
    }
    return false;
}
void ExogenousVariable::launchEditionWindow(
    const Glib::RefPtr < Gtk::Builder >& xml)
{
    ExogenousVarDialog box (xml, *this, mForrester);
    if(box.run() ==  Gtk::RESPONSE_ACCEPT) {
        mName = box.getName();
    }
}

void ExogenousVariable::checkSize(const Cairo::RefPtr<Cairo::Context>&)
{}
void ExogenousVariable::drawName(const Cairo::RefPtr<Cairo::Context>& context)
{
    setColor(Settings::settings().getForegroundColor(), context);
    context->select_font_face(Settings::settings().getFont(),
        Cairo::FONT_SLANT_NORMAL,
        Cairo::FONT_WEIGHT_NORMAL);
    context->set_font_size(Settings::settings().getFontSize());

    Cairo::TextExtents textExtents;

    context->get_text_extents(mName, textExtents);
    context->move_to(mX + (mWidth - textExtents.width) / 2.,
        mY -5 / 2.);
    context->show_text(mName);
    context->stroke();
}

void ExogenousVariable::computeAnchors()
{
    mAnchors.push_back(
        new Anchor(mX - EV_ANCHOR_GAP,
         mY - EV_ANCHOR_GAP - Settings::settings().getFontSize() - 7));
    mAnchors.push_back(
        new Anchor(mX + mWidth / 2.,
         mY - EV_ANCHOR_GAP - Settings::settings().getFontSize() - 7));
    mAnchors.push_back(
        new Anchor(mX + mWidth + EV_ANCHOR_GAP,
         mY - EV_ANCHOR_GAP - Settings::settings().getFontSize() - 7));
    mAnchors.push_back(
        new Anchor(mX + EV_ANCHOR_GAP + mWidth, mY + mHeight /2.));
    mAnchors.push_back(
        new Anchor(mX + EV_ANCHOR_GAP + mWidth, mY + EV_ANCHOR_GAP + mHeight));
    mAnchors.push_back(
        new Anchor(mX + mWidth / 2., mY + EV_ANCHOR_GAP + mHeight));
    mAnchors.push_back(
        new Anchor(mX - EV_ANCHOR_GAP, mY + EV_ANCHOR_GAP + mHeight));
    mAnchors.push_back(
        new Anchor(mX - EV_ANCHOR_GAP, mY + mHeight / 2.));

    for (std::vector<Anchor*>::iterator it = mAnchors.begin();
    it != mAnchors.end(); ++it) {
        (*it)->setOwner(this);
    }
}

std::string ExogenousVariable::toString () const {
    return (fmt("%1%,%2%,%3%,%4%,%5%")
            % mName % mX % mY % mWidth % mHeight).str();
}

void ExogenousVariable::generatePorts (vpz::AtomicModel& model) const {
    if (not model.existInputPort(mName))
        model.addInputPort(mName);
}

void ExogenousVariable::generateSource (utils::Template& tpl) const {
    tpl.listSymbol().append("exogenousVariables", toString());
    tpl.listSymbol().append("createVarExo",
        mName+" = createExt(\""+mName+"\");");
    tpl.listSymbol().append("definitionExoVariables", "Ext "+mName+";");
}

std::string ExogenousVariable::tooltipText() {
    std::string tooltip;
    tooltip = "<b>Element : </b>Exogenous Variable\n<b>Name</b> : "+mName+"\n";
    return tooltip;
}

}
}
}
} // namespace vle gvle modeling forrester
