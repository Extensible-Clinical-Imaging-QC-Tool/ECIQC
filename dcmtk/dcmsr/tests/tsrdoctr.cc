/*
 *
 *  Copyright (C) 2014-2016, J. Riesmeier, Oldenburg, Germany
 *  All rights reserved.  See COPYRIGHT file for details.
 *
 *  This software and supporting documentation are maintained by
 *
 *    OFFIS e.V.
 *    R&D Division Health
 *    Escherweg 2
 *    D-26121 Oldenburg, Germany
 *
 *
 *  Module: dcmsr
 *
 *  Author: Joerg Riesmeier
 *
 *  Purpose:
 *    test program for classes DSRDocumentTree, DSRDocumentSubTree, DSRDocumentTreeNode
 *    and DSRContentItem
 *
 */


#include "dcmtk/config/osconfig.h"    /* make sure OS specific configuration is included first */

#include "dcmtk/ofstd/oftest.h"

#include "dcmtk/dcmsr/dsrdoc.h"
#include "dcmtk/dcmsr/dsrtpltn.h"
#include "dcmtk/dcmsr/dsrnumtn.h"
#include "dcmtk/dcmsr/dsrtextn.h"


OFTEST(dcmsr_addContentItem_1)
{
    /* first, create an SR document to get an empty SR tree */
    DSRDocument doc(DSRTypes::DT_ComprehensiveSR);
    DSRDocumentTree &tree = doc.getTree();
    /* then try to add some content items */
    OFCHECK(tree.addContentItem(DSRTypes::RT_isRoot, DSRTypes::VT_Container));
    /* add content item with given pointer */
    DSRNumTreeNode *numNode = new DSRNumTreeNode(DSRTypes::RT_contains);
    OFCHECK(tree.addContentItem(numNode, DSRTypes::AM_belowCurrent).good());
    if (numNode != NULL)
    {
        OFCHECK(numNode->getRelationshipType() == DSRTypes::RT_contains);
        OFCHECK(numNode->getValueType() == DSRTypes::VT_Num);
        OFCHECK(numNode->setConceptName(DSRCodedEntryValue("121206", "DCM", "Distance")).good());
        OFCHECK(numNode->setValue("1.5", DSRCodedEntryValue("cm", "UCUM", "centimeter")).good());
        OFCHECK(numNode->setRationalRepresentation(3, 2).good());
        OFCHECK(numNode->setFloatingPointRepresentation(1.5).good());
        /* the following code usually implies "Measurement not available", but this is only a test */
        OFCHECK(numNode->setNumericValueQualifier(DSRCodedEntryValue("114006", "DCM", "Measurement failure")).good());
    } else
        OFCHECK_FAIL("could not create new NUM content item");
    /* try to add content items that should fail */
    OFCHECK(tree.addContentItem(NULL).bad());
    OFCHECK(tree.addContentItem(DSRTypes::createDocumentTreeNode(DSRTypes::RT_hasProperties, DSRTypes::VT_Text), DSRTypes::AM_afterCurrent, OFTrue /*deleteIfFail*/).bad());
    OFCHECK(tree.addContentItem(DSRTypes::RT_unknown, DSRTypes::VT_Text) == 0);
    OFCHECK(tree.addContentItem(DSRTypes::RT_isRoot, DSRTypes::VT_Container) == 0);
    /* NB: this test does not always delete allocated memory (if adding a node fails) */
}


OFTEST(dcmsr_addContentItem_2)
{
    /* first, create an empty document subtree */
    DSRDocumentSubTree tree;
    /* then, try to add some invalid content items */
    OFCHECK(tree.addContentItem(DSRTypes::createDocumentTreeNode(DSRTypes::RT_hasProperties, DSRTypes::VT_byReference), DSRTypes::AM_afterCurrent, OFTrue /*deleteIfFail*/).bad());
    OFCHECK(tree.addContentItem(new DSRIncludedTemplateTreeNode(DSRSharedSubTemplate(NULL), DSRTypes::RT_contains), DSRTypes::AM_afterCurrent, OFTrue /*deleteIfFail*/).bad());
}


