import os
import time


from tests.pipelinetests import PipelineTests
from pynetdicom import AE, debug_logger
from pydicom.uid import ExplicitVRLittleEndian
from pynetdicom.sop_class import CTImageStorage


if __name__ == '__main__':
    pipelinetest = PipelineTests('./DICOM_Images_temp/')
    pipelinetest.generate_file_list()
    pipelinetest.run_pipeline("./schema/MultipleConditionCaseShort.json",
                              "./result/",
                              "./result_quarantine/",
                              use_pynetdicom=True,
                              scan_directories=True,
                              #input_file=['testtext.dcm']
                              )
