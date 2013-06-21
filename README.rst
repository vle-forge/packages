====================================
Virtual Laboratory Environment 1.1.0
====================================

A lot of packages for the VFL.

See AUTHORS and COPYRIGHT files for the list of contributors.

Requirements
------------

* vle (>= 1.1)
* boost (>= 1.41)
* cmake (>= 2.8.0)
* make (>= 1.8)
* c++ compiler (gcc >= 4.4, clang >= 3.1, intel icc (>= 11.0)

Getting the code
----------------

The source tree is currently hosted on Github and Sourceforge. To view the
repository online: https://github.com/vle-forge/packages The URL to clone it:

::

 git clone git://github.com/vle-forge/packages.git

Installation
------------

Once you have met requirements, compiling and installing is simple:

::

 cd packages
 vle-1.1 --package=vle.output configure build
 vle-1.1 --package=vle.extension.celldevs configure build
 vle-1.1 --package=vle.extension.cellqss configure build
 vle-1.1 --package=vle.extension.decision configure build
 vle-1.1 --package=vle.extension.difference-equation configure build
 vle-1.1 --package=vle.extension.differential-equation configure build
 vle-1.1 --package=vle.extension.dsdevs configure build
 vle-1.1 --package=vle.extension.fsa configure build
 vle-1.1 --package=vle.extension.petrinet configure build
 vle-1.1 --package=vle.examples configure build
 vle-1.1 --package=vle.examples ext.muparser configure build
 vle-1.1 --package=vle.examples vle.forrester configure build

License
-------

This software in GPLv3 or later. See the file COPYING. Some files are under a
different license. Check the headers for the copyright info.