OFTEST(dcmsr_copyContentItem)
{
    /* first, create a new SR document */
    DSRDocument doc(DSRTypes::DT_ComprehensiveSR);
    DSRDocumentTree &tree = doc.getTree();
    /* then add some content items */
    OFCHECK(tree.addContentItem(DSRTypes::RT_isRoot, DSRTypes::VT_Container));
    OFCHECK(tree.addContentItem(DSRTypes::RT_contains, DSRTypes::VT_Text, DSRTypes::AM_belowCurrent));
    OFCHECK(tree.getCurrentContentItem().setStringValue("Sample text").good());
    OFCHECK(tree.getCurrentContentItem().setConceptName(DSRCodedEntryValue("121111", "DCM", "Summary")).good());
    OFCHECK(tree.addContentItem(DSRTypes::RT_contains, DSRTypes::VT_Num, DSRTypes::AM_afterCurrent));
    /* create a copy of the current content item */
    DSRContentItem item(tree.getCurrentContentItem());
    OFCHECK_EQUAL(item.getValueType(), DSRTypes::VT_Num);
    OFCHECK_EQUAL(item.getRelationshipType(), DSRTypes::RT_contains);
    /* clone the previous content item */
    OFCHECK(tree.gotoPrevious() > 0);
    DSRDocumentTreeNode *treeNode = tree.cloneCurrentTreeNode();
    if (treeNode != NULL)
    {
        /* and check some details */
        if (treeNode->getValueType() == DSRTypes::VT_Text)
        {
            DSRTextTreeNode *textNode = OFstatic_cast(DSRTextTreeNode *, treeNode);
            OFCHECK_EQUAL(textNode->getValue(), "Sample text");
        } else
            OFCHECK_FAIL("clone of TEXT content item has wrong value type");
        OFCHECK_EQUAL(treeNode->getRelationshipType(), DSRTypes::RT_contains);
        OFCHECK_EQUAL(treeNode->getConceptName().getCodeMeaning(), "Summary");
        delete treeNode;
    } else
        OFCHECK_FAIL("could not create clone of TEXT content item");
}


OFTEST(dcmsr_getCurrentNode)
{
    /* first, create a new SR document */
    DSRDocument doc(DSRTypes::DT_ComprehensiveSR);
    DSRDocumentTree &tree = doc.getTree();
    /* then add some content items */
    OFCHECK(tree.addContentItem(DSRTypes::RT_isRoot, DSRTypes::VT_Container, DSRCodedEntryValue("121111", "DCM", "Summary")).good());
    OFCHECK(tree.addChildContentItem(DSRTypes::RT_contains, DSRTypes::VT_Num, DSRCodedEntryValue("121206", "DCM", "Distance")).good());
    /* and check the current node */
    const DSRDocumentTreeNode *treeNode = tree.getCurrentNode();
    if (treeNode != NULL)
    {
        OFCHECK_EQUAL(treeNode->getRelationshipType(), DSRTypes::RT_contains);
        OFCHECK_EQUAL(treeNode->getValueType(), DSRTypes::VT_Num);
        OFCHECK_EQUAL(treeNode->getConceptName().getCodeMeaning(), "Distance");
    } else
        OFCHECK_FAIL("could not get read-only access to current node");
}


OFTEST(dcmsr_gotoNamedNode)
{
    /* first, create a new SR document */
    DSRDocument doc(DSRTypes::DT_ComprehensiveSR);
    DSRDocumentTree &tree = doc.getTree();
    /* then add some content items */
    OFCHECK(tree.addContentItem(DSRTypes::RT_isRoot, DSRTypes::VT_Container, DSRCodedEntryValue("121111", "DCM", "Summary")).good());
    OFCHECK(tree.addChildContentItem(DSRTypes::RT_contains, DSRTypes::VT_Num, DSRCodedEntryValue("121206", "DCM", "Distance")).good());
    const size_t nodeID = tree.getNodeID();
    OFCHECK(tree.addContentItem(DSRTypes::RT_contains, DSRTypes::VT_Text, DSRCodedEntryValue("1234", "99_PRV", "NOS")).good());
    /* and check the "search by name" function */
    OFCHECK_EQUAL(tree.gotoNamedNode(DSRCodedEntryValue("121206", "DCM", "Distance")), nodeID);
    OFCHECK_EQUAL(tree.gotoNamedNode(DSRCodedEntryValue("1234", "99_PRV", "NOS")), nodeID + 1);
}


