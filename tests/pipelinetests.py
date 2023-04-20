import os
import glob
import json
import pydicom
import pandas as pd
import time
import copy

class PipelineTests():
    '''
    This is the tests for the whole pipeline.
    We use the pipeline executable file, and DCMTK sender & receiver for testing.
    The basic idea is:
    Given several schemas (configuration files), we pass through a number of Dicom files.
    we need to automatically check whether these files are processed properly. 
    '''

    def __init__(self, input_file_path=None):
        self.input_file_path = input_file_path # Set up the input list
        self.ports = [11112,11113,11114] # Set up the ports for the DCMTK SCP & SCU
        self.kill_all_processes() # Kill all the processes before initializing the test.
        self.python = "python" # change this to python3 as necessary
        self.wait_time = 30 # max wait time for the pipeline to finish processing
        self.compare_df = pd.DataFrame(columns=["dcmName", 
                                                "jsonName", 
                                                "Result", 
                                                "Modality", 
                                                "SOPInstanceUID", 
                                                "tagName", 
                                                "realIFtagValue", 
                                                "expIFtagValue", 
                                                "realTHENtagOutput", 
                                                "expTHENtagOutput"])


        if self.input_file_path is not None:
            self.generate_file_list(input_file_path)



    def kill_all_processes(self):
        """
        Kill all the processes on the ports specified in self.ports ### TODO Need to find a way to kill the main qctool process...
        """

        kill_command = 'kill $(lsof -t -i:'
        full_command = []

        for port in self.ports:
            full_command.append(kill_command + str(port) + ')')

        full_command = ' & '.join(full_command)
        os.system(full_command)

        pkill_command = 'pkill -f qctool' ### TODO check this implementation
        os.system(pkill_command)

    def is_complete(self, scan_directories=True):
        '''
        Checks whether the input folder is the same size as the output folder and the quarantine folder.
        '''
        if scan_directories:
            input_file_list = glob.glob(self.input_file_path + '*.dcm')
            output_file_list = glob.glob(self.output_path + '*.dcm')
            quarantine_file_list = glob.glob(self.quarantine_path + '*.dcm')

            if len(input_file_list) == len(output_file_list) + len(quarantine_file_list):
                print("The pipeline is complete. " + str(len(input_file_list)) + " files have been processed.")
                return True
            else:
                print("The pipeline is not complete. " + str(len(output_file_list)+len(quarantine_file_list)) + " files have been processed out of " + str(len(input_file_list)) + " files.")
                return False
        else:
            raise ValueError("scan_directories=False has not been developed yet!")
        

        
    


    def generate_file_list(self, input_file_path=None, file_name_json=None):
        '''
        This method will return a dictionary.
        Keys: input file names.
        Values: output file names (use the SOP UID) ### We are using MODALTY+SOPUID as the output file name.
        '''
        if input_file_path is None:
            try:
                input_file_path = self.input_file_path
            except:
                raise ValueError("No input file path is specified.")
            
        if file_name_json is not None:
            self.file_name_json = file_name_json
            
        if file_name_json is None:
            file_name_json = 'file_name_list.json'

        file_list = {}
        for file in glob.glob(self.input_file_path + '*.dcm'):
            ds = pydicom.read_file(file)
            file_list[os.path.basename(file)] = ds.Modality + "." + ds.SOPInstanceUID
        

        with open(file_name_json,'w') as f:
            json.dump(file_list,f)
            #print(ds.data_element('Referenced SOP Instance UID'))
            #print(ds.data_element(''))
            #print(ds.data_element("PatientsName"))
            #print(ds.UI)

        self.file_list = file_list
        return file_list
    


    def auto_clean_threads(self):
        """
        Attempts to automatically clean up the threads after run_pipeline is finished. 
        To prevent hanging threads we kill all the processes after a certain amount of time.
        """
        wait_time = copy.copy(self.wait_time)
        while not self.is_complete():
            time.sleep(1)
            wait_time -= 1
            print("waiting " + str(wait_time))
            if wait_time == 0:
                self.kill_all_processes()
                raise TimeoutError("The pipeline has not finished after " + str(self.wait_time) + " seconds. Please check the pipeline or increase the wait time.")
                
        self.kill_all_processes()
        print("The pipeline has finished processing all the files.")        




    def run_pipeline(self, json_path, output_path, quarantine_path,use_pynetdicom=True, scan_directories=True, input_file = None):

        '''
        Main method to run the pipeline.
        
        json_path: path to configuration json files.
        output_path: path to save the output images.
        quarantine_path: path to save the quarantine images.
        use_pynetdicom: whether to use the pynetdicom implementation of DCMTK SCP & SCU,
                        giving us more flexibility.
        scan_directories: whether to scan the directories in the input path
        input_file: specify the input file name is scan_directories= False (Not yet developed)
        '''

        self.output_path = output_path
        self.quarantine_path = quarantine_path
        self.json_path = json_path



        cmd_1 = "./build/exe/qctool --config-file=" + "'" + json_path + "'"
        if use_pynetdicom:
            cmd_2 =  self.python + " tests/TestStorageSCP.py {} ".format(self.ports[1]) + output_path
            cmd_3 = self.python + " tests/TestStorageSCP.py {} ".format(self.ports[2]) + quarantine_path
            if scan_directories:
                cmd_4 = self.python + " tests/TestStorageSCU.py {} ".format(self.ports[0]) + self.input_file_path
            else:
                raise ValueError("scan_directories=False has not been developed yet!")
        else:
            cmd_2 = "storescp {} --output-directory ".format(self.ports[1]) + output_path + " --accept-all"
            cmd_3 = "storescp {} --output-directory ".format(self.ports[2]) + quarantine_path + " --accept-all"
            if scan_directories:
                ### TODO change "DICOM_Images" to the correct file path
                cmd_4 = "storescu localhost {} {DICOM_Images} --scan-directories --propose-jpeg8".format(self.ports[0], DICOM_Images="DICOM_Images")
            else:
                raise ValueError("scan_directories=False has not been developed yet!")
        cmd = cmd_1 + " & " + cmd_2 + " & " + cmd_3 + " & " + cmd_4
        os.system(cmd)

        self.auto_clean_threads()


    ### TODO change this to operate on the right json file instead of over the whole thing.
    def check_output(self, expected_output_json, test_result_json=None, scan_directories=True):
        """
        reads the expected output json file, and compares the output with the expected output. Also records the inputs 
        Iterates through the json line by line - inefficient but generates a more easily comparable report.
        """
        # read expected output json file
        expected_output = pd.read_json(expected_output_json)

        jsonName = self.json_path.split("/")[-1]

        expected_output = expected_output[expected_output["jsonName"] == jsonName]

        # iterate over json file
        for index, row in expected_output.iterrows():
            print(index)
            # read input dcm
            input_path = os.path.join(self.input_file_path, row["dcmName"])

            ds = pydicom.dcmread(input_path)

            dcmName = row["dcmName"]
            jsonName = row["jsonName"]
            Modality = ds.Modality
            SOPInstanceUID = ds.SOPInstanceUID
            tagName = row["tagName"]
            expIFtagValue = row["IFtagValue"]
            realIFtagValue = ds.data_element(tagName).value
            expTHENtagOutput = row["THENtagOutput"]

            output_name = Modality + "." + SOPInstanceUID + ".dcm"

            ### check both output and quarantine folders
            try:
                output_path = os.path.join(self.output_path, output_name)

                ds = pydicom.dcmread(output_path)

                realTHENtagOutput = ds.data_element(tagName).value
            except:
                print("File not found in output folder. Checking quarantine folder...")

            try:
                output_path = os.path.join(self.quarantine_path, output_name)

                ds = pydicom.dcmread(output_path)

                realTHENtagOutput = ds.data_element(tagName).value
            except:
                print("File not found in quarantine folder or output folder")
                realTHENtagOutput = "File not found in quarantine folder or output folder"

            ### Compare the expected output with the real output

            if realTHENtagOutput == expTHENtagOutput:
                print(dcmName,jsonName,"Test passed")
                Result = True
            else:
                print(dcmName,jsonName,"Test failed")
                Result = False

            #create row to append
            test_row = pd.DataFrame({"dcmName":dcmName,
                                     "jsonName":jsonName,
                                     "Modality":Modality,
                                     "SOPinstanceUID":SOPInstanceUID,
                                     "tagName":tagName,
                                     "expIFtagValue":expIFtagValue,
                                     "realIFtagValue":realIFtagValue,
                                     "expTHENtagOutput":expTHENtagOutput,
                                     "realTHENtagOutput":realTHENtagOutput,
                                     "Result":Result},
                                     index=[0])

            #append row to dataframe
            self.compare_df = pd.concat([self.compare_df,test_row],ignore_index=True)

        #save dataframe to json with jsonname in filename
        if test_result_json is None:
            result_json_name = "test_results_" + jsonName
            test_result_json = os.path.join(self.output_path, result_json_name)

        print("Saving test results to: " + test_result_json)
        self.compare_df.to_json(test_result_json)

        ## print number of tests passed and failed

        print("Test results:")
        print("Passed: " + str(len(self.compare_df[self.compare_df["Result"]==True])))
        print("Failed: " + str(len(self.compare_df[self.compare_df["Result"]==False])))

        return self.compare_df













#### MOVED HERE FOR CLARITY IN MAIN CODE - TODO remove once we are done

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
