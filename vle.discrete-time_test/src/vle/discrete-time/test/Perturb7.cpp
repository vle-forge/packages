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
 */

#include <iostream>
#include <vle/devs/Dynamics.hpp>

namespace vle {
namespace discrete_time {
namespace test {

class Perturb7 : public devs::Dynamics
{
public:
    Perturb7(const devs::DynamicsInit& model,
        const devs::InitEventList& events):
         devs::Dynamics(model,events), a(0),
         last_wake_up_time(0), next_output_time(0)
    {
    }

    ~Perturb7()
    {
    }

    virtual devs::Time init(devs::Time time) override
    {
        internalTransition(time);
        return timeAdvance();
    }

    devs::Time timeAdvance() const override
    {
        return next_output_time - last_wake_up_time;
    }

    void internalTransition(devs::Time time)
    {
        last_wake_up_time = time;
        next_output_time = time+5.0;
    }

    void externalTransition(const devs::ExternalEventList& event,
                                    devs::Time time) override
    {
        last_wake_up_time = time;
        a = event.begin()->attributes()->toDouble().value();
    }


    void output(vle::devs::Time /*time*/,
        vle::devs::ExternalEventList& output) const override
    {
        output.emplace_back("a");
        output.back().addDouble(a+3);
    }
    double a;
    double last_wake_up_time;
    double next_output_time;

};

}}} // namespaces

DECLARE_DYNAMICS(vle::discrete_time::test::Perturb7)
