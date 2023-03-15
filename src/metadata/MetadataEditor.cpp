#include <iostream>
#include <regex>
//#include "../dcmdata/apps/mdfdsman.h"
#include <dcmtk/dcmpstat/dcmpstat.h>
#include "MetadataEditor.hpp"

// TODO: AUDIT TRAIL OF VALIDATION AND ALL CHANGES MADE

// Constructor(s)
MetadataEditor::MetadataEditor() {}

MetadataEditor::MetadataEditor(DcmDataset* dataset) {
  setDset(dataset);
}

MetadataEditor::MetadataEditor(const OFString& file_path) {
  OFCondition result = setDset(file_path);
  if (result.bad()) {
    std::cout << "Error loading file: " << file_path << result.text();
  }
}

void MetadataEditor::setDset(DcmDataset* dataset) {
//   std::cout << "set dataset from DcmDataset" << std::endl;
  OFCondition result = loadFile(dataset);
  dset = dataset;
}

OFCondition MetadataEditor::setDset(const OFString& file_path) {
//   std::cout << "set dataset from path: " << file_path << std::endl;
  OFCondition result = loadFile(file_path.c_str());
  dset = getDataset();
  return result;
}

////////////////////////////////////////////////////////////////////
/*                    Public Member Functions                     */
////////////////////////////////////////////////////////////////////


// Set the tag data members
OFCondition MetadataEditor::setTag(const OFString& str) {
  tagString = str;
  OFString temp = str.c_str();
  OFCondition resultCond = splitTagPath(temp, tagKey);
  return resultCond;
}

void MetadataEditor::setTag(const DcmTagKey& key) {
  tagKey = key;
  tagString = tagKey.toString();
}

// Get tag data members
DcmTagKey MetadataEditor::getTagKey() { return tagKey; }
OFString MetadataEditor::getTagString() { return tagString; }

// Does  'this' tag exist?
OFCondition MetadataEditor::exists(OFBool searchIntoSub) {
  OFBool result = dset->tagExists(tagKey, searchIntoSub);
  std::cout << "Exists checked\t" << result << std::endl;

//  return result;
  if (result){
      return makeOFCondition(OFM_dcmdata, 23, OF_ok, "This tag does exist");
  }
  else {
      std::cout << "tag does not exist";
      return makeOFCondition(OFM_dcmdata, 23, OF_error, "This tag does not exist");
  }
}

// Does another tag exist?
OFCondition MetadataEditor::exists(const DcmTagKey &otherTagKey,
                              OFBool searchIntoSub) {
  OFBool result = dset->tagExists(otherTagKey, searchIntoSub);
//  return result;
    if (result){
        return makeOFCondition(OFM_dcmdata, 23, OF_ok, "This tag does exist");
    }
    else {
        return makeOFCondition(OFM_dcmdata, 23, OF_error, "This tag does not exist");
    }
}

OFCondition MetadataEditor::exists(const OFString& otherTagString, OFBool searchIntoSub) {
  DcmTagKey otherTagKey;
  OFCondition resultCond = stringToKey(otherTagString, otherTagKey);

  OFBool result = dset->tagExists(otherTagKey, searchIntoSub);
//  return result;
    if (result){
        return makeOFCondition(OFM_dcmdata, 23, OF_ok, "This tag does exist");
    }
    else {
        return makeOFCondition(OFM_dcmdata, 23, OF_error, "This tag does not exist");
    }
}

// Allows editing the value at the current or a different tag
// Use newValue = NULL to "clear" the value at a tag
OFCondition MetadataEditor::modify(const OFString& newValue, OFBool only_overwrite) {
  OFCondition resultCond =
      modifyOrInsertPath(tagString, newValue, only_overwrite);
  return resultCond;
}

OFCondition MetadataEditor::modify(const OFString& newValue, const OFString& otherTagString,
                                   OFBool only_overwrite) {
  OFCondition resultCond =
      modifyOrInsertPath(otherTagString, newValue, only_overwrite);
  return resultCond;
}

OFCondition MetadataEditor::modify(const OFString& newValue, const DcmTagKey& otherTagKey,
                                   OFBool only_overwrite) {
  OFCondition resultCond =
      modifyOrInsertPath(otherTagKey.toString(), newValue, only_overwrite);
  return resultCond;
}

