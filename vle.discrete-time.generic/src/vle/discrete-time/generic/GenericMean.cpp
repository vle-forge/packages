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


//@@tagdynamic@@
//@@tagdepends: vle.discrete-time @@endtagdepends


#include <vle/DiscreteTimeDbg.hpp>
#include <vle/devs/DynamicsDbg.hpp>

#include <iostream>




namespace vle {
namespace discrete_time {
namespace generic {

class GenericMean : public DiscreteTimeDyn
{
public:
    GenericMean(const vle::devs::DynamicsInit& init, const vle::devs::InitEventList& events)
        : DiscreteTimeDyn(init, events)
    {
        Mean.init(this, "Mean", events);
        vle::vpz::ConnectionList::const_iterator itb =
                getModel().getInputPortList().begin();
        vle::vpz::ConnectionList::const_iterator ite =
                getModel().getInputPortList().end();
        for (; itb != ite; itb++) {
            Var* v = new Var();
            v->init(this, itb->first, events);
            getOptions().syncs.insert(std::make_pair(itb->first, 1));
            inputs.push_back(v);
        }
    }

    virtual ~GenericMean()
    {
        std::vector<Var*>::iterator itb = inputs.begin();
        std::vector<Var*>::iterator ite = inputs.end();
        for (; itb!= ite; itb++) {
            delete *itb;
        }

    }

    void compute(const vle::devs::Time& /*t*/)
    {
        std::vector<Var*>::iterator itb = inputs.begin();
        std::vector<Var*>::iterator ite = inputs.end();
        double sum = 0;
        for (; itb!=ite; itb++) {
            sum += ((*itb)->operator()());
        }
        Mean = sum / (double) inputs.size();
    }

    Var Mean;
    std::vector<Var*> inputs;
};

}}}

DECLARE_DYNAMICS_DBG(vle::discrete_time::generic::GenericMean)

