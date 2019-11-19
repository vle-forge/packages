/**
  * @file SIRnoise.cpp
  * @author ...
  * ...
  */

/*
 * @@tagdynamic@@
 * @@tagdepends: vle.discrete-time @@endtagdepends
*/

#include <vle/DiscreteTime.hpp>
#include <vle/utils/Rand.hpp>
namespace vd = vle::devs;

namespace vv = vle::value;

namespace test_manager {

using namespace vle::discrete_time;

class SIRnoise : public DiscreteTimeDyn
{
public:
SIRnoise(
    const vd::DynamicsInit& init,
    const vd::InitEventList& evts)
        : DiscreteTimeDyn(init, evts)
{
    S.init(this, "S", evts);
    I.init(this, "I", evts);
    R.init(this, "R", evts);
    beta.init(this, "beta", evts);
    gamma.init(this, "gamma", evts);
    seed.init(this, "seed", evts);
mrand.seed((int) evts.getDouble("init_value_seed"));

}

virtual ~SIRnoise()
{}

void compute(const vle::devs::Time& t)
{

double epsilon = std::max(0.1,mrand.normal(1.0,0.3));
double epsilon2 = std::max(0.1,mrand.normal(1.0,0.3));
S = S(-1)-beta()*epsilon*S(-1)*I(-1);
I = I(-1)+beta()*epsilon*S(-1)*I(-1) - gamma()*epsilon2*I(-1);
R = R(-1)+gamma()*epsilon2*I(-1);
}

    Var S;
    Var I;
    Var R;
    Var beta;
    Var gamma;
    Var seed;
vle::utils::Rand mrand;
};

} // namespace test_manager

DECLARE_DYNAMICS(test_manager::SIRnoise)

