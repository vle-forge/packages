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
 * @@tagdepends: vle.extension.fsa @@endtagdepends
 */

#include <vle/extension/fsa/Statechart.hpp>
#include <iostream>

namespace vle {

namespace discrete_time {
namespace test {

namespace ve = vle::extension;
namespace vf = vle::extension::fsa;
namespace vd = vle::devs;

enum State { A, B };

class Perturb6 : public vf::Statechart
{
public:
    Perturb6(const vd::DynamicsInit& init, const vd::InitEventList& events) :
        vf::Statechart(init, events)
    {
        //std::cout << " DBG const perturb " << std::endl;
        states(this) << A << B;
        transition(this, A, B) << after(4.5) << send(&Perturb6::out1);
        transition(this, B, A) << after(3.2) << send(&Perturb6::out2);
        initialState(A);
    }

    virtual ~Perturb6() { }

    void out1(const vd::Time&  /*time*/, vd::ExternalEventList& output) const
    {
        vd::ExternalEvent* e = new vd::ExternalEvent("a");
        e->attributes().addString("name","a");
        e->attributes().addDouble("value",10);
        output.push_back(e);
        //std::cout << " DBG " << time << " " << std::endl;
    }

    void out2(const vd::Time& /* time */, vd::ExternalEventList& output) const
    {
        vd::ExternalEvent* e = new vd::ExternalEvent("a");
        e->attributes().addString("name","a");
        e->attributes().addDouble("value",0);
        output.push_back(e);
    }

};

}}} // namespaces

DECLARE_DYNAMICS(vle::discrete_time::test::Perturb6)
