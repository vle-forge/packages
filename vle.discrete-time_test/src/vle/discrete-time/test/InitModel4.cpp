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

#include <vle/devs/Executive.hpp>
#include <vle/discrete-time/TemporalValues.hpp>
#include <stack>



namespace vz = vle::vpz;

namespace vle {

namespace discrete_time {
namespace test {



class InitModel4 : public vle::devs::Executive
{

public:
    InitModel4(const vle::devs::ExecutiveInit& mdl,
                 const vle::devs::InitEventList& events) :
        vle::devs::Executive(mdl, events), a_0(0), b_0(0), c_0(0), d_0(0)
    {
        a_0 = events.getDouble("init_value_a");
        b_0 = events.getDouble("init_value_b");
        c_0 = events.getDouble("init_value_c");
    }

    virtual ~InitModel4()
    {
    }

    virtual vle::devs::Time init(vle::devs::Time /* time */) override
    {
        {//init d(0) = a(0) + c(0)
            std::cout << " dbg InitModel4:: init " << "\n";
            for (const auto& c : conditions().conditionnames()) {
                std::cout << " dbg InitModel4:: init c=" << c << "\n";
            }
            std::cout << " dbg InitModel4:: init2 " << "\n";
            vz::Condition& cond = conditions().get("cond_init_d");
            cond.clearValueOfPort("init_value_d");
            d_0 = a_0 + c_0;
            cond.setValueToPort("init_value_d",
                    std::unique_ptr<vle::value::Value>(
                         new vle::value::Double(d_0)));
        }
        {//init sum a + b + c + d;
            vz::Condition& cond = conditions().get("cond_init_sum_res");
            cond.clearValueOfPort("init_value_sum_res");
            cond.setValueToPort("init_value_sum_res",
                    std::unique_ptr<vle::value::Value>(
                         new vle::value::Double(a_0 + b_0 + c_0 + d_0)));
        }
        createModelFromClass("model4", "model4");
        return vle::devs::infinity;
    }

    double a_0;
    double b_0;
    double c_0;
    double d_0;

};

DECLARE_EXECUTIVE(InitModel4)

}}}


