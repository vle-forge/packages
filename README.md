Virtual Laboratory Environment 2.0
==================================

A lot of packages for the VFL.

See AUTHORS and COPYRIGHT files for the list of contributors.

### Changes since v1.2

Some utils packages have been added.
A new extension for discrete time models is also proposed.

#### Requirements

* vle (>= 2.0)
* boost (>= 1.47)
* cmake (>= 3.0)
* c++ compiler (gcc >= 4.9, clang >= 3.1, intel icc >= 11.0)

### Getting the code

The source tree is currently hosted on Github and Sourceforge. To view the
repository online: https://github.com/vle-forge/packages The URL to clone it:

    git clone git://github.com/vle-forge/packages.git

### Installation

Once you have met requirements, compiling and installing is simple:

    cd packages
    ./build.sh

Or:

    cd packages
    vle --package vle.extension.celldevs clean rclean configure build test
    vle --package vle.discrete-time clean rclean configure build test
    vle --package vle.discrete-time.generic clean rclean configure build test
    vle --package vle.extension.cellqss clean rclean configure build test
    vle --package vle.extension.decision clean rclean configure build test
    vle --package vle.discrete-time.decision clean rclean configure build test
    vle --package vle.reader clean rclean configure build test
    vle --package vle.tester clean rclean configure build test
    vle --package vle.recursive clean rclean configure build test
    vle --package vle.extension.difference-equation clean rclean configure build test
    vle --package vle.extension.differential-equation clean rclean configure build test
    vle --package vle.extension.differential-equation_test clean rclean configure build test
    vle --package vle.extension.dsdevs clean rclean configure build test
    vle --package vle.extension.fsa clean rclean configure build test
    vle --package vle.extension.petrinet clean rclean configure build test
    vle --package vle.examples clean rclean configure build test
    vle --package ext.muparser clean rclean configure build test
    vle --package vle.forrester clean rclean configure build test
    vle --package vle.reader_test clean rclean configure build test
    vle --package vle.recursive_test clean rclean configure build test
    vle --package vle.discrete-time_test clean rclean configure build test

### Documentation

* [vle.reader](docs_markdown/vle.reader.md)
* [vle.tester](docs_markdown/vle.tester.md)
* [vle.recursive](docs_markdown/vle.recursive.md)
* [vle.discrete-time, vle.discrete-time.generic, vle.discrete-time.decision](docs_markdown/vle.discrete-time.md)
* [vle.extension.differential-equation](docs_markdown/vle.extension.differential-equation.md)

## License

This software in GPLv3 or later. See the file COPYING. Some files are under a
different license. Check the headers for the copyright info.
