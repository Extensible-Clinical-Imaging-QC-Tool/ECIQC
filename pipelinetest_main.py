import os
import time


from tests.pipelinetests import PipelineTests
from pynetdicom import AE, debug_logger
from pydicom.uid import ExplicitVRLittleEndian
from pynetdicom.sop_class import CTImageStorage


if __name__ == '__main__':

    # TODO these paths need to be changed to use os.path.join
    # TODO this should also be able to run over multiple jsons.



    # load input directory
    pipelinetest = PipelineTests(input_file_path='./DICOM_Images/')

    # run pipeline
    pipelinetest.run_pipeline("./schema/PipelineCase.json", "./result/", "./result_quarantine/", use_pynetdicom=True)
    
    # File for test results  
    output_json = "schema/tests/complete_expected_output.json"
    # Test results
    pipelinetest.check_output(output_json)


    
    
    ### threads are killed at the end of run pipeline but just in case we dont kill the processes automatically:
    time.sleep(10)
    pipelinetest.kill_all_processes()