// Check if the value at 'this' tag matches the regex expression
OFCondition MetadataEditor::match(const OFString& str_expr, OFCondition &flag, const unsigned long pos) {
  // Ensure the element specified by the tag exists before matching
  if (exists(OFFalse).good()) {
    OFString str;
    flag = dset->findAndGetOFString(tagKey, str,pos);

    std::regex expr(str_expr.c_str());
    std::cout << "\t regex match\t" << str_expr << "\t" << str;
    if (std::regex_match(str.c_str(), expr)) {
      return makeOFCondition(OFM_dcmdata, 23, OF_ok, "Value specified by the tag matches the regex expression");
    } else {
      return makeOFCondition(OFM_dcmdata, 23, OF_error,
                             "Value specified by the tag doesn't match the regex expression");
    }
  } else {
      return makeOFCondition(OFM_dcmdata, 23, OF_error,
                             "element specified by the tag does not exist");
  }
}

OFCondition MetadataEditor::match(const OFString& otherTagString, const OFString& str_expr,
                             OFCondition &flag, const unsigned long pos) {
  // Ensure the element specified by the tag exists before matching
  if (exists(otherTagString,OFFalse).good()) {
    OFString str;
    DcmTagKey otherTagKey;
    flag = stringToKey(otherTagString, otherTagKey);
    if (flag.bad()) {
      return flag;
    }
    flag = dset->findAndGetOFString(otherTagKey, str,pos);

    std::regex expr(str_expr.c_str());
      if (std::regex_match(str.c_str(), expr)) {
          return makeOFCondition(OFM_dcmdata, 23, OF_ok, "Value specified by the tag matches the regex expression");
      } else {
          return makeOFCondition(OFM_dcmdata, 23, OF_error,
                                 "Value specified by the tag doesn't match the regex expression");
      }
  } else {
      return makeOFCondition(OFM_dcmdata, 23, OF_error,
                             "element specified by the tag does not exist");
  }
}

OFCondition MetadataEditor::match(const DcmTagKey& otherTagKey, const OFString& str_expr,
                             OFCondition &flag, const unsigned long pos) {
  // Ensure the element specified by the tag exists before matching
  if (exists(otherTagKey,OFFalse).good()) {
    OFString str;
    flag = dset->findAndGetOFString(otherTagKey, str,pos);

    std::regex expr(str_expr.c_str());
      if (std::regex_match(str.c_str(), expr)) {
          return makeOFCondition(OFM_dcmdata, 23, OF_ok, "Value specified by the tag matches the regex expression");
      } else {
          return makeOFCondition(OFM_dcmdata, 23, OF_error,
                                 "Value specified by the tag doesn't match the regex expression");
      }
  } else {
      return makeOFCondition(OFM_dcmdata, 23, OF_error,
                             "element specified by the tag does not exist");
  }
}

OFCondition MetadataEditor::equals(const OFString& str_expr, OFCondition &flag, const unsigned long pos){
    // Ensure the element specified by the tag exists before matching
    std::cout << "Inside equals" << std::endl;
    if (exists(OFFalse).good()) {
        OFString str;
        flag = dset->findAndGetOFString(tagKey, str,pos);
        std::cout << "Got value\t" << str << std::endl;

//        std::string expr = str_expr.c_str();
        std::cout << "Got expr\t" << str_expr << std::endl;
        std::cout << "compare\t" << str_expr.compare(str) << std::endl;
        if (str_expr.compare(str) == 0) {
            return makeOFCondition(OFM_dcmdata, 23, OF_ok, "Value specified by the tag matches the string");
        } else {
            return makeOFCondition(OFM_dcmdata, 23, OF_error,
                                   "Value specified by the tag doesn't match the string");
        }
    } else {
        std::cout << "Tag doesn't exist?" << std::endl;
        return makeOFCondition(OFM_dcmdata, 23, OF_error,
                               "element specified by the tag does not exist or is not a string");
    }
}

OFCondition MetadataEditor::equals(const OFString& otherTagString, const OFString& str_expr,
                                  OFCondition &flag, const unsigned long pos) {
    // Ensure the element specified by the tag exists before matching
    if (exists(otherTagString,OFFalse).good()) {
        OFString str;
        DcmTagKey otherTagKey;
        flag = stringToKey(otherTagString, otherTagKey);
        if (flag.bad()) {
            return flag;
        }
        flag = dset->findAndGetOFString(otherTagKey, str,pos);

        std::string expr = str_expr.c_str();
        if (str.c_str() == expr) {
            return makeOFCondition(OFM_dcmdata, 23, OF_ok, "Value specified by the tag matches the string");
        } else {
            return makeOFCondition(OFM_dcmdata, 23, OF_error,
                                   "Value specified by the tag doesn't match the string");
        }
    } else {
        return makeOFCondition(OFM_dcmdata, 23, OF_error,
                               "element specified by the tag does not exist or is not a string");
    }
}

