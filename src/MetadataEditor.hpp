#ifndef METADATAEDITOR_H_
#define METADATAEDITOR_H_

#include "mdfdsman.h"
#include <dcmtk/dcmpstat/dcmpstat.h>
#include <vector>

class MetadataEditor : public MdfDatasetManager {
public:
  // Constructor
  MetadataEditor();
  MetadataEditor(OFString file_path);
  MetadataEditor(DcmDataset* dataset);

  DcmDataset* pathToDataset(OFString file_path);


  OFCondition setTag(OFString str);
  void setTag(DcmTagKey key);

  DcmTagKey getTagKey();

  OFString getTagString();

  OFBool exists(OFBool searchIntoSub = OFFalse);
  OFBool exists(OFString otherTagString, OFBool searchIntoSub = OFFalse);
  OFBool exists(const DcmTagKey &key, OFBool searchIntoSub = OFFalse);

  OFBool match(OFString str_expr, OFCondition &flag, const unsigned long pos = 0);
  OFBool match(OFString otherTagString, OFString str_expr, OFCondition &flag, const unsigned long pos = 0);
  OFBool match(DcmTagKey otherTagKey, OFString str_expr, OFCondition &flag, const unsigned long pos = 0);

  OFCondition modify(OFString newValue, OFBool only_overwrite);
  OFCondition modify(OFString newValue, OFString otherTagString,
                     OFBool only_overwrite);
  OFCondition modify(OFString newValue, DcmTagKey otherTagKey,
                     OFBool only_overwrite);

  OFCondition copy(DcmTagKey otherTagKey, int posTo = 0, int posFrom = 0,
                      OFBool copyToThis = OFTrue,
                      OFBool searchIntoSub = OFFalse, OFBool replace = OFTrue);

  DcmDataset *dset;

private:
  OFCondition stringToKey(OFString str, DcmTagKey &key);

  std::vector<OFString> strValsToVector(OFString str);
  OFString vectorToStrVals(std::vector<OFString> vec);

  OFString tagString;

  DcmTagKey tagKey; 
};

#endif // METADATAEDITOR_H_