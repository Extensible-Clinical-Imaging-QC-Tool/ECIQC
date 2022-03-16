#ifndef METADATAEDITOR_H_
#define METADATAEDITOR_H_

#include "mdfdsman.h"
#include <dcmtk/dcmpstat/dcmpstat.h>
#include <vector>

/** This class encapsulates methods for loading, checking,
 * and editing dicom files
 */

/* Current functions:
 * EXISTS
 * MATCH (REGEX)
 * EQUALS
 * GREATER/LESS THAN
 * MODIFY (INSERT OR OVERWRITE)
 * COPY
 *
 *  TO BE ADDED:
 * REMOVE - can use deleteTag from mdfdsman
 * CLEAR (?) - clear value? clear()?
 * UPDATE (?) - add another value on top of what's there?
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
  explicit MetadataEditor(const OFString& file_path);

  /** Constructor to hand the scenario where a DICOM dataset
   * is passed directly to the class
   * @param dataset holds a pointer to a DICOM dataset
   */
  explicit MetadataEditor(DcmDataset* dataset);

  /** Creates a DICOM dataset using the DICOM file at the specified 
   * path
   * @param file_path holds the path to the DICOM file
   * @return a pointer to the created DcmDataset
   */
  DcmDataset* pathToDataset(const OFString& file_path);

  /** Sets the class' tagKey data member using a tag string. 
   * @param str holds the tag in (group, element) as a string 
   * @return EC_Normal, if conversion from str -> DcmTagKey was successful
   */
  OFCondition setTag(const OFString& str);

  /** Sets the class' tagKey data member using a DcmTagKey. 
   * @param key holds the tag in tag ket form i.e DCM_TagName
   */
  void setTag(const DcmTagKey& key);

  /** Gets the data member tagKey
   * @return returns the data member tagKey
  */
  DcmTagKey getTagKey();

  /** Gets the tagKey in string form
   * @return returns the data member tagString
  */
  OFString getTagString();

  // CHECKS

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
  OFCondition exists(const OFString& otherTagString, OFBool searchIntoSub = OFFalse);

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
  OFCondition match(const OFString& str_expr, OFCondition &flag, unsigned long pos = 0);

  /** Checks if the value at a tag matches a REGEX
   * @param otherTagString holds the desired tag in (group, element) string form
   * @param str_expr holds the REGEX in string form
   * @param flag a reference intended to store the result of the retrieval of the value
   *        at the specified tag. It should hold EC_Normal if all goes well
   * @param pos holds the index of the desired value. Useful for tags where VM > 1
   * @return OFCondition representing whether or not the value matches the REGEX
   */
  OFCondition match(const OFString& otherTagString, const OFString& str_expr, OFCondition &flag, unsigned long pos = 0);

  /** Checks if the value at a tag matches a REGEX
   * @param otherTagKey holds the desired tag as a DcmTagKey
   * @param str_expr holds the REGEX in string form
   * @param flag a reference intended to store the result of the retrieval of the value
   *        at the specified tag. It should hold EC_Normal if all goes well
   * @param pos holds the index of the desired value. Useful for tags where VM > 1
   * @return OFCondition representing whether or not the value matches the REGEX
   */
  OFCondition match(const DcmTagKey& otherTagKey, const OFString& str_expr, OFCondition &flag, unsigned long pos = 0);

  /** Checks if value at a tag matches a string exactly
   *
   * @param str_expr string to be matched
   * @param flag a reference intended to store the result of the retrieval of the value
   *        at the specified tag. It should hold EC_Normal if all goes well
   * @param pos holds the index of the desired value. Useful for tags where VM > 1
   * @return OFCondition representing whether or not the value matches the string exactly
   */
  OFCondition equals(const OFString& str_expr, OFCondition &flag, unsigned long pos = 0);

  /** Checks if value at a tag matches a string exactly
   *
   * @param otherTagString holds the desired tag in (group, element) string form
   * @param str_expr string to be matched
   * @param flag a reference intended to store the result of the retrieval of the value
   *        at the specified tag. It should hold EC_Normal if all goes well
   * @param pos holds the index of the desired value. Useful for tags where VM > 1
   * @return OFCondition representing whether or not the value matches the string exactly
   */
  OFCondition equals(const OFString& otherTagString, const OFString& str_expr, OFCondition &flag,
                     unsigned long pos = 0);

  /** Checks if value at a tag matches a string exactly
   *
   * @param otherTagKey holds the desired tag as a DcmTagKey
   * @param str_expr string to be matched
   * @param flag a reference intended to store the result of the retrieval of the value
   *        at the specified tag. It should hold EC_Normal if all goes well
   * @param pos holds the index of the desired value. Useful for tags where VM > 1
   * @return OFCondition representing whether or not the value matches the string exactly
   */
  OFCondition equals(const DcmTagKey& otherTagKey, const OFString& str_expr, OFCondition &flag, unsigned long pos = 0);

  // TODO: is Float64 the best type for comparison in equals and greater/less than????

  /** Checks if value at a tag matches a value exactly
   *
   * @param compare_value double to be matched
   * @param flag a reference intended to store the result of the retrieval of the value
   *        at the specified tag. It should hold EC_Normal if all goes well
   * @param pos holds the index of the desired value. Useful for tags where VM > 1
   * @return OFCondition representing whether or not the value matches the double exactly
   */
  OFCondition equals(Float64 compare_value, OFCondition &flag, unsigned long pos = 0);

  /** Checks if value at a tag matches a value exactly
   *
   * @param otherTagString holds the desired tag in (group, element) string form
   * @param compare_value double to be matched
   * @param flag a reference intended to store the result of the retrieval of the value
   *        at the specified tag. It should hold EC_Normal if all goes well
   * @param pos holds the index of the desired value. Useful for tags where VM > 1
   * @return OFCondition representing whether or not the value matches the double exactly
   */
  OFCondition equals(const OFString& otherTagString, Float64 compare_value, OFCondition &flag, unsigned long pos = 0);

  /** Checks if value at a tag matches a value exactly
   *
   * @param otherTagKey holds the desired tag as a DcmTagKey
   * @param compare_value double to be matched
   * @param flag a reference intended to store the result of the retrieval of the value
   *        at the specified tag. It should hold EC_Normal if all goes well
   * @param pos holds the index of the desired value. Useful for tags where VM > 1
   * @return OFCondition representing whether or not the value matches the double exactly
   */
  OFCondition equals(const DcmTagKey& otherTagKey, Float64 compare_value, OFCondition &flag, unsigned long pos = 0);

  /** Check if value at a tag is greater or less than a given value
   *
   * @param compare_value double to compare value at tag with
   * @param greaterThan if OFTRUE, value at tag will be checked for being greater than compare_value, otherwise will be
   * checked for less than.
   * @param flag a reference intended to store the result of the retrieval of the value
   *        at the specified tag. It should hold EC_Normal if all goes well
   * @param pos holds the index of the desired value. Useful for tags where VM > 1
   * @return OFCondition representing whether or not the value is less than the given double
   */
  OFCondition greaterOrLessThan(Float64 compare_value, OFBool greaterThan, OFCondition &flag, unsigned long pos = 0);

  /** Check if value at a tag is less than a given value
    *
    * @param otherTagKey holds the desired tag as a DcmTagKey
    * @param compare_value double to compare value at tag with
    * @param greaterThan if OFTRUE, value at tag will be checked for being greater than compare_value, otherwise will be
    * checked for less than.
    * @param flag a reference intended to store the result of the retrieval of the value
    *        at the specified tag. It should hold EC_Normal if all goes well
    * @param pos holds the index of the desired value. Useful for tags where VM > 1
    * @return OFCondition representing whether or not the value is less than the given double
    */
  OFCondition greaterOrLessThan(const DcmTagKey& otherTagKey, Float64 compare_value, OFBool greaterThan,
                                OFCondition &flag, unsigned long pos = 0);

  /** Check if value at a tag is less than a given value
   *
   * @param otherTagString holds the desired tag in (group, element) string form
   * @param compare_value double to compare value at tag with
   * @param greaterThan if OFTRUE, value at tag will be checked for being greater than compare_value, otherwise will be
   * checked for less than.
   * @param flag a reference intended to store the result of the retrieval of the value
   *        at the specified tag. It should hold EC_Normal if all goes well
   * @param pos holds the index of the desired value. Useful for tags where VM > 1
   * @return OFCondition representing whether or not the value is less than the given double
   */
  OFCondition greaterOrLessThan(const OFString& otherTagString, Float64 compare_value, OFBool greaterThan,
                                OFCondition &flag, unsigned long pos = 0);

  // EDITS

  /** Modifies or inserts a value at a tag
   * @param newValue denotes new string value of tag
   * @param only_overwrite if true, only existing tags are processed. If false,
   *        any not existing tag is inserted
   * @return OFCondition which has status EC_Normal if everything is OK, else an error
   */
  OFCondition modify(const OFString& newValue, OFBool only_overwrite);

  /** Modifies or inserts a value at a tag 
   * @param newValue denotes new string value of tag
   * @param otherTagString holds the desired tag in (group, element) string form
   * @param only_overwrite if true, only existing tags are processed. If false,
   *        any not existing tag is inserted
   * @return OFCondition which has status EC_Normal if everything is OK, else an error
   */
  OFCondition modify(const OFString& newValue, const OFString& otherTagString,
                     OFBool only_overwrite);

  /** Modifies or inserts a value at a tag 
   * @param newValue denotes new string value of tag
   * @param otherTagKey holds the desired tag as a DcmTagKey
   * @param only_overwrite if true, only existing tags are processed. If false,
   *        any not existing tag is inserted
   * @return OFCondition which has status EC_Normal if everything is OK, else an error
   */
  OFCondition modify(const OFString& newValue, const DcmTagKey& otherTagKey,
                     OFBool only_overwrite);