OFCondition MetadataEditor::equals(const DcmTagKey& otherTagKey, const OFString& str_expr,
                                  OFCondition &flag, const unsigned long pos) {
    // Ensure the element specified by the tag exists before matching
    if (exists(otherTagKey,OFFalse).good()) {
        OFString str;
        flag = dset->findAndGetOFString(otherTagKey, str,pos);

        std::string expr = str_expr.c_str();
        if (str.c_str() == expr) {
            return makeOFCondition(OFM_dcmdata, 23, OF_ok, "Value specified by the tag matches the string");
        } else {
            return makeOFCondition(OFM_dcmdata, 23, OF_error,
                                   "Value specified by the tag doesn't match the string");
        }
    } else {
        return makeOFCondition(OFM_dcmdata, 23, OF_error,
                               "element specified by the tag does not exist or is not a string");
    }
}

OFCondition MetadataEditor::equals(Float64 compare_value, OFCondition &flag, const unsigned long pos){
    // Ensure the element specified by the tag exists before matching
    DcmElement *thisElement;

    if (exists(OFFalse).bad()) {
        return makeOFCondition(OFM_dcmdata, 23, OF_error,
                               "tag does not exist in DICOM file");
    }

    dset->findAndGetElement(tagKey, thisElement, OFFalse);
    DcmTag thisTag{thisElement->getTag()};
    OFCondition resGet;
    OFCondition resCompare;
    Float64 thisValue;

    switch (thisTag.getEVR()){
        case EVR_US: {
            Uint16 uShort;
            resGet = thisElement->getUint16(uShort, pos);
            thisValue = static_cast<double>(uShort);
            break;
        }
            // Unsigned long integer
        case EVR_UL: {
            Uint32 uLong;
            resGet = thisElement->getUint32(uLong, pos);
            thisValue = static_cast<double>(uLong);
            break;
        }
            // Signed long integer
        case EVR_SL: {
            Sint32 sLong;
            resGet = thisElement->getSint32(sLong, pos);
            thisValue = static_cast<double>(sLong);
            break;
        }
            // Signed short integer
        case EVR_SS: {
            Sint16 sShort;
            resGet = thisElement->getSint16(sShort, pos);
            thisValue = static_cast<double>(sShort);
            break;
        }
            // Single precision float
        case EVR_FL: {
            Float32 fSingle;
            resGet = thisElement->getFloat32(fSingle, pos);
            thisValue = static_cast<double>(fSingle);
            break;
        }
            // Double precison float
        case EVR_FD: {
            Float64 fDouble;
            resGet = thisElement->getFloat64(fDouble, pos);
            thisValue = fDouble;
            break;
        }
            // Anything that can be dealt with as a string
        default: {
            flag = makeOFCondition(OFM_dcmdata, 23, OF_error, "Value at tag is not numeric");
            return flag;
        }

    }

    if (resGet.good()){
        //compare to 1e-8 tolerance to avoid precision errors
        if (std::abs(thisValue-compare_value) < 1e-8){
            return makeOFCondition(OFM_dcmdata, 23, OF_ok, "Value at tag equal to given double");
        }
        else {
            return makeOFCondition(OFM_dcmdata, 23, OF_error, "Value at tag not equal to given double");
        }
    }

    return resGet;
}

