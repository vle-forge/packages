
The vle packages **vle.discrete-time**, **vle.discrete-time.generic**
and **vle.discrete-time.decision** provide new extensions and generic models
for simulating discrete time models into VLE.

The first objective of this work is to improve the current extension 
**vle.extension.difference-equation** for modeling recurrent relations.
 The second objective is to go further by providing a base extension for all
discrete time atomic models.

Discrete time atomic models are models that compute at time _t_ outputs of
variables _v1, ..., vn_ taking into account values of variables _v1,..., vn_,
given a user specified parameter _D_, at times


_t, t - D, t - 2 * D,..._, 


There is the list of packages :
* **vle.discrete-time** provides the discrete time extension
 itself.
* **vle.discrete-time.generic** provides generic discrete
 time models (ie. with unkonwn input ports).
* **vle.discrete-time.decision** provides a decision agent having
 a discrete time dynamic. It embeds a _KnowledgeBase_ from package
 **vle.extension.decision**. This is an experimental package.
* **vle.discrete-time_test** provides tests for discrete time
models.

In package **vle.discrete-time** the API _vle/discrete-time/TemporalValues.hpp_
provides functionnalities to handle variables 
(double, vector or polymorphic _vle::value_) and their history. Particularly,
access operators are defined for these data structures. It could be
used in a more general framework than discrete time models.

### User Documentation

#### API _vle/discrete-time/TemporalValues.hpp_

This API provides functionnalities to handle variables and their history.
Particularly, access operators are defined for these data structures. It could
be used in a more general framework than discrete time models.
Available variables are :
* double. To use a temporal value of type double, you must use the struct 
  _vle::temporal_values::Var_ (see examples below).
* vector of double. To use a temporal value of type vector of double,
    you must use the struct _vle::temporal_values::Vect_.
* _vle::value::Value_: To use a temporal value of this type
 you must use the struct _vle::temporal_values::ValueVle_.

Temporal values maintains an historic of updates with dates and
are browseable using access operator. Updates can be directly set
by a _devs::Dynamic_ if the temporal value is declared 
(in constructor or not) using the function _Var::init_, 
_Vect::init_ or _ValueVle::init_ (see lines 13, 14 in code).

