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
 * @@tagdepends: @@endtagdepends
 */


#include <iostream>
#include <vle/devs/Dynamics.hpp>


namespace vle {
namespace discrete_time {
namespace test {


namespace vu = vle::utils;

class confluentNosync_output : public vle::devs::Dynamics
{
public:
    confluentNosync_output(const vle::devs::DynamicsInit& model,
        const vle::devs::InitEventList& events):
         vle::devs::Dynamics(model,events)
    {
        lastWakeUp = vle::devs::negativeInfinity;
    }
    ~confluentNosync_output()
    {
    }
    virtual vle::devs::Time init(vle::devs::Time /* time */) override
    {
        return vle::devs::infinity;
    }
    vle::devs::Time timeAdvance() const override
    {
        return vle::devs::infinity;
    }
    void externalTransition(
        const vle::devs::ExternalEventList& /* event */,
        vle::devs::Time time) override
    {
        if(time == lastWakeUp){
            throw vu::InternalError("two values at the same time");
        }
        lastWakeUp = time;
    }
    vle::devs::Time lastWakeUp;
};

DECLARE_DYNAMICS(confluentNosync_output)

}}}


