import os
import glob
import json
import pydicom
import subprocess
import time
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

    def __init__(self, 
                 json_path,
                 input_file_path,
                 output_file_path,
                 quarantine_file_path=None):
        self.json_path = json_path
        self.input_file_path = input_file_path
        self.output_file_path = output_file_path
        self.quarantine_file_path = quarantine_file_path
        self.ports = [11112,11113,11114] # Set up the ports for the DCMTK SCP & SCU
        # self.kill_all_processes() # Kill all the processes before initializing the test.
        self.python = "python" # change this to python3 as necessary
        self.wait_time = 120 # max wait time for the pipeline to finish processing
        self.file_list = {} # This is a dictionary that stores the input and output file names.
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

        if self.check_all_files_processed():
            raise ValueError("Output folder is not empty. Please empty the output folder before running the tests.")



    # def kill_all_processes(self):
    #     """
    #     Kill all the processes on the ports specified in self.ports ### TODO Need to find a way to kill the main qctool process...
    #     """

    #     kill_command = 'kill $(lsof -t -i:'
    #     full_command = []

    #     for port in self.ports:
    #         full_command.append(kill_command + str(port) + ')')

    #     full_command = ' & '.join(full_command)
    #     os.system(full_command)

    #     pkill_command = 'pkill -f qctool' ### TODO check this implementation
    #     os.system(pkill_command)


    # def auto_clean_threads(self):
    #     """
    #     Attempts to automatically clean up the threads after run_pipeline is finished. 
    #     To prevent hanging threads we kill all the processes after a certain amount of time.
    #     """
    #     wait_time = copy.copy(self.wait_time)
    #     while not self.is_complete():
    #         interval = 10
    #         time.sleep(interval)
    #         wait_time -= interval
    #         print("waiting " + str(wait_time))
    #         if wait_time == 0:
    #             self.kill_all_processes()
    #             raise TimeoutError("The pipeline has not finished after " + str(self.wait_time) + " seconds. Please check the pipeline or increase the wait time.")
                
    #     self.kill_all_processes()
    #     print("The pipeline has finished processing all the files.")        



# ################## TODO check which one we need 
#     def is_complete(self, scan_directories=True):
#         '''
#         Checks whether the input folder is the same size as the output folder and the quarantine folder.
#         '''
#         if scan_directories:
#             input_file_list = glob.glob(self.input_file_path + '*.dcm')
#             output_file_list = glob.glob(self.output_file_path + '*.dcm')
#             quarantine_file_list = glob.glob(self.quarantine_file_path + '*.dcm')

#             if len(input_file_list) == len(output_file_list) + len(quarantine_file_list):
#                 print("The pipeline is complete. " + str(len(input_file_list)) + " files have been processed.")
#                 return True
#             else:
#                 print("The pipeline is not complete. " + str(len(output_file_list)+len(quarantine_file_list)) + " files have been processed out of " + str(len(input_file_list)) + " files.")
#                 return False
#         else:
#             raise ValueError("scan_directories=False has not been developed yet!")
        
############ probs implement this one in the function above
    def check_all_files_processed(self) -> bool:
        input_image_counter = len(glob.glob1(self.input_file_path, "*.dcm"))
        output_image_counter = len(glob.glob1(self.output_file_path, "*.dcm"))
        quarantine_image_counter = len(glob.glob1(self.quarantine_file_path, "*.dcm"))
        print(f"Input image number: {input_image_counter}")
        print(f"Output image number: {output_image_counter}")
        print(f"Quarantine image number: {quarantine_image_counter}")
        return input_image_counter == (output_image_counter + quarantine_image_counter)