OFTEST(dcmsr_gotoAnnotatedNode)
{
    /* first, create a new SR document */
    DSRDocument doc(DSRTypes::DT_ComprehensiveSR);
    DSRDocumentTree &tree = doc.getTree();
    /* then add some content items (and annotate them) */
    OFCHECK(tree.addContentItem(DSRTypes::RT_isRoot, DSRTypes::VT_Container, DSRCodedEntryValue("121111", "DCM", "Summary")).good());
    OFCHECK(tree.getCurrentContentItem().setAnnotationText("Root Node").good());
    OFCHECK(tree.addChildContentItem(DSRTypes::RT_contains, DSRTypes::VT_Num, DSRCodedEntryValue("121206", "DCM", "Distance")).good());
    OFCHECK(tree.getCurrentContentItem().setAnnotationText("Numeric Node").good());
    const size_t nodeID = tree.getNodeID();
    OFCHECK(tree.addContentItem(DSRTypes::RT_contains, DSRTypes::VT_Text, DSRCodedEntryValue("1234", "99_PRV", "NOS")).good());
    OFCHECK(tree.getCurrentContentItem().setAnnotationText("Text Node").good());
    /* and check the "search by name" function */
    OFCHECK_EQUAL(tree.gotoAnnotatedNode("Numeric Node"), nodeID);
    OFCHECK_EQUAL(tree.gotoAnnotatedNode("Text Node"), nodeID + 1);
}


OFTEST(dcmsr_createDocSubTree)
{
    /* first, create an empty document subtree */
    DSRDocumentSubTree tree;
    /* then try to add some content items (no dedicated root) */
    OFCHECK(tree.addContentItem(DSRTypes::RT_unknown, DSRTypes::VT_Text));
    OFCHECK(tree.addContentItem(DSRTypes::RT_unknown, DSRTypes::VT_Num));
    OFCHECK(tree.addContentItem(DSRTypes::RT_hasProperties, DSRTypes::VT_Code, DSRTypes::AM_belowCurrent));
    /* try to add content items that should fail */
    OFCHECK(tree.addContentItem(DSRTypes::RT_unknown, DSRTypes::VT_Text) == 0);
    OFCHECK(tree.addContentItem(DSRTypes::RT_invalid, DSRTypes::VT_Date) == 0);
    OFCHECK(tree.addContentItem(DSRTypes::RT_contains, DSRTypes::VT_invalid) == 0);
    OFCHECK(tree.addContentItem(DSRTypes::RT_invalid, DSRTypes::VT_invalid) == 0);
    OFCHECK(tree.addContentItem(DSRTypes::RT_isRoot, DSRTypes::VT_Container) == 0);
}


OFTEST(dcmsr_copyDocSubTree)
{
    /* first, create a document subtree with some content items */
    DSRDocumentSubTree tree;
    OFCHECK(tree.addContentItem(DSRTypes::RT_unknown, DSRTypes::VT_Text));
    OFCHECK(tree.addContentItem(DSRTypes::RT_unknown, DSRTypes::VT_Num));
    OFCHECK(tree.addContentItem(DSRTypes::RT_hasProperties, DSRTypes::VT_Code, DSRTypes::AM_belowCurrent));
    /* then copy this tree and check the content items */
    DSRDocumentSubTree newTree(tree);
    OFCHECK_EQUAL(newTree.getCurrentContentItem().getValueType(), DSRTypes::VT_Text);
    OFCHECK(newTree.gotoNext() > 0);
    OFCHECK_EQUAL(newTree.getCurrentContentItem().getValueType(), DSRTypes::VT_Num);
    OFCHECK(newTree.goDown() > 0);
    OFCHECK_EQUAL(newTree.getCurrentContentItem().getValueType(), DSRTypes::VT_Code);
    OFCHECK_EQUAL(newTree.getCurrentContentItem().getRelationshipType(), DSRTypes::RT_hasProperties);
}


