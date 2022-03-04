#ifndef VALIDATOR_H // Include guard
#define VALIDATOR_H

// Packages to include
#include <dcmtk/config/osconfig.h> 
#include <dcmtk/dcmpstat/dcmpstat.h>

class Validator
{
    // Attributes
    private:
        DcmFileFormat *file;

    public:
        const char *file_name;
        DcmDataset *dataset;
        OFString validation_results;
        OFBool validation_output = OFTrue;

    // Default constructor
    // Loads the DICOM FILE to be tested 
    // from a file path
    Validator(const char *fname);

    // Alternative constructor
    // Loads the DICOM FILE to be tested 
    // from a DICOM dataset
    Validator(DcmDataset* dataset);

    // Member functions

    // Validate if in DICOM format
    // Not necessary if passing the DICOM dataset directly to the class
    OFBool dicomFormatValidation();

    // Validate the existence of a tag 
    OFBool validateTag(const DcmTagKey &key);
    OFBool validateTag(OFString tag_name);

    // Validate the existence of a tag and if there is a 
    // non-empty value
    OFBool validateTagValue(const DcmTagKey &key);
    OFBool validateTagValue(OFString tag_name);

    // Validate the existence of a tag, if there is a 
    // non-empty value, and if it is in a specified
    // range
    OFBool validateTagValueRange(const DcmTagKey &key, 
                                 const double lower_bound, 
                                 const double upper_bound);
    OFBool validateTagValueRange(OFString tag_name, 
                                 const double lower_bound, 
                                 const double upper_bound);

    // Write validation output to a .txt file
    // DEPRECATED
    void validationFileWriter();

    // Write validation output to the Text Value tag,
    // a tag with a VR of Ulimited Text
    void validationOutputTagWriter();

    // Get tag key from tag name using a dictionary
    static DcmTagKey getTagKeyFromDictionary(OFString tag_name);

    // Get tag name from tag key using a dictionary
    static OFString getTagNameFromDictionary(const DcmTagKey &key, 
                                             const char *privCreator = NULL);

};

#endif /* VALIDATOR_H */