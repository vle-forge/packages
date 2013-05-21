/*
 * @file vle/gvle/modeling/difference-equation/Multiple.hpp
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


#ifndef VLE_GVLE_MODELING_DIFFERENCEEQUATION_MULTIPLE_HPP
#define VLE_GVLE_MODELING_DIFFERENCEEQUATION_MULTIPLE_HPP

#include <vle/gvle/ModelingPlugin.hpp>
#include <vle/gvle/modeling/difference-equation/Plugin.hpp>
#include <vle/gvle/modeling/difference-equation/Variables.hpp>
#include <vle/utils/Template.hpp>
#include <gtkmm/dialog.h>

namespace vle { namespace gvle { namespace modeling { namespace de {

class Multiple : public Plugin
{
public:
    Multiple(const std::string& package, const std::string& library,
            const std::string& curr_package);

    virtual ~Multiple();

    virtual bool create(vpz::AtomicModel& model,
                        vpz::Dynamic& dynamic,
                        vpz::Conditions& conditions,
                        vpz::Observables& observables,
                        const std::string& classname,
                        const std::string& namespace_);

    virtual bool modify(vpz::AtomicModel& model,
                        vpz::Dynamic& dynamic,
                        vpz::Conditions& conditions,
                        vpz::Observables& observables,
                        const std::string& conf,
                        const std::string& buffer);

    virtual bool start(vpz::Condition& condition);

    virtual bool start(vpz::Condition&, const std::string&)
    { return true; }

private:
    Gtk::Dialog*         m_dialog;
    Gtk::Button*         m_buttonSource;
    Variables           mVariables;

    std::list < sigc::connection > mList;

    void assign(vpz::Condition& condition);
    void build(bool modeling);
    void destroy();
    void fillFields(const vpz::Condition& condition);
    virtual void generateCondition(vpz::AtomicModel& model,
                                   vpz::Conditions& conditions);
    virtual void generateObservables(vpz::AtomicModel& model,
                                     vpz::Observables& observables);
    virtual void generateOutputPorts(vpz::AtomicModel& model);
    virtual void generateVariables(utils::Template& tpl_);
    std::string getTemplate() const;
};

}}}} // namespace vle gvle modeling de

#endif
