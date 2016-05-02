/*
 * Copyright (c) 2014-2016 INRA http://www.inra.fr
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

#include <vle/discrete-time/DiscreteTimeExec.hpp>
#include <boost/format.hpp>

namespace vle {
namespace discrete_time {
namespace test {



class DTexec : public DiscreteTimeExec
{
public:
    DTexec(const vle::devs::ExecutiveInit& init,
            const vle::devs::InitEventList& events)
        :  DiscreteTimeExec(init,events)
    {
        num_model.init(this, "num_model", events);
    }

    virtual ~DTexec()
    {
    }

    void compute(const vle::devs::Time& /*t*/) override
    {
        num_model = num_model(-1) + 1;
        std::string current((boost::format("A_%1%") % (num_model())).str());
        std::string inPort((boost::format("a_%1%") % (num_model())).str());
        createModelFromClass("AClass", current);
        addInputPort("GenericSum", inPort);
        addConnection(current, "a", "GenericSum", inPort);
    }

    Var num_model;
};

DECLARE_EXECUTIVE(DTexec)

}}}

