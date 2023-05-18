import sys
import glob
from pydicom import dcmread
from pynetdicom import AE,debug_logger
from TestStorageContext import add_context
from pathlib import Path


if __name__ == '__main__':
    ae = AE()
    port_number = int(sys.argv[1])

    path_to_dataset = sys.argv[2]
    path_to_dataset = Path(path_to_dataset).resolve()
    if not path_to_dataset.exists():
            raise ValueError(f"The path provided: {path_to_dataset} does not exist!")
    
    add_context(ae,'SCU')
    assoc = ae.associate("localhost", port_number)

    if assoc.is_established:
        status = assoc.send_c_echo()
        for file_name in glob.glob1(path_to_dataset,'*.dcm'):
            file_path = Path.joinpath(path_to_dataset, file_name)
            ds = dcmread(file_path)
            status = assoc.send_c_store(ds)
        assoc.release()
    else:
        raise ValueError('Association rejected, aborted or never connected')