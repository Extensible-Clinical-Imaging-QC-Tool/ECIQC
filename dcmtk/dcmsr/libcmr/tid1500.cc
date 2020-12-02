/*
 *
 *  Copyright (C) 2015-2016, J. Riesmeier, Oldenburg, Germany
 *  All rights reserved.  See COPYRIGHT file for details.
 *
 *  Source file for class TID1500_MeasurementReport
 *
 *  Author: Joerg Riesmeier
 *
 */


#include "dcmtk/config/osconfig.h"    /* make sure OS specific configuration is included first */

#include "dcmtk/dcmsr/cmr/tid1500.h"
#include "dcmtk/dcmsr/cmr/logger.h"
#include "dcmtk/dcmsr/codes/dcm.h"
#include "dcmtk/dcmsr/codes/umls.h"
#include "dcmtk/dcmsr/dsrtpltn.h"

#include "dcmtk/dcmdata/dcuid.h"


// helper macros for checking the return value of API calls
#define CHECK_RESULT(call) if (result.good()) result = call
#define STORE_RESULT(call) result = call
#define GOOD_RESULT(call) if (result.good()) call
#define BAD_RESULT(call) if (result.bad()) call

// index positions in node list (makes source code more readable)
#define MEASUREMENT_REPORT               0
#define OBSERVATION_CONTEXT              1
#define LAST_PROCEDURE_REPORTED          2
#define IMAGING_MEASUREMENTS             3
#define LAST_VOLUMETRIC_ROI_MEASUREMENTS 4
#define QUALITATIVE_EVALUATIONS          5

// general information on TID 1500 (Measurement Report)
#define TEMPLATE_NUMBER      "1500"
#define MAPPING_RESOURCE     "DCMR"
#define MAPPING_RESOURCE_UID UID_DICOMContentMappingResource
#define TEMPLATE_TYPE        OFTrue  /* extensible */

// conditions constants
makeOFConditionConst(CMR_EC_NoMeasurementReport, OFM_dcmsr, 1500, OF_error, "No Measurement Report");


TID1500_MeasurementReport::TID1500_MeasurementReport(const CID7021_MeasurementReportDocumentTitles &title)
  : DSRRootTemplate(DT_EnhancedSR, TEMPLATE_NUMBER, MAPPING_RESOURCE, MAPPING_RESOURCE_UID),
    Language(new TID1204_LanguageOfContentItemAndDescendants()),
    ObservationContext(new TID1001_ObservationContext()),
    ImageLibrary(new TID1600_ImageLibrary()),
    VolumetricROIMeasurements(new TID1411_Measurements())
{
    setExtensible(TEMPLATE_TYPE);
    /* need to store position of various content items */
    reserveEntriesInNodeList(6, OFTrue /*initialize*/);
    /* if specified, create an initial report */
    if (title.hasSelectedValue())
        createMeasurementReport(title);
}


void TID1500_MeasurementReport::clear()
{
    DSRRootTemplate::clear();
    Language->clear();
    ObservationContext->clear();
    ImageLibrary->clear();
    VolumetricROIMeasurements->clear();
}


OFBool TID1500_MeasurementReport::isValid() const
{
    /* check whether base class is valid and all required content items are present */
    return DSRRootTemplate::isValid() &&
        Language->isValid() && ObservationContext->isValid() && ImageLibrary->isValid() &&
        hasProcedureReported() && (hasImagingMeasurements() || hasQualitativeEvaluations());
}


OFBool TID1500_MeasurementReport::hasProcedureReported() const
{
    /* check for content item at TID 1500 (Measurement Report) Row 4 */
    return (getEntryFromNodeList(LAST_PROCEDURE_REPORTED) > 0);
}


