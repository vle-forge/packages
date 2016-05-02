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

#include <vle/DiscreteTime.hpp>


#include <iostream>




namespace vle {
namespace discrete_time {
namespace test {



class earlyPerturbE2 : public DiscreteTimeDyn
{
public:
    earlyPerturbE2(const vle::devs::DynamicsInit& init, const vle::devs::InitEventList& evts)
        : DiscreteTimeDyn(init,evts)
    {
        fsa.init(this, "fsa",evts);
        e2.init(this, "e2",evts);
        e1.init(this, "e1",evts);
    }

    virtual ~earlyPerturbE2()
    { }

    void compute(const vle::devs::Time& /* time */) override
    {
        //can be perturbated
        fsa = 0;
        //other var
        e2 = fsa() + e1() + 1;
    }


private:
    Var fsa;
    Var e2;
    Var e1;
};

DECLARE_DYNAMICS(earlyPerturbE2)

}}}


