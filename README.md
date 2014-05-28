Virtual Laboratory Environment 1.3
==================================

A lot of packages for the VFL.

See AUTHORS and COPYRIGHT files for the list of contributors.

## Chances since v1.2

### Requirements

* vle (>= 1.3)
* boost (>= 1.41)
* cmake (>= 2.8.0)
* make (>= 1.8)
* c++ compiler (gcc >= 4.4, clang >= 3.1, intel icc (>= 11.0)

## Getting the code

The source tree is currently hosted on Github and Sourceforge. To view the
repository online: https://github.com/vle-forge/packages The URL to clone it:

    git clone git://github.com/vle-forge/packages.git

## Installation

Once you have met requirements, compiling and installing is simple:

    cd package
    ./build.sh

Or:

    cd package
    vle-1.3 --package=vle.output clean rclean configure build test
    vle-1.3 --package=vle.extension.celldevs clean rclean configure build test
    vle-1.3 --package=vle.extension.cellqss clean rclean configure build test
    vle-1.3 --package=vle.extension.decision clean rclean configure build test
    vle-1.3 --package=vle.extension.difference-equation clean rclean configure build test
    vle-1.3 --package=vle.extension.differential-equation clean rclean configure build test
    vle-1.3 --package=vle.extension.dsdevs clean rclean configure build test
    vle-1.3 --package=vle.extension.fsa clean rclean configure build test
    vle-1.3 --package=vle.extension.petrinet clean rclean configure build test
    vle-1.3 --package=vle.examples clean rclean configure build test
    vle-1.3 --package=vle.examples ext.muparser clean rclean configure build test
    vle-1.3 --package=vle.examples vle.forrester clean rclean configure build test

## License

This software in GPLv3 or later. See the file COPYING. Some files are under a
different license. Check the headers for the copyright info.