OFBool TID1500_MeasurementReport::hasImagingMeasurements(const OFBool checkChildren) const
{
    OFBool result = OFFalse;
    /* need to check for child nodes? */
    if (checkChildren)
    {
        DSRDocumentTreeNodeCursor cursor(getRoot());
        /* go to content item at TID 1500 (Measurement Report) Row 6 */
        if (gotoEntryFromNodeList(cursor, IMAGING_MEASUREMENTS) > 0)
        {
            /* check whether any of the "included templates" is non-empty */
            if (cursor.gotoChild())
            {
                while (cursor.isValid() && (cursor.getNode()->getValueType() == VT_includedTemplate))
                {
                    const DSRSubTemplate *subTempl = OFstatic_cast(const DSRIncludedTemplateTreeNode *, cursor.getNode())->getValue().get();
                    if (subTempl != NULL)
                    {
                        result = !subTempl->isEmpty();
                        if (result) break;
                    }
                    if (cursor.gotoNext() == 0)
                    {
                        /* invalidate cursor */
                        cursor.clear();
                    }
                }
            }
        }
    } else {
        /* check for content item at TID 1500 (Measurement Report) Row 6 */
        result = (getEntryFromNodeList(IMAGING_MEASUREMENTS) > 0);
    }
    return result;
}


OFBool TID1500_MeasurementReport::hasVolumetricROIMeasurements(const OFBool checkChildren) const
{
    OFBool result = OFFalse;
    /* need to check for child nodes? */
    if (checkChildren)
    {
        DSRDocumentTreeNodeCursor cursor(getRoot());
        /* go to content item at TID 1500 (Measurement Report) Row 8 */
        if (gotoEntryFromNodeList(cursor, LAST_VOLUMETRIC_ROI_MEASUREMENTS) > 0)
        {
            /* check whether any of the "included TID 1411 templates" is non-empty */
            while (cursor.isValid() && (cursor.getNode()->getValueType() == VT_includedTemplate))
            {
                const DSRSubTemplate *subTempl = OFstatic_cast(const DSRIncludedTemplateTreeNode *, cursor.getNode())->getValue().get();
                if (subTempl != NULL)
                {
                    if (subTempl->compareTemplateIdentication("1411", "DCMR"))
                    {
                        result = !subTempl->isEmpty();
                        if (result) break;
                    } else {
                        /* exit loop */
                        break;
                    }
                }
                if (cursor.gotoPrevious() == 0)
                {
                    /* invalidate cursor */
                    cursor.clear();
                }
            }
        }
    } else {
        /* check for content item at TID 1500 (Measurement Report) Row 8 */
        result = (getEntryFromNodeList(LAST_VOLUMETRIC_ROI_MEASUREMENTS) > 0);
    }
    return result;
}


OFBool TID1500_MeasurementReport::hasQualitativeEvaluations(const OFBool checkChildren) const
{
    OFBool result = OFFalse;
    /* need to check for child nodes? */
    if (checkChildren)
    {
        DSRDocumentTreeNodeCursor cursor(getRoot());
        /* go to content item at TID 1500 (Measurement Report) Row 12 */
        if (gotoEntryFromNodeList(cursor, QUALITATIVE_EVALUATIONS) > 0)
            result = cursor.hasChildNodes();
    } else {
        /* check for content item at TID 1500 (Measurement Report) Row 12 */
        result = (getEntryFromNodeList(QUALITATIVE_EVALUATIONS) > 0);
    }
    return result;
}


OFCondition TID1500_MeasurementReport::getDocumentTitle(DSRCodedEntryValue &titleCode)
{
    OFCondition result = EC_Normal;
    /* go to content item at TID 1500 (Measurement Report) Row 1 */
    if (gotoEntryFromNodeList(this, MEASUREMENT_REPORT) > 0)
    {
        titleCode = getCurrentContentItem().getConceptName();
    } else {
        /* in case of error, clear the result variable */
        titleCode.clear();
        result = SR_EC_ContentItemNotFound;
    }
    return result;
}


OFCondition TID1500_MeasurementReport::createNewMeasurementReport(const CID7021_MeasurementReportDocumentTitles &title)
{
    clear();
    /* TID 1500 (Measurement Report) Row 1 */
    return createMeasurementReport(title);
}


OFCondition TID1500_MeasurementReport::setLanguage(const CID5000_Languages &language,
                                                   const CID5001_Countries &country,
                                                   const OFBool check)
{
    /* TID 1500 (Measurement Report) Row 2 */
    return getLanguage().setLanguage(language, country, check);
}


