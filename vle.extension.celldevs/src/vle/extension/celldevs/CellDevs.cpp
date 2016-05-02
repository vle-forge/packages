/*
 * @file vle/extension/celldevs/CellDevs.cpp
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


#include <vle/extension/celldevs/CellDevs.hpp>
#include <vle/value/Double.hpp>
#include <vle/value/Integer.hpp>
#include <vle/value/Boolean.hpp>
#include <vle/value/String.hpp>
#include <vle/value/Set.hpp>
#include <vle/utils/Exception.hpp>
#include <cassert>
#include <algorithm>

using std::map;
using std::pair;
using std::string;
using std::vector;

namespace vle { namespace extension {

using namespace vle::devs;
using namespace vle::value;

/***********************************************************************/
//
// un port d'entree par voisin
// une entree du map m_neighbourState par voisin
// le port de sortie se nomme "out"
// le changement d'etat de la cellule est placee sur le port de sortie
//  apres un certain delai
// les evenements de notification des changements d'etat des cellules
//  voisines peuvent ou non etre synchrones
//
/***********************************************************************/

CellDevs::CellDevs(const vle::devs::DynamicsInit& model,
                   const vle::devs::InitEventList& events) :
    vle::devs::Dynamics(model, events),
    m_modified(false)
{
    InitEventList::const_iterator it = events.begin();
    while (it != events.end()) {
        string name = it->first;

        if (name == "Delay")
            setDelay(it->second->toDouble().value());
        else
            if (name == "Neighbourhood") {
                const value::Set& set = it->second->toSet();
                value::VectorValue::const_iterator it2 = set.begin();

                while (it2 != set.end()) {
                    std::string neighbour = vle::value::toString(*it2);

                    m_neighbourPortList.push_back(neighbour);
                    ++it2;
                }
            } else {
                if (m_state.find(name) != m_state.end()) {
                    initState(name,it->second->clone());
                } else {
                    m_parameters.add(name, it->second->clone());
                }
            }
        ++it;
    }
}

typedef std::map < std::string,
                   std::pair < std::unique_ptr<value::Value> ,
                               bool > > State_t;
typedef std::map < std::string,
                   std::map< std::string ,
                             std::unique_ptr<value::Value> > > NeighbourState_t;
typedef std::map < std::string, std::unique_ptr<value::Value> > Parameters_t;

struct StateDeleter
    : std::unary_function < State_t::value_type, void >
{
    void operator()(State_t::value_type &value) const
    {
        value.second.first = 0;
    }
};

struct NeighbourStateDeleter
    : std::unary_function < NeighbourState_t::value_type, void >
{
    void operator()(NeighbourState_t::value_type & /*value*/) const
    {
//        typedef std::map < std::string, std::unique_ptr<value::Value> > map_t;

////TODO
//        std::for_each(value.second.begin(), value.second.end(),
//                      boost::bind(
//                          boost::checked_deleter < value::Value >(),
//                          boost::bind(
//                              &map_t::value_type::second, _1)));
    }
};

CellDevs::~CellDevs()
{
    std::for_each(m_state.begin(), m_state.end(),
                  StateDeleter());

    std::for_each(m_neighbourState.begin(), m_neighbourState.end(),
                  NeighbourStateDeleter());
////TODO
//    std::for_each(m_parameters.begin(), m_parameters.end(),
//                  boost::bind(
//                      boost::checked_deleter < value::Value >(),
//                      boost::bind(
//                          &Parameters_t::value_type::second, _1)));
}

Time const & CellDevs::getSigma() const
{
    return m_sigma;
}


void CellDevs::setSigma(devs::Time const & p_sigma)
{
    m_sigma = p_sigma;
}

Time const & CellDevs::getLastTime() const
{
    return m_lastTime;
}


void CellDevs::setLastTime(devs::Time const & p_lastTime)
{
    m_lastTime = p_lastTime;
}

