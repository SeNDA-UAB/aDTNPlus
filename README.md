# aDTNPlus
[![Build Status](https://travis-ci.org/SeNDA-UAB/aDTNPlus.svg?branch=master)](https://travis-ci.org/SeNDA-UAB/aDTNPlus)

Active Delay Tolerant Network Plus is a DTN applications developed by the [SeNDA](http://senda.uab.cat/) research group. 

This project implements a DTN system with some improvements suchs as allowing each bundle to decide how to forward, or to whom delivery.

It's a major improvement over the [adtn-platform](https://github.com/SeNDA-UAB/aDTN-platform) also developed by the research group.

## Getting Started

These instructions will get you a copy of the project up and running on your local machine for development and testing purposes. See installation to install a deb package for only run it.

### Prerequisites

To develop you need the **build-essential**, **git** and **cmake** packages.

`sudo apt-get install build-essential git cmake`

If you want to view the coverage when testing, you must install **locv**

`sudo apt-get install lcov`

### Installing

First execute the **configureGitProject.sh** to configure the hooks used in this project.

`./configureGitProject.sh`

There is another script that generates the test executables and coverage.

`./generateTestAndCoverage.sh`

If you want to make it yourself, create an empty directory.

`mkdir build && cd build`

Call cmake to generate the files.

`cmake ..`

And a make call

`make`

If you want to install to the system.

`sudo make install`

To execute the platform.

`sudo BundleAgent /usr/local/etc/adtnPlus/adtn.ini`

## Installation

Explain how to install from deb packages

## Libraries

There are two libraries to interact with the platform, one in C++ and another one in Python.

The C++ one is installed by default with the name **adtnSocket.h**.

If you prefer to use the Python one you need to generate the module, for this its necessary to install [setuptools](https://pypi.python.org/pypi/setuptools).

When installed you can do.

`cd Python && sudo python setup.py install`

To generate the module and install it to the system, the module can be used by **import adtnPlus**

### C++

How it works, link to docs.

### Python

Explain how to generated the python library and dependencies, link to docs.

## Authors

* **Marc Dalmau** - [Blackcatn13](https://github.com/Blackcatn13)
* **Angela Fabregues** - [AngelaFabreques](https://github.com/AngelaFabregues)

See also the list of [contributors](https://github.com/SeNDA-UAB/aDTNPlus/contributors) who participated in this project.

## License

This project is licensed under Apache 2.0 License -see the [LICENSE](LICENSE) file for details