OFTEST(dcmsr_cloneDocSubTree)
{
    /* first, create a new SR document */
    DSRDocument doc(DSRTypes::DT_ComprehensiveSR);
    DSRDocumentTree &tree = doc.getTree();
    /* then add some content items */
    OFCHECK(tree.addContentItem(DSRTypes::RT_isRoot, DSRTypes::VT_Container));
    OFCHECK(tree.addContentItem(DSRTypes::RT_contains, DSRTypes::VT_Text, DSRTypes::AM_belowCurrent));
    OFCHECK(tree.addContentItem(DSRTypes::RT_contains, DSRTypes::VT_Num, DSRTypes::AM_afterCurrent));
    OFCHECK(tree.getCurrentContentItem().setConceptName(DSRCodedEntryValue("121206", "DCM", "Distance")).good());
    OFCHECK(tree.addContentItem(DSRTypes::RT_hasProperties, DSRTypes::VT_Code, DSRTypes::AM_belowCurrent));
    OFCHECK(tree.addContentItem(DSRTypes::RT_hasConceptMod, DSRTypes::VT_Code, DSRTypes::AM_afterCurrent));
    /* and clone a particular subtree */
    OFCHECK(tree.gotoNamedNode(DSRCodedEntryValue("121206", "DCM", "Distance")) > 0);
    DSRDocumentSubTree *newTree = tree.cloneSubTree();
    if (newTree != NULL)
    {
        /* check the content items */
        OFCHECK_EQUAL(newTree->countNodes(), 3);
        OFCHECK_EQUAL(newTree->getCurrentContentItem().getValueType(), DSRTypes::VT_Num);
        OFCHECK_EQUAL(newTree->getCurrentContentItem().getRelationshipType(), DSRTypes::RT_contains);
        OFCHECK(newTree->iterate() > 0);
        OFCHECK_EQUAL(newTree->getCurrentContentItem().getValueType(), DSRTypes::VT_Code);
        OFCHECK_EQUAL(newTree->getCurrentContentItem().getRelationshipType(), DSRTypes::RT_hasProperties);
        OFCHECK(newTree->iterate() > 0);
        OFCHECK_EQUAL(newTree->getCurrentContentItem().getValueType(), DSRTypes::VT_Code);
        OFCHECK_EQUAL(newTree->getCurrentContentItem().getRelationshipType(), DSRTypes::RT_hasConceptMod);
        delete newTree;
    } else
        OFCHECK_FAIL("could not create clone of subtree");
}