OFCondition TID1500_MeasurementReport::addProcedureReported(const CID100_QuantitativeDiagnosticImagingProcedures &procedure,
                                                            const OFBool check)
{
    OFCondition result = EC_IllegalParameter;
    /* make sure that there is a coded entry */
    if (procedure.hasSelectedValue())
    {
        /* go to last content item at TID 1500 (Measurement Report) Row 3 or 4 */
        if (gotoLastEntryFromNodeList(this, LAST_PROCEDURE_REPORTED, OBSERVATION_CONTEXT) > 0)
        {
            /* TID 1500 (Measurement Report) Row 4 */
            STORE_RESULT(addContentItem(RT_hasConceptMod, VT_Code, CODE_DCM_ProcedureReported, check));
            CHECK_RESULT(getCurrentContentItem().setCodeValue(procedure, check));
            CHECK_RESULT(getCurrentContentItem().setAnnotationText("TID 1500 - Row 4"));
            /* store ID of recently added node for later use */
            GOOD_RESULT(storeEntryInNodeList(LAST_PROCEDURE_REPORTED, getNodeID()));
        } else
            result = CMR_EC_NoMeasurementReport;
    }
    return result;
}


OFCondition TID1500_MeasurementReport::addVolumetricROIMeasurements()
{
    OFCondition result = CMR_EC_NoMeasurementReport;
    /* go to content item at TID 1500 (Measurement Report) Row 8 */
    if (gotoEntryFromNodeList(this, LAST_VOLUMETRIC_ROI_MEASUREMENTS) > 0)
    {
        /* create new instance of TID 1411 (Volumetric ROI Measurements) */
        TID1411_Measurements *subTempl = new TID1411_Measurements();
        if (subTempl != NULL)
        {
            /* store (shared) reference to new instance */
            VolumetricROIMeasurements.reset(subTempl);
            /* and add it to the current template (TID 1500 - Row 8) */
            STORE_RESULT(includeTemplate(VolumetricROIMeasurements, AM_afterCurrent, RT_contains));
            CHECK_RESULT(getCurrentContentItem().setAnnotationText("TID 1500 - Row 8"));
            GOOD_RESULT(storeEntryInNodeList(LAST_VOLUMETRIC_ROI_MEASUREMENTS, getNodeID()));
            /* tbc: what if the call of includeTemplate() fails? */
        } else
            result = EC_MemoryExhausted;
    }
    return result;
}


OFCondition TID1500_MeasurementReport::addQualitativeEvaluation(const DSRCodedEntryValue &conceptName,
                                                                const DSRCodedEntryValue &codeValue,
                                                                const OFBool check)
{
    OFCondition result = EC_IllegalParameter;
    /* make sure that the parameters are non-empty */
    if (conceptName.isComplete() && codeValue.isComplete())
    {
        /* create content item at TID 1500 (Measurement Report) Row 12 if not existing */
        result = createQualitativeEvaluations();
        /* go to content item at TID 1500 (Measurement Report) Row 12 */
        if (gotoEntryFromNodeList(this, QUALITATIVE_EVALUATIONS) > 0)
        {
            /* TID 1500 (Measurement Report) Row 13 */
            CHECK_RESULT(addChildContentItem(RT_contains, VT_Code, conceptName, check));
            CHECK_RESULT(getCurrentContentItem().setCodeValue(codeValue, check));
            CHECK_RESULT(getCurrentContentItem().setAnnotationText("TID 1500 - Row 13"));
        } else
            result = CMR_EC_NoMeasurementReport;
    }
    return result;
}


OFCondition TID1500_MeasurementReport::addQualitativeEvaluation(const DSRCodedEntryValue &conceptName,
                                                                const OFString &stringValue,
                                                                const OFBool check)
{
    OFCondition result = EC_IllegalParameter;
    /* make sure that the parameters are non-empty */
    if (conceptName.isComplete() && !stringValue.empty())
    {
        /* create content item at TID 1500 (Measurement Report) Row 12 if not existing */
        result = createQualitativeEvaluations();
        /* go to content item at TID 1500 (Measurement Report) Row 12 */
        if (gotoEntryFromNodeList(this, QUALITATIVE_EVALUATIONS) > 0)
        {
            /* TID 1500 (Measurement Report) Row 14 */
            CHECK_RESULT(addChildContentItem(RT_contains, VT_Text, conceptName, check));
            CHECK_RESULT(getCurrentContentItem().setStringValue(stringValue, check));
            CHECK_RESULT(getCurrentContentItem().setAnnotationText("TID 1500 - Row 14"));
        } else
            result = CMR_EC_NoMeasurementReport;
    }
    return result;
}


