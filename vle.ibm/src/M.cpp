/**
  * @file M.cpp
  * @author ...
  * ...
  *@@tagdynamic@@
  *@@tagdepends:vle.extension.differential-equation@@endtagdepends
  * @@tag Forrester@vle.forrester @@namespace:vle.ibm;class:M;400|400;c:C3,120,288,90,25|C1,109,104,90,25|DSAT,279,355,90,25|C2,119,199,90,25|;f:f2,271,256,50,20,DSAT * C2,false,,,|f1,267,165,50,20,C1 * p1,false,,,|;fa:C1,f1,5,2|f1,C2,3,5|C2,f2,5,2|f2,C3,3,5|;p:p1,342,73,30,16|;pa:C1,f1,5,0,245,134|p1,f1,0,0,317,116|C2,f2,5,0,252,227|DSAT,f2,0,6,299,315|;ev:;v:;@@end tag@@
  */

#include <vle/extension/differential-equation/DifferentialEquation.hpp>

//@@begin:include@@

//@@end:include@@
namespace vd = vle::devs;
namespace ve = vle::extension::differential_equation;
namespace vv = vle::value;

namespace vleextensionibm {

class M : public ve::DifferentialEquation
{
public:
    M(
       const vd::DynamicsInit& init,
       const vd::InitEventList& events)
    : ve::DifferentialEquation(init, events)
    {
        C3 = createVar("C3");
        C1 = createVar("C1");
        DSAT = createVar("DSAT");
        C2 = createVar("C2");


        if (events.exist("p1"))
            p1 = events.getDouble("p1");
        else
            throw vle::utils::ModellingError("Parameter p1 not found");

    }

    virtual void compute(const vd::Time& t)
    {
        f2 = DSAT() * C2();
        f1 = C1() * p1;
        grad(C3) = (f2);
        grad(C1) = 0 - (f1);
        grad(DSAT) = 0;
        grad(C2) = (f1) - (f2);
    }

    virtual ~M()
    { }

private:
    Var C3;
    Var C1;
    Var DSAT;
    Var C2;

    double p1;
    double f2;
    double f1;
virtual vv::Value* observation(
    const vd::ObservationEvent& event) const
{
   const std::string& port = event.getPortName();
    if (port == "f2" ) return new vv::Double(f2);
    if (port == "f1" ) return new vv::Double(f1);
   return ve::DifferentialEquation::observation(event);
}
};

} // namespace vle.ibm

DECLARE_DYNAMICS(vleextensionibm::M)

