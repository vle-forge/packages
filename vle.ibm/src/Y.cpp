/**
  * @file Y.cpp
  * @author ...
  * ...
  *@@tagdynamic@@
  *@@tagdepends:vle.extension.differential-equation@@endtagdepends
  * @@tag Forrester@vle.forrester @@namespace:IbminsideGVLE;class:Y;421|400;c:X,57,147,90,25|maturing,321,214,90,25|masse,59,220,90,25|;f:Growth,214,174,56,20,KG * X * masse,false,,,|Senescence,210,271,79,20,masse * KS *  ( 1 - maturing ) ,true,maturing == 0,masse * KS,0|;fa:X,Growth,5,2|Growth,masse,3,5|masse,Senescence,3,0|;p:KS,334,326,30,16|KG,355,87,30,16|;pa:KS,Senescence,3,5,304,308|X,Growth,5,0,194,160|masse,Senescence,3,2,168,256|maturing,Senescence,4,4,305,249|masse,Growth,1,2,170,197|KG,Growth,3,0,291,123|;ev:;v:;@@end tag@@
  */

#include <vle/extension/differential-equation/DifferentialEquation.hpp>

//@@begin:include@@

//@@end:include@@
namespace vd = vle::devs;
namespace ve = vle::extension::differential_equation;
namespace vv = vle::value;

namespace IbminsideGVLE {

class Y : public ve::DifferentialEquation
{
public:
    Y(
       const vd::DynamicsInit& init,
       const vd::InitEventList& events)
    : ve::DifferentialEquation(init, events)
    {
        X = createVar("X");
        maturing = createVar("maturing");
        masse = createVar("masse");


        if (events.exist("KS"))
            KS = events.getDouble("KS");
        else
            throw vle::utils::ModellingError("Parameter KS not found");

        if (events.exist("KG"))
            KG = events.getDouble("KG");
        else
            throw vle::utils::ModellingError("Parameter KG not found");

    }

    virtual void compute(const vd::Time& t)
    {
        Senescence = (maturing() == 0)? masse() * KS : 0;
        Growth = KG * X() * masse();
        grad(X) = 0 - (Growth);
        grad(maturing) = 0;
        grad(masse) = (Growth) - (Senescence);
    }

    virtual ~Y()
    { }

private:
    Var X;
    Var maturing;
    Var masse;

    double KS;
    double KG;
    double Growth;
    double Senescence;
virtual vv::Value* observation(
    const vd::ObservationEvent& event) const
{
   const std::string& port = event.getPortName();
    if (port == "Growth" ) return new vv::Double(Growth);
    if (port == "Senescence" ) return new vv::Double(Senescence);
   return ve::DifferentialEquation::observation(event);
}
};

} // namespace IbminsideGVLE

DECLARE_DYNAMICS(IbminsideGVLE::Y)

