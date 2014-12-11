
The package **vle.recursive** allow to performs recursive/embedded
simulations. Based on the vle API, one can launch simulations 
of a nested model into a vle Dynamics.

### Use case: performs a single simulation

Here is an example of code relying on this package

```
#include <vle/recursive/EmbeddedSimulatorSingle.hpp>
namespace vv = vle::value;
namespace vr = vle::recursive;
...
vv::Map init;                                //(1)
init.addString("package","vle.recursive");   //(2)
init.addString("vpz","ExBohachevsky.vpz");   //(3)
vv::Set& inputs = init.addSet("inputs");     //(4)
inputs.addString("cond/x1");                 //(5)
inputs.addString("cond/x2");                 //(6)
vv::Set& outputs = init.addSet("outputs");   //(7)
outputs.addString(
     "view/ExBohachevsky:ExBohachevsky.y");  //(8)
vr::EmbeddedSimulatorSingle sim;             //(9)
sim.init(init);                              //(10)
vv::Set point;                               //(11)
point.addDouble(3);                          //(12)
point.addDouble(-10);                        //(13)
vv::Double res;                              //(14)
sim.simulate(point);                         //(15)
sim.fillWithLastValuesOfOutputs(res);        //(16)
```                          

The different steps of the above code do the following :

```
(1) declare a Map that will be used to initialize the simulator 
(2,3) targets the nested model: here, the vpz 'ExBohachevsky.vpz'
  form the package 'vle.recursive'. 
(4,5,6) declare the inputs of the nested model: here, the inputs are
   'cond/x1' and 'cond/x2'
(7,8) declares the outputs of the nested model: here, only one output
  which is the port 'y' of the atomic model 'ExBohachevsky:ExBohachevsky'
  and that one can get into the view 'view'.
(9,10) build the embedde simulator and initalize with the map
(11, 12, 13) build a point to simulate: 
  here, inputs are cond/x1=3, cond/x2=-10, 
(14) declare the vle value that will be filled with outputs
(15) perform the simulation
(16) fills the value 'res' with the last value of the column
  'view/ExBohachevsky:ExBohachevsky.y' computed by simulation
```

### Use case: performs simulation of an experiment plan

To perform simulations of an experiment plan: you have to include 
_vle/recursive/EmbeddedSimulatorPlan.hpp_. In the initialization map
of the embedded simulator you can add an integer _nb-threads_ that gives
the number of threads to use. The simulation function is _simulatePlan_.
 