void CellDevs::hiddenState(std::string const & p_name)
{
    assert(m_state.find(p_name) != m_state.end());

    const value::Value& v_value = getState(p_name);

    m_state[p_name] = pair < std::unique_ptr<value::Value> , bool >(
            v_value.clone(),false);
}

void CellDevs::initState(std::string const & p_name,
                         std::unique_ptr<value::Value> p_value,
                         bool p_visible)
{
    assert(m_state.find(p_name) == m_state.end());

    m_stateNameList.push_back(p_name);

    m_state[p_name] = pair<std::unique_ptr<value::Value>,bool>(
            std::move(p_value),p_visible);
    if (p_visible) m_modified = true;
}

void CellDevs::initDoubleState(std::string const & p_name,
                               double p_value,
                               bool p_visible)
{
    return initState(p_name,
                     value::Double::create(p_value),
                     p_visible);
}

void CellDevs::initIntegerState(std::string const & p_name,
                                long p_value,
                                bool p_visible)
{
    return initState(p_name,
                     value::Integer::create(p_value),
                     p_visible);
}


void CellDevs::initBooleanState(std::string const & p_name,
                                bool p_value,
                                bool p_visible)
{
    return initState(p_name,
                     value::Boolean::create(p_value),
                     p_visible);
}


void CellDevs::initStringState(std::string const & p_name,
                               std::string const & p_value,
                               bool p_visible)
{
    return initState(p_name,
                     value::String::create(p_value),
                     p_visible);
}


void CellDevs::initNeighbourhood(std::string const & p_stateName,
                                 std::unique_ptr<value::Value> p_value)
{
    vector < string >::const_iterator it = m_neighbourPortList.begin();

    while (it != m_neighbourPortList.end()) {
        initNeighbourState(*it,p_stateName, p_value->clone());
        ++it;
    }
}

void CellDevs::initDoubleNeighbourhood(std::string const & p_stateName,
                                       double p_value)
{
    initNeighbourhood(p_stateName,
                      value::Double::create(p_value));
}


void CellDevs::initIntegerNeighbourhood(std::string const & p_stateName,
                                        long p_value)
{
    initNeighbourhood(p_stateName,
                      value::Integer::create(p_value));
}

void CellDevs::initBooleanNeighbourhood(std::string const & p_stateName,
                                        bool p_value)
{
    initNeighbourhood(p_stateName,
                      value::Boolean::create(p_value));
}

void CellDevs::initStringNeighbourhood(std::string const & p_stateName,
                                       std::string const & p_value)
{
    initNeighbourhood(p_stateName,
                      value::String::create(p_value));
}

void CellDevs::initNeighbourState(std::string const & p_neighbourName,
                                  std::string const & p_stateName,
                                  std::unique_ptr<value::Value> p_value)
{
    NeighbourState_t::iterator it = m_neighbourState.find(p_neighbourName);
    if (it != m_neighbourState.end()) {
        std::map < std::string , std::unique_ptr<value::Value> >::iterator jt;
        jt = it->second.find(p_stateName);

        if (jt != it->second.end()) {
            jt->second = p_value->clone();
        } else {
            it->second[p_stateName] = p_value->clone();
        }
    } else {
        m_neighbourState[p_neighbourName][p_stateName] = p_value->clone();
    }
}

void CellDevs::initDoubleNeighbourState(std::string const & p_neighbourName,
                                        std::string const & p_stateName,
                                        double p_value)
{
    initNeighbourState(p_neighbourName,
                       p_stateName,
                       value::Double::create(p_value));
}

void CellDevs::initIntegerNeighbourState(std::string const & p_neighbourName,
                                         std::string const & p_stateName,
                                         long p_value)
{
    initNeighbourState(p_neighbourName,p_stateName,
                       value::Integer::create(p_value));
}

