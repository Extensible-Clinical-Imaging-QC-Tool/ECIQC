@page Configuration_gallery Configuration Gallery

@tableofcontents

This webpage provides a set of examples to exemplify the configuration syntax. For end-users, you can compare your needs with the short descriptions in the following summary table, and adjust the configuration file accordingly.

| Example  | Operators used | Short description |
|:--------------|:--------------:|:--------------:|
| Example 1     | AND, REMOVE    | Check if BitsAllocated between certain numbers, remove if not|
| Example 2     | OR, REMOVE     | Check if BitsAllocated between certain numbers, remove if not|
| Example 3     |

**ATTENTION**: the following examples only contain the "metadata" value in the configuration. For complete json files, please refer to the tutorial to add proper receiver, destination, quarantine and transfer syntax.
### Example 1:
Description: Check if BitsAllocated between 10 and 20, remove if not
```
{
    "(0028,0100)": {
        "tagName": "BitsAllocated",
        "vr": "CS",
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
Description: Check if BitsAllocated between 10 and 20, remove if not
```
{
    "(0028,0102)": {
        "tagName": "HighBit",
        "vr": "CS",
        "description": "Check if BitsAllocated between certain numbers, remove if not",
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
