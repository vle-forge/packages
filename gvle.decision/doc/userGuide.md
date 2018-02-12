# gvle.decision modeling plugin **User Guide**

## overhall consideration

The gvle.decision has two main roles:
+ editing a plan file
+ providing a metadata file containing :
  - all the information of the plan model
  - the configuration of the vle atomic model that can run the plan

## The plan

Outputs are specified when configuring an activity.
They have a single reference name that can be shared by activities.

3 kinds of outputs can be associated to an activity:
- Output by Value
- Output by Parameter
- Output by Input


Formally outputs are discrete-time variable, and by default the value is 0.

At run time, when the activity starts, the output variable takes the value either from:

+ the constant value specified directly, if the king is Value
+ the value of the parameter, if the king is Parameter
+ the value of the input variable if the king is Input

At the next time of the simulation, what ever happens, the value of the variable
goes back to the défault value.

## The configuration

The configuration managed by the GUI handles many items.

### The plan to be used by the model

The GUI provide a configuration where only the single edited plan file
is loaded.  By default the plan is loaded a single time.

### Ouput managment

Output ports of the model are managed automatically.
Parameters involved for the outputs also.
Input ports corresponding to input values also.