OFTEST(dcmsr_insertDocSubTree_1)
{
    /* first, create an empty document subtree*/
    DSRDocumentSubTree tree;
    /* then create another document subtree with some content items */
    DSRDocumentSubTree *newTree1 = new DSRDocumentSubTree;
    if (newTree1 != NULL)
    {
        OFCHECK(newTree1->addContentItem(DSRTypes::RT_isRoot, DSRTypes::VT_Container));
        OFCHECK(newTree1->addContentItem(DSRTypes::RT_contains, DSRTypes::VT_Text, DSRTypes::AM_belowCurrent));
        OFCHECK(newTree1->addContentItem(DSRTypes::RT_contains, DSRTypes::VT_Num, DSRTypes::AM_afterCurrent));
        OFCHECK(newTree1->getCurrentContentItem().setConceptName(DSRCodedEntryValue("121206", "DCM", "Distance")).good());
        OFCHECK(newTree1->addContentItem(DSRTypes::RT_hasProperties, DSRTypes::VT_Code, DSRTypes::AM_belowCurrent));
        OFCHECK(newTree1->addContentItem(DSRTypes::RT_hasConceptMod, DSRTypes::VT_Code, DSRTypes::AM_afterCurrent));
        /* and insert it into the first one */
        OFCHECK(tree.insertSubTree(newTree1, DSRTypes::AM_belowCurrent, DSRTypes::RT_unknown, OFTrue /*deleteIfFail*/).good());
        OFCHECK_EQUAL(tree.countNodes(), 5);
    } else
        OFCHECK_FAIL("could not allocate memory for subtree");
    /* create a third document subtree with some content items */
    DSRDocumentSubTree *newTree2 = new DSRDocumentSubTree;
    if (newTree2 != NULL)
    {
        OFCHECK(newTree2->addContentItem(DSRTypes::RT_unknown, DSRTypes::VT_Text));
        OFCHECK(newTree2->addContentItem(DSRTypes::RT_unknown, DSRTypes::VT_Num));
        OFCHECK(newTree2->addContentItem(DSRTypes::RT_hasProperties, DSRTypes::VT_Code, DSRTypes::AM_belowCurrent));
        /* and insert it into the first one (below a certain node of the second subtree) */
        OFCHECK(tree.gotoNamedNode(DSRCodedEntryValue("121206", "DCM", "Distance")) > 0);
        OFCHECK(tree.insertSubTree(newTree2, DSRTypes::AM_belowCurrent, DSRTypes::RT_contains, OFTrue /*deleteIfFail*/).good());
        OFCHECK_EQUAL(tree.countNodes(), 8);
    } else
        OFCHECK_FAIL("could not allocate memory for subtree");
}


OFTEST(dcmsr_insertDocSubTree_2)
{
    /* first, create an empty document subtree*/
    DSRDocumentSubTree tree;
    /* then create another empty document subtree */
    DSRDocumentSubTree *newTree = new DSRDocumentSubTree;
    if (newTree != NULL)
    {
        /* and try to insert it (should fail) */
        OFCHECK(tree.insertSubTree(newTree, DSRTypes::AM_belowCurrent, DSRTypes::RT_contains, OFTrue /*deleteIfFail*/).bad());
    } else
        OFCHECK_FAIL("could not allocate memory for subtree");
    /* inserting a NULL subtrees should also fail */
    OFCHECK(tree.insertSubTree(NULL).bad());
    /* original tree should still be empty */
    OFCHECK(tree.isEmpty());
    OFCHECK_EQUAL(tree.countNodes(), 0);
}


