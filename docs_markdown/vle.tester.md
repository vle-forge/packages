The package **vle.tester** facilitates the writing of unit tests based
on the simulation of vpz files. 

### Use case: declares unit tests on models

First you have to write a test file into the _data_ directory of your package.
The test file is composed of a list of unit test. Each line of the test
file corresponds to a unit test. The test file has the following form: 

```
 column 1: the name of the package
 column 2: the name of a vpz into the package 
 column 3: the name of a view defined in the vpz 
 column 4: the name of a column into the view
 column 5: an index of the colum
 column 6: a precision or 'NA'; 'NA'' means we expect to test strings 
  and not double nor integers.
 column 7: the value expected (a string, double or a integer)
```

An example of a test file is given below.
```
pkg mod1.vpz view coupled:atomic.port 198 10e-5 4.16
pkg mod2.vpz view2 coupled2:coupled3:atomic2.port1 342 NA hello
```

The first line means that one expects, when simulating the model 
_mod.vpz_ of the package _pkg_, that  the 198th value of the column
_coupled:atomic.port_ from the view _view_ is a double and has the value 
4.16 with a precision of 10e-5.

Then you have to modify the file _test/test.cpp_ from your package
with the following code :

```
//@@tagtest@@
#include <vle/tester/package_tester.hpp>
VLE_UTILS_TEST("mypkg","test_file.txt");
```

Finally compile your package and launch tests.
It will perform unit tests located into : _mypkg/data/test_file.txt_
Note that only one test file can be checked into test.cpp.

```
vle -P mypkg configure build
vle -P mypkg test
```

To get full log, for each unit test, of the checking process,
launch  the following executable : _./mypkg/buildvle/test/test_
For example the test described above could result in the following report:
```
test_1: test ok of pkg/mod1.vpz: view=view; col=coupled:atomic.port; expected=16
test_2: fail to open pkg/mod2.vpz
```