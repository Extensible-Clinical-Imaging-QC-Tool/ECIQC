#include <iostream>
#include <regex>
//#include <dcmtk/dcmdata/dcitem.h>
#include "../dcmdata/apps/mdfdsman.h"
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

// Public Member Functions
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

OFBool MetadataEditor::match(OFString str_expr, OFCondition &flag) {
  // Ensure the element specified by the tag exists before matching
  if (exists(OFFalse)) {
    OFString str;
    flag = dset->findAndGetOFString(tagKey, str);

    std::string s(str.c_str());
    std::regex exp(str_expr.c_str());
    return std::regex_match(s, exp);
  } else {
    flag = makeOFCondition(OFM_dcmdata, 23, OF_error,
                           "element specified by the tag does not exst");
    return 0;
  }
}

// Overload match




// Copy Tag
OFCondition MetadataEditor::copyTag(OFString TagFromKey, OFString TagTo) {
  /* As is written, can only copy to tags with an identical VR to the
  origin tag */
  OFCondition a;
  return a;
}

// Private Member Functions
// key is the variable that will hold your results
OFCondition MetadataEditor::stringToKey(OFString str, DcmTagKey &key) {
  OFCondition result = splitTagPath(str, key);
  return result;
}

/*TODO Unknown types when copying?
Am I right in thinking the VR for an element should never change?

DcmTagKey might just be read in as a string either way?
*/
