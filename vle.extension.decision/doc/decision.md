### Priority of resource assigment

Each time an activity is not served by the the resource assigment
system, if the activity has a priority parameter set, the priority is
incremented by a value. By default the value of the increment is 0.

To set the increment, one can use the **setPriorityIncrement** of the **Activities** class.

Example of call, inside a Agent that does inherit from a KnowedgeBase classe:
```
    mPlan.activities().setPriorityIncrement(1.0);
```
