/*
 * @file vle/examples/gens/GenExecutiveGraph.cpp
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
#include <functional>
#include <vle/devs/Executive.hpp>
#include <vle/translator/GraphTranslator.hpp>
#include <vle/utils/Tools.hpp>

namespace vle { namespace examples { namespace gens {

class GenExecutiveGraph : public devs::Executive
{
public:
    GenExecutiveGraph(const devs::ExecutiveInit& mdl,
                      const devs::InitEventList& events)
        : devs::Executive(mdl, events)
    {
    }

    virtual ~GenExecutiveGraph()
    {}

    void make(const vle::translator::graph_generator::node_metrics& metrics,
        std::string& name,
        std::string& classname)
    {
        name = vle::utils::format("node-%d", metrics.id);

        if (metrics.id == 1 or metrics.id == 3)
            classname = "beepbeepbeep";
        else if (metrics.id == 0 or metrics.id == 2 or metrics.id == 4)
            classname = "beepbeep";
        else
            classname = "counter";
    }

    virtual devs::Time init(devs::Time /* time */) override
    {
        vle::translator::graph_generator::parameter params {
            std::bind(&GenExecutiveGraph::make, this,
                      std::placeholders::_1,
                      std::placeholders::_2,
                      std::placeholders::_3),
            vle::translator::graph_generator::connectivity::IN_OUT,
            true };

        vle::translator::graph_generator gt(params);
        vle::utils::Array<bool> adj(6, 6);

        adj = { 0, 0, 0, 0, 0, 1,
                0, 0, 1, 0, 0, 1,
                0, 1, 0, 0, 0, 1,
                0, 0, 0, 1, 0, 1,
                0, 0, 0, 1, 0, 1,
                0, 0, 0, 0, 0, 0 };

        gt.make_graph(*this, adj.rows(), adj);

        return devs::infinity;

    }
};

}}} // namespace vle examples gens

DECLARE_EXECUTIVE(vle::examples::gens::GenExecutiveGraph)
