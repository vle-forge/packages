/*
 * @file vle/examples/gens/GenExecutiveClass.cpp
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


#include <vle/devs/Executive.hpp>
#include <vle/utils/Tools.hpp>
#include <stack>
#include <sstream>

namespace vle { namespace examples { namespace gens {

class GenExecutiveClass : public devs::Executive
{
    enum state { INIT, IDLE, ADDMODEL, DELMODEL };

    std::stack < std::string >  m_stacknames;
    state                       m_state;
    bool                        m_buildbeepbeep;

public:
    GenExecutiveClass(const devs::ExecutiveInit& mdl,
                      const devs::InitEventList& events) :
        devs::Executive(mdl, events)
    {
    }

    virtual ~GenExecutiveClass()
    {
    }

    virtual devs::Time init(devs::Time /* time */) override
    {
        m_state = INIT;
        m_buildbeepbeep = true;
        return 0.0;
    }

    virtual devs::Time timeAdvance() const override
    {
        switch (m_state) {
        case INIT:
            return 0.0;
        case IDLE:
            return 0.0;
        case ADDMODEL:
            return 1.0;
        case DELMODEL:
            return 1.0;
        }
        throw utils::InternalError("GenExecutiveClass ta");
    }

    virtual void internalTransition(devs::Time time) override
    {
        switch (m_state) {
        case INIT:
            m_state = IDLE;
            break;
        case IDLE:
            if (time < 50.0) {
                m_state = ADDMODEL;
            }
            else {
                m_state = DELMODEL;
            }
            break;
        case ADDMODEL:
            add_new_model();
            m_state = IDLE;
            break;
        case DELMODEL:
            del_first_model();
            m_state = IDLE;
            break;
        }
    }

    virtual std::unique_ptr<value::Value> observation(
    		const devs::ObservationEvent& ev) const override
        {
            if (ev.onPort("nbmodel")) {
                return value::Integer::create(get_nb_model());
            } else if (ev.onPort("structure")) {
                std::ostringstream out;
                coupledmodel().writeXML(out);
                return value::String::create(out.str());
            } else {
                return devs::Executive::observation(ev);
            }
        }


private:
    void add_new_model()
    {
        std::string name(vle::utils::format("beep-%i",
                (int) m_stacknames.size()));
        if (m_buildbeepbeep) {
            createModelFromClass("beepbeep", name);
        } else {
            createModelFromClass("beepbeepbeep", name);
        }
        addConnection(name, "out", "counter", "in");

        m_stacknames.push(name);
        m_buildbeepbeep = not m_buildbeepbeep;
    }

    void del_first_model()
    {
        if (m_stacknames.empty()) {
            throw utils::InternalError(
                    "Cannot delete any model, the executive have no "
                    "element.");
        }

        delModel(m_stacknames.top());
        m_stacknames.pop();
    }

    int get_nb_model() const
    {
        return coupledmodel().getModelList().size();
    }

};

}}} // namespace vle examples gens

DECLARE_EXECUTIVE(vle::examples::gens::GenExecutiveClass)