OFCondition MetadataEditor::equals(const DcmTagKey& otherTagKey, Float64 compare_value, OFCondition &flag,
                                   const unsigned long pos){
    // Ensure the element specified by the tag exists before matching
    DcmElement *thisElement;

    if (exists(otherTagKey,OFFalse).bad()) {
        return makeOFCondition(OFM_dcmdata, 23, OF_error,
                               "tag does not exist in DICOM file");
    }

    dset->findAndGetElement(otherTagKey, thisElement, OFFalse);
    DcmTag thisTag{thisElement->getTag()};
    OFCondition resGet;
    OFCondition resCompare;
    Float64 thisValue;

    switch (thisTag.getEVR()){
        case EVR_US: {
            Uint16 uShort;
            resGet = thisElement->getUint16(uShort, pos);
            thisValue = static_cast<double>(uShort);
            break;
        }
            // Unsigned long integer
        case EVR_UL: {
            Uint32 uLong;
            resGet = thisElement->getUint32(uLong, pos);
            thisValue = static_cast<double>(uLong);
            break;
        }
            // Signed long integer
        case EVR_SL: {
            Sint32 sLong;
            resGet = thisElement->getSint32(sLong, pos);
            thisValue = static_cast<double>(sLong);
            break;
        }
            // Signed short integer
        case EVR_SS: {
            Sint16 sShort;
            resGet = thisElement->getSint16(sShort, pos);
            thisValue = static_cast<double>(sShort);
            break;
        }
            // Single precision float
        case EVR_FL: {
            Float32 fSingle;
            resGet = thisElement->getFloat32(fSingle, pos);
            thisValue = static_cast<double>(fSingle);
            break;
        }
            // Double precison float
        case EVR_FD: {
            Float64 fDouble;
            resGet = thisElement->getFloat64(fDouble, pos);
            thisValue = fDouble;
            break;
        }
            // Anything that can be dealt with as a string
        default: {
            flag = makeOFCondition(OFM_dcmdata, 23, OF_error, "Value at tag is not numeric");
            return flag;
        }
    }

    if (resGet.good()){
        //compare to 1e-8 tolerance to avoid precision errors
        if (std::abs(thisValue-compare_value) < 1e-8){
            return makeOFCondition(OFM_dcmdata, 23, OF_ok, "Value at tag equal to given double");
        }
        else {
            return makeOFCondition(OFM_dcmdata, 23, OF_error, "Value at tag not equal to given double");
        }
    }

    return resGet;
}

OFCondition MetadataEditor::equals(const OFString& otherTagString, Float64 compare_value, OFCondition &flag,
                                   const unsigned long pos){
    // Ensure the element specified by the tag exists before matching
    if (exists(otherTagString,OFFalse).bad()) {
        return makeOFCondition(OFM_dcmdata, 23, OF_error,
                               "tag does not exist in DICOM file");
    }
    DcmTagKey otherTagKey;
    flag = stringToKey(otherTagString, otherTagKey);
    if (flag.bad()) {
        std::cout << flag.text() << std::endl;
        return flag;
    }

    return MetadataEditor::equals(otherTagKey, compare_value, flag, pos);
}

OFCondition MetadataEditor::greaterOrLessThan(Float64 compare_value, OFBool greaterThan, OFCondition &flag,
                                              const unsigned long pos){
    // Ensure the element specified by the tag exists before matching
    DcmElement *thisElement = nullptr;

    if (exists(OFFalse).bad()) {
        return makeOFCondition(OFM_dcmdata, 23, OF_error,
                               "tag does not exist in DICOM file");
    }

    dset->findAndGetElement(tagKey, thisElement, OFFalse);
    DcmTag thisTag{thisElement->getTag()};
    OFCondition resGet;
    OFCondition resCompare;
    Float64 thisValue;

    switch (thisTag.getEVR()){
        case EVR_US: {
            Uint16 uShort;
            resGet = thisElement->getUint16(uShort, pos);
            thisValue = static_cast<double>(uShort);
            break;
        }
            // Unsigned long integer
        case EVR_UL: {
            Uint32 uLong;
            resGet = thisElement->getUint32(uLong, pos);
            thisValue = static_cast<double>(uLong);
            break;
        }
            // Signed long integer
        case EVR_SL: {
            Sint32 sLong;
            resGet = thisElement->getSint32(sLong, pos);
            thisValue = static_cast<double>(sLong);
            break;
        }
            // Signed short integer
        case EVR_SS: {
            Sint16 sShort;
            resGet = thisElement->getSint16(sShort, pos);
            thisValue = static_cast<double>(sShort);
            break;
        }
            // Single precision float
        case EVR_FL: {
            Float32 fSingle;
            resGet = thisElement->getFloat32(fSingle, pos);
            thisValue = static_cast<double>(fSingle);
            break;
        }
            // Double precison float
        case EVR_FD: {
            Float64 fDouble;
            resGet = thisElement->getFloat64(fDouble, pos);
            thisValue = fDouble;
            break;
        }
            // Anything that can be dealt with as a string
        default: {
            flag = makeOFCondition(OFM_dcmdata, 23, OF_error, "Value at tag is not numeric");
            return flag;
        }

    }

    if (resGet.good()){
        if (greaterThan){
            if (thisValue>compare_value){
                return makeOFCondition(OFM_dcmdata, 23, OF_ok, "Value at tag greater than given double");
            }
            else {
                return makeOFCondition(OFM_dcmdata, 23, OF_error, "Value at tag not greater than given double");
            }
        }
        else {
            if (thisValue<compare_value){
                return makeOFCondition(OFM_dcmdata, 23, OF_ok, "Value at tag less than given double");
            }
            else {
                return makeOFCondition(OFM_dcmdata, 23, OF_error, "Value at tag not less than given double");
            }
        }
    }

    return resGet;
}

