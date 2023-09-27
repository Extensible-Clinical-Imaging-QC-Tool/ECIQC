@page Configuration_gallery Configuration Gallery

@tableofcontents

This webpage provides a set of examples to exemplify the configuration syntax. For end-users, you can compare your needs with the short descriptions in the following summary table, and adjust the configuration file accordingly.

| Example  | Operators used | Short description |
|:--------------|:--------------:|:--------------:|
| Example 1     | AND, REMOVE         | Check if BitsAllocated between certain numbers, remove if not          |
| Example 2     | OR, REMOVE          | Check if HighBit between 10 and 20, remove if not                      |
| Example 3     | COPY                | Copy patient's name to patient's Mother's birth name                   |
| Example 4     | CLEAR               | Clear the value in patient's name tag                                  |
| Example 5     | INSERT              | Insert the value 100 to the tag HighBit                                |
| Example 6     | UPDATE              | Update the value 101 to the tag HighBit                                |
| Example 7     | EQUAL, OVERWRITE    | Check if patient is called 'John Doe' and change, if so.               |
| Example 8     | LESS_THAN, OVERWRITE| Check if intercept is less than 2500 and change to 2000.5 if so        |
| Example 9     | EQUAL, OVERWRITE    | Check if high bit is 11 and change to 12 if so                         |
| Example 10    | EXIST, APPEND       | Check if patient's name tag exists, append with John if so             |
| Example 11    | REGEX, PREPEND      | Check if patient's name is in specific pattern, prepend with John if so|
Four more examples to add!

**ATTENTION**: the following examples only contain the "metadata" value in the configuration. For complete json files, please refer to the tutorial to add proper receiver, destination, quarantine and transfer syntax.
### Example 1: AND, REMOVE
Description: Check if BitsAllocated between 10 and 20, remove if not
```
{
    "(0028,0100)": {
        "tagName": "BitsAllocated",
        "vr": "US",
        "description": "Check if BitsAllocated between certain numbers, remove if not",
        "operations": {
            "AND": {
                "GREATER_THAN": {
                    "otherTagString": "",
                    "compareValue": 10
                },
                "LESS_THAN": {
                    "otherTagString": "",
                    "compareValue": 20
                },
                "IF_FALSE": {
                    "REMOVE": {
                        "otherTagString": ""
                    }
                }
            }
        }
    }
}
```
### Example 2: OR, REMOVE
Description: Check if HighBit between 10 and 20, remove if not
```
{
    "(0028,0102)": {
        "tagName": "HighBit",
        "vr": "US",
        "description": "Check if HighBit between 10 and 20, remove if not",
        "operations": {
            "OR": {
                "GREATER_THAN": {
                    "otherTagString": "",
                    "compareValue": 20
                },
                "LESS_THAN": {
                    "otherTagString": "",
                    "compareValue": 10
                },
                "IF_TRUE": {
                    "REMOVE": {
                        "otherTagString": ""
                    }
                }
            }
        }
    }
}
```
### Example 3: COPY
Description: Copy patient's name to patient's Mother's birth name
```
{
    "(0010,1060)": {
        "tagName": "Patient's Mother's Birth Name",
        "vr": "PN",
        "description": "Copy patient's name to patient's Mother's birth name",
        "operations": {
            "COPY": {
                "otherTagString": "(0010,0010)",
                "copyToThis": true
            }
        }
    }
}

```
Alternatively,
```
{
    "(0010,0010)": {
        "tagName": "Patient's Name",
        "vr": "PN",
        "description": "Copy patient's name to patient's Mother's birth name",
        "operations": {
            "COPY": {
                "otherTagString": "(0010,1060)",
                "copyToThis": false
            }
        }
    }
}
```
### Example 4: CLEAR
Description: Clear the value in patient's name tag
```
{
    "(0010,0010)": {
        "tagName": "PatientName",
        "vr": "PN",
        "description": "Clear the value in patient's name tag",
        "operations": {
            "CLEAR": {
                "otherTagString": ""
            }
        }
    }
}
```
### Example 5: INSERT
Description: Insert the value 100 to the tag HighBit
```
{
    "(0028,0102)": {
        "tagName": "HighBit",
        "vr": "US",
        "description": "Insert the value 100 to the tag HighBit",
        "operations": {
            "INSERT": {
                "otherTagString": "",
                "value": "100"
            }
        }
    }
}
```
### Example 6: UPDATE
Description: Update the value 101 to the tag HighBit
```
{
    "(0028,0102)": {
        "tagName": "HighBit",
        "vr": "US",
        "description": "Update the value 101 to the tag HighBit",
        "operations": {
            "UPDATE": {
                "otherTagString": "",
                "value": "101"
            }
        }
    }
}
```
### Example 7: EQUAL, OVERWRITE
Description: check if patient is called 'John Doe' and change, if so.
```
{
    "(0010,0010)": {
        "tagName": "PatientName",
        "vr": "PN",
        "description": check if patient is called 'John Doe' and change, if so.",
        "operations": {
            "EQUAL": {
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
```
### Example 8: LESS_THAN, OVERWRITE
Description: Check if intercept is less than 2500 and change to 2000.5 if so
```
{
    "(0040,9224)": {
        "tagName": "RealWorldValueIntercept",
        "vr": "FD",
        "description": "Check if intercept is less than 2500 and change to 2000.5",
        "operations": {
            "LESS_THAN": {
                "otherTagString": "",
                "compareValue": 2500,
                "IF_TRUE": {
                    "OVERWRITE": {
                        "tag": "",
                        "replaceString": "2000.5"
                    }
                }
            }
        }
    }
}
```
### Example 9: EQUAL, OVERWRITE
Description: Check if high bit is 11 and change to 12
```
{
    "(0028,0102)": {
        "tagName": "HighBit",
        "vr": "US",
        "description": "Check if high bit is 11 and change to 12",
        "operations": {
            "EQUAL": {
                "otherTagString": "",
                "value": "11",
                "IF_TRUE": {
                    "OVERWRITE": {
                        "tag": "",
                        "replaceString": "12"
                    }
                }
            }
        }
    }
}
```
### Example 10: EXIST, APPEND
Description: Check if patient's name tag exists, append with John if so
```
{
    "(0010,0010)": {
        "tagName": "Patient's Name",
        "vr": "PN",
        "description": "Check if patient's name tag exists, append with John if so",
        "operations": {
            "EXIST": {
                "otherTagString": "",
                "IF_TRUE": {
                    "APPEND": {
                        "otherTagString": "",
                        "value": "John"
                    }
                }
            }
        }
    }
}
```

### Example 11: REGEX, PREPEND
Description: Check if patient's name is in specific pattern, prepend with John if so
```
{
    "(0010,0010)": {
        "tagName": "Patient's Name",
        "vr": "PN",
        "description": "Check if patient's name is in specific pattern, prepend with John if so",
        "operations": {
            "REGEX": {
                "otherTagString": "",
                "str_expr": "^[a-zA-Z].*[\\s\\.]*$",
                "IF_TRUE": {
                    "PREPEND": {
                        "otherTagString": "",
                        "value": "John"
                    }
                }
            }
        }
    }
}
```

