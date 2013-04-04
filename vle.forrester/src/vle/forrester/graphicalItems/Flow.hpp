/*
 * @file vle/forrester/graphicalItems/Flow.hpp
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

#ifndef VLE_GVLE_MODELING_FORRESTER_GRAPHICALITEMS_FLOW_HPP
#define VLE_GVLE_MODELING_FORRESTER_GRAPHICALITEMS_FLOW_HPP

#include <vle/forrester/graphicalItems/GraphicalItem.hpp>
#include <vle/forrester/graphicalItems/ForresterGI.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/algorithm/string.hpp>
#include <vle/utils/i18n.hpp>
#include <boost/polygon/polygon.hpp>
#include <boost/math/special_functions/round.hpp>
namespace vle {
namespace gvle {
namespace modeling {
namespace forrester {

/** @class Flow
 *  @brief Flow class
 */
class Flow : public GraphicalItem
{
public:
    Flow(const std::string&, const Forrester&);

    Flow(const Forrester&);

    Flow(int _x, int _y, const Forrester&);

    Flow(int _x, int _y, int _width, int _height,
        const std::string _name, const Forrester&);

    virtual ~Flow()
    {}

    bool launchCreationWindow(
        const Glib::RefPtr < Gtk::Builder >&);

    void launchEditionWindow(
        const Glib::RefPtr < Gtk::Builder >&);

    bool select(int x, int y) const;

    void draw(const Cairo::RefPtr<Cairo::Context>&);

    void checkSize(const Cairo::RefPtr<Cairo::Context>&);

    int getTopOffset () const
    { return 2 * ForresterGI::GLOBAL_OFFSET + FLOW_HEADER; }

    int getLeftOffset() const
    { return 2 * ForresterGI::GLOBAL_OFFSET; }

    int getBottomOffset() const
    { return ForresterGI::GLOBAL_OFFSET; }

    int getRightOffset() const
    { return ForresterGI::GLOBAL_OFFSET; }

    std::string getValue() const
    {
        return mValue;
    }

    std::string getTrueValue() const
    {
        return mTrueValue;
    }

    std::string getFalseValue() const
    {
        return mFalseValue;
    }

    std::string getPredicate() const
    {
        return mPredicate;
    }

    bool isConditionnal() const
    { return mConditionnality; }

    void setWidth(int _width);

    void setHeight(int _height);

    void setValue(const std::string& value)
    { mValue = value; }

    void setTrueValue (const std::string& value)
    { mTrueValue = value; }

    void setFalseValue(const std::string& value)
    { mFalseValue = value; }

    void setPredicate(const std::string& value)
    { mPredicate = value; }

    std::string toString() const;

    int getX1() const
    { return static_cast<int>(
                boost::math::iround<double>(mX+(mWidth/2.0)-15));
    }
    int getX2() const
    { return static_cast<int>(
                boost::math::iround<double>(mX+(mWidth/2.0)+15));
    }

    void resize(int _width, int _height);

    void generatePorts (vpz::AtomicModel&) const
    {}
    void generateObservable (vpz::Observable&) const;
    void generateSource (utils::Template& tpl_) const;
    void generateParenthesis(std::string&) const;
    void generateStdPrefix(std::string&)const;

    std::string tooltipText();

    static const int FLOW_HEIGHT;
    static const int FLOW_WIDTH;
    static const int FLOW_HEADER;
    static const std::string STL_FUNCTION[];
    static const int STL_FUNCTION_COUNT;
private:
    void drawName(const Cairo::RefPtr<Cairo::Context>&);
    void computeAnchors();

    std::string mValue;
    std::string mTrueValue;
    std::string mFalseValue;
    std::string mPredicate;
    bool mConditionnality;
};


}
}
}
} // namespace vle gvle modeling forrester

#endif
