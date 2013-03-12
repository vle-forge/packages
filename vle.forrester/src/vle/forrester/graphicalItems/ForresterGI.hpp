/*
 * @file vle/forrester/graphicalItems/ForresterGI.hpp
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

#ifndef VLE_GVLE_MODELING_FORRESTER_GRAPHICALITEMs_FORRESTERGI_HPP
#define VLE_GVLE_MODELING_FORRESTER_GRAPHICALITEMs_FORRESTERGI_HPP

#include <vle/forrester/graphicalItems/GraphicalItem.hpp>

namespace vle {
namespace gvle {
namespace modeling {
namespace forrester {


/** @class ForresterGI
 *  @brief Forrester graphical item
 *
 *  This class contains the size of the drawing area, and draw a frame which surrounded the diagram
 */
class ForresterGI : public GraphicalItem
{
public:
    ForresterGI(const std::string& _name, const Forrester& f):
        GraphicalItem(f)
    {
        mName = _name;
        mHeight = INITIAL_HEIGHT;
        mWidth = INITIAL_WIDTH;
    }

    double getTimeStep() const
    { return mTimeStep; }

    void setTimeStep(double _timeStep)
    { mTimeStep = _timeStep; }

    bool launchCreationWindow(
        const Glib::RefPtr < Gtk::Builder >&)
    { return false; }
    void launchEditionWindow(
        const Glib::RefPtr < Gtk::Builder >&)
    { }

    bool select(int x, int y) const;

    void draw(const Cairo::RefPtr<Cairo::Context>&);

    void checkSize(const Cairo::RefPtr<Cairo::Context>&);

    std::string toString() const
    { return ""; }

    void generatePorts (vpz::AtomicModel&) const
    {}

    void generateObservable (vpz::Observable&) const
    {}

    void generateSource (utils::Template&) const;

    static const int INITIAL_HEIGHT;
    static const int INITIAL_WIDTH;
    static const int MINIMAL_HEIGHT;
    static const int MINIMAL_WIDTH;
    static const int GLOBAL_OFFSET;
    static const int HEADER_HEIGHT;
private:
    virtual void drawName(const Cairo::RefPtr<Cairo::Context>&);

    double mTimeStep;
};


}
}
}
}
#endif