OFCondition MetadataEditor::greaterOrLessThan(const DcmTagKey& otherTagKey, Float64 compare_value, OFBool greaterThan,
                                              OFCondition &flag, const unsigned long pos){
    // Ensure the element specified by the tag exists before matching
    DcmElement *thisElement = nullptr;

    if (exists(otherTagKey,OFFalse).bad()) {
        return makeOFCondition(OFM_dcmdata, 23, OF_error,
                               "tag does not exist in DICOM file");
    }

    dset->findAndGetElement(otherTagKey, thisElement, OFFalse);
    DcmTag thisTag{thisElement->getTag()};
    OFCondition resGet;
    OFCondition resCompare;
    Float64 thisValue;

    switch (thisTag.getEVR()){
        case EVR_US: {
            Uint16 uShort;
            resGet = thisElement->getUint16(uShort, pos);
            thisValue = static_cast<double>(uShort);
            break;
        }
            // Unsigned long integer
        case EVR_UL: {
            Uint32 uLong;
            resGet = thisElement->getUint32(uLong, pos);
            thisValue = static_cast<double>(uLong);
            break;
        }
            // Signed long integer
        case EVR_SL: {
            Sint32 sLong;
            resGet = thisElement->getSint32(sLong, pos);
            thisValue = static_cast<double>(sLong);
            break;
        }
            // Signed short integer
        case EVR_SS: {
            Sint16 sShort;
            resGet = thisElement->getSint16(sShort, pos);
            thisValue = static_cast<double>(sShort);
            break;
        }
            // Single precision float
        case EVR_FL: {
            Float32 fSingle;
            resGet = thisElement->getFloat32(fSingle, pos);
            thisValue = static_cast<double>(fSingle);
            break;
        }
            // Double precison float
        case EVR_FD: {
            Float64 fDouble;
            resGet = thisElement->getFloat64(fDouble, pos);
            thisValue = fDouble;
            break;
        }
            // Anything that can be dealt with as a string
        default: {
            flag = makeOFCondition(OFM_dcmdata, 23, OF_error, "Value at tag is not numeric");
            return flag;
        }
    }

    if (resGet.good()){
        if (greaterThan){
            if (thisValue>compare_value){
                return makeOFCondition(OFM_dcmdata, 23, OF_ok, "Value at tag greater than given double");
            }
            else {
                return makeOFCondition(OFM_dcmdata, 23, OF_error, "Value at tag not greater than given double");
            }
        }
        else {
            if (thisValue<compare_value){
                return makeOFCondition(OFM_dcmdata, 23, OF_ok, "Value at tag less than given double");
            }
            else {
                return makeOFCondition(OFM_dcmdata, 23, OF_error, "Value at tag not less than given double");
            }
        }
    }

    return resGet;
}

OFCondition MetadataEditor::greaterOrLessThan(const OFString& otherTagString, Float64 compare_value, OFBool greaterThan,
                                              OFCondition &flag, const unsigned long pos) {
    // Ensure the element specified by the tag exists before matching
    if (exists(otherTagString,OFFalse).bad()) {
        return makeOFCondition(OFM_dcmdata, 23, OF_error,
                               "tag does not exist in DICOM file");
    }
    DcmTagKey otherTagKey;
    flag = stringToKey(otherTagString, otherTagKey);
    if (flag.bad()) {
        std::cout << flag.text() << std::endl;
        return flag;
    }

    return MetadataEditor::greaterOrLessThan(otherTagKey, compare_value, greaterThan, flag, pos);
}



