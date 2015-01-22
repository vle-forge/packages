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
#include <vle/devs/DynamicsDbg.hpp>
#include <iostream>




namespace vle {
namespace discrete_time {
namespace test {



class vectorAndValues_x : public DiscreteTimeDyn
{

public:
    vectorAndValues_x(const vle::devs::DynamicsInit& model,
       const vle::devs::InitEventList& events) :
           DiscreteTimeDyn(model, events)
    {
        x.init(this, "x", events);
        x_value.init(this, "x_value", events);
        x.dim(3);
        x_value.history_size(3);
    }

    ~vectorAndValues_x()
    {
    }


    void compute(const vle::devs::Time& time)
    {
        x[0] =  x[0](-1) + 0.8;
        x[1] =  x[0](-1) + 0.5;
        x[2] =  x[1]() + time;

        x_value().toDouble().set(x_value(-1).toDouble().value()-0.5);
    }

    Vect x;
    ValueVle x_value;
};

DECLARE_DYNAMICS_DBG(vectorAndValues_x)

}}}


