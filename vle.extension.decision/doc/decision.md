## Priority of resource assigment

Each activity has a priority that is used when the activity needs to
get resources in order to start. By default the priority is set to
_negativeInfinity_. Before to assign a resource, the activities are
sorted by priority in order to make sure that the activity with a high
priority is served first.

### Setting a priority

To set the priority of an activity, one can use the parameter block of
the activity item of the plan file. The reserved parameter name to use
is **priority**.

Example of setting, inside a plan file:
```

activities { # liste des activites
   activity {
      id = "Activity_1";
        temporal {
           ...
        }
        ...
	parameter {
           priority = 5;
           ...
	}
    }
}

```
### Managing activity priorities by plan

When filling the plan of the knowledge base, The fourth parameter can be used to tune the priority levels. The value passed to the ##fill## method will be used to increment inside the plan each activity that is parametrized with a priority.

Example of priority managment inside a Agent that does inherit from a KnowedgeBase class:
the same plan is loaded two times, but
priorities differs(10!=100):
```
   ...
   std::string filePath = mPack.getDataFile("Farm1.txt");
   std::ifstream fileStream(filePath.c_str());

   KnowledgeBase::plan().fill(fileStream, time, "@P1", 10);
   KnowledgeBase::plan().fill(fileStream, time, "@P2", 100);
   ...

```
### Incrementing the priority of an activity during the simulation

Each time an activity is not served by the the resource assigment
system, if the activity has a priority parameter set, the priority is
incremented by a value. By default the value of the increment is 0.

To set the increment, one can use the **setPriorityIncrement** of the **Activities** class.

Example of call, inside a Agent that does inherit from a KnowedgeBase class:
```
    mPlan.activities().setPriorityIncrement(1.0);
```
