import os
import time


from tests.pipelinetests import PipelineTests
from pynetdicom import AE, debug_logger
from pydicom.uid import ExplicitVRLittleEndian
from pynetdicom.sop_class import CTImageStorage


if __name__ == '__main__':
    ### Kill all the processes on port 11112 11113 11114
    #os.system('kill $(lsof -t -i:11112) & kill $(lsof -t -i:11113) & kill $(lsof -t -i:11114)')
    
    
    pipelinetest = PipelineTests('./DICOM_Images_temp/')
    pipelinetest.generate_file_list()
    pipelinetest.run_pipeline("./schema/MultipleConditionCase.json",
                              "./result/",
                              "./result_quarantine/",
                              use_pynetdicom=True,
                              scan_directories=True,
                              #input_file=['testtext.dcm']
                              )
    '''
    import matplotlib.pyplot as plt
    import pydicom
    post_file = pydicom.read_file('result/1.2.840.113663.1500.1.402372958.3.1.20180816.111159.971')
    print(post_file)
    '''
    #time.sleep(10)
    #os.system('kill $(lsof -t -i:11112) & kill $(lsof -t -i:11113) & kill $(lsof -t -i:11114)')
    #######################################################################################
    # Note: the following codes are all obsolete now, so just ignore them.
    # But we may need some codes in the future development.
    # So just leave them as commented.
    # Written by Yiming Wei 2023-03-03
    '''
    import pydicom
    file_list = ['1-01.dcm','1-1copy.dcm','1-003.dcm','1-004.dcm','1-005.dcm','1-006.dcm','test.dcm','test2.dcm','testtext.dcm']
    for file in file_list:
        ds = pydicom.dcmread('DICOM_Images/'+file)
        print(ds.file_meta)
        #ds= pydicom.read_file('DICOM_Images/' + file)
        #print(ds)
    '''

    'python3 tests/TestStorageSCP.py 11112 result/ & python3 tests/TestStorageSCU.py 11112 DICOM_Images/'
    
    ''' 
    cmd_1 = "./build/exe/qctool --config-file='./schema/PipelineCase.json'"
    cmd_2 = "storescp 11113 --output-directory ./result/ --accept-all"
    cmd_3 = "storescp 11114 --output-directory ./result_quarantine/ --accept-all"
    cmd_4 = "storescu localhost 11112 DICOM_Images --scan-directories --propose-jpeg8"
    cmd = cmd_1 + " & " + cmd_2 + " & " + cmd_3 + " & " + cmd_4
    os.system(cmd)
    '''
    '''
    debug_logger()

    ae = AE()
    ae.add_supported_context(CTImageStorage, ExplicitVRLittleEndian)
    ae.start_server(("127.0.0.1", 11112), block=True)
    '''
    '''
    debug_logger()
    ae = AE()
    ae.add_requested_context('1.2.840.10008.1.1')
    ae.add_requested_context("1.2.840.10008.5.1.4.1.1.2")
    ae.add_requested_context("1.2.840.10008.5.1.4.1.1.501.2.1")
    ae.add_requested_context("1.2.840.10008.5.1.4.1.1.7")
    ae.add_requested_context("1.2.840.10008.5.1.4.1.1.3.1")
    assoc = ae.associate("127.0.0.1", 11112)
    if assoc.is_established:
        status = assoc.send_c_echo()
        print(status)
        assoc.release()
    else:
        # Association rejected, aborted or never connected
        print('Failed to associate')
    '''
    '''
    ### Now we test the file
    import pydicom
    original_file = pydicom.read_file('DICOM_Images/1-1copy.dcm')
    result_file = pydicom.read_file('result/DX.1.3.6.1.4.1.14519.5.2.1.9999.103.2710019309218103448268301702692')
    print('Original name: ', original_file.PatientName)
    print('Postprocessed name: ',result_file.PatientName)
    '''
    
    '''
    Note: when we rerun the whole pipeline test,
    we need to kill the existing processes first.
    One way is to type: "ps aux" to find PIDs,
    and then kill <PID> 
    '''
    
    '''
    cmds_list = [
        ["./build/exe/qctool", "--config-file", "./schema/useCase.json"],
        ["storescp", "11113", "--output-directory", "./result/"],
        ["storescp", "11114", "--output-directory", "./result_quarantine/"],
        ["storescu", "localhost", "11112", "DICOM_Images/1-01.dcm"]
        ]
    procs_list = [Popen(cmd, stdout=PIPE, stderr=PIPE) for cmd in cmds_list]
    for proc in procs_list:
	    proc.wait()
    ''' 
    '''
    subprocess.run(["./build/exe/qctool", "--config-file", "./schema/useCase.json"])#,capture_output=True)
    subprocess.run(["storescp", "11113", "--output-directory", "./result/"])#,capture_output=True)
    subprocess.run(["storescp", "11114", "--output-directory", "./result_quarantine/"])#,capture_output=True)
    subprocess.run("storescu localhost 11112 DICOM_Images/1-01.dcm")
    '''
    '''
    os.system("./build/exe/qctool --config-file='./schema/useCase.json'")
    os.system("storescp 11114 --output-directory ./result/")
    os.system("storescu localhost 11112 DICOM_Images/1-01.dcm")
    os.system("storescu localhost 11112 DICOM_Images --scan-directories")
    '''
