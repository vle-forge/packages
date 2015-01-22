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
 * @@tagdepends: vle.extension.dsdevs, vle.discrete-time @@endtagdepends
 */

#include <vle/devs/Executive.hpp>
#include <vle/devs/ExecutiveDbg.hpp>
#include <vle/discrete-time/TemporalValues.hpp>
#include <boost/numeric/conversion/cast.hpp>
#include <sstream>



namespace vz = vle::vpz;

namespace vle {

namespace discrete_time {
namespace test {



class executiveDT_dsdevs : public vle::devs::Executive
{

public:
    executiveDT_dsdevs(const vle::devs::ExecutiveInit& mdl,
                 const vle::devs::InitEventList& events) :
        vle::devs::Executive(mdl, events), num_model(0)
    {
    }

    virtual ~executiveDT_dsdevs()
    {
    }

    virtual vle::devs::Time init(const vle::devs::Time& /* time */)
    {
        num_model = 1;
        return 2;
    }

    virtual void output(const vle::devs::Time& /* time */,
            vle::devs::ExternalEventList& /* output */) const
    {
    }

    virtual vle::devs::Time timeAdvance() const
    { return 2.0; }

    virtual void internalTransition(
            const vle::devs::Time& /* time */)
    {
        num_model ++;
        std::stringstream is, is2;
        is << "B_";
        is << num_model;
        createModelFromClass("ClassB", is.str());
        addConnection("A", "a", is.str(), "a");
        is2 << "B_";
        is2 << (num_model-1);
        delModel(is2.str());
    }

    unsigned int num_model;
};

DECLARE_EXECUTIVE_DBG(executiveDT_dsdevs)

}}}


