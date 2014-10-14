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

namespace vd = vle::devs;
namespace vv = vle::value;

namespace vle {

namespace discrete_time {
namespace test {



class F1 : public DiscreteTimeDyn
{
public:
    F1(const vd::DynamicsInit& init, const vd::InitEventList& events)
        : DiscreteTimeDyn(init, events)
    {
        a.init(this, "a", events);
        b.init(this, "b", events);
        c.init(this, "c", events);
        d.init(this, "d", events);
        g.init(this, "g", events);
    }

    virtual ~F1()
    {
    }

    void compute(const vd::Time& /*t*/)
    {
        //std::cout << " DBG compute F1 " << t << " a(-1)=" << a(-1) << std::endl;
        a = a(-1) + 1;
        b = b(-1) + d(-1) - g()  + 1;
        c = c(-1) + b(-1) + 1 ;
    }

    Var a;
    Var b;
    Var c;
    Var d;
    Var g;
};

}}}

DECLARE_DYNAMICS_DBG(vle::discrete_time::test::F1)

