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

namespace vd = vle::devs;
namespace vv = vle::value;
namespace ve = vle::extension;

namespace vle {
namespace discrete_time {
namespace test {

class earlyPerturbFSA : public ve::fsa::Statechart
{
public:
    earlyPerturbFSA(const vd::DynamicsInit& init, const vd::InitEventList& evts)
        : ve::fsa::Statechart(init, evts)
    {
        states(this) << 0;
        initialState(0);
        transition(this, 0, 0)
            << when(3.0)
            << send(&earlyPerturbFSA::out);
    }

    virtual ~earlyPerturbFSA()
    { }

    void out(const vd::Time& /*time*/, vd::ExternalEventList& output) const
    {
        vd::ExternalEvent* evt = new vd::ExternalEvent("fsa");

        evt->putAttribute("name", new vv::String("fsa"));
        evt->putAttribute("value", new vv::Double(1.0));
        output.push_back(evt);
    }
};

DECLARE_DYNAMICS(earlyPerturbFSA)
}}}


