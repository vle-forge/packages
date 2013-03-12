/*
 * @file vle/forrester/graphicalItems/Parameter.cpp
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

#include <vle/forrester/graphicalItems/Parameter.hpp>
#include <vle/forrester/dialogs/ParameterDialog.hpp>
#include <vle/utils/i18n.hpp>


namespace vle {
namespace gvle {
namespace modeling {
namespace forrester {

const int Parameter::PARAMETER_HEIGHT = 16;
const int Parameter::PARAMETER_WIDTH = 30;
const int Parameter::PARAMETER_TEXT_GAP = 2;

Parameter::Parameter(const std::string& conf, const Forrester& f)
:GraphicalItem(f),mValue(0.)
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

Parameter::Parameter(const Forrester& f)
:GraphicalItem(-1,-1,PARAMETER_WIDTH,PARAMETER_HEIGHT,"",f),mValue(0.)
{ computeAnchors(); }

Parameter::Parameter(int _x, int _y, const Forrester& f)
:GraphicalItem(_x, _y,PARAMETER_WIDTH,PARAMETER_HEIGHT,"", f),mValue(0.)
{  computeAnchors(); }

Parameter::Parameter(int _x, int _y, const std::string _name, const Forrester& f)
:GraphicalItem(_x,_y,PARAMETER_WIDTH,PARAMETER_HEIGHT,_name, f),mValue(0.)
{  computeAnchors(); }

bool Parameter::select(int _x, int _y) const
{
    return ( _x <= (mX + mWidth/2.) && _x >= (mX - mWidth/2.)
        && _y <= (mY + mHeight/2.) && _y >= (mY - mHeight/2.));
}

void Parameter::draw(const Cairo::RefPtr<Cairo::Context>& context)
{
    context->save();
    setColor(Gdk::Color("red"),context);
    context->set_line_width(2);
    context->arc(mX,mY,mHeight/2.,0,M_PI * 2);
    context->stroke();

    setColor(Gdk::Color("black"),context);
    context->move_to(mX - (mWidth/2.),mY);
    context->line_to(mX + (mWidth/2.),mY);
    context->stroke();
    context->restore();

    drawName(context);
}

void Parameter::checkSize(const Cairo::RefPtr<Cairo::Context>&)
{

}

void Parameter::drawName(const Cairo::RefPtr<Cairo::Context>& context)
{
    setColor(Settings::settings().getForegroundColor(), context);
    context->select_font_face(Settings::settings().getFont(),
        Cairo::FONT_SLANT_NORMAL,
        Cairo::FONT_WEIGHT_NORMAL);
    context->set_font_size(Settings::settings().getFontSize());

    Cairo::TextExtents textExtents;

    context->get_text_extents(mName, textExtents);
    context->move_to(
        mX - (textExtents.width / 2.),
        mY - (mHeight + textExtents.height) /2);
    context->show_text(mName);
    context->stroke();
}

std::string Parameter::toString() const
{
    return (fmt("%1%,%2%,%3%,%4%,%5%")
            % mName % mX % mY % mWidth % mHeight).str();
}

bool Parameter::launchCreationWindow(
    const Glib::RefPtr < Gtk::Builder >& xml)
{
    ParameterDialog dialog(xml, mForrester);
    if (dialog.run() == Gtk::RESPONSE_ACCEPT) {
        mValue = dialog.getValue();
        mName = dialog.getName();
        return true;
    }
    return false;
}

void Parameter::launchEditionWindow(
    const Glib::RefPtr < Gtk::Builder >& xml)
{
    ParameterDialog dialog(xml,*this, mForrester);
    if (dialog.run() == Gtk::RESPONSE_ACCEPT) {
        mName = dialog.getName();
        mValue = dialog.getValue();
    }
}

void Parameter::computeAnchors()
{
    mAnchors.push_back(new Anchor(mX,mY + mHeight/2.));
    mAnchors.push_back(new Anchor(mX,
        mY - mHeight/2. - PARAMETER_TEXT_GAP - 15));
    mAnchors.push_back(new Anchor(mX + mWidth / 2.,mY));
    mAnchors.push_back(new Anchor(mX - (mWidth / 2.),mY));


    for (std::vector<Anchor*>::iterator it = mAnchors.begin();
    it != mAnchors.end(); ++it) {
        (*it)->setOwner(this);
    }
}


void Parameter::setWidth(int /*_width*/)
{

}

void Parameter::setHeight(int /*_height*/)
{

}

void Parameter::resize(int /*_width*/, int /*_height*/)
{

}

void Parameter::generateSource (utils::Template& tpl_) const {
    std::string parameter;
    tpl_.listSymbol().append("parameters");
    tpl_.listSymbol().append("parameters", toString());
    parameter = "if (events.exist(\"" + mName + "\"))\n";
    parameter +="            " +mName + " = ";
    parameter +="events.getDouble(\""+mName+"\");\n";
    parameter +="        else\n";
    parameter +="            throw vle::utils::ModellingError(\"Parameter ";
    parameter +=mName+" not found\");";

    tpl_.listSymbol().append("parametersInitialization");
    tpl_.listSymbol().append("parametersInitialization",parameter);

    tpl_.listSymbol().append("definitionParameters");
    tpl_.listSymbol().append("definitionParameters", "double "
        + mName + ";");
}

std::string Parameter::tooltipText() {
    std::string tooltip;
    tooltip = "<b>Element : </b>Parameter\n<b>Name</b> : "+mName+"\n";
    tooltip += "<b>Value</b> : "+boost::lexical_cast<std::string>(mValue)+"\n";
    return tooltip;
}

}
}
}
} // namespace vle gvle modeling forrester
