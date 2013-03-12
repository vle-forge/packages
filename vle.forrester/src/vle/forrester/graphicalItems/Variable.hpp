/*
 * @file vle/forrester/graphicalItems/Variable.hpp
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

#ifndef VLE_GVLE_MODELING_FORRESTER_GRAPHICALITEMS_VARIABLE_HPP
#define VLE_GVLE_MODELING_FORRESTER_GRAPHICALITEMS_VARIABLE_HPP

#include <vle/forrester/graphicalItems/GraphicalItem.hpp>
#include <vle/forrester/graphicalItems/ForresterGI.hpp>

namespace vle {
namespace gvle {
namespace modeling {
namespace forrester {

/** @class Variable
 *  @brief Variable class
 *
 * Variables are defined by a value computed during the simulation.
 */
class Variable : public GraphicalItem
{
public:
    Variable(const Forrester&);

    Variable(const std::string&, const Forrester&);

    Variable(const std::string&,
        int, int, int, int, const Forrester&);

    Variable(int, int, const Forrester&);

    bool launchCreationWindow(const Glib::RefPtr < Gtk::Builder >&);

    void launchEditionWindow(const Glib::RefPtr < Gtk::Builder >&);

    bool select(int x, int y) const;

    void draw(const Cairo::RefPtr<Cairo::Context>&);

    int getTopOffset () const
    { return 2 * ForresterGI::GLOBAL_OFFSET; }

    int getLeftOffset() const
    { return 2 * ForresterGI::GLOBAL_OFFSET; }

    int getBottomOffset() const
    { return ForresterGI::GLOBAL_OFFSET; }

    int getRightOffset() const
    { return ForresterGI::GLOBAL_OFFSET; }

    std::string toString() const;


    void checkSize(const Cairo::RefPtr<Cairo::Context>&){}

    void generatePorts (vpz::AtomicModel&) const;

    void generateObservable (vpz::Observable&) const;

    void generateSource (utils::Template&) const;

    std::string tooltipText();

    std::string getValue() const {
        return mValue;
    }

    void setValue(const std::string& val) {
        mValue = val;
    }

    static const int VARIABLE_WIDTH;
    static const int VARIABLE_HEIGHT;

private:
    void drawName(const Cairo::RefPtr<Cairo::Context>&);
    void computeAnchors();

    std::string mValue;
};



}
}
}
} // namespace vle gvle modeling forrester

#endif
