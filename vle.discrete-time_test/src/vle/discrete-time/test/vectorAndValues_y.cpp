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

namespace vd = vle::devs;
namespace vv = vle::value;

namespace vle {
namespace discrete_time {
namespace test {



class vectorAndValues_y : public DiscreteTimeDyn
{

public:
    vectorAndValues_y(const vd::DynamicsInit& model,
       const vd::InitEventList& events) :
           DiscreteTimeDyn(model, events)
    {
        x.init(this, "x", events);
        x.dim(3);
        x_value.init(this, "x_value", events);
        y.init(this, "y",events);
        y_value.init(this, "y_value", events);
        res.init(this, "res", events);
    }

    ~vectorAndValues_y()
    {
    }

    void compute(const vd::Time& /*time*/)
    {
        for (unsigned int i =0; i<dim(y); i++ ) {
            y[i] = x[0]() - x[1]() + x[2]() + i;
        }
        y_value = x_value(-1);
        y_value().toDouble().set(y_value().toDouble().value() + 1.0);
        res = y_value().toDouble().value() + y[dim(y)-1]();
    }

    Vect x;
    ValueVle x_value;
    Vect y;
    ValueVle y_value;
    Var res;
};

DECLARE_DYNAMICS_DBG(vectorAndValues_y)

}}}


