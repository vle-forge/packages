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

#include <vle/devs/Executive.hpp>
#include <memory>

std::string make_name(int x, int y)
{
    std::string ret = "m-";
    ret += std::to_string(x);
    ret += '.';
    ret += std::to_string(y);
    return ret;
}

struct position {
    int x, y;
};

class World2d : public vle::devs::Executive
{
    std::vector<position> m_starts;
    int m_height;
    int m_width;
    int m_start_x;
    int m_start_y;

public:
    World2d(const vle::devs::ExecutiveInit &init,
            const vle::devs::InitEventList &events)
        : vle::devs::Executive(init, events)
    {
        m_height = events.getInt("height");
        m_width = events.getInt("width");

        if (m_height * m_width <= 0)
            throw vle::utils::ModellingError(
                "World2D: bad parameter height or width");

        m_start_x = events.getInt("start_x");
        m_start_y = events.getInt("start_y");

        if (m_start_x < 0 or m_start_x > m_width or
            m_start_y < 0 or m_start_y > m_height)
            throw vle::utils::ModellingError(
                "World2D: bad parameter start_x and start_y");
    }

    virtual ~World2d() {}

    void create(int x, int y)
    {
        auto name = make_name(x, y);
        int neighborhood = 0;

        if (y - 1 >= 0)
            neighborhood++;

        if (y + 1 < m_height)
            neighborhood++;

        if (x - 1 >= 0)
            neighborhood++;

        if (x + 1 < m_width)
            neighborhood++;

        auto& cond_cell = conditions().get("world_builder");

        auto default_value = (x == m_start_x and y == m_start_y) ? 10000. : 0.;
        cond_cell.setValueToPort("X_0",
                                 vle::value::Double::create(default_value));

        cond_cell.setValueToPort("neighborhood",
                                 vle::value::Integer::create(neighborhood));
        createModelFromClass("cell", name);
    }

    void connect(int x, int y)
    {
        auto from = make_name(x, y);

        if (y - 1 >= 0)
            addConnection(from, "out", make_name(x, y - 1), "south");

        if (y + 1 < m_height)
            addConnection(from, "out", make_name(x, y + 1), "north");

        if (x - 1 >= 0)
            addConnection(from, "out", make_name(x - 1, y), "east");

        if (x + 1 < m_width)
            addConnection(from, "out", make_name(x + 1, y), "west");
    }

    virtual vle::devs::Time init(vle::devs::Time /* time */) override
    {
        for (auto x = 0; x != m_width; ++x)
            for (auto y = 0; y != m_height; ++y)
                create(x, y);

        for (auto x = 0; x != m_width; ++x)
            for (auto y = 0; y != m_height; ++y)
                connect(x, y);

        return vle::devs::infinity;
    }
};

DECLARE_EXECUTIVE(World2d)
