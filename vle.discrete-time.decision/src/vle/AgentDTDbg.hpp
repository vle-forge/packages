/*
 *
 * This file is part of VLE, a framework for multi-modeling, simulation
 * and analysis of complex dynamical systems
 * http://www.vle-project.org
 *
 * Copyright (c) 2013-2013 INRA http://www.inra.fr
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


#ifndef VLE_DISCRETE_TIME_DEC_AGENT_DBG_HPP
#define VLE_DISCRETE_TIME_DEC_AGENT_DBG_HPP 1

#include <vle/temporal_values/TemporalValues.hpp>
#include <vle/discrete-time/DiscreteTimeDyn.hpp>
#include <vle/AgentDT.hpp>
#include <vle/utils/Trace.hpp>
#include <vle/version.hpp>

#define DECLARE_VLE_DISCRETE_TIME_DBG(mdl)                                    \
  extern "C" {                                                                \
    vle::devs::Dynamics*                                                      \
    vle_make_new_dynamics(const vle::devs::DynamicsInit& init,                \
                              const vle::devs::InitEventList& events)         \
    {                                                                         \
      return new vle::discrete-time::DiscreteTimeDynDbg                       \
      < mdl >(init, events);                                                  \
    }                                                                         \
                                                                              \
    void                                                                      \
    vle_api_level(vle::uint32_t* major,                                       \
                  vle::uint32_t* minor,                                       \
                  vle::uint32_t* patch)                                       \
    {                                                                         \
            *major = VLE_MAJOR_VERSION;                                       \
            *minor = VLE_MINOR_VERSION;                                       \
            *patch = VLE_PATCH_VERSION;                                       \
    }                                                                         \
  }



namespace vle {
namespace discrete_time {

namespace vt = vle::temporal_values;

template < typename UserModel >
class DiscreteTimeDynDbg : public UserModel
{
public:
    DiscreteTimeDynDbg(const vle::devs::DynamicsInit& model,
               const vle::devs::InitEventList& events)
        : UserModel(model, events), mName(model.model().getCompleteName())
    {
        TraceExtension(vle::fmt(_("                     %1% "
                "[Rec:DifferenceEquation] constructor")) % mName);
    }

    virtual ~DiscreteTimeDynDbg() {}


    void compute(const vle::devs::Time& time)
    {
        TraceExtension(vle::fmt(_("%1$20.10g %2% [DiscreteTimeDyn] "
                             " compute with: %3%")) % time % mName
                                                    % traceVariables());

        UserModel::compute(time);

        TraceExtension(vle::fmt(_("                .... %1% "
                "[DiscreteTimeDyn] compute returns %2%"))
                % mName % traceVariables());

    }
private:
    std::string traceVariables()
    {
        std::string line;
        {
            std::string l(" ");
            for (vt::Variables::const_iterator itv =
                    UserModel::tvp.variables.begin();
                 itv != UserModel::tvp.variables.end(); ++itv) {
                l += (vle::fmt("%1% ") % itv->first).str();
            }
            line += (vle::fmt(" %1% = (%2%) ;") % mName % l).str();
        }
        return line;
    }

    std::string mName;
};

}} // namespace



#endif
