import os
import glob
import json
import pydicom
import subprocess
import time

class PipelineTests():
    '''
    This is the tests for the whole pipeline.
    We use the pipeline executable file, and DCMTK sender & receiver for testing.
    The basic idea is:
    Given several schemas (configuration files), we pass through a number of Dicom files.
    we need to automatically check whether these files are processed properly. 
    '''
    def __init__(self,
                 input_file_path,
                 output_file_path,
                 quarantine_file_path):
        self.input_file_path = input_file_path
        self.output_file_path = output_file_path
        self.quarantine_file_path = quarantine_file_path

    def generate_file_list(self):
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

    def check_all_files_processed(self) -> bool:
        input_image_counter = len(glob.glob1(self.input_file_path, "*.dcm"))
        output_image_counter = len(glob.glob1(self.output_file_path, "*.dcm"))
        quarantine_image_counter = len(glob.glob1(self.quarantine_file_path, "*.dcm"))
        print(f"Input image number: {input_image_counter}")
        print(f"Output image number: {output_image_counter}")
        print(f"Quarantine image number: {quarantine_image_counter}")
        return input_image_counter == (output_image_counter + quarantine_image_counter)

    def run_pipeline(
            self,
            json_path,
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
        conductor_proc = subprocess.Popen(["./build/exe/qctool", "--config-file", json_path])
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
            
        # Kill the processes once all the images have been processed
        while(True):
            if self.check_all_files_processed():
                conductor_proc.kill()
                receiver_proc.kill()
                quarantine_receiver_proc.kill()
                sender_proc.kill()
                break
            time.sleep(10)