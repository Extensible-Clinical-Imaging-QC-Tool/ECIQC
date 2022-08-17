# Extensible Clinical Imaging Quality Control Tool

[![codecov](https://codecov.io/gh/Extensible-Clinical-Imaging-QC-Tool/ECIQC/branch/main/graph/badge.svg?token=KUDHVB07FW)](https://codecov.io/gh/Extensible-Clinical-Imaging-QC-Tool/ECIQC)
[![Ubuntu](https://github.com/Extensible-Clinical-Imaging-QC-Tool/ECIQC/actions/workflows/Unittests_Ubuntu.yml/badge.svg)](https://github.com/Extensible-Clinical-Imaging-QC-Tool/ECIQC/actions/workflows/Unittests_Ubuntu.yml)
[![macOS](https://github.com/Extensible-Clinical-Imaging-QC-Tool/ECIQC/actions/workflows/Unittests_macOS.yml/badge.svg)](https://github.com/Extensible-Clinical-Imaging-QC-Tool/ECIQC/actions/workflows/Unittests_macOS.yml)

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

- Download ECIQC github repository

```bash
git clone https://github.com/Extensible-Clinical-Imaging-QC-Tool/ECIQC.git
cd ECIQC/
```

- Install [CMake](https://cmake.org/download/) and a C++ compiler and other dependencies.
  This can be done using the install_dependencies file found in /scripts.

```bash
./scripts/install_dependencies.sh
```

- Make a build directory and configure the project. E.g. Assuming you are in a
  directory containing the source code:

```bash
mkdir build
cd build
cmake ..
```

- Compile the project

```bash
make
```

- Run all the tests using CTest (this is bundled with CMake)

```bash
ctest --output-on-failure
```

#### Mac OS 

- Download ECIQC github repository

```bash
git clone https://github.com/Extensible-Clinical-Imaging-QC-Tool/ECIQC.git
cd ECIQC/
```

- Install [CMake](https://cmake.org/download/) and a C++ compiler and other dependencies.
  This can be done using the install_dependencies file found in /scripts.

```bash
brew install dcmtk boost cmake opencv tesseract
```

## Command Line Interface (CLI) Tool 
- An interface containing a simple application to receive and send DICOM images. To use the CLI, run the code from the source directory:

```bash
./build/exe/qctool
```
- To configure sender and receiver parameters, run the code:

```bash
./build/exe/qctool --help
```

## Feedback and suggestions

If you have any feedback or suggestions about this project, please get in touch or open an issue.
