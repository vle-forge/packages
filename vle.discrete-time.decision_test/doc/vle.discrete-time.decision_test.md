# Many cases to illustrate and test the AgentDTG

The package **AgentDTG** is a ready to use model.
Only plan files and configuration are required to provide
Discrete Time Decision models.

## Case 0: A Farmer(event output), 2 parcels, 3 sequences</a>

The synchronization is not considered.

### Files envolved:

+ case_0.vpz
+ 31035002.csv
+ itk1.txt
+ itk2.txt
+ itk3.txt
+ AwwdmDt.cpp

## Case 1:  A Farmer(event outputs + dtvar outputs), 2 parcels, 3 sequences</a>

The synchronization is not considered.

### Files envolved:

+ case_1.vpz
+ 31035002.csv
+ itk1bis.txt
+ itk2bis.txt
+ itk3bis.txt
+ BwwdmDt.cpp

## Case 2: A Farmer(event outputs)(ack inside), 2 parcels, 3 sequences</a>

The synchronization is not considered.

### Files envolved:

+ case_2.vpz
+ 31035002.csv
+ itk1autoAck.txt
+ itk2autoAck.txt
+ itk3autoAck.txt
+ AwwdmDt.cpp

## Case 3: A Farmer(event outputs + dtvar outputs)(ack inside), 2 parcels, 3 sequences</a>

The synchronization is not considered.

### Files envolved:

+ case_0.vpz
+ 31035002.csv
+ itk1bisautoAck.txt
+ itk2bisautoAck.txt
+ itk3bisautoAck.txt
+ BwwdmDt.cpp

## Case 4: A Farmer(event outputs)(ack inside), 1 parcel, 3 sequences</a>

The synchronization is not considered.

In this case we test the opportunity to specify the rotation on a single parcel.
This can be done by an empty string as key of the rotation map.
The AgenDTG will not automatically manage Variables and Ports with a prefix.

### Files envolved:

+ case_4.vpz
+ 31035002.csv
+ itk1autoAck.txt
+ itk2autoAck.txt
+ itk3autoAck.txt
+ AwwdmDt.cpp

## Case 5: A Farmer(event outputs)(ack inside), 1 tank </a>

No synchronization provided. But to

A simple action on the Tank tested.

### Files envolved:

+ case_5.vpz
+ agroclim.csv
+ itkTank.txt
+ Atank.cpp

## Case 6: A Farmer(event outputs)(ack inside), 1 tank </a>

The synchronization is done by bags, there is no other solution in this case
(between Agent and Tank).
Sync Dt variables can not be mixed with "pure" event.
Also to mention, the input port of the Tank connected to the Farmer need to be
updatable.

In this case we test a recurrent activity, that can happen everyday, if a rule
is valid.

### Files envolved:

+ case_6.vpz
+ agroclim.csv
+ itkTankIter_0.txt
+ Atank.cpp

## Case 7: A Farmer(event outputs)(ack inside), 1 tank </a>

Same situation as case 6 but with a maxIter constraint on the reccurence.

### Files envolved:

+ case_7.vpz
+ agroclim.csv
+ itkTankIter_1.txt
+ Atank.cpp

## Case 8: A Farmer(event outputs)(ack inside), 1 tank </a>

Same situation as case 7 but with a timeLag provided.

### Files envolved:

+ case_8.vpz
+ agroclim.csv
+ itkTankIter_2.txt
+ Atank.cpp

## Case 9: A Farmer(Var outputs)(ack out), 1 tank </a>

Same situation as case 8 but with a timeLag provided.
Synchronized by bags.

### Files envolved:

+ case_9.vpz
+ agroclim.csv
+ itkTankIter_3.txt
+ Anothertank.cpp

## Case 10: A Farmer(Var outputs)(ack out), 1 tank </a>

Same situation as case 8 but with a timeLag provided.
Synchronized by sync Vars.

### Files envolved:

+ case_10.vpz
+ agroclim.csv
+ itkTankIter_4.txt
+ Anothertank.cpp

## Case 11: A Farmer(Var outputs)(ack inside), 1 tank </a>

Same situation as case 8 but with a timeLag provided.
Synchronized by sync Vars, but with AutoAck.

### Files envolved:

+ case_11.vpz
+ agroclim.csv
+ itkTankIter_3.txt
+ Anothertank.cpp

## Case 12: A Farmer(Var outputs)(ack inside), 1 tank </a>

Same situation as case 8 but with a timeLag provided.
Synchronized by sync Vars, but with AutoAck.
Testing if every day decsision can be made on the same var.

### Files envolved:

+ case_12.vpz
+ agroclim.csv
+ itkTankIter_6.txt
+ Anothertank.cpp