OFTEST(dcmsr_insertDocSubTree_3)
{
    /* first, create a new SR document */
    DSRDocument doc(DSRTypes::DT_ComprehensiveSR);
    DSRDocumentTree &tree = doc.getTree();
    /* then add some content items */
    OFCHECK(tree.addContentItem(DSRTypes::RT_isRoot, DSRTypes::VT_Container));
    OFCHECK(tree.addContentItem(DSRTypes::RT_contains, DSRTypes::VT_Text, DSRTypes::AM_belowCurrent));
    OFCHECK(tree.addContentItem(DSRTypes::RT_contains, DSRTypes::VT_Num, DSRTypes::AM_afterCurrent));
    OFCHECK(tree.getCurrentContentItem().setConceptName(DSRCodedEntryValue("121206", "DCM", "Distance")).good());
    OFCHECK(tree.addContentItem(DSRTypes::RT_hasProperties, DSRTypes::VT_Code, DSRTypes::AM_belowCurrent));
    OFCHECK(tree.addContentItem(DSRTypes::RT_hasConceptMod, DSRTypes::VT_Code, DSRTypes::AM_afterCurrent));
    OFCHECK_EQUAL(tree.countNodes(), 5);
    /* clone a particular subtree */
    OFCHECK(tree.gotoNamedNode(DSRCodedEntryValue("121206", "DCM", "Distance")) > 0);
    DSRDocumentSubTree *newTree1 = tree.cloneSubTree();
    DSRDocumentSubTree *newTree2 = newTree1->clone();
    DSRDocumentSubTree *newTree3 = newTree2->clone();
    /* and try to add it to the SR document at different nodes */
    OFCHECK(tree.insertSubTree(newTree1, DSRTypes::AM_belowCurrent, DSRTypes::RT_unknown, OFTrue /*deleteIfFail*/).bad());
    OFCHECK_EQUAL(tree.countNodes(), 5);
    /* second try at another node (this time it should work) */
    OFCHECK(tree.gotoPrevious() > 0);
    OFCHECK(tree.insertSubTree(newTree2, DSRTypes::AM_beforeCurrent, DSRTypes::RT_unknown, OFTrue /*deleteIfFail*/).good());
    OFCHECK_EQUAL(tree.countNodes(), 8);
    /* third try at another node (should also work) */
    OFCHECK(tree.gotoRoot() > 0);
    OFCHECK(tree.insertSubTree(newTree3, DSRTypes::AM_belowCurrent, DSRTypes::RT_unknown, OFTrue /*deleteIfFail*/).good());
    OFCHECK_EQUAL(tree.countNodes(), 11);
    /* and finally, delete all nodes */
    tree.clear();
    OFCHECK(tree.isEmpty());
    OFCHECK_EQUAL(tree.countNodes(), 0);
}


OFTEST(dcmsr_removeDocSubTree)
{
    /* first, create a new SR document */
    DSRDocument doc(DSRTypes::DT_ComprehensiveSR);
    DSRDocumentTree &tree = doc.getTree();
    /* then add some content items */
    OFCHECK(tree.addContentItem(DSRTypes::RT_isRoot, DSRTypes::VT_Container));
    OFCHECK(tree.addContentItem(DSRTypes::RT_contains, DSRTypes::VT_Text, DSRTypes::AM_belowCurrent));
    OFCHECK(tree.addContentItem(DSRTypes::RT_contains, DSRTypes::VT_Num, DSRTypes::AM_afterCurrent));
    OFCHECK(tree.getCurrentContentItem().setConceptName(DSRCodedEntryValue("121206", "DCM", "Distance")).good());
    OFCHECK(tree.addContentItem(DSRTypes::RT_hasProperties, DSRTypes::VT_Code, DSRTypes::AM_belowCurrent));
    OFCHECK(tree.addContentItem(DSRTypes::RT_hasConceptMod, DSRTypes::VT_Code, DSRTypes::AM_afterCurrent));
    OFCHECK_EQUAL(tree.countNodes(), 5);
    /* and, remove a particular subtree */
    OFCHECK(tree.gotoNamedNode(DSRCodedEntryValue("121206", "DCM", "Distance")) > 0);
    OFCHECK(tree.removeSubTree().good());
    OFCHECK_EQUAL(tree.countNodes(), 2);
    OFCHECK(tree.gotoRoot());
    OFCHECK(tree.removeSubTree().good());
    OFCHECK_EQUAL(tree.countNodes(), 0);
    OFCHECK(tree.removeSubTree().bad());
}