void CellDevs::initBooleanNeighbourState(std::string const & p_neighbourName,
                                         std::string const & p_stateName,
                                         bool p_value)
{
    initNeighbourState(p_neighbourName,p_stateName,
                       value::Boolean::create(p_value));
}

void CellDevs::initStringNeighbourState(std::string const & p_neighbourName,
                                        std::string const & p_stateName,
                                        std::string const & p_value)
{
    initNeighbourState(p_neighbourName,p_stateName,
                       value::String::create(p_value));
}

bool CellDevs::existNeighbourState(std::string const & p_name) const
{
    return m_neighbourState.find(p_name) !=
        m_neighbourState.end();
}

bool CellDevs::existState(std::string const & p_name) const
{
    vector < string >::const_iterator it = m_stateNameList.begin();

    while (it != m_stateNameList.end()) {
        if (*it == p_name) return true;
        else ++it;
    }
    return false;
}

bool CellDevs::isNeighbourEvent(devs::ExternalEvent* event) const
{
    return existNeighbourState(event->getPortName());
}

double CellDevs::getDelay() const
{
    return m_delay;
}

const value::Value&
CellDevs::getState(std::string const & p_name) const
{
    assert(existState(p_name));
    assert(m_state.find(p_name) != m_state.end());

    return *(m_state.find(p_name)->second.first);
}

double CellDevs::getDoubleState(std::string const & p_name) const
{
    return (value::toDouble(getState(p_name)));
}

long CellDevs::getIntegerState(std::string const & p_name) const
{
    return (value::toInteger(getState(p_name)));
}

bool CellDevs::getBooleanState(std::string const & p_name) const
{
    return (value::toBoolean(getState(p_name)));
}

string CellDevs::getStringState(std::string const & p_name) const
{
    return (value::toString(getState(p_name)));
}

const value::Value& CellDevs::getNeighbourState(
        std::string const & p_neighbourName,
        std::string const & p_stateName) const
{
    assert(m_neighbourState.find(p_neighbourName) !=
           m_neighbourState.end());
    assert(m_neighbourState.find(p_neighbourName)->second.
           find(p_stateName) != m_neighbourState.
           find(p_neighbourName)->second.end());

    return *(m_neighbourState.find(p_neighbourName)->second.
        find(p_stateName)->second);
}

double CellDevs::getDoubleNeighbourState(std::string const & p_neighbourName,
                                         std::string const & p_stateName) const
{
    return value::toDouble(getNeighbourState(p_neighbourName,p_stateName));
}

long CellDevs::getIntegerNeighbourState(std::string const & p_neighbourName,
                                        std::string const & p_stateName) const
{
    return value::toInteger(getNeighbourState(p_neighbourName,p_stateName));
}

bool CellDevs::getBooleanNeighbourState(std::string const & p_neighbourName,
                                        std::string const & p_stateName) const
{
    return value::toBoolean(getNeighbourState(p_neighbourName,p_stateName));
}

string CellDevs::getStringNeighbourState(std::string const & p_neighbourName,
                                         std::string const & p_stateName) const
{
    return value::toString(getNeighbourState(p_neighbourName,p_stateName));
}

unsigned int CellDevs::getNeighbourStateNumber() const
{
    return m_neighbourState.size();
}

unsigned int CellDevs::getBooleanNeighbourStateNumber(std::string const & p_stateName,
                                                      bool p_value) const
{
    unsigned int v_counter = 0;
    map < string , map < string ,
                   std::unique_ptr<value::Value> > >::const_iterator it =
        m_neighbourState.begin();

    while (it != m_neighbourState.end()) {
        const value::Value& v_value = *(it->second.find(p_stateName)->second);

        if (v_value.toBoolean().value() == p_value)
            v_counter++;
        it++;
    }
    return v_counter;
}

