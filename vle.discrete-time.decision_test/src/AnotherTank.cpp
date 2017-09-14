/**
  * @file anotherTankDt.cpp
  * @author ...
  * ...
  */

/*
 * @@tagdynamic@@
 * @@tagdepends: vle.discrete-time @@endtagdepends
*/

#include <vle/DiscreteTime.hpp>
#include <iostream>
//includes
namespace vd = vle::devs;

namespace vv = vle::value;

namespace vle {
namespace discrete_time {
namespace anotherTankDt {

class anotherTankDt : public DiscreteTimeDyn
{
public:

anotherTankDt(const vd::DynamicsInit& init,
       const vd::InitEventList& evts)
    : DiscreteTimeDyn(init, evts) {

    inletIN.init(this, "inletIN", evts);
    outletIN.init(this, "outletIN", evts);
    storageVAR.init(this, "storageVAR", evts);

}

virtual ~anotherTankDt(){
}

void compute(const vle::devs::Time& /* t */) {

    storageVAR = storageVAR() + inletIN();
    storageVAR = storageVAR() - outletIN();
}

    Var inletIN;
    Var outletIN;
    Var storageVAR;
};

} // namespace anotherTankDt
} // namespace discrete_time
} // namespace vle

DECLARE_DYNAMICS(vle::discrete_time::anotherTankDt::anotherTankDt)
