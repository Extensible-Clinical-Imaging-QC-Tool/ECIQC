@page Tutorials Tutorials

@tableofcontents

ECIQC is a flexible framework designed for medical imaging quality control. Users can guides its behaviour by crafting configuration files. In this tutorial, we will guide you through the process of specifying configuration files with increasing levels of detail. This tutorial caters to both non-developers (end-users) and developers who are new to this software. By the end of this tutorial, you will understand:
- General syntaxes of configuration files
- How to modify example configuration files to meet users' requirements
- How to start from strach to write proper configuration files

Here are some common scenarios proposed by Chris from GE Healthcare. Corresponding configuration codes can be found on the configuration gallery page:
- Case 1: Check whether the patient name is a 6-digit number. If so, insert the patient name to clinical trial subject id; reject otherwise.
- Case 2: Check whether the clinical trial protocol id is 3 letters followed by 6 digits. If so, prepend the patient name with "_" and clinical trial protocol id; reject otherwise.
- Case 3: Check whether the date of birth is a 8-digit number. Replace the day with 30 and month with 06 if so.
- Case 4: check whether the modality is 'MR'. If so, check whether the manufacturer is 'SIEMENS' or 'HITACHI'. If true, check whether the magnetic field strength equals 1.5 or 3.0. Set the clinical trial series description to T1 if all these conditions are satisfied; reject otherwise.

 We recommend readers to go through this tutorial and the configuration gallery for a better understanding.If you have any questions, please refer to the FAQ page first. If the issue persists, feel free to email Yiming Wei(yiming.wei@dtc.ox.ac.uk) or Dr. Martin Robinson (martin.robinson@dtc.ox.ac.uk) for assistance.