unsigned int CellDevs::getIntegerNeighbourStateNumber(std::string const & p_stateName,
                                                      long p_value) const
{
    unsigned int v_counter = 0;
    map < string , map < string ,
                   std::unique_ptr<value::Value> > >::const_iterator it =
        m_neighbourState.begin();

    while (it != m_neighbourState.end()) {
        const value::Value& v_value = *(it->second.find(p_stateName)->second);

        if (v_value.toInteger().value()  == p_value)
            v_counter++;
        it++;
    }
    return v_counter;
}

void CellDevs::setState(std::string const & p_name,
        std::unique_ptr<value::Value> p_value)
{
    assert(existState(p_name));

    std::map < string ,
               pair <std::unique_ptr<value::Value> , bool > >::iterator it =
        m_state.find(p_name);

    bool v_visible = it->second.second;

    it->second = std::pair < std::unique_ptr<value::Value> ,
                        bool >(std::move(p_value), v_visible);

    if (v_visible)
        m_modified = true;
}

void CellDevs::setDoubleState(std::string const & p_name,double p_value)
{
    setState(p_name, value::Double::create(p_value));
}

void CellDevs::setIntegerState(std::string const & p_name,long p_value)
{
    setState(p_name, value::Integer::create(p_value));
}

void CellDevs::setBooleanState(std::string const & p_name,bool p_value)
{
    setState(p_name, value::Boolean::create(p_value));
}

void CellDevs::setStringState(std::string const & p_name,
                              std::string const & p_value)
{
    setState(p_name, value::String::create(p_value));
}

void CellDevs::setNeighbourState(std::string const & p_neighbourName,
                                 std::string const & p_stateName,
                                 const value::Value& p_value)
{
    assert(m_neighbourState.find(p_neighbourName) !=
           m_neighbourState.end());

    std::map < string , std::unique_ptr<value::Value> >& v_state =
            m_neighbourState.find(p_neighbourName)->second;
    std::map < string , std::unique_ptr<value::Value> >::iterator it =
            v_state.find(p_stateName);

    it->second = p_value.clone();
}

//***********************************************************************
//***********************************************************************
//
//  DEVS Methods
//
//***********************************************************************
//***********************************************************************

Time CellDevs::init(Time /* time */)
{
    setLastTime(Time(0));
    m_neighbourModified = false;
    return devs::infinity;
}

void CellDevs::output(Time /* time */, ExternalEventList& output) const
{
    if (m_modified) {
        output.emplace_back("out");
        value::Map& attr = output.back().addMap();

        map <string ,
             pair< std::unique_ptr<value::Value>,bool > >::const_iterator it =
            m_state.begin();

        while (it != m_state.end()) {
            if (it->second.second)
                attr.add(it->first,it->second.first->clone());
            ++it;
        }
    }
}

Time CellDevs::timeAdvance() const
{
    return m_sigma;
}

void CellDevs::internalTransition(vle::devs::Time /* time */)
{
    if (m_modified)
        m_modified = false;
}

void CellDevs::externalTransition(const ExternalEventList& event,
                                  Time time)
{
    ExternalEventList::const_iterator it = event.begin();

    while (it != event.end()) {
        string v_portName = it->getPortName();

        if (existNeighbourState(v_portName)) {
            map <string, std::unique_ptr<value::Value> >::const_iterator it2 =
                m_neighbourState[v_portName].begin();

            while (it2 != m_neighbourState[v_portName].end()) {
                string v_name = it2->first;
                const value::Value& v_value =
                        *it->attributes()->toMap().get(v_name);
                setNeighbourState(v_portName,v_name,v_value);
                ++it2;
            }
            m_neighbourModified = true;
            updateSigma(time);
        }
        else // c'est une perturbation
            processPerturbation(*it);
        ++it;
    }
}

std::unique_ptr<vle::value::Value> CellDevs::observation(
        const ObservationEvent& event) const
{
    if (existState(event.getPortName())) {
        return getState(event.getPortName()).clone();
    } else {
        return 0;
    }
}

}} // namespace vle extension
