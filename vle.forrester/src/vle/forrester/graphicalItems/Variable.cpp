/*
 * @file vle/forrester/graphicalItems/Variable.cpp
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
#include <vle/forrester/dialogs/VariableDialog.hpp>
#include <vle/forrester/graphicalItems/Variable.hpp>
#include <boost/math/special_functions/round.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/algorithm/string.hpp>
#include <vle/utils/i18n.hpp>

namespace vle {
namespace gvle {
namespace modeling {
namespace forrester {

const int Variable::VARIABLE_WIDTH = 25;
const int Variable::VARIABLE_HEIGHT = 25;

Variable::Variable(const Forrester& f)
    :GraphicalItem(-1,-1,VARIABLE_WIDTH,VARIABLE_HEIGHT,"",f),
    mValue("")
{
    computeAnchors();
}

Variable::Variable(const std::string& _name,
    int _x, int _y, int _width, int _height, const Forrester& f)
    :GraphicalItem(_x,_y,_width,_height,_name,f), mValue("")
{
    computeAnchors();
}

Variable::Variable(int _x, int _y, const Forrester& f)
:GraphicalItem(_x, _y, VARIABLE_WIDTH, VARIABLE_HEIGHT, "",f),
    mValue("")
{
    computeAnchors();
}

Variable::Variable(const std::string& conf, const Forrester& f):
GraphicalItem(f)
{
    std::vector<std::string> variable;
    boost::split(variable,conf,boost::is_any_of(","));

    mName = variable[0];
    mX = boost::lexical_cast < int > (variable[1]);
    mY = boost::lexical_cast < int > (variable[2]);
    mWidth = boost::lexical_cast < int > (variable[3]);
    mHeight = boost::lexical_cast < int > (variable[4]);
    mValue = variable[5];
    computeAnchors();
}

bool Variable::select(int _x, int _y) const
{
    return _x >= mX and _x <= mX + static_cast<int>(mWidth) and _y >= mY
        and _y <= mY + static_cast<int>(mHeight);
}

void Variable::draw(const Cairo::RefPtr<Cairo::Context>& context)
{
    context->save();
    context->set_line_width(1.);
    context->move_to(mX + 0.5 * mWidth, mY + 0.5 * mHeight);

    context->arc (mX + 0.5 * mWidth, mY + 0.5 * mHeight,
        mWidth/2., 0.0, 2 * M_PI);
    setColor(Gdk::Color("green"),context);
    context->fill();

    context->arc (mX + 0.5 * mWidth, mY + 0.5 * mHeight,
        mWidth/2., 0.0, 2 * M_PI);
    setColor(Gdk::Color("black"),context);
    context->stroke();

    context->restore();
    drawName(context);
}

void Variable::drawName(const Cairo::RefPtr<Cairo::Context>& context)
{
    setColor(Settings::settings().getForegroundColor(), context);
    context->select_font_face(Settings::settings().getFont(),
        Cairo::FONT_SLANT_NORMAL,
        Cairo::FONT_WEIGHT_NORMAL);
    context->set_font_size(Settings::settings().getFontSize());

    Cairo::TextExtents textExtents;

    context->get_text_extents(mName, textExtents);
    context->move_to(mX + (mWidth - textExtents.width) / 2.,
        mY);
    context->show_text(mName);
    context->stroke();
}

std::string Variable::toString() const
{
    return (fmt("%1%,%2%,%3%,%4%,%5%,%6%")
            % mName % mX % mY % mWidth % mHeight % mValue).str();
}
bool Variable::launchCreationWindow(
    const Glib::RefPtr < Gtk::Builder >& xml)
{
    VariableDialog dialog(xml,mForrester);
    if (dialog.run() == Gtk::RESPONSE_ACCEPT) {
        mName = dialog.getName();
        mValue = dialog.getValue();
        return true;
    }
    else
        return false;
}

void Variable::launchEditionWindow(
    const Glib::RefPtr < Gtk::Builder >& xml)
{
    VariableDialog dialog(xml,*this,mForrester);
    if (dialog.run()) {
        mName = dialog.getName();
        mValue = dialog.getValue();
    }
}

void Variable::computeAnchors()
{
    mAnchors.push_back(new Anchor(mX,
        mY - Settings::settings().getFontSize()));
    mAnchors.push_back(new Anchor(boost::math::iround(0.5 * mWidth) + mX,
        mY - Settings::settings().getFontSize()));
    mAnchors.push_back(new Anchor(mWidth + mX,
        mY - Settings::settings().getFontSize()));
    mAnchors.push_back(new Anchor(mWidth + mX,
        mY + mHeight / 2.));
    mAnchors.push_back(new Anchor(mWidth + mX,
        mY + mHeight));
    mAnchors.push_back(new Anchor(mWidth/2. + mX,
        mY + mHeight));

    mAnchors.push_back(new Anchor(mX, mHeight + mY));
    mAnchors.push_back(new Anchor(mX,
        boost::math::iround(0.5 * mHeight) + mY));

    for (anchors_t::const_iterator it = mAnchors.begin();
    it != mAnchors.end(); ++it) {
        (*it)->setOwner(this);
    }
}

void Variable::generatePorts (vpz::AtomicModel&) const
{

}

void Variable::generateObservable (vpz::Observable& obs) const
{
    obs.add(mName);
}

void Variable::generateSource (utils::Template& tpl_) const
{
    tpl_.listSymbol().append("Variables", toString());
    tpl_.listSymbol().append("variableCompute", mName+" = "+ mValue + ";");
    tpl_.listSymbol().append("variableDeclaration", "double " + mName + ";");
    tpl_.listSymbol().append("variableObservation", "if (port == \""
                             + mName +
                             "\" ) return new vv::Double(" + mName + ");");
}

std::string Variable::tooltipText()
{
    std::string tooltip;
    tooltip = "<b>Element : </b>Variable\n<b>Name</b> : "+mName+"\n";
    tooltip += "<b>Value</b> : "+boost::lexical_cast<std::string>(mValue)+"\n";
    return tooltip;
}

}
}
}
} // namespace vle gvle modeling forrester
