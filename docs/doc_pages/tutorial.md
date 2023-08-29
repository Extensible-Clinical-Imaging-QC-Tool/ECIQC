@page Tutorials Tutorials

@tableofcontents

ECIQC is a flexible framework for medical imaging quality control. Users can teach it what to do by writing configuration files. In this tutorial, we are going to introduce how to specify proper configuration files at successively increasing levels of details. This tutorial is suitable for both non-developers (end-users) and developers new to this software. At the end of this tutorial, you will learn
- General syntaxes of configuration files;
- How to modify the example configuration files to cater to users' needs;
- How to start from strach to write proper configuration files.

Here are some common scenarios proposed by Chris from GE Healthcare. You can find the corresponding configuration codes in configuration gallery page.
- Case1: check whether the patient name is a 6-digit number. If so, insert the patient name to clinical trial subject id; reject otherwise.
- Case2: check whether the clinical trial protocol id is 3 letters followed by 6 digits. If so, prepend the patient name with "_" and clinical trial protocol id, reject otherwise.
- Case3: check whether the date of birth is a 8-digit number. Replace the day with 30 and month with 06 if so.
- Case4: check whether the modality is 'MR'. If so, check whether the manufacturer is 'SIEMENS' or 'HITACHI'. If that's true, check whether the magnetic field strenght equals 1.5 or 3.0. Set the clinical trial series description to T1 if all these conditions are satisfied; reject otherwise.

 We recommend the readers to read through this tutorial and the configuration gallery for a better understanding.If you have any questions, please refer to FAQ page first. If not solved, please email to Yiming Wei(yiming.wei@dtc.ox.ac.uk) or Dr. Martin Robinson (martin.robinson@dtc.ox.ac.uk) for help.
## First Example
Configuration files are written in json format. Please refer to the mainpage for spinning up the ECIQC. You can specify your own configuration files by
```
./build/exe/qctool --config-file <your_configuration_file.json>
```
### Example codes
### Explanation

## Receiver, Destination and Quarantine

## MetaData
### Basic syntax

### Operation types
#### Comparison Operators ==,<=,>=

#### Logical Operators NOT, AND, ALL

#### Actions

### Multiple operations

#### Repeated keys
### Nested operations

### Miscellaneous

#### Operations cheat sheet

## Presentation context and transfer syntax