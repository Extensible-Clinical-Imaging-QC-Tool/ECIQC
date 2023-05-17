import os
import time


from tests.pipelinetests import PipelineTests
from pynetdicom import AE, debug_logger
from pydicom.uid import ExplicitVRLittleEndian
# from pynetdicom.sop_class import CTImageStorage


if __name__ == '__main__':
    ### Kill all the processes on port 11112 11113 11114
    
    # remove the old .dcm files in results folder before starting
    for filename in os.listdir('./result'):
        if filename.endswith(".dcm"):
            os.remove('./result/'+filename)

    pipelinetest = PipelineTests(json_path="./schema/PipelineCase.json",
                                 input_file_path='./DICOM_Images',
                                 output_file_path="./result/",
                                 quarantine_file_path="./result_quarantine/")
    
    pipelinetest.run_pipeline(use_pynetdicom=True)
    # File for test answers to compare with the real results  
    output_json = "schema/tests/complete_expected_output.json"
    # Test results
    pipelinetest.check_output(output_json)

    ### threads are killed at the end of run pipeline but just in case we dont kill the processes automatically:
    time.sleep(10)
    # pipelinetest.kill_all_processes()
