/*
 * @file vle/forrester/Forrester.hpp
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

#ifndef VLE_GVLE_MODELING_FORRESTER_FORRESTER_HPP
#define VLE_GVLE_MODELING_FORRESTER_FORRESTER_HPP

#include <string>
#include <list>
#include <vle/forrester/graphicalItems/Arrow.hpp>
#include <vle/forrester/graphicalItems/GraphicalItem.hpp>
#include <vle/forrester/graphicalItems/ForresterGI.hpp>
#include <vle/forrester/graphicalItems/FlowArrow.hpp>
#include <vle/forrester/graphicalItems/Compartment.hpp>
#include <vle/forrester/graphicalItems/Flow.hpp>


namespace vle {
namespace gvle {
namespace modeling {
namespace forrester {

/**
 * @class Forrester
 * @brief Forrester diagram main model class.
 */
class Forrester
{
public:
    enum Integration_Schemes { EULER, RK4, QSS2 };

    Forrester(const std::string&);

    ~Forrester();

    /** Add a graphical item to the Forrester diagram */
    void add(GraphicalItem*);

    /** Add an arrow to the Forrester diagram */
    bool add(Arrow*);

    /** Remove a graphical item from the Forrester diagram */
    void remove(GraphicalItem*);

    /** Remove a arrow from the Forrester diagram */
    void remove(Arrow*);

    /** Check if the name is already taken by an item
     * @return true if the name exist, false otherwise
     */
    bool exist(const std::string& name) const;

    /**
     * Check if the name is valid
     * @return true if the name is valid, false otherwise
     */
    bool isValidName(const std::string&)const;

    /**
     * @return The GraphicalItem corresponding to the string, or 0
     */
    GraphicalItem* getItem(const std::string&) const;

    /**
     * @return The GraphicalItem corresponding to the position, or 0
     */
    GraphicalItem* getSelectedItem(int _x, int _y);

    /**
     * @return The Arrow corresponding to the position, or 0
     */
    Arrow* getSelectedArrow(int x, int y) const;

    /**
     * @return The last selected item
     */
    GraphicalItem* getSelectedItem();

    /**
     * Check if the diagram size is correct, and resize it if it is not the case
     * @return true if the diagram has been resized, false otherwise
     */
    bool checkDiagramSize();

    //Getters
    Integration_Schemes integrationScheme() const
    { return mIntegrationScheme; }

    const graphical_items& getItems() const
    { return mGraphicalItems; }

    const arrows& getArrows() const
    { return mArrows; }

    ForresterGI& getForresterGI()
    { return mForresterGI; }

    void linkFlowCompartments (Flow* flow,
                                Compartment* origin,
                                Compartment* dest);

    //Setters
    void integrationScheme(Integration_Schemes integrationScheme)
    {mIntegrationScheme = integrationScheme; }
private:
    Forrester(const Forrester&);
    Forrester& operator= (const Forrester&);

    Integration_Schemes mIntegrationScheme;/**< Current integration scheme */
    ForresterGI mForresterGI;/**< Main frame of the diagram */
    graphical_items mGraphicalItems;/**< List of all GraphicalItem items of the diagram*/
    arrows mArrows;/**< List of all Arrow items of the diagram*/
};

}
}
}
} // namespace vle gvle modeling forrester

#endif
