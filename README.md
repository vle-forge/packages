Virtual Laboratory Environment 2.1
==================================


This repository stores the packages distributed into the distribution of VLE.
See AUTHORS and COPYRIGHT files for the list of contributors.

## Requirements

* VLE (≥ 2.1)
* expat (≥ 2.0)
* boost (≥ 1.47)
* cmake (≥ 3.5)
* c++ compiler (gcc ≥ 4.9, clang ≥ 3.3, intel icc (≥ 11.0)
* Qt5

## Getting the code

The source tree is currently hosted on Github and Sourceforge. To view
the repository online: https://github.com/vle-forge/packages. The URL to
clone it:

```
cd $HOME/
git clone git://github.com/vle-forge/packages
```

Install dependencies (recent ubuntu/debian). Copy/paste the following lines:

```bash
apt-get install curl libexpat1-dev libboost-dev cmake pkg-config g++ \
        qttools5-dev qttools5-dev-tools qtbase5-dev qtbase5-dev-tools \
        qtchooser qt5-default libqt5opengl5-dev libqt5svg5-dev \
        libqt5xmlpatterns5-dev
```

## Documentation

Documentation of the packages are given on the [dedicated VLE web page]
(https://www.vle-project.org/packages/)

## License

This software in GPLv3 or later. See the file COPYING. Some files are under a
different license. Check the headers for the copyright info.
