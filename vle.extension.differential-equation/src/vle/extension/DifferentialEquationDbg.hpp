/*
 * @file vle/extension/DifferentialEquationDbg.hpp
 *
 * This file is part of VLE, a framework for multi-modeling, simulation
 * and analysis of complex dynamical systems
 * http://www.vle-project.org
 *
 * Copyright (c) 2011 INRA http://www.inra.fr
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


#ifndef VLE_EXTENSION_DIFFERENTIAL_EQUATION_DBG_HPP
#define VLE_EXTENSION_DIFFERENTIAL_EQUATION_DBG_HPP 1

#include <vle/extension/differential-equation/DifferentialEquation.hpp>
#include <vle/utils/Trace.hpp>
#include <vle/version.hpp>

#define DECLARE_DIFFERENTIAL_EQUATION_DBG(mdl)                                \
  extern "C" {                                                                \
    VLE_EXTENSION_EXPORT vle::devs::Dynamics*                                 \
    vle_make_new_dynamics(const vle::devs::DynamicsInit& init,                \
                              const vle::devs::InitEventList& events)         \
    {                                                                         \
      return new vle::extension::differential_equation::DifferentialEquationDbg\
      < mdl >(init, events);                                                  \
    }                                                                         \
                                                                              \
    VLE_EXTENSION_EXPORT void                                                 \
    vle_api_level(vle::uint32_t* major,                                       \
                  vle::uint32_t* minor,                                       \
                  vle::uint32_t* patch)                                       \
    {                                                                         \
            *major = VLE_MAJOR_VERSION;                                       \
            *minor = VLE_MINOR_VERSION;                                       \
            *patch = VLE_PATCH_VERSION;                                       \
    }                                                                         \
  }



namespace vle { namespace extension { namespace differential_equation {

template < typename UserModel >
class DifferentialEquationDbg : public UserModel
{
public:
    DifferentialEquationDbg(const devs::DynamicsInit& model,
               const devs::InitEventList& events)
        : UserModel(model, events), mName(model.model().getCompleteName())
    {
        TraceExtension(fmt(_("                     %1% [Differential] "
                "constructor")) % mName);
    }

    virtual ~DifferentialEquationDbg() {}


    void compute(const vle::devs::Time& time)
    {
        TraceExtension(fmt(_("%1$20.10g %2% [Differential] compute with: %3%"))
                               % time % mName % traceVariables());

        UserModel::compute(time);

        TraceExtension(fmt(_("                .... %1% [Differential] compute "
                "returns %2%")) % mName % traceVariables());

    }

    virtual void reinit(const vv::Set& evt, bool perturb, const vd::Time& time)
    {
        TraceExtension(fmt(_("%1$20.10g %2% [Differential] re-initialisation "
                " (perturbation : %3%)  with: %4%"))
                       % time % mName % perturb  % traceVariables());

        UserModel::reinit(evt,perturb,time);

        TraceExtension(fmt(_("                .... %1% [Differential] "
                " re-initialisation finished")) % mName);
    }

private:
    std::string traceVariables()
    {
        std::string line;
        {
            std::string l(" ");
            for (Variables::const_iterator itv =
                     DifferentialEquation::mvars.begin();
                 itv != DifferentialEquation::mvars.end(); ++itv) {
                l += (fmt("%1% ") % itv->first).str();
            }
            line += (fmt(" %1% = (%2%) ;") % mName % l).str();
        }
        return line;
    }

    std::string mName;
};

}}} // namespace



#endif
