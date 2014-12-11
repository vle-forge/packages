
The vle packages **vle.extension.differential-equation** 
provides an extension to simulate ordinary differential equations into VLE.

### User Documentation

#### Atomic model interface


![MISSING FIG](http://www.vle-project.org/pub/1.3/docs/userInterfaceDifferential.png)

**Fig:** The user interface of an atomic model.

Evolution of state variables _Vi_ is described by differential equation. 
These expressions can rely on the value of external variables _Ei_.
For time sclicing methods, external variables _Ei_  are expected to be
piecewise constant functions computed from continuous and derivable functions. 
For QSS2, external variables are expected to be
piecewise linear functions.

Atomic model ports _Ei_ and _Vi_ can carry data at time _t_ that contain:
* 'name': the name of the external variable (or state variable)
* 'value': this is the value at $t$ of the variable 'name'.
* 'gradient' (optionnal): this is the value of the gradient of
  the variable 'name' at $t$.
* 'discontinuities' (the structure proposed in
  the next section section): this should not be handled by the user.

No other assumption, than continuity and derivability of the producing
function, is made regarding external variables updates. Particularly, updates of
_Ei_ variables can occur at any time.
   
Port 'perturb' can carry instant data (with no assumption on the producing
function) that is used to reset some state variables values. Reinitialisation 
of the state variables and their gradients is achieved by a function $reinit$
that the user can override. By default, the pertubation is interpreted as a  
reset of one state variable, and data must contain:

* 'name': the name of the state variable targeted by the perturbation.
* 'value': the new value at of the state variable 'name'.

Perturbations and discontinuities are handled by the formalism and do not
require special care of the user (see next section) unless
perturbation is required to behave differently, in which case the function
_reinit_ should be proposed by the user.

#### Writing differential equations atomic models


Below is given an example of dynamic for an atomic model that relies 
on the **vle.extension.differential-equation**

```
class MyModel : public DifferentialEquation
{
public:
 MyModel(const DynamicsInit& model,
	     const InitEventList& events) :
    DifferentialEquation(model,events)
 {
 	//Initialisation of variables is done 
 	//into the class constructor:
	v = createVar("v");
	e = createExt("e");
 }
 
 //gradients of state variables are expressed 
 //into the 'compute' function 
 void compute(const Time& time)
 {
    	grad(v) = v() - e();
 }
 //Reinitialisation of state variables after a perturbation or 
 //a discontinuity is performed into the 'reinit' function
 //(optionnal)
 void reinit(bool isPerturb, const Value& message) 
 {
 }
 //states and external variables are attributes of the class
 Var v;
 Ext e;
};
```

#### Configuring atomics models into _vpz_ conditions

The common structure of the conditions for configuring atomic models is the
following:
* 'variables': a map that gives initialisation values of the
  state variables.
* 'method': the name of the numerical integration method to use for
  simulation ('rk4','euler' or 'qss2').
* 'method-parameters': a map that contains parameters of the specified
  method (see above).

Three numerical integration methods are provided Runge Kutta 4 (rk4) Euler
(euler) and QSS2 (qss2). These are the parameters specific for each method.

 For rk4 and euler:
* 'timestep' is a double value that gives the time step for
    integration.
* 'synchronisation' (optionnal) is a boolean. If true, integration steps
    are synchronized with external variables updates.
* 'output_period' (integer, default=1). It gives the period at which
    output of variable values are performed. They are output every
    _output_period * timestep_ time unit.

For qss2: 
* 'DeltaQ' is a map that gives for each state variable the 
  quantum for integration.
* 'expect-gradients' (optionnal) is a boolean. If true, gradients are
  required for initialisation of state variables and external variables updates. 



### Background theory

Three methods are provided for numerical integration: Euler, Runge Kutta 4 and
QSS2 \cite{kofman.SIM02}. Euler and Runge Kutta 4 are well known explicit and
forward methods based on the discretization of the time. They are said to be 
time-slicing methods (TSM). QSS2 is also a forward and explicit method but it is
based on the discretization of variables, and developped especially for DEVS
models. For QSS2 implementation, the most general case is considered; 
ie. gradient expressions are non linear
functions depending on all state variables and thus :

* gradient derivatives are computed numerically and not analytically as
 proposed for linear gradients.
* quantization of variables involves an update of all variable gradients. 

Here, we focus on the strategies for handling
discontinuities into the systems and propose one strategy for VLE. These
discontinuities are the result of coupling discrete systems whith continuous
ones.

#### State of the art: time events, state events

Hybrid systems are defined as systems exhibiting both
continuous and discrete behaviors \cite{barros.ACM03}.
For numerical integration methods developper, 
these systems present the difficulty of handling discontinuities.
    
For Cellier and Kofman \cite{cellier.06}, discontinuities are 
discrete events and there exists two type of discontinuities :

* state events. E.g. a ball that bounces the soil
   has a continuous behavior except at the time of contact 
   with the soil, at which the direction is changed. Equations 
   of the ball can be written with _switching function_ 
   like in Dymola modeling environment, such as
   _y' = if  y < 0 then -1 else +1_
* time events. E.g. a control agent that takes decisions 
   on the conduct of a continuous system. 

One of the principal concern for handling state events is to detect them.
This can be achieved by using for example step-size control
\cite{esposito.ACM2007} and zero-crossing methods \cite{mao.CMA02}.
The family of QSSn methods \cite{cellier.SSIM08}
are well suited for handling state events since they 
rely on a discretization of state variable rather than discretization of time.
 
HFSS \cite{barros.ACM03} and DEV\&DESS \cite{zeigler.IMS06}
are formalisms that allow the simulation of hybrid systems.
DEV\&DESS rely on quantization methods and HFSS do not make assumptions 
on the integration method. It is not clear nevertheless how
 discontinuities are handled into these frameworks.  
 
For time events handling, no dedicated integration methods have been proposed,
only advises are given, as in \cite{cellier.06}.
If time events are scheduled (known in advance), step-size control can be
achieved in order to integrate over these events. In any case, the consequence
of a time event should be the beginning of a completely new integration,
that necesseraly starts with a re-initialisation of the system of differential
equations.

#### Proposed strategy for time events management

The strategy proposed for handling perturbations consists in reinitialising the
system of differential equation when a perturbation occurs as suggested
in \cite{cellier.06}. On perturbation, a propagation of the discontinuity is
performed into the system.

Following figure is an example of the consequence of a
discontinuities into a system of differential equations. _S_ is a state variable
which gradient expression depends on variable _E_. At _tp_, a perturbation on
state variable _S_ occurs and a discontinuity on _S_ is provoked. At _td_,
a discontinuity on variables _E_ occurs and provokes a discontinuity on the
gradient value of _S_.

![MISSING FIG](http://www.vle-project.org/pub/1.3/docs/perturbMgmt.png)

**Fig:** Perturbations management strategy

On this example, the pertubation handling strategy is the following.
At time _t2_, there is no perturbation yet and the numerical
integration processes as usual. 
Gradients computed at _t1_ are used to compute the new value of _S_ and
a time step _D = t3 - t2_ is provided for the duration of 
integration step. Into DEVS, this consists in calculating the time advance
function as _sigma = D_.

At time _tp_ in \[t2, t3\], a perturbation occurs on the model and the
following steps are performed:
* Update _S_ values at _tp_  using a time step of value _tp - t2_.
* Apply perturbation: reset the value of _S_ and his gradient.
* Propagate a new discontinuity to variables that depends on _S_ in
  order to reinitialise them at _tp_.
* Compute the new sigma (in the figure, _sigma = t4 - tp_)

At time _td_ in \[t4, t5\], similar steps are performed, unless that
the discontinuity is propagated and not built:
* Update _S_ values at _td_  using a time step of value _td - t4_.
* Apply discontinuity: reset the value of _S_ and his gradient.
* Propagate a discontinuity to variables that depends on $S$ in order 
  to reinitialise them at $t_d$.
* Compute the new sigma (in the figure, _sigma = t6 - td_)

Intuitively, a couple P-DEVS model that represents these systems 
are formed by a set of atomic models, each representing one variable.
In the case of a cycle of atomic models, e.g. when _S_ depends on _E_ and _E_
depends on _S_, using the strategy proposed will lead to discontinuities for
_S_ that are simultaneous (_td = tp_) and an illegitimate P-DEVS model
\cite{zeigler.84} will be formed (ie. a infinite succession of bags with
_ta=0_) due to the propagation of discontinuity at _td_.

To prevent this to happen, we build a discontinuity structure whith a message
_<iP,Md>_ where _iP_ is an identifier of the perturbation and 
_Md_ is the set of identifiers of models the perturbation passes through.
At time _td_, model _S_ propagates the discontinuity _<iP,Md intersection
\{S\}>_ only if new models identifiers are present into _Md_ for perturbation
_iP_.
On a system of differential equations of _nv_ variables which is
perturbated by _np_ events at time _tp_, the number of bags with _ta=0_ 
dedicated to reinitialisation will be in the worst case _nv*np*(nv-1)_ and
is finite. Indeed, the worst case is reached when each variable depends on
all others variables of the system. Using this strategy, the P-DEVS model is
legitimate.



### Technical details

Some technical details are given below:
* Description of the dynamic is based on DEVS state graph transition 
  and _confluentTransition_ implementation is used (see e.g. DEVS state
  graph for Time Slicing Methods implementation) 
* The base class _DifferentialEquation_ implementation is based,
  as far as possible, on the PIMPL idiom. Thus dynamics can be distinguished
  (eg. between QSS2 and Time Slicing Methods) and most of bugs corrections 
  would be stable regarding ABI.
* In an optimized use (no external variable and no perturbation), the
  integration step is ensured to be performed in only one bag.  


#### Global architecture

![MISSING FIG](http://www.vle-project.org/pub/1.3/docs/TimeSlicingDEVS.png)

**Fig:** The DEVS state transition graph for Time Slicing methods (TSM).

#### Simulation time profiling

Simulation time comparisons are based on the simulation of the Lotka Volterra
model and concern the three softwares VLE, powerdevs and R package deSolve. 
End time of simulation is set to 1500.
Two integration schemes are tested:
* RK4 scheme with a time step of 0.01 (for deSolve and VLE)
* QSS2 scheme with a quantum value of 0.0001 (for powerdevs and VLE). Note
  that a lower quantum value results in a divergence process (and a $ta \to
  0$).

Different observations schemes are tested:
* For VLE and powerdevs. A null observation which provides no output
  data.
* For VLE. A timed observation with a time-step of 0.01 and a 
  storage into a file.
* For VLE. A timed observation with a time-step of 0.01 and a 
  storage into memory. At the beginning, the matrix contains 
  10000 rows and is updated with 10000 more rows each time it is required to
  enlarge the matrix (these are the maximal values for VLE).
* For deSolve. A timed observation with a time-step of 0.01 and a 
  storage into a R matrix.
* For powerdevs. A plot using gnuplot of quantized values.


soft      | observation         | RK4      | QSS2
----------|---------------------|----------|--------
powerdevs | gnuplot             | -        | 3.86
powerdevs | null                | -        | 0.76(2)
deSolve   | timed, mem. storage | 14.78(1) | -
VLE       | timed, mem. storage | 6.47(1)  | 10.54
VLE       | timed, file storage | 3.46     | 7.81
VLE       | null                | 1.08     | 4.82(2)

**TAB:** Comparison of time executions given in seconds.

<!--
\begin{figure}
\begin{center}
\begin{tabular}{|c|c|c|c|}
\hline
soft                      & observation         & RK4     & QSS2  \\\hline\hline
\multirow{2}{*}{powerdevs}& gnuplot             &  -      & 3.86  \\ \cline{2-4}
                          & null                &  -      & 0.76**  \\ \hline
deSolve                   & timed, mem. storage &  14.776* & -     \\ \hline
\multirow{3}{*}{VLE}      & timed, mem. storage &  0.74*  & 1.07\\\cline{2-4} 
                          & timed, file storage &  0.94  & 1.28 \\ \cline{2-4}
                          & null                &  0.1  & 0.37** \\ \hline
\end{tabular}
\caption{Comparison of time executions given in seconds. New TO BE
confirmed?!!.}
\end{center}
\end{figure}
-->




Values quoted (1) can be used to compare deSolve and VLE. This first
experiment shows better results for VLE. Moreover, VLE results should be 
improved if initialization size of the matrix is directly set to the final size
which is 150000 (rather than 10000). Values quoted (2) can be used to compare
powerdevs and VLE and show clearly that powerdevs is faster.  

<!--
% LotkaVolterra (rk4, 0.01, 1500) ::
% deSolve :: 
%   utilisateur=14.761 systeme=0.000 ecoule= 14.776
% 
% LotkaVolterra (rk4, 0.01, 1500) :: 
% vle(without view) ::
%    1.072u 0.004s 0:01.08 99.0\% 0+0k 0+16io 0pf+0w
% 
% LotkaVolterra (rk4, 0.01, 1500) :: 
% vle(file view 0.01) ::
%    3.468u 0.036s 0:03.50 99.7\%	0+0k 0+24928io 0pf+0w
% 
% LotkaVolterra (rk4, 0.01, 1500) :: 
% vle(storage view 0.01 opt nrow=10000 updrow=10000) ::
% 	6.468u 0.068s 0:06.54 99.6\% 0+0k 0+16io 0pf+0w
%   
% LotkaVolterra (qss2, 0.0001, 1500) :: 
% powerdevs(gnuplot) ::
% 	3.86 secs
% 	
% LotkaVolterra (qss2, 0.0001, 1500) :: 
% powerdevs(without view) ::
% 	0.76 secs
% 
% LotkaVolterra (qss2, 0.0001, 1500) :: 
% vle(without view) ::	
% 	4.824u 0.008s 0:04.83 99.7\% 0+0k 0+16io 0pf+0w
% 		 
% LotkaVolterra (qss2, 0.0001, 1500) :: 
% vle(file view 0.01) :: 
% 	7.812u 0.020s 0:07.84 99.8\% 0+0k 0+24928io 0pf+0w
%   
% LotkaVolterra (qss2, 0.0001, 1500) :: 
% vle(storage view 0.01 opt nrow=10000 updrow=10000) ::
% 	10.536u 0.088s 0:10.63 99.8\% 0+0k 0+16io 0pf+0w
-->