## First Example
Configuration files are composed in JSON format. For guidance on initialising ECIQC, please refer to the main page. You can designate your custom configuration files by entering the following commands in the command line:
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
The key `metadata` works on the metadata of a DICOM image.It usually comprises of one or more DICOM tags to work on. For full list of DICOM tags, please refer to the [DICOM library](https://www.dicomlibrary.com/dicom/dicom-tags/).
### Basic syntax within DICOM tags
Within each DICOM tag, you need to specify the `tagName`,`vr` and `description`. You can look up the `tagName` and `vr` in the [DICOM library](https://www.dicomlibrary.com/dicom/dicom-tags/). The `description` value is written by the user to enhace readibility. Then it comes to the key `operations`.
### Operation types
There are typicall four types of operators.
1. Comparison operators: EQUAL (==), GREATER_THAN, LESS_THAN,IS_IN. These operators compare the tag value with the ones provided by the user. They are usually followed by `IF_TRUE` and `IF_FALSE`.
2. regular expression operators: EXIST, REGEX. These two operations check whether thare are specific 'patterns' in the tag value
3.Logical Operators: NOT, AND, ALL. These are similar to those used in programming languages and typically link multiple comparison operations and regular expression operations.
4. Actions: OVERWRITE,REMOVE,INSERT,CLEAR,COPY,UPDATE,APPEND,PREPEND,REJECT. Those are the real actions to perform if certain conditions are satisfied. The users can specify unconditional actions, which is not recommended in most cases.

We introduce the syntaxes of different operators in the following subsections.
#### Comparison Operators ==,<=,>=,\in
The basic syntax of comparison operators is
```
"operations":{
  "<comparison_operator>":{
    "otherTagString":"<other_tag_string>",
    "<value_key>":<compared_value>,
    "IF_TRUE":{<action_clause>},
    "IF_FALSE":{<action_clause>}
  }
}
```
where
- "&lt;comparison_operator&gt;": EQUAL, GREATER_THAN, LESS_THAN, IS_IN;
- "&lt;other_tag_string&gt;": tag for comparison; If specified as an empty string, the system will use the root tag;
- "&lt;value_key&gt;": There's a correspondence between &lt;comparison_operator&gt; and &lt;value_key&gt;.
| &lt;comparison_operator&gt; | &lt;value_key&gt; |
|:----------:|:---------:|
| EQUAL       | value         |
| GREATER_THAN| compareValue  |
| LESS_THAN   | compareValue  |
| IS_IN       | valueList     |
- &lt;action_clause&gt;: actions to take (also could be nested operations), please refer to actions section.

#### Regular expression EXIST REGEX
EXIST checks if the specified tag exists, while REGEX determines whether the tag value satisfies a regular expression pattern.

#### Logical Operators NOT, AND, ALL
Logical operators usually combine different comparison operators or regular expression operators. One typical syntax is:
```
"operations":{
  "<logical_operator>":{
      "<comparison_operator_1>":{
        "otherTagString":"<other_tag_string_1>",
        "<value_key_1>":<compared_value_1>,
      },
      "<comparison_operator_2>":{
        "otherTagString":"<other_tag_string_2>",
        "<value_key_2>":<compared_value_2>,
      },
      "IF_TRUE":{<action_clause>},
      "IF_FALSE":{<action_clause>}
  }
}
```
Remark:
- &lt;logical_operator&gt;: `NOT`, `AND`, `OR`. `NOT` only permits one logical operator while `AND` and `OR` permits two or more.
- If &lt;comparison_operator_1&gt; and &lt;comparison_operator_2&gt; are the same (e.g. EQUAL), use `EQUAL_1` and `EQUAL_2`,etc.
#### Actions
Actions might be the most complicated part in the configurations. A wide variety of actions are enabled in the ECIQC, and many requires further specifications. The actions can be divided into several groups as follows.
 - **Group 1**: UPDATE, INSERT, OVERWRITE
    UPDATE means adding something in the next value channel without removing current ones; INSERT means adding specified value if the tag doesn't exist. OVERWRITE replaces the value if the tag exists.
 - **Group 2**: REMOVE, CLEAR
    REMOVE means eliminating both the tag and the value while CLEAR asks to "remove" the value (tag is perserved).
 - **Group 3**: APPEND, PREPEND,
    APPEND means adding string at the end of the original value, while PREPEND means adding string at the start. Differetn from Group actions, the original tag value is presered in APPEND and PREPEND.
 - **Group 4**: REJECT
    REJECT means putting the image to the quarantine. 

### Multiple operations
The users bump into multiple operations when multiple actions are needed. For example, you may want to prepend the patient name with a '_', and then copy the '_patient_name' to other tags. Also, users will naturally use multiple operations when logical operators are introduced. A typical instance is telling whether the tag value is in some range (AND, GREATER_THAN lower bound, LESS_THAN upper bound). The implementation of ECIQC naturally supports multiple operations as long as you put your desired order of actions correctly. However, you need to pay attention to the repeated keys issue.
#### Repeated keys
A typical scenario for repeated keys is that you need to decide whether the tag value is `EQUAL` to valueA or valueB. One solution is to use the operation `IS_in` and specify the comparison value to be a list. A more general approach is to prepend the `EQUAL` key with an unscore followed by some digits (`EQUAL_1`,`EQUAL_2`,...etc). The ECIQC will recognize them as different keys but behave in the same way as `EQUAL`. 

### Nested operations
The implementation of ECIQC also provides some flexibility for nested operations. A typical case would be: compare one tag to some value, and if the condition is true, you compare another tag with some other values and make further decision on your action. In this case, you need to nest the second comparison within `IF_TRUE`/`IF_FALSE` key in the first operation. In the second operation, you need to specify `otherTagString` if you work on a tag different from the first operation.
<!---
### Miscellaneous
There is also some more details to introduce. Typically they involve multiple tags (e.g. copy value from one tag to another; the tag to be acted on are differnt from that in the logical conditions,etc). We recommend to ignore this first, and come back to the following details when you are stuck in some difficulities. Since this project is still in active developement, you can contact the authors for help. 
#### Operations cheat sheet

## Presentation context and transfer syntax
-->