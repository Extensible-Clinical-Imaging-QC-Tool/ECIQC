#ifndef METADATAEDITOR_H_
#define METADATAEDITOR_H_

#include "../dcmod/apps/mdfdsman.cc"
#include "../dcmod/apps/mdfdsman.h"
//#include <dcmtk/dcmdata/dcitem.h>
class MetadataEditor : public MdfDatasetManager {
public:
  // Constructor
  MetadataEditor(OFString file_path);

  OFCondition setTag(OFString str);
  void setTag(DcmTagKey key);
  DcmTagKey getTagKey();
  OFString getTagString();
  OFBool exists(OFBool searchIntoSub = OFFalse);
  OFBool exists(OFString otherTagString, OFBool searchIntoSub= OFFalse);
  OFBool exists(const DcmTagKey &key, OFBool searchIntoSub = OFFalse);
  OFBool match(OFString str_expr, OFCondition &flag);
  OFCondition modify(OFString newValue, OFBool only_overwrite);
  OFCondition modify(OFString newValue, OFBool only_overwrite,
                     OFString otherTagString);
  OFCondition modify(OFString newValue, OFBool only_overwrite,
                     DcmTagKey otherTagKey);
  OFCondition copyTag(OFString TagFrom, OFString TagTo);

  DcmDataset *dset;

private:
  OFCondition stringToKey(OFString str, DcmTagKey &key);

  OFString tagString;
  DcmTagKey tagKey;
};

#endif // METADATAEDITOR_H_