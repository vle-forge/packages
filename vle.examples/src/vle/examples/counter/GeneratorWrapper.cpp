/*
 * @file vle/examples/counter/GeneratorWrapper.cpp
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


#include <vle/examples/counter/GeneratorWrapper.hpp>
#include <vle/examples/counter/LogNormalGenerator.hpp>
#include <vle/examples/counter/NormalGenerator.hpp>
#include <vle/examples/counter/RegularGenerator.hpp>
#include <vle/examples/counter/UniformGenerator.hpp>
#include <vle/utils/Rand.hpp>


namespace vle { namespace examples { namespace generator {

GeneratorWrapper::GeneratorWrapper(const devs::DynamicsInit& model,
                                   const devs::InitEventList& events) :
    devs::Dynamics(model, events),
    m_events(events),
    m_generator(0),
    m_timeStep(0.0),
    m_time(0.0)
{
}

GeneratorWrapper::~GeneratorWrapper()
{
    delete m_generator;
}

devs::Time GeneratorWrapper::init(const vle::devs::Time& /* time */)
{
    std::string formalism(value::toString(m_events.get("formalism")));

    if (formalism == "regular") {
        m_generator = new RegularGenerator(m_rand,
            value::toDouble(m_events.get("timestep")));
    } else if (formalism == "uniform") {
        m_generator = new UniformGenerator(m_rand,
            value::toDouble(m_events.get("min")),
            value::toDouble(m_events.get("max")));
    } else if (formalism == "normal") {
        m_generator = new NormalGenerator(m_rand,
            value::toDouble(m_events.get("average")),
            value::toDouble(m_events.get("stdvariation")));
    } else if (formalism == "lognormal") {
        m_generator = new LogNormalGenerator(m_rand,
            value::toDouble(m_events.get("average")),
            value::toDouble(m_events.get("stdvariation")));
    } else {
        throw utils::InternalError(fmt(_("Unknow generator %1%")) %
              formalism);
    }

    m_generator->init();
    m_timeStep = 0;
    return devs::Time(0);
}

void GeneratorWrapper::output(const devs::Time& /* time */,
                              devs::ExternalEventList& output) const
{
    output.push_back(new devs::ExternalEvent("out"));
}

devs::Time GeneratorWrapper::timeAdvance() const
{
    return devs::Time(m_timeStep);
}

void GeneratorWrapper::internalTransition(const devs::Time& /* event */)
{
    m_time += m_timeStep;
    m_timeStep = m_generator->generate();
}

}}} // namespace vle examples generator


DECLARE_DYNAMICS(vle::examples::generator::GeneratorWrapper)