#################
        


    def generate_file_list(self, input_file_path=None, file_name_json=None):
        '''
        This method will return a dictionary. # this doesn't return anything - dumps to json
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





    def run_pipeline(
            self,
            use_pynetdicom=True,
            scan_directories=True,
            ):
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
        # self.kill_all_processes()

        conductor_proc = subprocess.Popen(["./build/exe/qctool", "--config-file", self.json_path])
        if use_pynetdicom:
            time.sleep(1)
            receiver_proc = subprocess.Popen(["python", "tests/TestStorageSCP.py", "11113", self.output_file_path])
            quarantine_receiver_proc = subprocess.Popen(["python", "tests/TestStorageSCP.py", "11114", self.quarantine_file_path])
            if scan_directories:
                time.sleep(1)
                sender_proc = subprocess.Popen(["python", "tests/TestStorageSCU.py", "11112", self.input_file_path])
            else:
                raise ValueError("scan_directories=False has not been developed yet!")
        else:
            time.sleep(1)
            receiver_proc = subprocess.Popen(["storescp", "11113", "--output-directory", self.output_file_path, "--accept-all"])
            quarantine_receiver_proc = subprocess.Popen(["storescp", "11114", "--output-directory", self.quarantine_file_path, "--accept-all"])
            if scan_directories:
                time.sleep(1)
                sender_proc = subprocess.Popen(["storescu", "localhost", "11112", "DICOM_Images", "--scan-directories", "--propose-jpeg8"])
            else:
                raise ValueError("scan_directories=False has not been developed yet!")
            
        # Kill the processes once all the images have been processed ### TODO make this work with max wait time before closing all processes
        while(True):
            if self.check_all_files_processed():
                conductor_proc.kill()
                receiver_proc.kill()
                quarantine_receiver_proc.kill()
                sender_proc.kill()
                break
            time.sleep(10)       
            
    def create_answer_card(self, expected_output_json, jsonName):
        #read in json
        expected_output = pd.read_json(expected_output_json)
        #select the correct test
        expected_output = expected_output[expected_output["jsonName"] == jsonName]

        ### compare expected files with real files
        #create a list of the expected output file names
        expected_file_list = set(expected_output["dcmName"].tolist())
        real_file_list = set(self.file_list.keys())

        # check if real file list is a superset of expected file list
        if real_file_list.issuperset(expected_file_list):
            raise ValueError("The real file list contains files not included the expected file list.")
        # check if expected file list is a superset of real file list
        if expected_file_list.issuperset(real_file_list):
            print("The expected file list contains all the files in the real file list.")
            # strip the expected output to only include the files in the real file list
            expected_output = expected_output[expected_output["dcmName"].isin(real_file_list)]
            return expected_output
        

    def try_load_dcm(self, input_path, test_row):

        try:
            ds = pydicom.dcmread(input_path)
        except:
            print("input_path: ", input_path)
            print("dcmName: ", test_row["dcmName"])
            print("jsonName: ", test_row["jsonName"])
            print("File could not be found/loaded. Skipping...")
            # time.sleep(1)
            # continue
            ds = None
            test_row["Comment"].append("File could not be found/loaded.")

        return ds
    

    def check_output_folders(self, test_row, ds):
            
        ### check both output and quarantine folders
        
        try:
            output_path = os.path.join(self.output_file_path, test_row["output_name"])
            print("Excpeted Output path: ", output_path)
            ds = pydicom.dcmread(output_path, force=True)
            print("Dicom read")
            print("tagName: ", test_row["tagName"])
            realTHENtagOutput = ds.data_element(test_row["tagName"]).value
            print("expTHENtagOutput: ", test_row["expTHENtagOutput"])
            print("realTHENtagOutput: ", realTHENtagOutput)
            test_row["realTHENtagOutput"] = realTHENtagOutput
        except UserWarning:
            print("File not found in output folder. Checking quarantine folder...")
            try:
                output_path = os.path.join(self.quarantine_file_path, test_row["output_name"])

                ds = pydicom.dcmread(output_path, force=True)
                print("Dicom read")
                print("tagName: ", test_row["tagName"])

                realTHENtagOutput = ds.data_element(test_row["tagName"]).value
                print("expTHENtagOutput: ", test_row["expTHENtagOutput"])
                print("realTHENtagOutput: ", realTHENtagOutput)
 
                test_row["realTHENtagOutput"] = realTHENtagOutput
                test_row["Comment"].append("File found in quarantine.")
            except FileNotFoundError:
                print("File {} not found in either output or quarantine folder...".format(test_row["dcmName"]))
                realTHENtagOutput = None
                test_row["realTHENtagOutput"] = realTHENtagOutput
                test_row["Comment"].append("File not found in either output folders.")
        return test_row


    def evaluate_result(self, test_row, ds):
        # Compare the expected output with the real output
        ### NEED TO CHECK THAT THIS LOGIC WORKS CORRECTLY
        if ds is None:
            print(test_row["dcmName"],test_row["jsonName"], "no DS")
            Result = False
            test_row["Comment"].append("no input DS")
            return Result

        elif test_row["realTHENtagOutput"] is None:
            test_row["realTHENtagOutput"] = "No output file found."
            print(test_row["dcmName"], test_row["jsonName"], test_row["realTHENtagOutput"])
            Result = False
            test_row["Comment"].append(test_row["realTHENtagOutput"])
            return Result


        elif test_row["realIFtagValue"] != test_row["expIFtagValue"]:
            print(test_row["dcmName"],test_row["jsonName"],"Test not covered")
            Result = True
            test_row["Comment"].append("Test not covered")
            return Result


        elif test_row["realIFtagValue"] == test_row["expIFtagValue"]:
            if test_row["realTHENtagOutput"] == test_row["expTHENtagOutput"]:
                print(test_row['dcmName'],test_row["jsonName"],"Test passed")
                Result = True
                test_row["Comment"].append("Test passed")

            else:
                print(test_row["dcmName"],test_row["jsonName"],"Test failed")
                Result = False
                test_row["Comment"].append("Test failed")

            return Result



    ### TODO change this to operate on the right json file instead of over the whole thing.
    def check_output(self, expected_output_json, test_result_json=None, scan_directories=True):
        """
        reads the expected output json file, and compares the output with the expected output. Also records the inputs 
        Iterates through the json line by line - inefficient but generates a more easily comparable report.
        """
        # read expected output json file
        print("Checking output...")


        ###

        jsonName = self.json_path.split("/")[-1]
        print(jsonName)

        expected_output_to_check = self.create_answer_card(expected_output_json, jsonName)


        # iterate over json file
        for index, row in expected_output_to_check.iterrows():
            print(index)



            dcmName = row["dcmName"]
            jsonName = row["jsonName"]
            tagName = row["tagName"]
            print("tagName: ", tagName)
            expIFtagValue = row["IFtagValue"]
            expTHENtagOutput = row["THENtagOutput"]

            #create row to append
            test_row = pd.DataFrame(columns=["dcmName",
                                             "jsonName",
                                             "Modality",
                                             "SOPinstanceUID",
                                             "tagName",
                                             "expIFtagValue",
                                             "realIFtagValue",
                                             "expTHENtagOutput",
                                             "realTHENtagOutput",
                                             "Result",
                                             "output_name",
                                             "Comment"])

            test_row["dcmName"] = dcmName
            test_row["jsonName"] = jsonName
            test_row["tagName"] = tagName
            test_row["expIFtagValue"] = expIFtagValue
            test_row["expTHENtagOutput"] = expTHENtagOutput
            test_row["Comment"] = []

            input_path = os.path.join(self.input_file_path, dcmName)

            ds = self.try_load_dcm(input_path)


            if ds is not None:
                Modality = ds.Modality
                SOPInstanceUID = ds.SOPInstanceUID
                realIFtagValue = ds.data_element(tagName).value
                output_name = SOPInstanceUID + ".dcm"
                print("Expected Output name: ", output_name)

                test_row["Modality"] = Modality
                test_row["SOPinstanceUID"] = SOPInstanceUID
                test_row["realIFtagValue"] = realIFtagValue
                test_row["output_name"] = output_name

            test_row = self.check_output_folders(test_row, ds)

            test_row["Result"] = self.evaluate_result(test_row, ds)

            #append row to dataframe
            self.compare_df = pd.concat([self.compare_df,test_row],ignore_index=True)

        #save dataframe to json with jsonname in filename
        if test_result_json is None:
            result_json_name = "test_results_" + jsonName
            test_result_json = os.path.join(self.output_file_path, result_json_name)

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