OFTEST(dcmsr_extractDocSubTree_1)
{
    /* first, create a new SR document */
    DSRDocument doc(DSRTypes::DT_ComprehensiveSR);
    DSRDocumentTree &tree = doc.getTree();
    /* then add some content items */
    OFCHECK(tree.addContentItem(DSRTypes::RT_isRoot, DSRTypes::VT_Container));
    OFCHECK(tree.addContentItem(DSRTypes::RT_contains, DSRTypes::VT_Text, DSRTypes::AM_belowCurrent));
    OFCHECK(tree.addContentItem(DSRTypes::RT_contains, DSRTypes::VT_Num, DSRTypes::AM_afterCurrent));
    OFCHECK(tree.getCurrentContentItem().setConceptName(DSRCodedEntryValue("121206", "DCM", "Distance")).good());
    OFCHECK(tree.addContentItem(DSRTypes::RT_hasProperties, DSRTypes::VT_Code, DSRTypes::AM_belowCurrent));
    OFCHECK(tree.addContentItem(DSRTypes::RT_hasConceptMod, DSRTypes::VT_Code, DSRTypes::AM_afterCurrent));
    OFCHECK_EQUAL(tree.countNodes(), 5);
    /* and, extract a particular subtree */
    OFCHECK(tree.gotoNamedNode(DSRCodedEntryValue("121206", "DCM", "Distance")) > 0);
    DSRDocumentSubTree *subTree = tree.extractSubTree();
    if (subTree != NULL)
    {
        OFCHECK_EQUAL(tree.countNodes(), 2);
        OFCHECK_EQUAL(subTree->countNodes(), 3);
        OFCHECK_EQUAL(subTree->getCurrentContentItem().getValueType(), DSRTypes::VT_Num);
        OFCHECK_EQUAL(subTree->getCurrentContentItem().getRelationshipType(), DSRTypes::RT_contains);
        delete subTree;
    } else
        OFCHECK_FAIL("could not extract subtree from document");
}


OFTEST(dcmsr_extractDocSubTree_2)
{
    /* first, create a new SR document */
    DSRDocument doc(DSRTypes::DT_ComprehensiveSR);
    DSRDocumentTree &tree = doc.getTree();
    /* then, add some content items (incl. by-reference relationship) */
    OFCHECK(tree.addContentItem(DSRTypes::RT_isRoot, DSRTypes::VT_Container));
    OFCHECK(tree.addContentItem(DSRTypes::RT_contains, DSRTypes::VT_Text, DSRTypes::AM_belowCurrent));
    OFCHECK(tree.getCurrentContentItem().setConceptName(DSRCodedEntryValue("121111", "DCM", "Summary")).good());
    const size_t refTarget = tree.addContentItem(DSRTypes::RT_hasProperties, DSRTypes::VT_Num, DSRTypes::AM_belowCurrent);
    OFCHECK(refTarget > 0);
    OFCHECK(tree.addContentItem(DSRTypes::RT_hasProperties, DSRTypes::VT_Code, DSRTypes::AM_afterCurrent));
    const size_t refSource = tree.addByReferenceRelationship(DSRTypes::RT_inferredFrom, refTarget);
    OFCHECK(refSource > 0);
    OFCHECK(tree.gotoRoot());
    OFCHECK(tree.addContentItem(DSRTypes::RT_contains, DSRTypes::VT_Text, DSRTypes::AM_belowCurrent));
    /* extract a particular subtree */
    OFCHECK(tree.gotoNamedNode(DSRCodedEntryValue("121111", "DCM", "Summary")) > 0);
    DSRDocumentSubTree *subTree = tree.extractSubTree();
    if (subTree != NULL)
    {
        OFCHECK_EQUAL(tree.countNodes(), 2);
        OFCHECK_EQUAL(subTree->countNodes(), 4);
        /* and finally, check whether the by-reference relationship is still valid */
        OFCHECK(subTree->gotoNode(refSource));
        OFCHECK_EQUAL(subTree->getCurrentContentItem().getRelationshipType(), DSRTypes::RT_inferredFrom);
        OFCHECK_EQUAL(subTree->getCurrentContentItem().getReferencedNodeID(), refTarget);
        OFCHECK(subTree->gotoNode(refTarget));
        OFCHECK_EQUAL(subTree->getCurrentContentItem().getValueType(), DSRTypes::VT_Num);
        OFCHECK_EQUAL(subTree->getCurrentContentItem().getRelationshipType(), DSRTypes::RT_hasProperties);
        delete subTree;
    } else
        OFCHECK_FAIL("could not extract subtree from document");
}


