/**
  * @file SIR.cpp
  * @author ...
  * ...
  */

/*
 * @@tagdynamic@@
 * @@tagdepends: vle.discrete-time @@endtagdepends
*/

#include <vle/DiscreteTime.hpp>

namespace vd = vle::devs;

namespace vv = vle::value;

namespace test_manager {

using namespace vle::discrete_time;

class SIR : public DiscreteTimeDyn
{
public:
SIR(
    const vd::DynamicsInit& init,
    const vd::InitEventList& evts)
        : DiscreteTimeDyn(init, evts)
{
    S.init(this, "S", evts);
    I.init(this, "I", evts);
    R.init(this, "R", evts);
    beta.init(this, "beta", evts);
    gamma.init(this, "gamma", evts);


}

virtual ~SIR()
{}

void compute(const vle::devs::Time& t)
{
S = S(-1)-beta()*S(-1)*I(-1);
I = I(-1)+beta()*S(-1)*I(-1) - gamma()*I(-1);
R = R(-1)+gamma()*I(-1);
}

    Var S;
    Var I;
    Var R;
    Var beta;
    Var gamma;

};

} // namespace test_manager

DECLARE_DYNAMICS(test_manager::SIR)

