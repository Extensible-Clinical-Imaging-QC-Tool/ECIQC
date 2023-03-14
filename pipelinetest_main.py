import os
import time


from tests.pipelinetests import PipelineTests
from pynetdicom import AE, debug_logger
from pydicom.uid import ExplicitVRLittleEndian
from pynetdicom.sop_class import CTImageStorage


if __name__ == '__main__':
    ### Kill all the processes on port 11112 11113 11114
    os.system('kill $(lsof -t -i:11112) & kill $(lsof -t -i:11113) & kill $(lsof -t -i:11114)')
    
    
    pipelinetest = PipelineTests(input_file_path='./DICOM_Images/')
    pipelinetest.generate_file_list()
    pipelinetest.run_pipeline("./schema/PipelineCase.json","./result/","./result_quarantine/",use_pynetdicom=True)
    
    time.sleep(10)
    os.system('kill $(lsof -t -i:11112) & kill $(lsof -t -i:11113) & kill $(lsof -t -i:11114)')
    