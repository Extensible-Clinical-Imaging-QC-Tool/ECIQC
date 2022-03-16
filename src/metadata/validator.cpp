// Packages to include
#include "validator.hpp"
#include <iostream>
#include <fstream> // Stream class to both read from and write to files
#include <string>
//#include <type_traits> // part of the type support library, can verify types
#include <stdlib.h> // NULL pointer and convert string to double

//#include "dcmtk/config/osconfig.h"  //
#include "dcmtk/dcmdata/dcmetinf.h" //Interface of class DcmMetaInfo
// #include "dcmtk/dcmdata/dcuid.h" // Definitions of "well known" 
// DICOM Unique Identifiers, routines for finding and creating UIDs.
// dcmtk version names
//#include <dcmtk/dcmpstat/dcmpstat.h>
//#include <dcmtk/dcmdata/dcdatset.h>
//#include <dcmtk/dcmdata/dctk.h>
//#include <dcmtk/dcmnet/dstorscu.h>
//#include <dcmtk/dcmnet/scu.h>
//#include <dcmtk/ofstd/ofstdinc.h>

// Validator constructors
Validator::Validator(const char *fname)
{ 
    file_name = fname;
    file =  std::make_unique<DcmFileFormat>();
    OFCondition status;
    
    /*
    DcmFileFormat dfile;
    dataset = dfile.getDataset();
    OFCondition status;
    */

    if (OFStandard::fileExists(fname))
    {
        OFCondition status = file->loadFile(fname);
        //OFCondition status = dfile.loadFile(fname);
    } else {
        std::cout << "File does not exist" << std::endl;
    }
    
    if (status.good())
    {
        dataset = file->getDataset();
        //dataset = dfile.getDataset();
        std::cout << "class Constructor status: " << status.text() << std::endl;
    } else if (status.bad()) {
        std::cout << "class Constructor error: " << status.text() << std::endl;
    }
};

Validator::Validator(DcmDataset *dset) 
{
    dataset = dset;
}

// Validator member functions

/* NOTE: This test will fail if the dataset is passed directly to
 * to the constructor and not the file path of the dataset. In that
 * case the test is unecessary.
 * 
 * The following function was adapted from the dcmtk 
 *  "dcmftest.cc" file developed by
 *
 *    OFFIS e.V.
 *    R&D Division Health
 *    Escherweg 2
 *    D-26121 Oldenburg, Germany
 *
 *  Purpose: Test if a file uses DICOM Part 10 format. 
 *  
 *  The file is checked for the presence of the DICOM 
 *  "magic word" 'DICM' at byte position 128.
 *
 *  From dcmetinf.h:
 *  
 *  magic string identifying DICOM files
 *  define DCM_Magic "DICM"
 *
 *  length of magic string identifying DICOM files
 *  define DCM_MagicLen 4;
 *
 *  length of DICOM file preamble, in bytes
 *  define DCM_PreambleLen 128
 */

OFBool Validator::dicomFormatValidation()
{
    int failure_count = 0;
    OFBool validated = OFFalse;
    /*  The boolean type used throughout the DCMTK project.
    *   Mapped to the built-in type "bool" if the current C++ compiler 
    *   supports it. Mapped to int for old-fashioned compilers which do 
    *   not yet support bool.
    */

    const char *fname = file_name;
    FILE* f = NULL;

    if (fname && (strlen(fname) > 0)) f = fopen(fname, "rb"); { 
        if (f == 0) {
            validated = OFFalse;
        } else {
            char signature[4];
            if ((fseek(f, DCM_PreambleLen, SEEK_SET) < 0) ||
                (fread(signature, 1, DCM_MagicLen, f) != DCM_MagicLen)) {
                validated = OFFalse;
            } else if (strncmp(signature, DCM_Magic, DCM_MagicLen) != 0) {
                validated = OFFalse;
            } else {
                //  The file has been validated to have the DICOM 
                //  "magic word" 'DICM' at byte position 128.
                validated = OFTrue;
            }
            fclose(f);
        }
    }

    if (validated) {
        //COUT << "validated: " << fname << OFendl;
    } else {
        //COUT << "failed: " << fname << OFendl;
        failure_count++;
    }

    if (validation_output && validated) {
        validation_output = OFTrue;
    } else {
        validation_output = OFFalse;
    }

    //  if the file passes return OFTrue otherwise OFFalse
    return validated;
};