// Copy Tag
OFCondition MetadataEditor::copy(const DcmTagKey& otherTagKey, const unsigned long posTo,
                                    const unsigned long posFrom, OFBool copyToThis,
                                    OFBool searchIntoSub, OFBool replace) {
  if (exists(otherTagKey).bad()) {
    return makeOFCondition(OFM_dcmdata, 23, OF_error,
                           "tag does not exist in DICOM file");
  }
  
  // Get the two relevant elements and determine the origin and destination
  // for the copy command
  DcmElement *thisElement, *otherElement, *destElement, *originElement;
  dset->findAndGetElement(tagKey, thisElement, searchIntoSub);
  dset->findAndGetElement(otherTagKey, otherElement, searchIntoSub);
  if (copyToThis) {
    originElement = otherElement;
    destElement = thisElement;
  } else {
    originElement = thisElement;
    destElement = otherElement;
  }

  // Choose method based on element VR. Both elements should have the same VR
  DcmTag destTag{destElement->getTag()};
  OFCondition resGet, resPut;
  switch (destTag.getEVR()) {
  // Unsigned short integer
  case EVR_US:
    Uint16 uShort;
    resGet = originElement->getUint16(uShort, posFrom);
    resPut = destElement->putUint16(uShort, posTo);
    break;
  // Unsigned long integer
  case EVR_UL:
    Uint32 uLong;
    resGet = originElement->getUint32(uLong, posFrom);
    resPut = destElement->putUint32(uLong, posTo);
    break;
  // Signed long integer
  case EVR_SL:
    Sint32 sLong;
    resGet = originElement->getSint32(sLong, posFrom);
    resPut = destElement->putSint32(sLong, posTo);
    break;
  // Signed short integer
  case EVR_SS:
    Sint16 sShort;
    resGet = originElement->getSint16(sShort, posFrom);
    resPut = destElement->putSint16(sShort, posTo);
    break;
  // Single precision float
  case EVR_FL:
    Float32 fSingle;
    resGet = originElement->getFloat32(fSingle, posFrom);
    resPut = destElement->putFloat32(fSingle, posTo);
    break;
  // Double precison float
  case EVR_FD:
    Float64 fDouble;
    resGet = originElement->getFloat64(fDouble, posFrom);
    resPut = destElement->putFloat64(fDouble, posTo);
    break;
  // Anything that can be dealt with as a string
  default:
    //std::cout << "here in default" << std::endl;
    OFString destStringVal, originStringVal;
    OFCondition res;
    resGet = originElement->getOFString(originStringVal, posFrom);
    std::cout << "posFrom is " << posFrom << " #####" << std::endl;
    std::cout << "originStringVal is " << originStringVal << " #####" << std::endl;
    if (resGet.bad()) {
      return resGet;
    }

    unsigned long destVM = destElement->getVM();
    std::cout << "destVM is " << destVM << std::endl;
    if (replace == OFTrue){
        std::cout << "replace is OFTrue!" << std::endl;
    }
    else{
        std::cout << "replace is OFFalse!" << std::endl;
    }
    if ((destVM == 0) || ((destVM == 1) & (replace == OFTrue))) {
      return destElement->putString(originStringVal.c_str());
    } else {
      // Get all values at destination element
      resGet = destElement->getOFStringArray(destStringVal);
      if (resGet.bad()) {
        return resGet;
      }
      std::vector<OFString> strs = strValsToVector(destStringVal);
      // replace or add value at destination position
      if (replace) {
        strs.at(posTo) = originStringVal;
      } else {
        auto it = strs.begin();
        strs.insert(it + posTo, originStringVal);
      }
      return destElement->putOFStringArray(vectorToStrVals(strs));
    }
    break;
  }
  return (resGet.bad() ? resGet : resPut);
}

OFCondition MetadataEditor::copy(const OFString& otherTagString, const unsigned long posTo,
                                 const unsigned long posFrom, OFBool copyToThis,
                                 OFBool searchIntoSub, OFBool replace) {
    if (exists(otherTagString,OFFalse).bad()) {
        return makeOFCondition(OFM_dcmdata, 23, OF_error,
                               "tag does not exist in DICOM file");
    }
    DcmTagKey otherTagKey;
    OFCondition flag = stringToKey(otherTagString, otherTagKey);
    if (flag.bad()) {
        std::cout << flag.text() << std::endl;
        return flag;
    }
    return copy(otherTagKey, posTo, posFrom, copyToThis, searchIntoSub, replace);
}