![MISSING FIG](http://www.vle-project.org/pub/1.3/docs/temporal_values.png)

**Fig:** Example of updates for a temporal value of type _Var_.

In example depicted in the above figure, at current time _t_, the state
of historic of _Var_ _S_ is _[t1, a1],[t2, a3],[t3, a2],[t4, a1]_.
Operator _()_ on _S_ will give one of the update value. The signal is 
supposed to be piecewise constant function. Then 
* _S(-0.3)_ returns value at time _t - 0.3 * D_, ie _a1_.
* _S(-3.6)_ returns value at time _t - 3.6 * D_, ie _a3_.
* _S()_ returns last update value, ie _a1_.

Operator _=_ of _Var_ allows to set an update
(for example into the _compute_ function of discrete-time model).

For Vectors (struct _Vect_):

* _S\[1\](-0.3)_ returns value at time _t - 0.3 * D_ for the
  dimension 1.
* _S\[0\](-3.6)_ returns value at time _t - 3.6 * D_
  for the dimension 0.
* _S\[2\]()_ returns last update vector at index 2.
* _S\[2\]=1.5_ updates value of _S_ at dim 2 with 1.5.


#### Atomic model interface

Evolution of state variables _vi_ is described by C++ equation syntax close to
mathematical formulation.

For example the expression _V1 = V1(-1) + V2() + V3(-2)_ computes, at current
time _t_, the value of _V1_ as the sum of the value of _V1_ at time 
_t - D_, the current value of _V2_ and the value of _V3_ at time 
_t - 2*D_. These expressions must be written in the
_compute_ function.


![MISSING FIG](http://www.vle-project.org/pub/1.3/docs/userInterfaceDiscreteTime.png)

**Fig:** A discrete-time model with 6 variables, 3 can be
updated from external event (inputs), 3 are sent by default at each time step 
on the network (outputs)

The interface (input and output ports) is given for an example in the above 
figure. Note that variables _v1, ..., v3_ are not necessary
inputs they could also be outputs. And variables _v4, ..., v6_ could also
have their input ports.


#### Writing recurrent relations atomic models


Below is given an example of dynamic for an atomic model that relies 
on the extension **vle.discrete-time**.

``` 
class MyModel : public DiscreteTimeDyn
{
public:
  //Declaration of variables
  Var x;
  Vect y;
  Var z; // used in this module as an input
  MyModel(const vd::DynamicsInit& model, 
          const vd::InitEventList& events) :
            DiscreteTimeDyn(model, events)
  {
    //Initialisation of variables from experimental conditions
    x.init(this, "x", events);
    y.init(this, "y", events);
    //Overwrite initialisations (Optionnal)
    x.init_value(3.0);
    x.history_size(3);
    y.dim(2)
  }
  void compute(const vd::Time& /*time*/)
  {
    x =  x(-1) - y[1]() / z();
    y[0] = z() - 1;
    y[1] = y[0]() + 1;
  }
};
```

#### Configuring atomic models into _vpz_ conditions

To configure discrete time atomic models, one can use the parameters listed
below. The *X* refers to an internal variable (a real, a vector or a vle value).

* **time_step** (double, default 1.0) : the time step of the discrete time
  atomic model.
* **init_value_X** (vle::Value, default vle::Double(0.0)) :
  the initial value of the internal variable *X*. It also contains
  the historic values if **history_size_X** > 0 using e.g. vv::Set.
* **dim_X** (int, default 2) : if *X* is a vector, it defines
  the dimension of the vector.
* **history_size_X** (uint, default 1) : it gives the size of the
  history of internal variable *X*.
* **syncs** (set of strings, default empty): each variable into this set are
  parameterized with a value of 1 for sync parameter.
* **sync_X** (uint, default 0): if *sync_X* > 0, the value of
  *X* at times n * *sync_X* * *time_step*, with n > 0 is
  expected to be provided by an external event before calling the *compute*
  function. This option has priority on *syncs*.
* **output_nil** (bool, default false): if true, the output function will
  produce a Null value for a variable which last update is not equals to
  current time, otherwise it will gives the last updated value.
* **output_nil_X** (bool, default false): a specific value of *output_nil*
  for variable *X*. This option has priority on *output_nil*.
* **output_period** (uint, default 1): gives the time step of output.
  Output will produce values each *time_step* * *output_period*.
* **output_period_X** (uint, default 1): a specific value of *output_period*
  for variable *X*. This option has priority on *output_period*.
* **allow_update_X** (bool, default false): if false, the first
  value set for *X* at a given time step is kept. The following updates for
  *X* at this time step are ignored.
* **error_no_sync_X** (bool, default false) : if true, the
  access to *X* at the current time _X()_ will send an error if the
  last time of update of *X* is before the current time.
* **bags_to_eat** (int, default 0) : the number of bags to wait before
  computing the values of variables (calls of _compute_ user function).
* **snapshot_before** (bool, default false) : if true, a snapshot of variable
  values is done before the compute function. It can be observed on the
  port *X_before*.
* **snapshot_after** (bool, default false) : if true, a snapshot of variable
  values is done after the compute function. It can be observed on the
  port *X_before*.
* **forcing_X** (a vle Map or Set, default empty): this option can not be used
 simultaneously with *allow_update*. The map (or set of such map)
 represents a forcing event. A forcing event forces the model to set a value
 to *X* at a given time. The map should contain:
  * **time** (double): the time of forcing event
  * **value** (real, vector or vle Value): the value of forcing event
  * **before_output** (optionnal, default false): true if the forcing event
    should occur before the output function of the dynamic.

### Technical details


#### Activity diagram of a discrete time dynamic 

![MISSING FIG](http://www.vle-project.org/pub/1.3/docs/dt_activity_diagram.png)

**Fig:** The activity diagram of a discrete time model: sequence of calls at 
a given time step.

#### Global architecture

![MISSING FIG](http://www.vle-project.org/pub/1.3/docs/DEVS_states.png)

**Fig:** The DEVS state transition graph  for discrete-time atomic models.

#### Link to **vle.extension.difference-equation**

This package is intended to improve the functionnalities of (package
**vle.extension.difference-equation**) and to limit the behavior 
in order to facilitate the coupling with other extensions :

* there is no propagation of the perturbation : 
  it requires a state backup and other extensions do not have this.
* there is no initialization process of the dynamics (no _initValue_
  function): synchronisation at the time of instantiation of the atomic model
  makes it difficult to couple with DSDEVS. 
* there is no external variables. All variables are internal variables. 
* it provides vector of variables and polymorphic vle values.
* It uses a DEVS state approah for the code of transitions

Time performances comparison based on 2CV model (from Record project) are given
in the followinfg table. Note that the 2CV-parcelle model
can no be directly translated into discrete-time models since
it uses propagation of perturbation. In order to compare simulation times, 
There is a new version of 2CV-parcelle (2CVdt/2CV-parcelle-de-fusion) where 
CropTT_TT and CropStade have been merged (propagation of 
perturbation is on CropTT_TT).  

model                        |  comment                     |  nb simu | time (sec) with obs | time (sec) without obs 
-----------------------------|------------------------------|----------|---------------------|------------------------
2CV/2CV-parcelle             | original version             |  200     | 25                  | 12 
2CVdt/2CV-parcelle-de-fusion | merging CropTT and CropStade |  200     | 24                  | 12 
2CVdt/2CV-parcelle-dt        | discrete-time version        |  200     | 20                  | 7  

**TAB:** Comparison of time executions given in seconds.


#### Source code

This work has been conducted for vle 1.3.

For vle extensions:

```
git clone https://github.com/vle-forge/packages.git
```

For 2CV models (discrete time version and original version):

```
git clone http://mulcyber.toulouse.inra.fr/anonscm/git/recordb/recordb.git
git checkout -b using1.3 origin/using1.3
```