// Validate the existence of a tag 
// (tag key)
OFBool Validator::validateTag(const DcmTagKey &key)
{
    OFString result;
    OFString tag_name = getTagNameFromDictionary(key);
    OFString tag_key_string = key.toString();
    OFString tag_description = tag_name + " " + tag_key_string;

    OFBool validated = dataset->tagExists(key, OFFalse);

    if (validated) {
        result = tag_description.append(" tag existence: PASSED \n");
        validation_results.append(result);
    } else {
        result = tag_description.append(" tag existence: FAILED \n"); 
        validation_results.append(result); 
    }

    if (validation_output && validated) {
        validation_output = OFTrue;
    } else {
        validation_output = OFFalse;
    }
    
    return validated;
};

// Validate the existence of a tag 
// (tag name)
OFBool Validator::validateTag(OFString tag_name)
{
    OFString result;
    DcmTagKey key = getTagKeyFromDictionary(tag_name);
    OFString tag_key_string = key.toString();
    OFString tag_description = tag_name + " " + tag_key_string;

    OFBool validated = dataset->tagExists(key, OFFalse);

    if (validated) {
        result = tag_description.append(" tag existence: PASSED \n");
        validation_results.append(result);
    } else {
        result = tag_description.append(" tag existence: FAILED \n"); 
        validation_results.append(result); 
    }

    if (validation_output && validated) {
        validation_output = OFTrue;
    } else {
        validation_output = OFFalse;
    }
    
    return validated;
};

// Validate the existence of a tag and whether it has a non-empty value
// (tag key)
OFBool Validator::validateTagValue(const DcmTagKey &key)
{
    OFString result;
    OFString tag_name = getTagNameFromDictionary(key);
    OFString tag_key_string = key.toString();
    OFString tag_description = tag_name + " " + tag_key_string;

    OFBool validated = dataset->tagExistsWithValue(key, OFFalse);

    if (validated) {
        result = tag_description.append(" tag existence with non-empty value: PASSED \n");
        validation_results.append(result);
    } else {
        result = tag_description.append(" tag existence with non-empty value: FAILED \n"); 
        validation_results.append(result); 
    }

    if (validation_output && validated) {
        validation_output = OFTrue;
    } else {
        validation_output = OFFalse;
    }
    
    return validated;
};

// Validate the existence of a tag and whether it has a non-empty value
// (tag name)
OFBool Validator::validateTagValue(OFString tag_name)
{
    OFString result;
    DcmTagKey key = getTagKeyFromDictionary(tag_name);
    OFString tag_key_string = key.toString();
    OFString tag_description = tag_name + " " + tag_key_string;

    OFBool validated = dataset->tagExistsWithValue(key, OFFalse);

    if (validated) {
        result = tag_description.append(" tag existence with non-empty value: PASSED \n");
        validation_results.append(result);
    } else {
        result = tag_description.append(" tag existence with non-empty value: FAILED \n"); 
        validation_results.append(result); 
    }

    if (validation_output && validated) {
        validation_output = OFTrue;
    } else {
        validation_output = OFFalse;
    }
    
    return validated;
};

// Validate the existence, non-empty value, and range of a tag 
// (tag key)
OFBool Validator::validateTagValueRange(const DcmTagKey &key, 
                                        const double lower_bound, 
                                        const double upper_bound)
{
    OFBool validated;
    OFString result;
    OFString lower_bound_str = OFString(std::to_string(lower_bound).c_str());
    OFString upper_bound_str = OFString(std::to_string(upper_bound).c_str());
    OFString tag_name = getTagNameFromDictionary(key);
    OFString tag_key_string = key.toString();
    OFString tag_description = tag_name + " " + tag_key_string;

    OFBool value_test_outcome = dataset->tagExistsWithValue(key, OFFalse);

    OFString content_string;
    double content_double;

    if (value_test_outcome) {
        dataset->findAndGetOFString(key, content_string);

        // Convert string to a double
        content_double = atof(content_string.c_str());
        
        if(lower_bound < content_double && content_double < upper_bound) {
            validated = OFTrue;
        } else {
            validated = OFFalse;
        }
    } else {
        validated = OFFalse;
    }

    if (validated) {
        //result = tag_description.append(" tag existence with non-empty value in specified range: PASSED \n");
        result = tag_description.append(" tag existence with non-empty value" 
                                        " in specified range of " 
                                        + lower_bound_str
                                        + " to " 
                                        + upper_bound_str 
                                        + ": PASSED \n"
                                        );        
        validation_results.append(result);
    } else {
        //result = tag_description.append(" tag existence with non-empty value in specified range: FAILED \n"); 
        result = tag_description.append(" tag existence with non-empty value" 
                                        " in specified range of " 
                                        + lower_bound_str
                                        + " to " 
                                        + upper_bound_str 
                                        + ": FAILED \n"
                                        ); 
        validation_results.append(result); 
    }

    if (validation_output && validated) {
        validation_output = OFTrue;
    } else {
        validation_output = OFFalse;
    }
    
    return validated;
};

