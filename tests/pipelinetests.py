import os
import glob
import json
import pydicom
import subprocess
import time
from pathlib import Path

class PipelineTests():
    '''
    This is the tests for the whole pipeline.
    We use the pipeline executable file, and DCMTK sender & receiver for testing.
    The basic idea is:
    Given several schemas (configuration files), we pass through a number of Dicom files.
    we need to automatically check whether these files are processed properly. 
    '''
    def __init__(self,
                 input_file_path: str,
                 output_file_path: str,
                 quarantine_file_path: str):
        self.input_file_path = Path(input_file_path).resolve()
        self.output_file_path = Path(output_file_path).resolve()
        self.quarantine_file_path = Path(quarantine_file_path).resolve()

        if not self.input_file_path.exists():
            raise ValueError(f"The input path provided: {input_file_path} does not exist!")
        
        if not self.output_file_path.exists():
            raise ValueError(f"The output path provided: {output_file_path} does not exist!")
        
        if not self.quarantine_file_path.exists():
            raise ValueError(f"The quantine path provided: {quarantine_file_path} does not exist!")

    def generate_file_list(self, json_to_save_name: str = "file_name_list") -> None:
        '''
        This method will save a dictionary in the provided {json_to_save_name}.json file.
        Keys: input file names
        Values: output file names (use the SOP UID)
        '''
        file_list = {}
        for file in glob.glob1(self.input_file_path, '*.dcm'):
            file = Path.joinpath(self.input_file_path, file)
            ds = pydicom.read_file(file)
            file_list[os.path.basename(file)] = ds[(0x0008,0x0018)].value
        
        with open(f'{json_to_save_name}.json','w') as f:
            json.dump(file_list,f)

    def check_all_files_processed(self) -> bool:
        input_image_counter = len(glob.glob1(self.input_file_path, "*.dcm"))
        output_image_counter = len(glob.glob1(self.output_file_path, "*.dcm"))
        quarantine_image_counter = len(glob.glob1(self.quarantine_file_path, "*.dcm"))

        # TODO add these prints to the logger
        # print(f"Input image number: {input_image_counter}")
        # print(f"Output image number: {output_image_counter}")
        # print(f"Quarantine image number: {quarantine_image_counter}")
        
        return input_image_counter == (output_image_counter + quarantine_image_counter)

    def run_pipeline(
            self,
            json_path: str,
            use_pynetdicom: bool = True,
            scan_directories: bool = True,
            ) -> None:
        '''
        Main method to run the pipeline.
        
        json_path: path to configuration json files.
        use_pynetdicom: whether to use the pynetdicom implementation of DCMTK SCP & SCU,
                        giving us more flexibility.
        scan_directories: whether to scan the directories in the input path
        '''
        if not Path(json_path).exists():
            raise ValueError(f"The JSON path provided: {json_path} does not exist!")
        
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
