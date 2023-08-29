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
Configuration files are written in json format. Please refer to the mainpage for spinning up the ECIQC. You can specify your own configuration files by typing in the following codes in the command line:
```
./build/exe/qctool --config-file <your_configuration_file.json>
```
### Example codes
We illustrate the basic components with a simple configuration file.
```
{
    "receiver": {
      "aetitle": "TestQCTool",
      "port": 11112
    },
    "destination": {
      "aetitle": "TestDestination",
      "hostname": "localhost",
      "port": 11113
    },
    "quarentine": {
      "aetitle": "TestQuarentine",
      "hostname": "localhost",
      "port": 11114
    },
    "metadata": {
        "(0010,0010)": {
          "tagName": "PatientName",
          "vr": "CS",
          "description": "check if patient is called 'bob' and change, if so.",
          "operations": {
            "EQUAL_1234": {
                "otherTagString": "",
                "value": "John Doe",
                "IF_TRUE": {
                    "OVERWRITE": {
                        "tag": "",
                        "replaceString": "Robert"
                    }
                }
            }
          }
        }
        
    }
}

```
### Explanation
The ECIQC receives medical images via **receiver** and sends post-processed images via **sender**.The key `receiver` specifies the port number so that another remote sender can identify it and send proper images. The ECIQC pipeline checks the image and send normal images to the **destination** and suspicious ones to the **quarantine**. `Destination` and `quarantine` tell the ECIQC their addresses.

The key `metadata` is the main component of the configuration file. You will see long lines of code within this part. Typically, you will see one or more DICOM tags to work on(0010,0010), accompanied by the tag name (Patient Name),vr (data type and format) and a short line of description. The key `operations` inform the ECIQC what to do with this tag. You can guess with your gut instinct that this configuration file requires the pipeline to replace the name with Robert if the patient name is John Doe. 

## Receiver, Destination and Quarantine
As said before, `receiver` specifies the proper of the ECIQC receiver; `destination` tells the ECIQC where to send "good" images; `quarantine` provides the place to send "bad" images. You can use the local port for most test cases. However,if you are interested in communicating between different network nodes in real-world application (e.g. GE Healthcare), please ask your colleagues for more information.

## MetaData
The key `meta` usually comprises of one or more DICOM tags to work on. For full list of DICOM tags, please refer to the [DICOM library](https://www.dicomlibrary.com/dicom/dicom-tags/).
### Basic syntax within DICOM tags
Within each DICOM tag, you need to specify the `tagName`,`vr` and `description`. You can look up the `tagName` and `vr` in the [DICOM library](https://www.dicomlibrary.com/dicom/dicom-tags/). The `description` value is written by the user to enhace readibility. Then it comes to the key `operations`.
### Operation types
There are typicall three types of operations.
1.. Comparison operations: EQUAL
#### Comparison Operators ==,<=,>=

#### Logical Operators NOT, AND, ALL

#### Actions

### Multiple operations

#### Repeated keys
### Nested operations

### Miscellaneous

#### Operations cheat sheet

## Presentation context and transfer syntax