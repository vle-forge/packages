/*
 * Copyright 2016 INRA
 *
 * Licensed under the Apache License, Version 2.0 (the "License"); you may
 * not use this file except in compliance with the License.  You may
 * obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or
 * implied.  See the License for the specific language governing
 * permissions and limitations under the License.
 */

#include <vle/value/Map.hpp>
#include <vle/devs/Dynamics.hpp>
#include <unordered_map>
#include <vector>
#include <numeric>

class Sum : public vle::devs::Dynamics
{
    std::unordered_map <std::string, int> m_dict;
    std::vector <double> m_value;
    vle::devs::Time m_next;

public:
    Sum(const vle::devs::DynamicsInit &init,
        const vle::devs::InitEventList &events)
        : vle::devs::Dynamics(init, events)
    {
        const auto& lst = getModel().getInputPortList();

        int i = 0;
        for (auto &elem : lst)
            m_dict.emplace(elem.first, i++);

        m_value.resize(m_dict.size(), 0.0);
    }

    virtual ~Sum() {}

    virtual vle::devs::Time init(vle::devs::Time /*time*/) override
    {
        std::fill(m_value.begin(), m_value.end(), 0);
        m_next = vle::devs::infinity;

        return timeAdvance();
    }

    virtual void output(vle::devs::Time /*time*/,
                        vle::devs::ExternalEventList &output) const override
    {
        output.emplace_back("out");
        output.back().addMap().addDouble("value",
                                         std::accumulate(m_value.begin(),
                                                         m_value.end(),
                                                         0.0));
    }

    virtual vle::devs::Time timeAdvance() const override
    {
        return m_next;
    }

    virtual void internalTransition(vle::devs::Time /*time*/) override
    {
        m_next = vle::devs::infinity;
    }

    virtual void externalTransition(const vle::devs::ExternalEventList &lst,
                                    vle::devs::Time /*time*/) override
    {
        for (auto &elem : lst) {
            auto it = m_dict.find(elem.getPortName());
            if (it != m_dict.end())
                m_value[it->second] = elem.getMap().getDouble("d_val");
        }

        m_next = 0;
    }
};

DECLARE_DYNAMICS(Sum)
