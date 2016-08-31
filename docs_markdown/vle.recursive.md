
The package **vle.recursive** allows to perform recursive/embedded
simulations. Based on the vle API, one can launch simulations 
of a nested model into a vle Dynamics.
It provides:
* An API MetaManager for performing multi simulations
* A dynamic based on this API that can be configured by experimental conditions

The goals are:
* to provide both an API and a PDEVS dynamic for multi simulation, based on the
same approach for configuration.
* to use the same approach for configuring MPI and thread parallelization 
provided into vle. Use either the vle implementation of vle (if available)
or native threading parallelization. 
* to facilitate the stochastic simulation.
* to provide simple tools for aggregating simulation results.
* to provide an alternative to multi simulation with R, with less memory
allocation (see tests at the end).

### Tutorial on vle.recursive package

Let us consider the SIR model as described on the 
[Wikipedia page](https://en.wikipedia.org/wiki/Compartmental_models_in_epidemiology):

```
dS/dt = - beta * I * S;
dI/dt = beta * I * S - gamma * I;
dR/dt = gamma * I;
```
One also defines a simple stochastic version (called SIRnoise):

```
dS/dt = - beta * I * S * espilon1;
dI/dt = beta * I * S * epislon1  - gamma * I * epsilon2;
dR/dt = gamma * I * epsilon2;
epsilon1 ~ N(1, 0.3)
epsilon2 ~ N(1, 0.3)
```
The default parameters of the models are:
S(0) = 99; I(0)=1; R(0)=0; beta=0.002; gamma=0.1. For the stochastic version,
we also have a parameter 'seed' that gives the seed of the random number 
generator. Below, the result of the deterministic version with default
paramters, for 60 integration time steps, are plotted into gvle.

![MISSING FIG](http://www.vle-project.org/pub/1.3/docs/vle_recursive_SIR.png)

 * Performing multiple simulations: we consider 5 different values of S(0) and
 for each, we compute the mean value of the finals state of S 
 for 6 replicates with different 'seed' values.


```
namespace vr = vle::recursive;
namespace vv = vle::value;
vv::Map init;
init.addString("package","vle.recursive_test");
init.addString("vpz","SIRnoise.vpz");

//config output, 'mean' on replicate, 'all' on inputs
vv::Map& conf_out = init.addMap("output_Sfinal");
conf_out.addString("path", "view/top:SIRnoise.S");
conf_out.addString("integration", "last");
conf_out.addString("aggregation_replicate", "mean");
conf_out.addString("aggregation_input", "all");

//set 5 input values for S(0)
vv::Tuple& Svalues = init.addTuple(
"input_condSIRnoise.init_value_S", 5, 0.0);
Svalues[0] = 150;
Svalues[1] = 120;
Svalues[2] = 99;
Svalues[3] = 75;
Svalues[4] = 50;

//set 6 seeds for initializing the rng.
vv::Tuple& seeds = init.addTuple(
"replicate_condSIRnoise.init_value_seed", 6, 0.0);
seeds[0] = 45694;
seeds[1] = 55695;
seeds[2] = 65696;
seeds[3] = 85698;
seeds[4] = 95699;

vr::MetaManager meta;
vle::manager::Error err;
std::unique_ptr<vv::Map> res = meta.run(init, err);

if (err.code ==-1) {
    std::cout << " error: " << err.message << "\n";
}
std::cout << " Results of final S: " << *res << "\n";
//(Sfinal, ((11.5526,22.2598,36.1712,51.3126,45.0968)))
```

* Evaluating the model on observations: let us consider we have at our disposal
three observations of the number of infectious subjects, at times 20, 30 and 40.
And we want to compute the mean square error of the models for different values
of beta.

```
vv::Map init;
init.addString("package","vle.recursive_test");
init.addString("vpz","SIR.vpz");

//config output, mse on S
vv::Map& conf_out = init.addMap("output_mseI");
conf_out.addString("path", "view/top:SIR.I");
conf_out.addString("integration", "mse");
vv::Tuple& mseTimes = conf_out.addTuple("mse_times", 3,0.0);
mseTimes[0] = 20;
mseTimes[1] = 30;
mseTimes[2] = 40;
vv::Tuple& mseObs = conf_out.addTuple("mse_observations", 3,0.0);
mseObs[0] = 6;
mseObs[1] = 10;
mseObs[2] = 15;
conf_out.addString("aggregation_input", "all");

//set 3 input values for beta parameter
vv::Tuple& beta = init.addTuple("input_condSIR.init_value_beta", 3, 0.0);
beta[0] = 0.001;
beta[1] = 0.002;
beta[2] = 0.003;

vr::MetaManager meta;
vle::manager::Error err;
std::unique_ptr<vv::Map> res = meta.run(init, err);
if (err.code ==-1) {
    std::cout << " error: " << err.message << "\n";
}
std::cout << " Mse on I for beta in (0.001,0.002,0.003): " << *res << "\n";
//(mseI, ((102.158,0.233604,228.278)))
```

(code of the tutorial into vle.recursive_test)

### The MetaManager API.

The MetaManager is configured by a *vle::value::Map* provided by the user.
It can contains:

* **vpz** (string) : it gives the name of the *vpz* file. It allows to
 identify together with *package* the nested model.
* **package** (string) : it gives the name of the *package* where is located
 the nested model. It allows to identify together with *vpz*
 the nested model.
* **propagate_X** (*value::Value*, optional) : where X is of the 
 form *condname.portname*, it specifies the value to set for all simulations
 to a port condition of the embedded simulator. 
* **input_X** (*value::Value*): where X is of the form *condname.portname*,
  it gives for one input the values to simulate. *value::Tuple* 
  and *value::Set* are interpreted as multiple inputs.
* **replicate_X** (*vle::value::Set* or *vle:value::Tuple*, optional) where X
 is of the form *condname.portname*, it gives the values to simulate for the
 replicates (eg. seeds).
* **output_Y** (vle::value::Map) : where Y is an id for the output. 
 The map should/could provide:
    * **path** (string): is of the form *viewname/pathOfTheAtomicModel.ObsPort*.
  It identifies the column of outputs *ObsPort* computed by the atomic
  model *pathOfTheAtomicModel* and saved into the view *viewname*. 
    * **integration** (amongst "last", "max", "mse" or "all", default "all"):
  the type of temporal integration to perform.
  the output with id *X*. The string has the following form:
    * **mse_observations** (*vle::value::Tuple*): required only if
  integration="mse". It gives the series of observations for computing the MSE
    * **mse_times** (*vle::value::Tuple*): required only if integration="mse".
  It gives times at which the mse_observations are given (mse_observatiosn and
  mse_times must have the same length.
    * **aggregation_replicate** (amongst "mean", default "mean"):
  It gives the type of aggregation to perform on the simulations replicates.
    * **aggregation_input** (amongst "mean", "quantile", "max", "all"
  default "all"): It gives the type of aggregation to perform on the
  simulations inputs (once replicates have been aggregated).
* **config_parallel_type** (string amongst *threads*, *mvle* and *single*;
 default *single*). It sets the type of parallelization to perform.
* **config_parallel_rm_files** (bool; default *true*). Used only 
 if *config_parallel_type* is set to *mvle*. Simulation files created into
 directory *working_dir* are removed after analysis.
* **config_parallel_nb_slots** (int > 0). it gives the number of slots to
 use for parallelization.
* **working_dir** (string). Required only if *config_parallel_type* is set 
 to *mvle*. It gives the working directory where are produced result file of single
 simulations.


### The MetaManager dynamic

 The MetaManager atomic model has the following dynamic, implemented in PDEVS
 framework:
 * If the conditions provide an initialization map for the MetaManager API,
 it performs the simulation of the experimental plan during the PDEVS 
 initialization step, then it falls asleep for a given duration, then it outputs 
 the results, and then falls asleep indefinitely.
 * Each time it receives an experiment plan setting on its input port, 
 it performs the simulation of the experiment plan, then it falls asleep for a
 duration, then it outputs the results and then falls asleep indefinitely. 
 the results.


Configuring the dynamic consists in giving the elements required for the API 
 configuration directly into conditions ports. Other configuration keys are:
 * **step_duration** (double, default 0). It gives the duration time, for 
 the embedding simulator engine clock, required for the simulation of the
 experiment plan. 
 
### Memory allocation performance tests

First results show that we can expect 50 times less allocated memory using
the MetaManager Dynamic embedded into a vpz (depending on the experiment). 
Much of output simulation data is not converted into R structures.

Script R directly based on the Rvle API for multi simulation:

```
set.seed(12369)
n = 500;
x1 = runif(n,0.0,2.0);
x2 = runif(n,0.0,2.0);

library(rvle)
g = new("Rvle", pkg="vle.recursive_test", file ="ExBohachevsky.vpz")
g = setDefault(g,
 outputplugin=c(view="vle.output/storage"),
 plan = "linear",
 cond.x1 = x1,
 cond.x2 = x2
)
system.time(gres <- results(run(g)))
## user time=0.608, CPU time=0.028, real elapsed time=0.635
print(object.size(gres))
## 648200 bytes
```

Script R based on the vle.recursive R API:

```
set.seed(12369)
n = 500;
x1 = runif(n,0.0,2.0);
x2 = runif(n,0.0,2.0);

library(rvle)
source(paste(Sys.getenv("VLE_HOME"), 
       "/pkgs-2.0/vle.recursive/R/vle.recursive.R", sep=""))
vle.recursive.init(pkg="vle.recursive_test", file="ExBohachevsky.vpz");
vle.recursive.configInput(input="cond.x1", values=x1);
vle.recursive.configInput(input="cond.x2", values=x2);
vle.recursive.configOutput(id="y", 
  path="view/ExBohachevsky:ExBohachevsky.y", integration="last")
vle.recursive.configOutput(id="ynoise", 
  path="view/ExBohachevsky:ExBohachevsky.y_noise", integration="last")

system.time(fres <- vle.recursive.simulate())
## user time=0.596, CPU time=0.008, real elapsed time=0.605
print(object.size(fres))
## 8952 bytes
```



 
 
 
 
 
