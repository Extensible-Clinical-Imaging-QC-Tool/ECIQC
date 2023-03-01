import os
import glob
import json
import pydicom

class PipelineTests():
    def __init__(self,input_file_path):
        self.input_file_path = input_file_path

    def generate_file_list(self,):
        file_list = {}
        for file in glob.glob(self.input_file_path + '*.dcm'):
            ds = pydicom.read_file(file)
            file_list[os.path.basename(file)] = ds[(0x0008,0x0060)].value + '.' + ds[(0x0008,0x0018)].value
        
        with open('file_name_list.json','w') as f:
            json.dump(file_list,f)
            #print(ds.data_element('Referenced SOP Instance UID'))
            #print(ds.data_element(''))
            #print(ds.data_element("PatientsName"))
            #print(ds.UI)

    def run_pipeline(self,json_path,output_path,quarantine_path,scan_directories=True,input_file = None):
        cmd_1 = "./build/exe/qctool --config-file=" + "'" + json_path + "'"
        cmd_2 = "storescp 11113 --output-directory " + output_path
        cmd_3 = "storescp 11114 --output-directory " + quarantine_path
        if scan_directories:
            cmd_4 = "storescu localhost 11112 DICOM_Images --scan-directories"
        else:
            raise ValueError("We are working on scan directories now!")
        cmd = cmd_1 + " & " + cmd_2 + " & " + cmd_3 + " & " + cmd_4
        os.system(cmd)

