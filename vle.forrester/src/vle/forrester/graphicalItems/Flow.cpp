/*
 * @file vle/forrester/graphicalItems/Flow.cpp
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

#include <vle/forrester/graphicalItems/Flow.hpp>
#include <vle/forrester/graphicalItems/Compartment.hpp>
#include <vle/forrester/dialogs/FlowDialog.hpp>
#include <vle/utils/i18n.hpp>

#include <vle/forrester/utilities/Tools.hpp>

namespace vle {
namespace gvle {
namespace modeling {
namespace forrester {

const int Flow::FLOW_HEIGHT = 20;
const int Flow::FLOW_WIDTH = 50;
const int Flow::FLOW_HEADER = 20;

Flow::Flow(const std::string& confTag, const Forrester& f):GraphicalItem(f)
{
    std::vector<std::string> flowInformation;
    boost::split(flowInformation,confTag,boost::is_any_of(","));

    mName = flowInformation[0];
    mX = boost::lexical_cast < int > (flowInformation[1]);
    mY = boost::lexical_cast < int > (flowInformation[2]);
    mWidth = boost::lexical_cast < int > (flowInformation[3]);
    mHeight = boost::lexical_cast < int > (flowInformation[4]);
    mValue = flowInformation[5];
    std::replace(mValue.begin(), mValue.end(), '\'', ',');
    mConditionnality = (flowInformation[6] == "true")? true : false;
    mPredicate = flowInformation[7];
    mTrueValue = flowInformation[8];
    std::replace(mTrueValue.begin(), mTrueValue.end(), '\'', ',');
    mFalseValue = flowInformation[9];
    std::replace(mFalseValue.begin(), mFalseValue.end(), '\'', ',');
    computeAnchors();
}

Flow::Flow(const Forrester& f)
:GraphicalItem(-1,-1,FLOW_WIDTH,FLOW_HEIGHT,"",f),mConditionnality(false)
{ computeAnchors(); }

Flow::Flow(int _x, int _y, const Forrester& f)
:GraphicalItem(_x, _y,FLOW_WIDTH,FLOW_HEIGHT,"",f),mConditionnality(false)
{  computeAnchors(); }

Flow::Flow(int _x, int _y, int _width, int _height,
    const std::string _name, const Forrester& f):
 GraphicalItem(_x,_y,_width,_height,_name,f),
 mConditionnality(false)
{  computeAnchors(); }

bool Flow::select(int _x, int _y) const
{
    namespace bp = boost::polygon;
    namespace bpo = boost::polygon::operators;

    typedef bp::polygon_data<int> Polygon;
    typedef bp::polygon_traits<Polygon>::point_type Point;

    Polygon polyBottom, polyMiddle, polyTop;
    double angle = atan(FLOW_HEADER / (getX2()-mX));
    unsigned int yMiddlePoint = boost::math::iround<double>(
        (mWidth / 2.0) * tan(angle));
    unsigned int middle = boost::math::iround<double>(mWidth/2.0);

    Point ptsBottom[] = {bp::construct<Point>(mX, mY),
        bp::construct<Point>(mX, mY + mHeight),
        bp::construct<Point>(mX + mWidth, mY + mHeight),
        bp::construct<Point>(mX + mWidth, mY),
        bp::construct<Point>(mX, mY)};

    Point ptsMiddle[] = { bp::construct<Point>(mX, mY),
        bp::construct<Point>(mX + middle, mY - yMiddlePoint),
        bp::construct<Point>(mX + mWidth, mY),
        bp::construct<Point>(mX, mY) };

    Point ptsTop[] = {bp::construct<Point>(getX1(), mY - FLOW_HEADER),
        bp::construct<Point>(mX + middle, mY - yMiddlePoint),
        bp::construct<Point>(getX2(), mY - FLOW_HEADER),
        bp::construct<Point>(getX1(), mY - FLOW_HEADER)};

    bp::set_points(polyBottom, ptsBottom, ptsBottom+5);
    bp::set_points(polyMiddle, ptsMiddle, ptsMiddle+4);
    bp::set_points(polyTop, ptsTop, ptsTop+4);


    return bp::contains(polyBottom, bp::construct<Point>(_x, _y))
           or bp::contains(polyMiddle, bp::construct<Point>(_x, _y))
           or bp::contains(polyTop, bp::construct<Point>(_x, _y));

}

void Flow::draw(const Cairo::RefPtr<Cairo::Context>& context)
{
    context->set_line_width(1);

    context->begin_new_sub_path();
    context->move_to(mX, mY);
    context->line_to(mX, mY + mHeight);
    context->line_to(mX + mWidth, mY + mHeight);
    context->line_to(mX + mWidth, mY);
    context->line_to(getX1(), mY - FLOW_HEADER );
    context->line_to(getX2(), mY - FLOW_HEADER );
    context->line_to(mX, mY);

    context->close_path();
    context->set_source_rgb(1, 1, 1);
    context->fill_preserve();
    setColor(Settings::settings().getForegroundColor(), context);
    context->stroke();

    drawName(context);
}

void Flow::checkSize(const Cairo::RefPtr<Cairo::Context>& context)
{
    Cairo::TextExtents textExtents;

    context->get_text_extents(mName, textExtents);
    if (mWidth < textExtents.width + 2 * TEXT_OFFSET) {
        resize(textExtents.width + 2 * TEXT_OFFSET, mHeight);
    }
}

void Flow::drawName(const Cairo::RefPtr<Cairo::Context>& context)
{
    setColor(Settings::settings().getForegroundColor(), context);
    context->select_font_face(Settings::settings().getFont(),
        Cairo::FONT_SLANT_NORMAL,
        Cairo::FONT_WEIGHT_NORMAL);
    context->set_font_size(Settings::settings().getFontSize());

    Cairo::TextExtents textExtents;

    context->get_text_extents(mName, textExtents);
    context->move_to(
        mX + (mWidth - textExtents.width) / 2,
        mY + (mHeight +
                      textExtents.height) / 2);
    context->show_text(mName);
    context->stroke();
}

std::string Flow::toString() const
{
    std::string isConditionnal = (mConditionnality)? "true" : "false";

    std::string qvalue = mValue;
    std::replace(qvalue.begin(), qvalue.end(), ',', '\'');
    std::string qtruevalue = mTrueValue;
    std::replace(qtruevalue.begin(), qtruevalue.end(), ',', '\'');
    std::string qfalsevalue = mFalseValue;
    std::replace(qfalsevalue.begin(), qfalsevalue.end(), ',', '\'');

    return (fmt("%1%,%2%,%3%,%4%,%5%,%6%,%7%,%8%,%9%,%10%")
            % mName % mX % mY % mWidth % mHeight % qvalue
            % isConditionnal % mPredicate % qtruevalue
            % qfalsevalue).str();
}

bool Flow::launchCreationWindow(
    const Glib::RefPtr < Gtk::Builder >& xml)
{
    FlowDialog dialog(xml, mForrester, "Flow dialog");
    if (dialog.run() == Gtk::RESPONSE_ACCEPT) {
        mName = dialog.getName();
        mValue = dialog.getValue();
        mTrueValue = dialog.getTrueValue();
        mFalseValue = dialog.getFalseValue();
        mPredicate = dialog.getPredicate();
        mConditionnality = dialog.isConditionnal();
        return true;
    }
    return false;
}

void Flow::launchEditionWindow(
    const Glib::RefPtr < Gtk::Builder >& xml)
{
    FlowDialog dialog(xml, *this, mForrester);
    if (dialog.run() == Gtk::RESPONSE_ACCEPT) {
        mName = dialog.getName();
        mValue = dialog.getValue();
        mTrueValue = dialog.getTrueValue();
        mFalseValue = dialog.getFalseValue();
        mPredicate = dialog.getPredicate();
        mConditionnality = dialog.isConditionnal();
    }
}

void Flow::computeAnchors()
{
    double middleHeight = (FLOW_HEADER*0.5*mWidth)/((mWidth*0.5)+15);

    mAnchors.push_back(
        new Anchor(mX + boost::math::iround(mWidth / 2.),
         mY - middleHeight));
    mAnchors.push_back(
        new Anchor(mX + boost::math::iround(mWidth / 2.),
         mY - FLOW_HEADER));
    mAnchors.push_back(new Anchor(mX,mY));
    mAnchors.push_back(new Anchor(mX,mY + mHeight));
    mAnchors.push_back(new Anchor(mX + mWidth, mY));
    mAnchors.push_back(new Anchor(mX + mWidth, mY + mHeight));
    mAnchors.push_back(new Anchor(mX + mWidth / 2., mY + mHeight));

    for (std::vector<Anchor*>::iterator it = mAnchors.begin();
    it != mAnchors.end(); ++it) {
        (*it)->setOwner(this);
    }
}


void Flow::setWidth(int _width)
{
    mWidth = _width;
    double middleHeight = (FLOW_HEADER*0.5*mWidth)/((mWidth*0.5)+15);

    mAnchors[0]->setX(mX + boost::math::iround(mWidth / 2.));
    mAnchors[1]->setX(mX + boost::math::iround(mWidth / 2.));
    mAnchors[4]->setX(mX + mWidth);
    mAnchors[5]->setX(mX + mWidth);
    mAnchors[6]->setX(mX + mWidth / 2.);

    mAnchors[0]->setY(mY - middleHeight);

}

void Flow::setHeight(int _height)
{
    mHeight = _height;

    mAnchors[3]->setY(mY + mHeight);
    mAnchors[5]->setY(mY + mHeight);
    mAnchors[6]->setY(mY + mHeight);
}

void Flow::resize(int _width, int _height)
{
    setHeight(_height);
    setWidth(_width);
}

void Flow::generateObservable (vpz::Observable& obs) const {
    obs.add(mName);
}

void Flow::generateSource (utils::Template& tpl_) const {
    std::string finalPredicate(mPredicate);
    std::string finalTrueValue(mTrueValue);
    std::string finalFalseValue(mFalseValue);
    std::string finalExpression(mValue);

    utilities::generateParenthesis(finalPredicate, mForrester);
    utilities::generateParenthesis(finalTrueValue, mForrester);
    utilities::generateParenthesis(finalFalseValue, mForrester);
    utilities::generateParenthesis(finalExpression, mForrester);
    utilities::generateStdPrefix(finalExpression);

    tpl_.listSymbol().append("flows", toString());
    if(mConditionnality) {
        tpl_.listSymbol().append("conditionalFlowVariableDeclaration",
                                 "double " + mName + ";");
        tpl_.listSymbol().append("conditionalFlowVariableCompute",
                                 mName + " = ("+ finalPredicate +")? " +
                                 finalTrueValue + " : " +
                                 finalFalseValue + ";");
    } else {
        tpl_.listSymbol().append("FlowVariableDeclaration",
                                 "double " + mName + ";");
        tpl_.listSymbol().append("FlowVariableCompute",
                                 mName + " = " + finalExpression + ";");
    }
    tpl_.listSymbol().append("flowObservation", "if (port == \""
                             + mName +
                             "\" ) return new vv::Double(" + mName + ");");
}

std::string Flow::tooltipText() {
    std::string tooltip;
    tooltip = "<b>Element : </b>Flow\n<b>Name</b> : "+mName+"\n";
    tooltip += "<b>Conditional</b> : ";
    tooltip += (mConditionnality)? "Yes\n" : "No\n";
    if(mConditionnality) {
        tooltip += "<b>Predicate</b> : "+mPredicate+"\n";
        tooltip += "<b>If true</b> : "+mTrueValue+"\n";
        tooltip += "<b>If false</b> : "+mFalseValue+"\n";
    } else {
        tooltip += "<b>Value</b> : "+mValue+"\n";
    }
    return tooltip;
}

}
}
}
} // namespace vle gvle modeling forrester
