import os
import time


from tests.pipelinetests import PipelineTests
from pynetdicom import AE, debug_logger
from pydicom.uid import ExplicitVRLittleEndian
from pynetdicom.sop_class import CTImageStorage
from pathlib import Path


if __name__ == '__main__':
    
    # Create directories to store the results of the pipeline test
    Path("./result").mkdir(exist_ok=True)
    Path("./result_quarantine").mkdir(exist_ok=True)

    # Run the pipeline test
    pipelinetest = PipelineTests(input_file_path='./DICOM_Images',
                                 output_file_path="./result",
                                 quarantine_file_path="./result_quarantine")
    pipelinetest.generate_file_list(json_to_save_name="pipeline_test_file_name_list")
    pipelinetest.run_pipeline(json_path="./schema/PipelineCase.json", use_pynetdicom=True)
