<?xml version='1.0' encoding='UTF-8' standalone='yes' ?>
<tagfile doxygen_version="1.9.1">
  <compound kind="class">
    <name>Conductor</name>
    <filename>classConductor.html</filename>
    <member kind="function">
      <type></type>
      <name>Conductor</name>
      <anchorfile>classConductor.html</anchorfile>
      <anchor>aa71464dda7e87348a6e9e569fc430378</anchor>
      <arglist>(std::istream &amp;json_config)</arglist>
    </member>
  </compound>
  <compound kind="class">
    <name>cpp_template::Exception</name>
    <filename>classcpp__template_1_1Exception.html</filename>
  </compound>
  <compound kind="class">
    <name>ImageEditor</name>
    <filename>classImageEditor.html</filename>
    <base>MdfDatasetManager</base>
    <member kind="function">
      <type></type>
      <name>ImageEditor</name>
      <anchorfile>classImageEditor.html</anchorfile>
      <anchor>a94fb61ac830361cc23e5976b9384d491</anchor>
      <arglist>(OFString file_path)</arglist>
    </member>
    <member kind="function">
      <type></type>
      <name>ImageEditor</name>
      <anchorfile>classImageEditor.html</anchorfile>
      <anchor>ab502da028414c55c80474fd707a09633</anchor>
      <arglist>(DcmDataset *dataset)</arglist>
    </member>
    <member kind="function">
      <type>DcmDataset *</type>
      <name>pathToDataset</name>
      <anchorfile>classImageEditor.html</anchorfile>
      <anchor>a31a5d8970d3e7533c9a5dabb2b1d29ef</anchor>
      <arglist>(OFString file_path)</arglist>
    </member>
    <member kind="function">
      <type>OFBool</type>
      <name>lettersOnly</name>
      <anchorfile>classImageEditor.html</anchorfile>
      <anchor>ab18c770ac9b67b71e1a0c347434c3c5d</anchor>
      <arglist>(std::string text)</arglist>
    </member>
    <member kind="function">
      <type>OFBool</type>
      <name>lessThanFourChars</name>
      <anchorfile>classImageEditor.html</anchorfile>
      <anchor>a3725097bf8c6aebe5ae146ff4f53e351</anchor>
      <arglist>(std::string text)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>coverText</name>
      <anchorfile>classImageEditor.html</anchorfile>
      <anchor>af381dd5e984d2c57fa50c27b5b66a6bf</anchor>
      <arglist>()</arglist>
    </member>
    <member kind="function" static="yes">
      <type>static OFBool</type>
      <name>digitsOnly</name>
      <anchorfile>classImageEditor.html</anchorfile>
      <anchor>aeebf264fb85eff53d95bb2cc5f2a15e1</anchor>
      <arglist>(std::string text)</arglist>
    </member>
  </compound>
  <compound kind="class">
    <name>MdfDatasetManager</name>
    <filename>classMdfDatasetManager.html</filename>
    <member kind="function">
      <type></type>
      <name>MdfDatasetManager</name>
      <anchorfile>classMdfDatasetManager.html</anchorfile>
      <anchor>a3e5dd9d7be105086b92a56c85529cd6e</anchor>
      <arglist>()</arglist>
    </member>
    <member kind="function">
      <type></type>
      <name>~MdfDatasetManager</name>
      <anchorfile>classMdfDatasetManager.html</anchorfile>
      <anchor>a9cf558e8796d2fd570785d9140cf9613</anchor>
      <arglist>()</arglist>
    </member>
    <member kind="function">
      <type>OFCondition</type>
      <name>loadFile</name>
      <anchorfile>classMdfDatasetManager.html</anchorfile>
      <anchor>aae44c06b861db24365b1485420010a89</anchor>
      <arglist>(DcmDataset *dataset, const E_FileReadMode readMode=ERM_autoDetect, const E_TransferSyntax xfer=EXS_Unknown, const OFBool createIfNecessary=OFFalse)</arglist>
    </member>
    <member kind="function">
      <type>OFCondition</type>
      <name>modifyOrInsertPath</name>
      <anchorfile>classMdfDatasetManager.html</anchorfile>
      <anchor>a48cd22890a793d0c5785ed806fc807b6</anchor>
      <arglist>(OFString tag_path, const OFString &amp;value, const OFBool only_modify, const OFBool update_metaheader=OFTrue, const OFBool ignore_missing_tags=OFFalse, const OFBool no_reservation_checks=OFFalse)</arglist>
    </member>
    <member kind="function">
      <type>OFCondition</type>
      <name>modifyOrInsertFromFile</name>
      <anchorfile>classMdfDatasetManager.html</anchorfile>
      <anchor>a1397e76cbf6ed39ebb60543bc07f7bb0</anchor>
      <arglist>(OFString tag_path, const OFString &amp;filename, const OFBool only_modify, const OFBool update_metaheader=OFTrue, const OFBool ignore_missing_tags=OFFalse, const OFBool no_reservation_checks=OFFalse)</arglist>
    </member>
    <member kind="function">
      <type>OFCondition</type>
      <name>modifyAllTags</name>
      <anchorfile>classMdfDatasetManager.html</anchorfile>
      <anchor>a106bb0c93370fafdbec506100717a2f8</anchor>
      <arglist>(OFString tag_path, const OFString &amp;value, const OFBool update_metaheader, int &amp;count, const OFBool ignore_missing_tags=OFFalse)</arglist>
    </member>
    <member kind="function">
      <type>OFCondition</type>
      <name>deleteTag</name>
      <anchorfile>classMdfDatasetManager.html</anchorfile>
      <anchor>a3bc551c5372ec702bc13d0a2c8aefeaa</anchor>
      <arglist>(OFString tag_path, const OFBool all_tags, const OFBool ignore_missing_tags)</arglist>
    </member>
    <member kind="function">
      <type>OFCondition</type>
      <name>deletePrivateData</name>
      <anchorfile>classMdfDatasetManager.html</anchorfile>
      <anchor>a7a3d1ac7b01dbc8950702dce02745b3d</anchor>
      <arglist>()</arglist>
    </member>
    <member kind="function">
      <type>OFCondition</type>
      <name>generateAndInsertUID</name>
      <anchorfile>classMdfDatasetManager.html</anchorfile>
      <anchor>ae03032fc3e9932d11bd539672c21127b</anchor>
      <arglist>(const DcmTagKey &amp;uidKey)</arglist>
    </member>
    <member kind="function">
      <type>OFCondition</type>
      <name>saveFile</name>
      <anchorfile>classMdfDatasetManager.html</anchorfile>
      <anchor>a758b5910a7931c5afd908175eb9ee700</anchor>
      <arglist>(const char *file_name, E_TransferSyntax opt_xfer=EXS_Unknown, E_EncodingType opt_enctype=EET_UndefinedLength, E_GrpLenEncoding opt_glenc=EGL_recalcGL, E_PaddingEncoding opt_padenc=EPD_noChange, OFCmdUnsignedInt opt_filepad=0, OFCmdUnsignedInt opt_itempad=0, OFBool opt_dataset=OFFalse)</arglist>
    </member>
    <member kind="function">
      <type>OFCondition</type>
      <name>saveFile</name>
      <anchorfile>classMdfDatasetManager.html</anchorfile>
      <anchor>a01960a9d7c257875c0b4dcdf52bb7136</anchor>
      <arglist>()</arglist>
    </member>
    <member kind="function">
      <type>DcmDataset *</type>
      <name>getDataset</name>
      <anchorfile>classMdfDatasetManager.html</anchorfile>
      <anchor>a17eee1831a6932219d7d4d12deb94d52</anchor>
      <arglist>()</arglist>
    </member>
    <member kind="function">
      <type>DcmFileFormat *</type>
      <name>getFileFormat</name>
      <anchorfile>classMdfDatasetManager.html</anchorfile>
      <anchor>acac3104351961b5884458bbca27fc01f</anchor>
      <arglist>()</arglist>
    </member>
    <member kind="function">
      <type>OFString</type>
      <name>getFilename</name>
      <anchorfile>classMdfDatasetManager.html</anchorfile>
      <anchor>a2b7b1a550d161ab770098a2bdbca4062</anchor>
      <arglist>() const</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>setModifyUNValues</name>
      <anchorfile>classMdfDatasetManager.html</anchorfile>
      <anchor>a43550fcb8af0a41562c23ac580c4146a</anchor>
      <arglist>(OFBool modifyUNValues)</arglist>
    </member>
    <member kind="function" protection="protected">
      <type>OFCondition</type>
      <name>startModify</name>
      <anchorfile>classMdfDatasetManager.html</anchorfile>
      <anchor>a80d4c4d61bfc5bd3c21fba2fc8ea8ece</anchor>
      <arglist>(DcmElement *elem, const OFString &amp;value)</arglist>
    </member>
    <member kind="function" protection="protected">
      <type>void</type>
      <name>deleteRelatedMetaheaderTag</name>
      <anchorfile>classMdfDatasetManager.html</anchorfile>
      <anchor>a155e6e4d6df40460fcc9ec8bdba9972f</anchor>
      <arglist>(const DcmTagKey &amp;key)</arglist>
    </member>
    <member kind="function" protection="protected">
      <type>OFBool</type>
      <name>isTagInDictionary</name>
      <anchorfile>classMdfDatasetManager.html</anchorfile>
      <anchor>af83ddf3c45bd9297421881b276bbb7be</anchor>
      <arglist>(const DcmTagKey &amp;search_key)</arglist>
    </member>
  </compound>
  <compound kind="class">
    <name>MetadataEditor</name>
    <filename>classMetadataEditor.html</filename>
    <base>MdfDatasetManager</base>
    <member kind="function">
      <type></type>
      <name>MetadataEditor</name>
      <anchorfile>classMetadataEditor.html</anchorfile>
      <anchor>aff5ec7ca8861177710a4d491ba59b855</anchor>
      <arglist>()</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>setDset</name>
      <anchorfile>classMetadataEditor.html</anchorfile>
      <anchor>a1f50dfcb88577272659eb337689f3157</anchor>
      <arglist>(DcmDataset *dataset)</arglist>
    </member>
    <member kind="function">
      <type>OFCondition</type>
      <name>setTag</name>
      <anchorfile>classMetadataEditor.html</anchorfile>
      <anchor>a57f6dc56bf43647da68fa02fd8c853bf</anchor>
      <arglist>(const OFString &amp;str)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>setTag</name>
      <anchorfile>classMetadataEditor.html</anchorfile>
      <anchor>a8bdc9db69252ca3879d0c9b3183dc348</anchor>
      <arglist>(const DcmTagKey &amp;key)</arglist>
    </member>
    <member kind="function">
      <type>DcmTagKey</type>
      <name>getTagKey</name>
      <anchorfile>classMetadataEditor.html</anchorfile>
      <anchor>a19a9b2e04243456e77ec256c40f1e4e6</anchor>
      <arglist>()</arglist>
    </member>
    <member kind="function">
      <type>OFString</type>
      <name>getTagString</name>
      <anchorfile>classMetadataEditor.html</anchorfile>
      <anchor>a8d0333309a2e3e27c79182e1b2c9bf12</anchor>
      <arglist>()</arglist>
    </member>
    <member kind="function">
      <type>OFCondition</type>
      <name>exists</name>
      <anchorfile>classMetadataEditor.html</anchorfile>
      <anchor>ad352769ff74ef0cd4285c2e6b23dc370</anchor>
      <arglist>(OFBool searchIntoSub=OFFalse)</arglist>
    </member>
    <member kind="function">
      <type>OFCondition</type>
      <name>exists</name>
      <anchorfile>classMetadataEditor.html</anchorfile>
      <anchor>a10353fa82d539fb0f33630ee4bdec0f0</anchor>
      <arglist>(const OFString &amp;otherTagString, OFBool searchIntoSub=OFFalse)</arglist>
    </member>
    <member kind="function">
      <type>OFCondition</type>
      <name>exists</name>
      <anchorfile>classMetadataEditor.html</anchorfile>
      <anchor>a20278af977349b2cf0fae334e89f25bb</anchor>
      <arglist>(const DcmTagKey &amp;key, OFBool searchIntoSub=OFFalse)</arglist>
    </member>
    <member kind="function">
      <type>OFCondition</type>
      <name>match</name>
      <anchorfile>classMetadataEditor.html</anchorfile>
      <anchor>aaf990e8ed3006a9e934bcbf762aad5d8</anchor>
      <arglist>(const OFString &amp;str_expr, OFCondition &amp;flag, unsigned long pos=0)</arglist>
    </member>
    <member kind="function">
      <type>OFCondition</type>
      <name>match</name>
      <anchorfile>classMetadataEditor.html</anchorfile>
      <anchor>a613963ffae4f85d82d15f7ca32c230b2</anchor>
      <arglist>(const OFString &amp;otherTagString, const OFString &amp;str_expr, OFCondition &amp;flag, unsigned long pos=0)</arglist>
    </member>
    <member kind="function">
      <type>OFCondition</type>
      <name>match</name>
      <anchorfile>classMetadataEditor.html</anchorfile>
      <anchor>a11b104ce10b3c80cd1db2a571e7a9d44</anchor>
      <arglist>(const DcmTagKey &amp;otherTagKey, const OFString &amp;str_expr, OFCondition &amp;flag, unsigned long pos=0)</arglist>
    </member>
    <member kind="function">
      <type>OFCondition</type>
      <name>equals</name>
      <anchorfile>classMetadataEditor.html</anchorfile>
      <anchor>a10c34346bc58e03ecf7f4714e42fb02f</anchor>
      <arglist>(const OFString &amp;str_expr, OFCondition &amp;flag, unsigned long pos=0)</arglist>
    </member>
    <member kind="function">
      <type>OFCondition</type>
      <name>equals</name>
      <anchorfile>classMetadataEditor.html</anchorfile>
      <anchor>aec60eb6e0d64ec878b8f633c40891dc3</anchor>
      <arglist>(const OFString &amp;otherTagString, const OFString &amp;str_expr, OFCondition &amp;flag, unsigned long pos=0)</arglist>
    </member>
    <member kind="function">
      <type>OFCondition</type>
      <name>equals</name>
      <anchorfile>classMetadataEditor.html</anchorfile>
      <anchor>a8b0a02b6e100dc743863057886f9f214</anchor>
      <arglist>(const DcmTagKey &amp;otherTagKey, const OFString &amp;str_expr, OFCondition &amp;flag, unsigned long pos=0)</arglist>
    </member>
    <member kind="function">
      <type>OFCondition</type>
      <name>equals</name>
      <anchorfile>classMetadataEditor.html</anchorfile>
      <anchor>a5638cd5014ec1793b2978ba4d90de050</anchor>
      <arglist>(Float64 compare_value, OFCondition &amp;flag, unsigned long pos=0)</arglist>
    </member>
    <member kind="function">
      <type>OFCondition</type>
      <name>equals</name>
      <anchorfile>classMetadataEditor.html</anchorfile>
      <anchor>aec4132481394dc4506fe1e3ae76fa66b</anchor>
      <arglist>(const OFString &amp;otherTagString, Float64 compare_value, OFCondition &amp;flag, unsigned long pos=0)</arglist>
    </member>
    <member kind="function">
      <type>OFCondition</type>
      <name>equals</name>
      <anchorfile>classMetadataEditor.html</anchorfile>
      <anchor>a22004d20c9c3d29679f76781949c98ac</anchor>
      <arglist>(const DcmTagKey &amp;otherTagKey, Float64 compare_value, OFCondition &amp;flag, unsigned long pos=0)</arglist>
    </member>
    <member kind="function">
      <type>OFCondition</type>
      <name>is_in</name>
      <anchorfile>classMetadataEditor.html</anchorfile>
      <anchor>a6f0bd7833059de6c0cdf70dbfcc769ff</anchor>
      <arglist>(const std::vector&lt; std::string &gt; &amp;str_vec, OFCondition &amp;flag, unsigned long pos=0)</arglist>
    </member>
    <member kind="function">
      <type>OFCondition</type>
      <name>is_in</name>
      <anchorfile>classMetadataEditor.html</anchorfile>
      <anchor>a34a6c119a0424e9f7fff75591915b07b</anchor>
      <arglist>(const OFString &amp;otherTagString, const std::vector&lt; std::string &gt; &amp;str_vec, OFCondition &amp;flag, unsigned long pos=0)</arglist>
    </member>
    <member kind="function">
      <type>OFCondition</type>
      <name>is_in</name>
      <anchorfile>classMetadataEditor.html</anchorfile>
      <anchor>a201a69709b3186d4c65471631e02cb72</anchor>
      <arglist>(const DcmTagKey &amp;otherTagKey, const std::vector&lt; std::string &gt; &amp;str_vec, OFCondition &amp;flag, unsigned long pos=0)</arglist>
    </member>
    <member kind="function">
      <type>OFCondition</type>
      <name>is_in</name>
      <anchorfile>classMetadataEditor.html</anchorfile>
      <anchor>a3af25454a1a4558814124437862d0fab</anchor>
      <arglist>(std::vector&lt; Float64 &gt; compare_value_vec, OFCondition &amp;flag, unsigned long pos=0)</arglist>
    </member>
    <member kind="function">
      <type>OFCondition</type>
      <name>is_in</name>
      <anchorfile>classMetadataEditor.html</anchorfile>
      <anchor>ae522efb5021b6d07ec2fe97a037c1736</anchor>
      <arglist>(const OFString &amp;otherTagString, std::vector&lt; Float64 &gt; compare_value_vec, OFCondition &amp;flag, unsigned long pos=0)</arglist>
    </member>
    <member kind="function">
      <type>OFCondition</type>
      <name>is_in</name>
      <anchorfile>classMetadataEditor.html</anchorfile>
      <anchor>af457b9ee67eefe677766a9084e23e586</anchor>
      <arglist>(const DcmTagKey &amp;otherTagKey, std::vector&lt; Float64 &gt; compare_value_vec, OFCondition &amp;flag, unsigned long pos=0)</arglist>
    </member>
    <member kind="function">
      <type>OFCondition</type>
      <name>greaterOrLessThan</name>
      <anchorfile>classMetadataEditor.html</anchorfile>
      <anchor>a97599fb145b53c50bb841da7902dbf12</anchor>
      <arglist>(Float64 compare_value, OFBool greaterThan, OFCondition &amp;flag, unsigned long pos=0)</arglist>
    </member>
    <member kind="function">
      <type>OFCondition</type>
      <name>greaterOrLessThan</name>
      <anchorfile>classMetadataEditor.html</anchorfile>
      <anchor>a548f4aefb7bfa41d73e130c0cb1cd25f</anchor>
      <arglist>(const DcmTagKey &amp;otherTagKey, Float64 compare_value, OFBool greaterThan, OFCondition &amp;flag, unsigned long pos=0)</arglist>
    </member>
    <member kind="function">
      <type>OFCondition</type>
      <name>greaterOrLessThan</name>
      <anchorfile>classMetadataEditor.html</anchorfile>
      <anchor>a9e8ae1157106bc3dc1e533a1f9148122</anchor>
      <arglist>(const OFString &amp;otherTagString, Float64 compare_value, OFBool greaterThan, OFCondition &amp;flag, unsigned long pos=0)</arglist>
    </member>
    <member kind="function">
      <type>OFCondition</type>
      <name>modify</name>
      <anchorfile>classMetadataEditor.html</anchorfile>
      <anchor>ac2e44571f117432d698b793924289279</anchor>
      <arglist>(const OFString &amp;newValue, OFBool only_overwrite)</arglist>
    </member>
    <member kind="function">
      <type>OFCondition</type>
      <name>modify</name>
      <anchorfile>classMetadataEditor.html</anchorfile>
      <anchor>abc18a129791cc7f29ff50c048abd0b5a</anchor>
      <arglist>(const OFString &amp;newValue, const OFString &amp;otherTagString, OFBool only_overwrite)</arglist>
    </member>
    <member kind="function">
      <type>OFCondition</type>
      <name>modify</name>
      <anchorfile>classMetadataEditor.html</anchorfile>
      <anchor>af554ec118ed3cef1cbd8103f777dbad0</anchor>
      <arglist>(const OFString &amp;newValue, const DcmTagKey &amp;otherTagKey, OFBool only_overwrite)</arglist>
    </member>
    <member kind="function">
      <type>OFCondition</type>
      <name>append</name>
      <anchorfile>classMetadataEditor.html</anchorfile>
      <anchor>a450f2597bfcf5a848fb17d244133166f</anchor>
      <arglist>(const OFString &amp;appendValue, OFCondition &amp;flag, unsigned long pos=0)</arglist>
    </member>
    <member kind="function">
      <type>OFCondition</type>
      <name>append</name>
      <anchorfile>classMetadataEditor.html</anchorfile>
      <anchor>a463a5d83caac7675d3ecb38f99f6fbbc</anchor>
      <arglist>(const OFString &amp;appendValue, const OFString &amp;otherTagString, OFCondition &amp;flag, unsigned long pos=0)</arglist>
    </member>
    <member kind="function">
      <type>OFCondition</type>
      <name>append</name>
      <anchorfile>classMetadataEditor.html</anchorfile>
      <anchor>ae3a968b1a04aa3ce5ebcc454b1e1c99d</anchor>
      <arglist>(const OFString &amp;appendValue, const DcmTagKey &amp;otherTagKey, OFCondition &amp;flag, unsigned long pos=0)</arglist>
    </member>
    <member kind="function">
      <type>OFCondition</type>
      <name>prepend</name>
      <anchorfile>classMetadataEditor.html</anchorfile>
      <anchor>a1c65501a85fd9fb9b767e654dcc8658a</anchor>
      <arglist>(const OFString &amp;prependValue, OFCondition &amp;flag, unsigned long pos=0)</arglist>
    </member>
    <member kind="function">
      <type>OFCondition</type>
      <name>prepend</name>
      <anchorfile>classMetadataEditor.html</anchorfile>
      <anchor>a4a429cc8fc27c48916c3b5e36efd7a00</anchor>
      <arglist>(const OFString &amp;prependValue, const OFString &amp;otherTagString, OFCondition &amp;flag, unsigned long pos=0)</arglist>
    </member>
    <member kind="function">
      <type>OFCondition</type>
      <name>prepend</name>
      <anchorfile>classMetadataEditor.html</anchorfile>
      <anchor>a51e2a2b684c7b626a00731d3b13dc6ce</anchor>
      <arglist>(const OFString &amp;prependValue, const DcmTagKey &amp;otherTagKey, OFCondition &amp;flag, unsigned long pos=0)</arglist>
    </member>
    <member kind="function">
      <type>OFCondition</type>
      <name>overwrite</name>
      <anchorfile>classMetadataEditor.html</anchorfile>
      <anchor>aa317a4f6812a4e356c589176a2328717</anchor>
      <arglist>(const OFString &amp;str_expr, const OFString &amp;replaceString)</arglist>
    </member>
    <member kind="function">
      <type>OFCondition</type>
      <name>copy</name>
      <anchorfile>classMetadataEditor.html</anchorfile>
      <anchor>a04b711197f38889543683c1289d0c91e</anchor>
      <arglist>(const DcmTagKey &amp;otherTagKey, const unsigned long posTo=0, const unsigned long posFrom=0, OFBool copyToThis=OFTrue, OFBool searchIntoSub=OFFalse, OFBool replace=OFTrue, OFBool concatenate=OFTrue, OFBool prepend=OFTrue)</arglist>
    </member>
    <member kind="function">
      <type>OFCondition</type>
      <name>copy</name>
      <anchorfile>classMetadataEditor.html</anchorfile>
      <anchor>a76be8cd03c2b7fa2cb2fb624491d9fb9</anchor>
      <arglist>(const OFString &amp;otherTagString, const unsigned long posTo=0, const unsigned long posFrom=0, OFBool copyToThis=OFTrue, OFBool searchIntoSub=OFFalse, OFBool replace=OFTrue, OFBool concatenate=OFTrue, OFBool prepend=OFTrue)</arglist>
    </member>
    <member kind="function">
      <type>OFCondition</type>
      <name>stringToKey</name>
      <anchorfile>classMetadataEditor.html</anchorfile>
      <anchor>a766b2daeff20e71741b7077f9821e4d3</anchor>
      <arglist>(OFString str, DcmTagKey &amp;key)</arglist>
    </member>
  </compound>
  <compound kind="class">
    <name>Parser</name>
    <filename>classParser.html</filename>
    <member kind="function">
      <type></type>
      <name>Parser</name>
      <anchorfile>classParser.html</anchorfile>
      <anchor>a12234f6cd36b61af4b50c94a179422c1</anchor>
      <arglist>()</arglist>
    </member>
    <member kind="function">
      <type></type>
      <name>Parser</name>
      <anchorfile>classParser.html</anchorfile>
      <anchor>a5bc95f1a89c6139217800ea7e02d8264</anchor>
      <arglist>(const OFString &amp;configFpath)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>setConfig</name>
      <anchorfile>classParser.html</anchorfile>
      <anchor>a14de15f833bbe995965d9c39fb363cce</anchor>
      <arglist>(const json &amp;config)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>setConfigFile</name>
      <anchorfile>classParser.html</anchorfile>
      <anchor>a438ab8439000402e5ce5bb4f7b515293</anchor>
      <arglist>(const OFString &amp;configFpath)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>setDicomDset</name>
      <anchorfile>classParser.html</anchorfile>
      <anchor>a78652eb1cb5606739116a15db2d702d5</anchor>
      <arglist>(DcmDataset *dset)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>setDicomDset</name>
      <anchorfile>classParser.html</anchorfile>
      <anchor>a04318f63eb79e74f574493d71053dd22</anchor>
      <arglist>(OFString path)</arglist>
    </member>
    <member kind="function">
      <type>DcmDataset *</type>
      <name>getDicomDset</name>
      <anchorfile>classParser.html</anchorfile>
      <anchor>a57090042befe38fea179224b0b5d448f</anchor>
      <arglist>()</arglist>
    </member>
    <member kind="function">
      <type>DcmDataset *</type>
      <name>pathToDset</name>
      <anchorfile>classParser.html</anchorfile>
      <anchor>a2e372de6daa4463dff139df496956e5e</anchor>
      <arglist>(OFString path)</arglist>
    </member>
    <member kind="function">
      <type>DcmDataset *</type>
      <name>parse</name>
      <anchorfile>classParser.html</anchorfile>
      <anchor>a7dbfdd09624fdabd4b08ee017eecd1c4</anchor>
      <arglist>()</arglist>
    </member>
  </compound>
  <compound kind="class">
    <name>Receiver</name>
    <filename>classReceiver.html</filename>
    <member kind="function">
      <type></type>
      <name>Receiver</name>
      <anchorfile>classReceiver.html</anchorfile>
      <anchor>ab05f062ad8bc9105e7708c9567552908</anchor>
      <arglist>()</arglist>
    </member>
    <member kind="function" virtualness="virtual">
      <type>virtual void</type>
      <name>request_stop</name>
      <anchorfile>classReceiver.html</anchorfile>
      <anchor>a46afb4a8e68cb4b72b3ce1ce6371f36a</anchor>
      <arglist>()</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>setaetitle</name>
      <anchorfile>classReceiver.html</anchorfile>
      <anchor>a61b2f7be1877a0b6dc0b687e06b81003</anchor>
      <arglist>(std::string ae_title)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>setportnumber</name>
      <anchorfile>classReceiver.html</anchorfile>
      <anchor>a06924865984df4c074197d2553855e5a</anchor>
      <arglist>(Uint16 port)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>set_additional_context</name>
      <anchorfile>classReceiver.html</anchorfile>
      <anchor>a8a78486a989bb67895c002b853a43237</anchor>
      <arglist>(const json &amp;context)</arglist>
    </member>
    <member kind="function" protection="protected">
      <type>void</type>
      <name>run</name>
      <anchorfile>classReceiver.html</anchorfile>
      <anchor>a392c6befa029b49c2d7a6aa901598c15</anchor>
      <arglist>() override</arglist>
    </member>
  </compound>
  <compound kind="class">
    <name>ReceiverThread</name>
    <filename>classReceiverThread.html</filename>
    <member kind="function">
      <type></type>
      <name>ReceiverThread</name>
      <anchorfile>classReceiverThread.html</anchorfile>
      <anchor>a1592d074fcde9c46697eccea721b9b3a</anchor>
      <arglist>()</arglist>
    </member>
    <member kind="function">
      <type></type>
      <name>~ReceiverThread</name>
      <anchorfile>classReceiverThread.html</anchorfile>
      <anchor>a047caeb840bcdfe4aa429c2451ffc25e</anchor>
      <arglist>() override</arglist>
    </member>
    <member kind="function">
      <type>OFCondition</type>
      <name>handleIncomingCommand</name>
      <anchorfile>classReceiverThread.html</anchorfile>
      <anchor>a47aa75a34f0f865f961064f2d06b0a38</anchor>
      <arglist>(T_DIMSE_Message *incomingMsg, const DcmPresentationContextInfo &amp;presInfo) override</arglist>
    </member>
    <member kind="function" virtualness="virtual">
      <type>virtual OFCondition</type>
      <name>setIPs</name>
      <anchorfile>classReceiverThread.html</anchorfile>
      <anchor>a57d8498140c62bd8cad124097e3aa9d1</anchor>
      <arglist>(const OFList&lt; OFString &gt; &amp;source_list)</arglist>
    </member>
    <member kind="function" virtualness="virtual">
      <type>virtual OFCondition</type>
      <name>setpeerAETitles</name>
      <anchorfile>classReceiverThread.html</anchorfile>
      <anchor>a364ce3806699abc0572b8232a0c96aa7</anchor>
      <arglist>(const OFList&lt; OFString &gt; &amp;peerae_list)</arglist>
    </member>
    <member kind="function">
      <type>OFBool</type>
      <name>checkCallingHostAccepted</name>
      <anchorfile>classReceiverThread.html</anchorfile>
      <anchor>ada03bc2ff4c66549d19ec2a130bebf89</anchor>
      <arglist>(const OFString &amp;hostOrIP) override</arglist>
    </member>
    <member kind="function">
      <type>OFBool</type>
      <name>checkCallingAETitleAccepted</name>
      <anchorfile>classReceiverThread.html</anchorfile>
      <anchor>a207566653d72635e358ef6410defe89b</anchor>
      <arglist>(const OFString &amp;callingAE) override</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>setdatasetaddress</name>
      <anchorfile>classReceiverThread.html</anchorfile>
      <anchor>a9fd53aca740b0afb8dfb6552443c9751</anchor>
      <arglist>(OFshared_ptr&lt; ThreadSafeQueue&lt; DcmDataset &gt;&gt; dset)</arglist>
    </member>
  </compound>
  <compound kind="class">
    <name>Sender</name>
    <filename>classSender.html</filename>
    <member kind="function">
      <type></type>
      <name>Sender</name>
      <anchorfile>classSender.html</anchorfile>
      <anchor>ae14150bc5755bf478a7ad1904c017ba1</anchor>
      <arglist>()</arglist>
    </member>
  </compound>
  <compound kind="class">
    <name>ThreadSafeQueue</name>
    <filename>classThreadSafeQueue.html</filename>
    <templarg></templarg>
  </compound>
  <compound kind="class">
    <name>Validator</name>
    <filename>classValidator.html</filename>
  </compound>
  <compound kind="struct">
    <name>WorkerParameters</name>
    <filename>structWorkerParameters.html</filename>
  </compound>
  <compound kind="page">
    <name>Configuration_gallery</name>
    <title>Configuration Gallery</title>
    <filename>Configuration_gallery.html</filename>
  </compound>
  <compound kind="page">
    <name>md_doc_pages_FAQ</name>
    <title>FAQ</title>
    <filename>md_doc_pages_FAQ.html</filename>
  </compound>
  <compound kind="page">
    <name>Introduction</name>
    <title>Introduction</title>
    <filename>Introduction.html</filename>
  </compound>
  <compound kind="page">
    <name>Tutorials</name>
    <title>Tutorials</title>
    <filename>Tutorials.html</filename>
  </compound>
  <compound kind="page">
    <name>index</name>
    <title>Extensible Clinical Imaging Quality Control Tool</title>
    <filename>index.html</filename>
  </compound>
</tagfile>