OFCondition MetadataEditor::append(const OFString& appendValue, OFCondition &flag, const unsigned long pos) {
    if (exists(OFFalse).good()) {

        OFString oldValue;
        flag = dset->findAndGetOFString(tagKey, oldValue, pos);
        if (flag.bad()) {
            return flag;
        }
        OFString newValue = (oldValue + appendValue).c_str();
        OFCondition resultCond =
                modifyOrInsertPath(tagString, newValue, OFTrue);
        return resultCond;

    } else {
        return makeOFCondition(OFM_dcmdata, 23, OF_error,
                               "element specified by the tag does not exist or is not a string");
    }
}

OFCondition MetadataEditor::append(const OFString& appendValue, const OFString& otherTagString, OFCondition &flag,
                                   const unsigned long pos) {
    if (exists(otherTagString,OFFalse).good()) {
        DcmTagKey otherTagKey;
        flag = stringToKey(otherTagString, otherTagKey);
        if (flag.bad()) {
            std::cout << flag.text() << std::endl;
            return flag;
        }

        OFString oldValue;
        flag = dset->findAndGetOFString(otherTagKey, oldValue, pos);
        if (flag.bad()) {
            return flag;
        }
        OFString newValue = (oldValue + appendValue).c_str();
        OFCondition resultCond =
                modifyOrInsertPath(tagString, newValue, OFTrue);
        return resultCond;

    } else {
        return makeOFCondition(OFM_dcmdata, 23, OF_error,
                               "element specified by the tag does not exist or is not a string");
    }
}

OFCondition MetadataEditor::append(const OFString& appendValue, const DcmTagKey& otherTagKey, OFCondition &flag,
                                   const unsigned long pos) {
    if (exists(otherTagKey,OFFalse).good()) {
        OFString oldValue;
        flag = dset->findAndGetOFString(otherTagKey, oldValue, pos);
        if (flag.bad()) {
            return flag;
        }
        OFString newValue = (oldValue + appendValue).c_str();
        OFCondition resultCond =
                modifyOrInsertPath(tagString, newValue, OFTrue);
        return resultCond;

    } else {
        return makeOFCondition(OFM_dcmdata, 23, OF_error,
                               "element specified by the tag does not exist or is not a string");
    }
}

OFCondition MetadataEditor::prepend(const OFString& prependValue, OFCondition &flag,
                                    const unsigned long pos) {
    if (exists(OFFalse).good()) {

        OFString oldValue;
        flag = dset->findAndGetOFString(tagKey, oldValue, pos);
        if (flag.bad()) {
            return flag;
        }
        OFString newValue = (prependValue + oldValue).c_str();
        OFCondition resultCond =
                modifyOrInsertPath(tagString, newValue, OFTrue);
        return resultCond;

    } else {
        return makeOFCondition(OFM_dcmdata, 23, OF_error,
                               "element specified by the tag does not exist or is not a string");
    }
}

OFCondition MetadataEditor::prepend(const OFString& prependValue, const OFString& otherTagString, OFCondition &flag,
                                    const unsigned long pos) {
    if (exists(otherTagString,OFFalse).good()) {
        DcmTagKey otherTagKey;
        flag = stringToKey(otherTagString, otherTagKey);
        if (flag.bad()) {
            std::cout << flag.text() << std::endl;
            return flag;
        }

        OFString oldValue;
        flag = dset->findAndGetOFString(otherTagKey, oldValue, pos);
        if (flag.bad()) {
            return flag;
        }
        OFString newValue = (prependValue + oldValue).c_str();
        OFCondition resultCond =
                modifyOrInsertPath(tagString, newValue, OFTrue);
        return resultCond;

    } else {
        return makeOFCondition(OFM_dcmdata, 23, OF_error,
                               "element specified by the tag does not exist or is not a string");
    }
}

OFCondition MetadataEditor::prepend(const OFString& prependValue, const DcmTagKey& otherTagKey, OFCondition &flag,
                                    const unsigned long pos) {
    if (exists(otherTagKey,OFFalse).good()) {
        OFString oldValue;
        flag = dset->findAndGetOFString(otherTagKey, oldValue, pos);
        if (flag.bad()) {
            return flag;
        }
        OFString newValue = (prependValue + oldValue).c_str();
        OFCondition resultCond =
                modifyOrInsertPath(tagString, newValue, OFTrue);
        return resultCond;

    } else {
        return makeOFCondition(OFM_dcmdata, 23, OF_error,
                               "element specified by the tag does not exist or is not a string");
    }
}