OFTEST(dcmsr_extractAndInsertDocSubTree_1)
{
    /* first, create a new SR document */
    DSRDocument doc(DSRTypes::DT_ComprehensiveSR);
    DSRDocumentTree &tree = doc.getTree();
    /* then add some content items */
    OFCHECK(tree.addContentItem(DSRTypes::RT_isRoot, DSRTypes::VT_Container));
    OFCHECK(tree.addContentItem(DSRTypes::RT_contains, DSRTypes::VT_Text, DSRTypes::AM_belowCurrent));
    OFCHECK(tree.addContentItem(DSRTypes::RT_contains, DSRTypes::VT_Num, DSRTypes::AM_afterCurrent));
    OFCHECK(tree.getCurrentContentItem().setConceptName(DSRCodedEntryValue("121206", "DCM", "Distance")).good());
    OFCHECK(tree.addContentItem(DSRTypes::RT_hasProperties, DSRTypes::VT_Code, DSRTypes::AM_belowCurrent));
    OFCHECK(tree.addContentItem(DSRTypes::RT_hasConceptMod, DSRTypes::VT_Code, DSRTypes::AM_afterCurrent));
    OFCHECK_EQUAL(tree.countNodes(), 5);
    /* extract the complete (sub)tree */
    OFCHECK(tree.gotoRoot() > 0);
    DSRDocumentSubTree *subTree = tree.extractSubTree();
    if (subTree != NULL)
    {
        OFCHECK_EQUAL(tree.countNodes(), 0);
        OFCHECK_EQUAL(subTree->countNodes(), 5);
        OFCHECK(subTree->isValidDocumentTree());
        /* and, finally, re-add it to the original SR document */
        OFCHECK(tree.insertSubTree(subTree, DSRTypes::AM_belowCurrent, DSRTypes::RT_unknown, OFTrue /*deleteIfFail*/).good());
        OFCHECK_EQUAL(tree.countNodes(), 5);
    } else
        OFCHECK_FAIL("could not extract subtree from document");
}


OFTEST(dcmsr_extractAndInsertDocSubTree_2)
{
    /* first, create a new SR document */
    DSRDocument doc(DSRTypes::DT_ComprehensiveSR);
    DSRDocumentTree &tree = doc.getTree();
    /* then add some content items */
    OFCHECK(tree.addContentItem(DSRTypes::RT_isRoot, DSRTypes::VT_Container));
    OFCHECK(tree.addContentItem(DSRTypes::RT_contains, DSRTypes::VT_Text, DSRTypes::AM_belowCurrent));
    OFCHECK(tree.addContentItem(DSRTypes::RT_contains, DSRTypes::VT_Num, DSRTypes::AM_afterCurrent));
    OFCHECK(tree.getCurrentContentItem().setConceptName(DSRCodedEntryValue("121206", "DCM", "Distance")).good());
    OFCHECK(tree.addContentItem(DSRTypes::RT_hasProperties, DSRTypes::VT_Code, DSRTypes::AM_belowCurrent));
    OFCHECK(tree.addContentItem(DSRTypes::RT_hasConceptMod, DSRTypes::VT_Code, DSRTypes::AM_afterCurrent));
    OFCHECK_EQUAL(tree.countNodes(), 5);
    /* extract a particular subtree */
    OFCHECK(tree.gotoNamedNode(DSRCodedEntryValue("121206", "DCM", "Distance")) > 0);
    DSRDocumentSubTree *subTree = tree.extractSubTree();
    if (subTree != NULL)
    {
        OFCHECK_EQUAL(tree.countNodes(), 2);
        OFCHECK_EQUAL(subTree->countNodes(), 3);
        OFCHECK(!subTree->isValidDocumentTree());
        /* clear the original document tree */
        tree.clear();
        OFCHECK_EQUAL(tree.countNodes(), 0);
        /* and, finally, try to re-add the subtree */
        OFCHECK(tree.insertSubTree(subTree, DSRTypes::AM_belowCurrent, DSRTypes::RT_unknown, OFTrue /*deleteIfFail*/).bad());
        OFCHECK_EQUAL(tree.countNodes(), 0);
    } else
        OFCHECK_FAIL("could not extract subtree from document");
}
