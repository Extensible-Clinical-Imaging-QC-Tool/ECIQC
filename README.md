# Extensible Clinical Imaging Quality Control Tool

[![codecov](https://codecov.io/gh/OxfordRSE/template-project-cpp/branch/master/graph/badge.svg)](https://codecov.io/gh/OxfordRSE/template-project-cpp)
[![Build
Status](https://travis-ci.org/OxfordRSE/template-project-cpp.svg?branch=master)](https://travis-ci.org/OxfordRSE/template-project-cpp)
[![Build status](https://ci.appveyor.com/api/projects/status/5245a76r7ikhaass/branch/master?svg=true)](https://ci.appveyor.com/project/fcooper8472/template-project-cpp/branch/master)

## Overview

This repository contains the ECIQC tool developed between SABS:R3, University of Oxford and GE Healthcare. The primary purpose of the tool is to automatically vaildate the DICOM images using for medical research. 

The project is under active development. 

This project has made use of the Oxford RSE C++ template. 


## Installation 

### Where does it run?

We expect this project to be usable on all modern systems
It is regularly tested on the following operating systems:
- Linux: Ubuntu 14.04

and with the following C++ compilers/toolchains:
- GCC 5.x, 6.x, 7.x
- Clang 4.x, 5.x

### First run

#### Linux

- Install [CMake](https://cmake.org/download/) and a C++ compiler and other dependencies. 
  This can be done using the install_dependencies file found in /scripts.

```bash
$ ./scripts/install_dependencies.sh
```

- Make a build directory and configure the project. E.g. Assuming you are in a 
  directory containing the source code:

```bash
$ mkdir build
$ cd build
$ cmake ..
```

- Compile the project

```bash
$ make
```

- Run all the tests using CTest (this is bundled with CMake)

```bash
$ ctest --output-on-failure
```
## Feedback and suggestions

If you have any feedback or suggestions about this project, please get in touch or open an issue.
