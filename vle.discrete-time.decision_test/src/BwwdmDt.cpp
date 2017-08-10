/**
  * @file wwdmDt.cpp
  * @author ...
  * ...
  */

/*
 * @@tagdynamic@@
 * @@tagdepends: vle.discrete-time @@endtagdepends
*/

#include <vle/DiscreteTime.hpp>
//includes
namespace vd = vle::devs;

namespace vv = vle::value;

namespace vle {
namespace discrete_time {
namespace wwdmDt {

class wwdmDtbis : public DiscreteTimeDyn
{
public:
wwdmDtbis(
    const vd::DynamicsInit& init,
    const vd::InitEventList& evts)
        : DiscreteTimeDyn(init, evts)
{
    SemRec.init(this, "SemRec", evts);
    Eb.init(this, "Eb", evts);
    Eimax.init(this, "Eimax", evts);
    K.init(this, "K", evts);
    Lmax.init(this, "Lmax", evts);
    A.init(this, "A", evts);
    B.init(this, "B", evts);
    TI.init(this, "TI", evts);
    Tmin.init(this, "Tmin", evts);
    Tmax.init(this, "Tmax", evts);
    RG.init(this, "RG", evts);
    ST.init(this, "ST", evts);
    LAI.init(this, "LAI", evts);
    U.init(this, "U", evts);
    Tr.init(this, "Tr", evts);
    Tmean.init(this, "Tmean", evts);
    PAR.init(this, "PAR", evts);
//constructor

Tr.init_value((1 / B()) * std::log(1 + std::exp(A() * TI())));

}

virtual ~wwdmDtbis()
{}

    void compute(const vle::devs::Time& /* t */)
{
PAR = 0.5 * 0.01 * RG();

Tmean = std::max(0.0, (Tmin() + Tmax()) / 2);

if (SemRec() == 0) {
ST = 0;
LAI = 0;
} else if (SemRec() == 1) {
ST = ST() + Tmean();
LAI = std::max(0.0, Lmax() * ((1 / (1 + std::exp(-A() * (ST() - TI())))) -
                                std::exp(B() * (ST() - Tr()))));
}

U = U(-1) + Eb() * Eimax() * (1 - std::exp(-K() * LAI())) * PAR();

}
    Var SemRec; 
    Var Eb;
    Var Eimax;
    Var K;
    Var Lmax;
    Var A;
    Var B;
    Var TI;
    Var Tmin;
    Var Tmax;
    Var RG;
    Var ST;
    Var LAI;
    Var U;
    Var Tr;
    Var Tmean;
    Var PAR;
//user
};

} // namespace wwdmDt
} // namespace discrete_time
} // namespace vle

DECLARE_DYNAMICS(vle::discrete_time::wwdmDt::wwdmDtbis)
