#include <iostream>
#include <regex>
//#include "../dcmdata/apps/mdfdsman.h"
#include <dcmtk/dcmpstat/dcmpstat.h>
#include "MetadataEditor.hpp"



// Constructor(s)
MetadataEditor::MetadataEditor(OFString file_path) {
  OFCondition result = loadFile(file_path.c_str());
  if (result.bad()) {
    std::cout << "Constructor error: " << result.text();
  }
  dset = getDataset();
}

MetadataEditor::MetadataEditor(DcmDataset* dataset) {
  dset = dataset;
}

////////////////////////////////////////////////////////////////////
/*                    Public Member Functions                     */
////////////////////////////////////////////////////////////////////

// Set the tag data members
OFCondition MetadataEditor::setTag(OFString str) {
  tagString = str;
  OFString temp = str.c_str();
  OFCondition resultCond = splitTagPath(temp, tagKey);
  return resultCond;
}

void MetadataEditor::setTag(DcmTagKey key) {
  tagKey = key;
  tagString = tagKey.toString();
}

// Get tag data members
DcmTagKey MetadataEditor::getTagKey() { return tagKey; }
OFString MetadataEditor::getTagString() { return tagString; }

// Does  'this' tag exist?
OFBool MetadataEditor::exists(OFBool searchIntoSub) {
  OFBool result = dset->tagExists(tagKey, searchIntoSub);
  return result;
}

// Does another tag exist?
OFBool MetadataEditor::exists(const DcmTagKey &otherTagKey,
                              OFBool searchIntoSub) {
  OFBool result = dset->tagExists(otherTagKey, searchIntoSub);
  return result;
}

OFBool MetadataEditor::exists(OFString otherTagString, OFBool searchIntoSub) {
  DcmTagKey otherTagKey;
  OFCondition resultCond = stringToKey(otherTagString, otherTagKey);

  OFBool result = dset->tagExists(otherTagKey, searchIntoSub);
  return result;
}

// Allows editing the value at the current or a different tag
OFCondition MetadataEditor::modify(OFString newValue, OFBool only_overwrite) {
  OFCondition resultCond =
      modifyOrInsertPath(tagString, newValue, only_overwrite);
  return resultCond;
}

OFCondition MetadataEditor::modify(OFString newValue, OFString otherTagString,
                                   OFBool only_overwrite) {
  OFCondition resultCond =
      modifyOrInsertPath(otherTagString, newValue, only_overwrite);
  return resultCond;
}

OFCondition MetadataEditor::modify(OFString newValue, DcmTagKey otherTagKey,
                                   OFBool only_overwrite) {
  OFCondition resultCond =
      modifyOrInsertPath(otherTagKey.toString(), newValue, only_overwrite);
  return resultCond;
}

// Check if the value at 'this' tag matches the regex expression
OFBool MetadataEditor::match(OFString str_expr, OFCondition &flag, const unsigned long pos) {
  // Ensure the element specified by the tag exists before matching
  if (exists(OFFalse)) {
    OFString str;
    flag = dset->findAndGetOFString(tagKey, str,pos);

    std::regex expr(str_expr.c_str());
    return std::regex_match(str.c_str(), expr);
  } else {
    flag = makeOFCondition(OFM_dcmdata, 23, OF_error,
                           "element specified by the tag does not exst");
    return 0;
  }
}

OFBool MetadataEditor::match(OFString otherTagString, OFString str_expr,
                             OFCondition &flag, const unsigned long pos) {
  // Ensure the element specified by the tag exists before matching
  if (exists(OFFalse)) {
    OFString str;
    DcmTagKey otherTagKey;
    flag = stringToKey(otherTagString, otherTagKey);
    if (flag.bad()) {
      std::cout << flag.text() << std::endl;
      return 0;
    }
    flag = dset->findAndGetOFString(otherTagKey, str,pos);

    std::regex expr(str_expr.c_str());
    return std::regex_match(str.c_str(), expr);
  } else {
    flag = makeOFCondition(OFM_dcmdata, 23, OF_error,
                           "element specified by the tag does not exst");
    return 0;
  }
}

OFBool MetadataEditor::match(DcmTagKey otherTagKey, OFString str_expr,
                             OFCondition &flag, const unsigned long pos) {
  // Ensure the element specified by the tag exists before matching
  if (exists(OFFalse)) {
    OFString str;
    flag = dset->findAndGetOFString(otherTagKey, str,pos);

    std::regex expr(str_expr.c_str());
    return std::regex_match(str.c_str(), expr);
  } else {
    flag = makeOFCondition(OFM_dcmdata, 23, OF_error,
                           "element specified by the tag does not exst");
    return 0;
  }
}

// Copy Tag
OFCondition MetadataEditor::copy(DcmTagKey otherTagKey, int posFrom,
                                    int posTo, OFBool copyToThis,
                                    OFBool replace, OFBool searchIntoSub) {
  if (!exists(otherTagKey)) {
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
    if (resGet.bad()) {
      return resGet;
    }

    unsigned long destVM = destElement->getVM();
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