//  TODO: function for overwriting according to current value (e.g. DOB 19810605 -> 19810101 per Chris' email)
//  OFCondition editValue(); - using regex?


  /** Appends a string on to the end of the current value at a tag
   *
   * @param appendValue is the string to append to the current value
   * @param flag a reference intended to store the result of the retrieval of the value
   *        at the specified tag. It should hold EC_Normal if all goes well
   * @return OFCondition which has status EC_Normal if everything is OK, else an error
   */
  OFCondition append(const OFString& appendValue, OFCondition &flag, unsigned long pos = 0);

  /** Appends a string on to the end of the current value at a tag
   *
   * @param appendValue is the string to append to the current value
   * @param otherTagString holds the desired tag in (group, element) string form
   * @param flag a reference intended to store the result of the retrieval of the value
   *        at the specified tag. It should hold EC_Normal if all goes well
   * @return OFCondition which has status EC_Normal if everything is OK, else an error
   */
  OFCondition append(const OFString& appendValue, const OFString& otherTagString, OFCondition &flag,
                     unsigned long pos = 0);

  /** Appends a string on to the end of the current value at a tag
   *
   * @param appendValue is the string to append to the current value
   * @param otherTagKey holds the desired tag as a DcmTagKey
   * @param flag a reference intended to store the result of the retrieval of the value
   *        at the specified tag. It should hold EC_Normal if all goes well
   * @return OFCondition which has status EC_Normal if everything is OK, else an error
   */
  OFCondition append(const OFString& appendValue, const DcmTagKey& otherTagKey, OFCondition &flag,
                     unsigned long pos = 0);

  /** Appends a string on to the start of the current value at a tag
   *
   * @param prependValue is the string to prepend to the current value
   * @param flag a reference intended to store the result of the retrieval of the value
   *        at the specified tag. It should hold EC_Normal if all goes well
   * @return OFCondition which has status EC_Normal if everything is OK, else an error
   */
  OFCondition prepend(const OFString& prependValue, OFCondition &flag, unsigned long pos = 0);

  /** Appends a string on to the start of the current value at a tag
   *
   * @param prependValue is the string to prepend to the current value
   * @param otherTagString holds the desired tag in (group, element) string form
   * @param flag a reference intended to store the result of the retrieval of the value
   *        at the specified tag. It should hold EC_Normal if all goes well
   * @return OFCondition which has status EC_Normal if everything is OK, else an error
   */
  OFCondition prepend(const OFString& prependValue, const OFString& otherTagString, OFCondition &flag,
                      unsigned long pos = 0);

  /** Appends a string on to the start of the current value at a tag
   *
   * @param prependValue is the string to prepend to the current value
   * @param otherTagKey holds the desired tag as a DcmTagKey
   * @param flag a reference intended to store the result of the retrieval of the value
   *        at the specified tag. It should hold EC_Normal if all goes well
   * @return OFCondition which has status EC_Normal if everything is OK, else an error
   */
  OFCondition prepend(const OFString& prependValue, const DcmTagKey& otherTagKey, OFCondition &flag,
                      unsigned long pos = 0);

