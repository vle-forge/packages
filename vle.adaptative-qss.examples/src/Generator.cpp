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

class ConstantGenerator : public vle::devs::Dynamics
{
    int m_neighborhood;

public:
    ConstantGenerator(const vle::devs::DynamicsInit &init,
                      const vle::devs::InitEventList &events)
        : vle::devs::Dynamics(init, events)
    {
        m_neighborhood = events.getInt("neighborhood");
    }

    virtual ~ConstantGenerator()
    {}

    virtual vle::devs::Time init(vle::devs::Time /*time*/) override
    {
        return 0;
    }

    virtual void output(vle::devs::Time /*time*/,
                        vle::devs::ExternalEventList &output) const override
    {
        output.emplace_back("out");
        output.back().addMap().addDouble("value",
                                         static_cast<double>(m_neighborhood));
    }

    virtual vle::devs::Time timeAdvance() const override
    {
        return vle::devs::infinity;
    }
};

DECLARE_DYNAMICS(ConstantGenerator)
