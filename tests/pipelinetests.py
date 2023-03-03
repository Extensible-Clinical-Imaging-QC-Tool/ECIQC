import os
import glob
import json
import pydicom

class PipelineTests():
    '''
    This is the tests for the whole pipeline.
    We use the pipeline executable file, and DCMTK sender & receiver for testing.
    The basic idea is:
    Given several schemas (configuration files), we pass through a number of Dicom files.
    we need to automatically check whether these files are processed properly. 
    '''
    def __init__(self,input_file_path):
        self.input_file_path = input_file_path # Set up the input list

    def generate_file_list(self,):
        '''
        This method will return a dictionary.
        Keys: input file names.
        Values: output file names (use the SOP UID)
        '''
        file_list = {}
        for file in glob.glob(self.input_file_path + '*.dcm'):
            ds = pydicom.read_file(file)
            file_list[os.path.basename(file)] = ds[(0x0008,0x0018)].value
        
        with open('file_name_list.json','w') as f:
            json.dump(file_list,f)
            #print(ds.data_element('Referenced SOP Instance UID'))
            #print(ds.data_element(''))
            #print(ds.data_element("PatientsName"))
            #print(ds.UI)

    def run_pipeline(self,json_path,output_path,quarantine_path,use_pynetdicom=True,scan_directories=True,input_file = None):
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
        cmd_1 = "./build/exe/qctool --config-file=" + "'" + json_path + "'"
        if use_pynetdicom:
            cmd_2 = "python3 tests/TestStorageSCP.py 11113 " + output_path
            cmd_3 = "python3 tests/TestStorageSCP.py 11114 " + quarantine_path
            if scan_directories:
                cmd_4 = "python3 tests/TestStorageSCU.py 11112 " + self.input_file_path
            else:
                raise ValueError("scan_directories=False has not been developed yet!")
        else:
            cmd_2 = "storescp 11113 --output-directory " + output_path + " --accept-all"
            cmd_3 = "storescp 11114 --output-directory " + quarantine_path + " --accept-all"
            if scan_directories:
                cmd_4 = "storescu localhost 11112 DICOM_Images --scan-directories --propose-jpeg8"
            else:
                raise ValueError("scan_directories=False has not been developed yet!")
        cmd = cmd_1 + " & " + cmd_2 + " & " + cmd_3 + " & " + cmd_4
        os.system(cmd)

