/*
 * @file vle/forrester/graphicalItems/Parameter.hpp
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

#ifndef VLE_GVLE_MODELING_FORRESTER_GRAPHICALITEMS_PARAMETER_HPP
#define VLE_GVLE_MODELING_FORRESTER_GRAPHICALITEMS_PARAMETER_HPP

#include <vle/forrester/graphicalItems/GraphicalItem.hpp>

#include <boost/lexical_cast.hpp>
#include <boost/algorithm/string.hpp>
#include <vle/utils/i18n.hpp>
#include <boost/polygon/polygon.hpp>
#include <boost/math/special_functions/round.hpp>

namespace vle {
namespace gvle {
namespace modeling {
namespace forrester {

/** @class Parameter
 *  @brief Parameter class
 *
 *  Parameter is a graphical item which have a constant value during the simulation
 */
class Parameter : public GraphicalItem
{
public:
    Parameter(const std::string& confTag, const Forrester&);

    Parameter(const Forrester&);

    Parameter(int, int, const Forrester&);

    Parameter(int, int, const std::string, const Forrester&);

    virtual ~Parameter()
    {}

    bool launchCreationWindow(
        const Glib::RefPtr < Gtk::Builder >&);

    void launchEditionWindow(
        const Glib::RefPtr < Gtk::Builder >&);

    bool select(int x, int y) const;

    void draw(const Cairo::RefPtr<Cairo::Context>&);

    void checkSize(const Cairo::RefPtr<Cairo::Context>&);

    int getTopOffset () const
    { return 10; }

    int getLeftOffset() const
    { return 30; }

    int getBottomOffset() const
    { return 10; }

    int getRightOffset() const
    { return 10; }

    double getValue() const
    { return mValue; }

    void setWidth(int _width);

    void setHeight(int _height);

    void setValue(double value)
    { mValue = value; }

    std::string tooltipText();

    std::string toString() const;

    void resize(int _width, int _height);

    void generatePorts (vpz::AtomicModel&) const
    {}

    void generateObservable (vpz::Observable&) const
    {}

    void generateSource (utils::Template&) const;


    static const int PARAMETER_HEIGHT;
    static const int PARAMETER_WIDTH;
    static const int PARAMETER_TEXT_GAP;
private:
    void drawName(const Cairo::RefPtr<Cairo::Context>&);
    void computeAnchors();

    double mValue;
};


}
}
}
} // namespace vle gvle modeling forrester

#endif