// Validate the existence, non-empty value, and range of a tag 
// (tag name)
OFBool Validator::validateTagValueRange(OFString tag_name, 
                                        const double lower_bound, 
                                        const double upper_bound)
{
    OFBool validated;
    OFString result;
    OFString lower_bound_str = OFString(std::to_string(lower_bound).c_str());
    OFString upper_bound_str = OFString(std::to_string(upper_bound).c_str());
    DcmTagKey key = getTagKeyFromDictionary(tag_name);
    OFString tag_key_string = key.toString();
    OFString tag_description = tag_name + " " + tag_key_string;

    OFBool value_test_outcome = dataset->tagExistsWithValue(key, OFFalse);

    OFString content_string;
    double content_double;

    if (value_test_outcome) {
        dataset->findAndGetOFString(key, content_string);

        // Convert string to a double
        content_double = atof(content_string.c_str());
        
        if(lower_bound < content_double && content_double < upper_bound) {
            validated = OFTrue;
        } else {
            validated = OFFalse;
        }
    } else {
        validated = OFFalse;
    }

    if (validated) {
        result = tag_description.append(" tag existence with non-empty value" 
                                        " in specified range of " 
                                        + lower_bound_str
                                        + " to " 
                                        + upper_bound_str 
                                        + ": PASSED \n"
                                        );    
        validation_results.append(result);
    } else {
        result = tag_description.append(" tag existence with non-empty value" 
                                        " in specified range of " 
                                        + lower_bound_str
                                        + " to " 
                                        + upper_bound_str 
                                        + ": FAILED \n"
                                        );     
        validation_results.append(result); 
    }

    if (validation_output && validated) {
        validation_output = OFTrue;
    } else {
        validation_output = OFFalse;
    }
    
    return validated;
};

/*  Writes the output of all the validations to a
*   text file. 
*/
void Validator::validationFileWriter()
{
    //validation_results.append("Validation file written");
    std::ofstream out("output.txt");
    out << validation_results;
    out.close();
}; 

/*  Writes the output of all the validations to an
*   Unlimited Text tag.
*/
void Validator::validationOutputTagWriter()
{
    // Text Value is a tag with a VR of Unlimited Text
    // (0040,A160)	UT	Text Value
    dataset->putAndInsertOFStringArray(DCM_TextValue, validation_results);
}; 

/* Get the tag key from the tag name using the DCMTK data dictionary
 *
 *
 * Method from mdfdsman.cc
 *
 *  Copyright (C) 2003-2017, OFFIS e.V.
 *  All rights reserved.  See COPYRIGHT file for details.
 *
 *  This software and supporting documentation were developed by
 *
 *    OFFIS e.V.
 *    R&D Division Health
 *    Escherweg 2
 *    D-26121 Oldenburg, Germany
 *
 *
 *  Module:  dcmdata
 *
 *  Author:  Michael Onken
 *
 *  Purpose: Class for modifying DICOM files
 *
 */
DcmTagKey Validator::getTagKeyFromDictionary(OFString tag_name)
{
    DcmTagKey key(0xffff,0xffff);
    const DcmDataDictionary& globalDataDict = dcmDataDict.rdlock();
    const DcmDictEntry *dicent = globalDataDict.findEntry(tag_name.c_str());

    // successful lookup in dictionary -> translate to tag and return
    if (dicent) {
        key = dicent->getKey();
    }
    dcmDataDict.rdunlock();

    return key;
}

// Get the tag name from the tag key using the DCMTK data dictionary
OFString Validator::getTagNameFromDictionary(const DcmTagKey &key, 
                                             const char *privCreator)
{   
    OFString tag_name = "????";
    const DcmDataDictionary& globalDataDict = dcmDataDict.rdlock();
    const DcmDictEntry *dicent = globalDataDict.findEntry(key, privCreator);

    // successful lookup in dictionary -> translate to tag name and return
    if (dicent) {
        tag_name = dicent->getTagName();
    }
    dcmDataDict.rdunlock();

    return tag_name;
}  