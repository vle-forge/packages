## Resource Management

A resource is defined by an Id and categories. To define the available
resources of the knowledge base a single method **addResources** is
provided.

Be aware that ressources have to be defined before loading plans.

Example of a resource definition inside the source code of an Agent
that does inherit from a KnowedgeBase class:
```
addResources("farmworker", "Bob");
addResources("farmworker", "Bill");
addResources("farmer", "Phil");
addResources("worker", "Bob");
addResources("worker", "Bill");
addResources("worker", "Phil");

```
In this case we define 3 resources, they three belongs to 2
categories each

Resources can also be defined with out categories first or only, like
this:
```
addResources("Bob");
addResources("Bill");
addResources("Phil");
...
addResources("worker", "Bob");
addResources("worker", "Bill");
addResources("worker", "Phil");

```

Once the resources have be defined, they can be mobilised to define
requirement for activities.

To set the resources required to start an activity, one can use the
parameter block of the activity item of a plan file. The reserved
parameter name to use is **resources**. Requirement are explicited by
categories.

Four operators are available:
* "&" enables to specify a resource that belongs to multiple categories
* "." enables to specify a quantity of a category of resource wanted
* "+" enables to specify a conjonction of a categories needed
* "|" enables to specify alternatives

The priority order is: "&" >  "." > "+" >  "|"

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
           resources = "farmer + farmworker | worker.2;
           ...
	}
    }
}
```
In this case the Activity_1 require to start either a farmer and a
farmworker or two workers.

Example of settings with multiple categories:
```
        ...
	parameter {
           resources = "farmer&young.2| worker.2;
           ...
	}
...
```
In this case the activity prefers to start with 2 young farmers.

If you want to mention a resource by Id, you can.

Example of settings direct by ids:
```
        ...
	parameter {
           resources = "Bob + Bill | Bob + Phil";
           ...
	}
...
```
## Observing the the resource assigment during the simulation.

An observable port can be used to observe the resource used by a
particular activity.  If the activity name is "sowing@field1", in
order to build the portname one should use the
**Activity(resources)_** prefix to form this portname
**Activity(resources)_sowing@field1**.

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

When filling the plan of the knowledge base, The fourth parameter can be used to tune the priority levels. The value passed to the **fill** method will be used to increment inside the plan each activity that is parametrized with a priority.

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

## Miscellaneous

### The neverfail activity parameter

If case you need that an activity always start, even if there is
either no rules valid, or no resources available, or even a constraint
not valid, you might use the **neverfail** activity parameter. This
should be use in conjonction with the maxstart time range constraint.

Example of setting, inside a plan file:
```
activities { # liste des activites
   activity {
      id = "Activity_1";
        temporal {
          minstart = 0;
          maxstart = 5;
          finish = 100;
        }
        ...
	parameter {
           neverfail = 1.0;
           ...
	}
    }
}
```
In this case, the **Activity_1** will start at 5, even if the
conditions specified are not verified.

### The plan time stamp

If you need predicates that refer to when activities have been planed,
one can use the **planTimeStep** predicate parameter. When the plan is
loaded with the **fill** method, the time value is used to reset
this parameter.

Example of setting, inside a plan file:
```
   predicate {
      id = "p_P";
      type = "predUsingPlanTimeStamp";
      parameter {
         dayThreshold = 3.0;
         planTimeStamp = 0.0;
      }
   }
```
At load time the 0.0 value will be replaced with the time passed to the fill method.

### The plan suffix

Activities name have to be uniq inside the knowledge base. But if you
need to load many times the same plan, you can use the suffix
parameter of the **fill** method. The suffix is added to each activities name.

Example of multiple loading of the same plan inside a Agent that does inherit from a KnowedgeBase class, the same plan is loaded two times:
```
   ...
   std::string filePath = mPack.getDataFile("Plan1.txt");
   std::ifstream fileStream(filePath.c_str());

   KnowledgeBase::plan().fill(fileStream, time, "@P1", 10);
   KnowledgeBase::plan().fill(fileStream, time, "@P2", 100);
   ...

```
"@P1" and "@P2" are two suffixes, and if in the Plan1 plan there is an activity named "Sowing". After loading the plan to times, inside the knowledge base there will be 2 activities "sowing" named respectivly "sowing@P1" and "sowing@P2".