/* Regex expression to match, part of expression to overwrite, what to overwrite with?
 * e.g. "([0-9]{4})([0-9]{4})", 2, "0101"
 * could replace the 2nd half of DOB with "0101"
 * using s = currentValue, e = "([0-9]{4})([0-9]{4})" and
 * std::regex_replace (std::back_inserter(result), s.begin(), s.end(), e, "$1"+overwriteString)
 */
  OFCondition overwrite();

  OFCondition overwrite(const OFString& otherTagString);

  OFCondition overwrite(const DcmTagKey& otherTagKey);


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
  OFCondition copy(const DcmTagKey& otherTagKey, unsigned long posTo = 0, unsigned long posFrom = 0,
                      OFBool copyToThis = OFTrue,
                      OFBool searchIntoSub = OFFalse, OFBool replace = OFTrue);

  /** Exclusive method for copying data from tags and using them too overwrite or insert
   * at a different tag
   * @param otherTagString holds the desired tag as a string
   * @param posTo destination index. Useful when VM > 0
   * @param posFrom origination index. Useful when VM > 0
   * @param copyToThis if true, information is taken from other tag and used to modify tag
   * @param searchIntoSub if true, the search will dive into nested values and search
   *        the lower levels as well
   * @param replace if true, the data at destination tag is overwritten (VM unchanged), else,
   *         it is inserted at the specified tag in the specified position (VM = VM + 1)
   * @return OFCondition which has status EC_Normal if everything is OK, else an error
   */
  OFCondition copy(const OFString& otherTagString, unsigned long posTo = 0, unsigned long posFrom = 0,
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