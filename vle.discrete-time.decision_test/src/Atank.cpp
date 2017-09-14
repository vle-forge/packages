/**
  * @file tamkDt.cpp
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
namespace tankDt {

class tankDt : public DiscreteTimeDyn
{
public:

tankDt(const vd::DynamicsInit& init,
       const vd::InitEventList& evts)
    : DiscreteTimeDyn(init, evts) {

    inletIN.init(this, "inletIN", evts);
    inletVAR.init(this, "inletVAR", evts);
    outletIN.init(this, "outletIN", evts);
    storageVAR.init(this, "storageVAR", evts);

}

virtual ~tankDt(){
}

void compute(const vle::devs::Time& /* t */) {
    if (inletIN() != 0) {
        inletVAR = inletIN();
        inletIN = 0;
    } else {
        inletVAR = 0;
    }

    storageVAR = storageVAR() + inletVAR();

    if (outletIN() != 0) {
        storageVAR = storageVAR() - outletIN();
        outletIN = 0;
    }
}

    Var inletIN;
    Var inletVAR;
    Var outletIN;
    Var storageVAR;
};

} // namespace tankDt
} // namespace discrete_time
} // namespace vle

DECLARE_DYNAMICS(vle::discrete_time::tankDt::tankDt)
