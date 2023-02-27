import os


from tests.pipelinetests import PipelineTests

if __name__ == '__main__':
    pipelinetest = PipelineTests('./DICOM_Images/')
    pipelinetest.generate_file_list()
    pipelinetest.run_pipeline("./schema/useCase.json","./result/","./result_quarantine/")
    '''
    cmd_1 = "./build/exe/qctool --config-file='./schema/useCase.json'"
    cmd_2 = "storescp 11113 --output-directory ./result/"
    cmd_3 = "storescp 11114 --output-directory ./result_quarantine/"
    cmd_4 = "storescu localhost 11112 DICOM_Images --scan-directories"
    cmd = cmd_1 + " & " + cmd_2 + " & " + cmd_3 + " & " + cmd_4
    os.system(cmd)
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