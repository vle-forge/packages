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

namespace vle {
namespace discrete_time {
namespace vle_recursive_test {

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

} // namespace vle_recursive_test
} // namespace discrete_time
} // namespace vle

DECLARE_DYNAMICS(vle::discrete_time::vle_recursive_test::SIR)