OFCondition MetadataEditor::overwrite(const OFString& str_expr, const OFString& replaceString){
    if (exists(OFFalse).bad()) {
        return makeOFCondition(OFM_dcmdata, 23, OF_error,
                               "tag does not exist in DICOM file");
    }
    // Regex replace
    if (str_expr.length() != 0) {
        OFString currentValue;
        OFCondition flag = dset->findAndGetOFString(tagKey, currentValue);
        if (flag.bad()){
            return flag;
        }

        std::regex expr(str_expr.c_str());
        std::string repl = replaceString.c_str();
        std::string curr = currentValue.c_str();
        std::string newValue;
        std::regex_replace(std::back_inserter(newValue), curr.begin(), curr.end(), expr, repl);

        const char *newValueChar = newValue.c_str();
        OFString newVal = OFString(newValueChar, strlen(newValueChar));

        if (newVal == currentValue){
            return makeOFCondition(OFM_dcmdata, 23, OF_ok, "string not modified");
        }
        return modify(newVal, OFTrue);
    }
    else {
        return modify(replaceString, OFTrue);
    }
}

OFCondition MetadataEditor::overwrite(const OFString& otherTagString, const OFString& str_expr, const OFString& replaceString){
    if (exists(otherTagString, OFFalse).bad()) {
        return makeOFCondition(OFM_dcmdata, 23, OF_error,
                               "tag does not exist in DICOM file");
    }

    OFString currentValue;
    DcmTagKey otherTagKey;
    OFCondition resultCond = stringToKey(otherTagString, otherTagKey);
    OFCondition flag = dset->findAndGetOFString(otherTagKey, currentValue);
    if (flag.bad()){
        return flag;
    }

    std::regex expr(str_expr.c_str());
    std::string repl = replaceString.c_str();
    std::string curr = currentValue.c_str();
    std::string newValue;
    std::regex_replace(std::back_inserter(newValue), curr.begin(), curr.end(), expr, repl);

    const char *newValueChar = newValue.c_str();
    OFString newVal = OFString(newValueChar, strlen(newValueChar));

    if (newVal == currentValue){
        return makeOFCondition(OFM_dcmdata, 23, OF_ok, "string not modified");
    }
    return modify(newVal, otherTagString, OFTrue);
}

OFCondition MetadataEditor::overwrite(const DcmTagKey& otherTagKey, const OFString& str_expr, const OFString& replaceString){
    if (exists(otherTagKey, OFFalse).bad()) {
        return makeOFCondition(OFM_dcmdata, 23, OF_error,
                               "tag does not exist in DICOM file");
    }

    OFString currentValue;
    OFCondition flag = dset->findAndGetOFString(otherTagKey, currentValue);
    if (flag.bad()){
        return flag;
    }

    std::regex expr(str_expr.c_str());
    std::string repl = replaceString.c_str();
    std::string curr = currentValue.c_str();
    std::string newValue;
    std::regex_replace(std::back_inserter(newValue), curr.begin(), curr.end(), expr, repl);

    const char *newValueChar = newValue.c_str();
    OFString newVal = OFString(newValueChar, strlen(newValueChar));

    if (newVal == currentValue){
        return makeOFCondition(OFM_dcmdata, 23, OF_ok, "string not modified");
    }
    return modify(newVal, otherTagKey, OFTrue);
}

////////////////////////////////////////////////////////////////////
/*                    Private Member Functions                    */
////////////////////////////////////////////////////////////////////

OFCondition MetadataEditor::stringToKey(OFString str, DcmTagKey &key) {
  return splitTagPath(str, key);
}

std::vector<OFString> MetadataEditor::strValsToVector(OFString str) {
  std::vector<OFString> values;

  size_t pos = 0;
  while (pos != OFString_npos) {
    pos = str.find("\\");
    values.push_back(str.substr(0, pos));
    str = str.substr(pos + 1, OFString_npos);
  }

  return values;
}

OFString MetadataEditor::vectorToStrVals(std::vector<OFString> vec) {
  OFString newStringVals = "";
  for (OFString val : vec) {
    newStringVals += val;
    newStringVals += "\\";
  }

  

  return newStringVals.substr(0, newStringVals.length() - 1);
}
