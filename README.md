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

- Make a build directory and configure the project. E.g. Assuming you are in a
  directory containing the source code:

```bash
mkdir build
cd build
cmake .. -DUSE_DCMTK_TARGETS=TRUE
```

If you encounter errors regarding missing libraries, try pointing Cmake to the location of the library directories, like:

```bash
cmake -DCMAKE_PREFIX_PATH=/usr/local ..
```

- Compile the project

```bash
make
```

- Run all the tests using CTest (this is bundled with CMake)

```bash
ctest --output-on-failure
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

## How to test on your own machine (Update 18th May 2023)
After you've cloned the package and installed it properly, we provide two alternatives to test whether it works on your own machine.

### Choice1: Using python script to test with the local senders and receivers.
There's a python script [pipelinetest.py](pipelinetest_main.py) as a simple test tool. To run this script properly, you need to install two packages [pydicom>=2.3.1](https://pydicom.github.io) and [pynetdicom>=2.0.2](https://pydicom.github.io/pynetdicom/stable/). For example, if you're using a linux system, you can create a virtual environment **in the root directory** and then install these two packages.

```
python -m venv venv
source ./venv/bin/activate
pip install pydicom
pip install pynetdicom
```
Make sure that `python` command means `python3` if you are using a linux system (This is also a prerequisite when running the test script). Otherwise, add `python` as an alias by typing `alias python='python3'` in your command line.

Finally, you will be happy to run the test script by `python pipelinetest_main.py` in the root directory. Remember to check the postprocessed files and logs in the ./result and ./result_quarantine. Please also note that we've used the [PipelineCase.json](schema/PipelineCase.json) as the configuration file.

### Choice2: Using the executable file with more flexibility
If you need more flexible test (for example, use non-local sender and receiver, or use your own datasets), we recommend to use the executable file. Before spinning up the pipeline, you need to create a directory ./result to store the logging file, and set up the configuration file properly. Have a look at the [PipelineCase.json](schema/PipelineCase.json) and modify the hostname and port number according to your own system.

Also, we've noticed that the sender and receiver need to agree on specific abstract syntaxes and transfer syntaxes. The syntaxes we're supporting now are listed as follows. We'll come back to the syntax issue in June and July.

CTImageStorage (1.2.840.10008.5.1.4.1.1.2): LittleEndianExplicit, LittleEndianImplicit;
MRImageStorage (1.2.840.10008.5.1.4.1.1.4): LittleEndianExplicit, LittleEndianImplicit;
SecondaryCaptureImageStorage (1.2.840.10008.5.1.4.1.1.7): LittleEndianExplicit, LittleEndianImplicit;
DigitalXRayImageStorageForPresentation (1.2.840.10008.5.1.4.1.1.1.1): LittleEndianImplicit;
UltrasoundMultiframeImageStorage (1.2.840.10008.5.1.4.1.1.3.1): JPEGProcess1TransferSyntax.

Once you've done all the preparation work, now type in `./build/exe/qctool --config-file YourOwnConfig.json` to start the pipeline. Also remember to start your remote/local sender and receiver.

## Feedback and suggestions

If you have any feedback or suggestions about this project, please get in touch or open an issue.
