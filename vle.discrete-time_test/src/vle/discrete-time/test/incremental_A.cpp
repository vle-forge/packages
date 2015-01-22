/*
 * Copyright (c) 2014-2014 INRA http://www.inra.fr
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to
 * deal in the Software without restriction, including without limitation the
 * rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
 * sell copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
 * IN THE SOFTWARE.
 */

/*
 * @@tagdynamic@@
 * @@tagdepends: vle.discrete-time @@endtagdepends
 */

#include <vle/DiscreteTimeDbg.hpp>
#include <vle/devs/DynamicsDbg.hpp>

#include <iostream>




namespace vle {
namespace discrete_time {
namespace test {



class incremental_A : public DiscreteTimeDyn
{
public:
    incremental_A(const vle::devs::DynamicsInit& init, const vle::devs::InitEventList& events)
        : DiscreteTimeDyn(init, events)
    {
        vars.push_back(new Var());
        Var& v = *vars[0];
        v.init(this, "a_1",events);
        v.init_value(-10);

    }

    virtual ~incremental_A()
    {
        std::vector<Var*>::iterator itb = vars.begin();
        std::vector<Var*>::iterator ite = vars.end();
        for (; itb != ite; itb++) {
            delete *itb;
        }
    }

    void compute(const vle::devs::Time& t)
    {
        for (unsigned int i=0; i < vars.size(); i++) {
            Var& v = *vars[i];
            v = v() + 1;
        }
        //add a Var initialized to -10.0
        vars.push_back(new Var());
        std::stringstream is;
        is << "a_";
        is << vars.size();
        Var& vlast = *vars[vars.size() -1];
        vlast.init(this, is.str(), initMap);
        vlast.init_value(-10);
        vlast.init_history(t);
    }
    std::vector<Var*> vars;
    vle::value::Map initMap;
};

}}}

DECLARE_DYNAMICS_DBG(vle::discrete_time::test::incremental_A)