// protected methods

OFCondition TID1500_MeasurementReport::createMeasurementReport(const CID7021_MeasurementReportDocumentTitles &title)
{
    OFCondition result = EC_IllegalParameter;
    /* make sure that there is a coded entry */
    if (title.hasSelectedValue())
    {
        /* reassure that the report is definitely empty */
        if (isEmpty())
        {
            /* TID 1500 (Measurement Report) Row 1 */
            STORE_RESULT(addContentItem(RT_isRoot, VT_Container, title));
            CHECK_RESULT(getCurrentContentItem().setAnnotationText("TID 1500 - Row 1"));
            /* store ID of root node for later use */
            if (result.good())
                storeEntryInNodeList(MEASUREMENT_REPORT, getNodeID());
            /* TID 1500 (Measurement Report) Row 2 */
            CHECK_RESULT(includeTemplate(Language, AM_belowCurrent, RT_hasConceptMod));
            CHECK_RESULT(getCurrentContentItem().setAnnotationText("TID 1500 - Row 2"));
            /* TID 1500 (Measurement Report) Row 3 */
            CHECK_RESULT(includeTemplate(ObservationContext, AM_afterCurrent, RT_hasObsContext));
            CHECK_RESULT(getCurrentContentItem().setAnnotationText("TID 1500 - Row 3"));
            GOOD_RESULT(storeEntryInNodeList(OBSERVATION_CONTEXT, getNodeID()));
            /* TID 1500 (Measurement Report) Row 5 */
            CHECK_RESULT(includeTemplate(ImageLibrary, AM_afterCurrent, RT_contains));
            CHECK_RESULT(getCurrentContentItem().setAnnotationText("TID 1500 - Row 5"));
            /* TID 1500 (Measurement Report) Row 6 */
            CHECK_RESULT(addContentItem(RT_contains, VT_Container, CODE_DCM_ImagingMeasurements));
            CHECK_RESULT(getCurrentContentItem().setAnnotationText("TID 1500 - Row 6"));
            GOOD_RESULT(storeEntryInNodeList(IMAGING_MEASUREMENTS, getNodeID()));
            /* TID 1500 (Measurement Report) Row 8 */
            CHECK_RESULT(includeTemplate(VolumetricROIMeasurements, AM_belowCurrent, RT_contains));
            CHECK_RESULT(getCurrentContentItem().setAnnotationText("TID 1500 - Row 8"));
            GOOD_RESULT(storeEntryInNodeList(LAST_VOLUMETRIC_ROI_MEASUREMENTS, getNodeID()));
            /* if anything went wrong, clear the report */
            BAD_RESULT(clear());
        } else
            result = SR_EC_InvalidTemplateStructure;
    }
    return result;
}


OFCondition TID1500_MeasurementReport::createQualitativeEvaluations()
{
    OFCondition result = EC_Normal;
    /* check whether content item at TID 1500 (Measurement Report) Row 12 already exists */
    if (!hasQualitativeEvaluations())
    {
        /* if not, go to the preceding content item, which always exists */
        if (gotoEntryFromNodeList(this, IMAGING_MEASUREMENTS) > 0)
        {
            /* ... and add TID 1500 (Measurement Report) Row 12 */
            STORE_RESULT(addContentItem(RT_contains, VT_Container, CODE_UMLS_QualitativeEvaluations));
            CHECK_RESULT(getCurrentContentItem().setAnnotationText("TID 1500 - Row 12"));
            GOOD_RESULT(storeEntryInNodeList(QUALITATIVE_EVALUATIONS, getNodeID()));
        } else
            result = CMR_EC_NoMeasurementReport;
    }
    return result;
}
