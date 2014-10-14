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



class earlyPerturbE1 : public DiscreteTimeDyn
{
public:
    earlyPerturbE1(const vd::DynamicsInit& init, const vd::InitEventList& evts)
        : DiscreteTimeDyn(init,evts)
    {
        e1.init(this, "e1",evts);
        e0.init(this, "e0",evts);
    }

    virtual ~earlyPerturbE1()
    { }


    void compute(const vd::Time& /*time*/)
    { e1 = e0() + e1(-1) + 1; }

private:
    Var e1;
    Var e0;
};

DECLARE_DYNAMICS(earlyPerturbE1)

}}}


