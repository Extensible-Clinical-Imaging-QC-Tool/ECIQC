#Extensible Clinical Imaging QC Tool

This is a repository for the SABS-R3 year-long software project with GE. 

Project description:

Imaging in clinical trials typically involves collecting data from multiple sites centrally for analysis.  Data quality can vary considerably due to differences in technology and local procedures.  Ingestion requires robust deidentification (to meet privacy and GCP regulations) and quality checks (to ensure adherence to the trial protocol), both of which can be painstakingly manual.  Initiatives providing data to researchers, such as UK Biobank or CSDR, perform further recoding on egress. These processes benefit from automatic checks and updates on image (meta)data according to user-defined rules.

Existing tools are typically proprietary or focussed on a specific modality or therapeutic area.  We believe data quality is fundamental and such tools are precompetitive: a standardised, open, extensible approach benefits the research community.
We envision an in-stream device that receives DICOM data, accepts or rejects based on configurable criteria (specific to the trial, site, anatomy and/or modality), performs corrections or redactions, and finally exports to a repository.

The focus will be on checking, updating and tracking metadata according to a defined schema (real-world examples will be provided).  The software shall have a plugin architecture, allowing e.g. image QC modules to be added in future
GE will provide domain expertise, documented user requirements and regular feedback.

