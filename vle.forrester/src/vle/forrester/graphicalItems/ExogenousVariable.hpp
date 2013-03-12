/*
 * @file vle/forrester/graphicalItems/ExogenousVariable.hpp
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

#ifndef VLE_GVLE_MODELING_FORRESTER_GRAPHICALITEMS_EXOGENOUSVARIABLE_HPP
#define VLE_GVLE_MODELING_FORRESTER_GRAPHICALITEMS_EXOGENOUSVARIABLE_HPP

#include <vle/forrester/graphicalItems/GraphicalItem.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/algorithm/string.hpp>
#include <vle/utils/i18n.hpp>

namespace vle {
namespace gvle {
namespace modeling {
namespace forrester {

/** @class ExogenousVariable
 *  @brief Exogenous variable class
 */
class ExogenousVariable : public GraphicalItem
{
public:
    ExogenousVariable(const std::string& conf, const Forrester&);

    ExogenousVariable(int x, int y, const Forrester&);

    ExogenousVariable(int x, int y, const std::string&, const Forrester&);

    void draw(const Cairo::RefPtr<Cairo::Context>&);

    bool select(int x, int y) const;

    bool launchCreationWindow(
        const Glib::RefPtr < Gtk::Builder >& xml);
    void launchEditionWindow(
        const Glib::RefPtr < Gtk::Builder >& xml);

    void checkSize(const Cairo::RefPtr<Cairo::Context>&);

    std::string toString() const;

    void generatePorts (vpz::AtomicModel& model) const ;

    void generateObservable (vpz::Observable&) const
    {}

    virtual std::string tooltipText();

    virtual ~ExogenousVariable()
    {}

    void generateSource (utils::Template&) const;

    static const int EV_WIDTH;
    static const int EV_HEIGHT;
    static const int EV_LARGE_CIRCLE_RADIUS;
    static const int EV_TINY_CIRCLE_RADIUS;
    static const int EV_ANCHOR_GAP;
private:
    void drawName(const Cairo::RefPtr<Cairo::Context>&);
    void computeAnchors();
};


}
}
}
} // namespace vle gvle modeling forrester

#endif
