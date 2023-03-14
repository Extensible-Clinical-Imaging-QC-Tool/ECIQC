import os
import time


from tests.pipelinetests import PipelineTests
from pynetdicom import AE, debug_logger
from pydicom.uid import ExplicitVRLittleEndian
from pynetdicom.sop_class import CTImageStorage


if __name__ == '__main__':
    ### Kill all the processes on port 11112 11113 11114
    # os.system('kill $(lsof -t -i:11112) & kill $(lsof -t -i:11113) & kill $(lsof -t -i:11114)')
    
    
    pipelinetest = PipelineTests(input_file_path='./DICOM_Images/')

    # generate file list now run on init
    # pipelinetest.generate_file_list()


    # TODO these paths need to be changed to use os.path.join
    pipelinetest.run_pipeline("./schema/PipelineCase.json","./result/","./result_quarantine/",use_pynetdicom=True)
    

    ### threads are killed at the end of run pipeline but just in case we dont kill the processes automatically:
    time.sleep(10)
    pipelinetest.kill_all_processes()


    output_json = "schema/tests/complete_expected_output.json"
    
    pipelinetest.check_output(output_json)

    # os.system('kill $(lsof -t -i:11112) & kill $(lsof -t -i:11113) & kill $(lsof -t -i:11114)')
    