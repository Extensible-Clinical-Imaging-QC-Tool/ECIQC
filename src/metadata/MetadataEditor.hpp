#ifndef METADATAEDITOR_H_
#define METADATAEDITOR_H_

#include "mdfdsman.h"
#include <dcmtk/dcmpstat/dcmpstat.h>
#include <vector>

/** This class encapsulates methods for loading, checking,
 * and editing dicom files
 */


class MetadataEditor : public MdfDatasetManager {
public:
  /** Default constructor
   */
  MetadataEditor();

  /** Constructor to handle the scenario where a path to
   * a DICOM file is provided 
   * @param file_path holds the path to the DICOM file
   */
  MetadataEditor(OFString file_path);

  /** Constructor to hand the scenario where a DICOM dataset
   * is passed directly to the class
   * @param dataset holds a pointer to a DICOM dataset
   */
  MetadataEditor(DcmDataset* dataset);

  /** Creates a DICOM dataset using the DICOM file at the specified 
   * path
   * @param file_path holds the path to the DICOM file
   * @return a pointer to the created DcmDataset
   */
  DcmDataset* pathToDataset(OFString file_path);

  /** Sets the class' tagKey data member using a tag string. 
   * @param str holds the tag in (group, element) as a string 
   * @return EC_Normal, if conversion from str -> DcmTagKey was successful
   */
  OFCondition setTag(OFString str);

  /** Sets the class' tagKey data member using a DcmTagKey. 
   * @param key holds the tag in tag ket form i.e DCM_TagName
   */
  void setTag(DcmTagKey key);

  /** Gets the data member tagKey
   * @return returns the data member tagKey
  */
  DcmTagKey getTagKey();

  /** Gets the tagKey in string form
   * @return returns the data member tagString
  */
  OFString getTagString();

  /** Checks whether the tag specified by tagKey is present in the dataset
   * @param searchIntoSub if true, the search will dive into nested values and search
   *        the lower levels as well
   * @return OFCondition representing whether or not the tag is present
   */
  OFCondition exists(OFBool searchIntoSub = OFFalse);

  /** Checks whether a tag is present in the dataset
   * @param otherTagString holds the desired tag in (group, element) string form
   * @param searchIntoSub if true, the search will dive into nested values and search
   *        the lower levels as well
   * @return OFCondition representing whether or not the tag is present
   */
  OFCondition exists(OFString otherTagString, OFBool searchIntoSub = OFFalse);

  /** Checks whether a tag is present in the dataset
   * @param key holds the desired tag as a DcmTagKey
   * @param searchIntoSub if true, the search will dive into nested values and search
   *        the lower levels as well
   * @return OFCondition representing whether or not the tag is present
   */
  OFCondition exists(const DcmTagKey &key, OFBool searchIntoSub = OFFalse);

  /** Checks if the value at a tag matches a REGEX
   * @param str_expr holds the REGEX in string form
   * @param flag a reference intended to store the result of the retrieval of the value
   *        at the specified tag. It should hold EC_Normal if all goes well
   * @param pos holds the index of the desired value. Useful for tags where VM > 1
   * @return OFCondition representing whether or not the value matches the REGEX
   */
  OFCondition match(OFString str_expr, OFCondition &flag, const unsigned long pos = 0);

  /** Checks if the value at a tag matches a REGEX
   * @param otherTagString holds the desired tag in (group, element) string form
   * @param str_expr holds the REGEX in string form
   * @param flag a reference intended to store the result of the retrieval of the value
   *        at the specified tag. It should hold EC_Normal if all goes well
   * @param pos holds the index of the desired value. Useful for tags where VM > 1
   * @return OFCondition representing whether or not the value matches the REGEX
   */
  OFCondition match(OFString otherTagString, OFString str_expr, OFCondition &flag, const unsigned long pos = 0);

  /** Checks if the value at a tag matches a REGEX
   * @param otherTagKey holds the desired tag as a DcmTagKey
   * @param str_expr holds the REGEX in string form
   * @param flag a reference intended to store the result of the retrieval of the value
   *        at the specified tag. It should hold EC_Normal if all goes well
   * @param pos holds the index of the desired value. Useful for tags where VM > 1
   * @return OFCondition representing whether or not the value matches the REGEX
   */
  OFCondition match(DcmTagKey otherTagKey, OFString str_expr, OFCondition &flag, const unsigned long pos = 0);

  /** Modifies or inserts a value at a tag 
   * @param newValue denotes new value of tag
   * @param only_overwrite if true, only existing tags are processed. If false,
   *        any not existing tag is inserted
   * @return OFCondition which has status EC_Normal if everything is OK, else an error
   */
  OFCondition modify(OFString newValue, OFBool only_overwrite);

  /** Modifies or inserts a value at a tag 
   * @param newValue denotes new value of tag
   * @param otherTagString holds the desired tag in (group, element) string form
   * @param only_overwrite if true, only existing tags are processed. If false,
   *        any not existing tag is inserted
   * @return OFCondition which has status EC_Normal if everything is OK, else an error
   */
  OFCondition modify(OFString newValue, OFString otherTagString,
                     OFBool only_overwrite);

  /** Modifies or inserts a value at a tag 
   * @param newValue denotes new value of tag
   * @param otherTagKey holds the desired tag as a DcmTagKey
   * @param only_overwrite if true, only existing tags are processed. If false,
   *        any not existing tag is inserted
   * @return OFCondition which has status EC_Normal if everything is OK, else an error
   */
  OFCondition modify(OFString newValue, DcmTagKey otherTagKey,
                     OFBool only_overwrite); 



  /** Exclusive method for copying data from tags and using them too overwrite or insert
   * at a different tag
   * @param otherTagKey holds the desired tag as a DcmTagKey
   * @param posTo destination index. Useful when VM > 0
   * @param posFrom origination index. Useful when VM > 0
   * @param copyToThis if true, information is taken from other tag and used to modify tag
   * @param searchIntoSub if true, the search will dive into nested values and search
   *        the lower levels as well
   * @param replace if true, the data at destination tag is overwritten (VM unchanged), else,
   *         it is inserted at the specified tag in the specified position (VM = VM + 1)
   * @return OFCondition which has status EC_Normal if everything is OK, else an error
   */
  OFCondition copy(DcmTagKey otherTagKey, const unsigned long posTo = 0, const unsigned long posFrom = 0,
                      OFBool copyToThis = OFTrue,
                      OFBool searchIntoSub = OFFalse, OFBool replace = OFTrue);

  /** Function that converts a tag from (group, element) string form into a DcmTagKey
   * @param str holds the tag in string form
   * @param key reference that will contain the DcmTagKey if operation is successful
   * @return OFCondition which has status EC_Normal if everything is OK, else an error
   */
  OFCondition stringToKey(OFString str, DcmTagKey &key);

  // Will hold the dataset that should be modified
  DcmDataset *dset;

private:


  std::vector<OFString> strValsToVector(OFString str);
  OFString vectorToStrVals(std::vector<OFString> vec);

  // Will hold, in string form, the tag that will be modified unless specified otherwise
  OFString tagString;
  // Will hold, as a DcmTagKey, the tag that will be modified unless specified otherwise
  DcmTagKey tagKey; 
};

#endif // METADATAEDITOR_H_