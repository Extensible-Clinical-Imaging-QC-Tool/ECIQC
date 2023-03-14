import sys
import glob
from pydicom import dcmread
from pynetdicom import AE,debug_logger
from TestStorageContext import add_context


if __name__ == '__main__':
    ae = AE()
    #ae.add_requested_context()
    #debug_logger()
    port_number = int(sys.argv[1])
    path_to_dataset = sys.argv[2]
    #ae.add_supported_context()
    add_context(ae,'SCU')
    assoc = ae.associate("localhost", port_number)
    if assoc.is_established:
        status = assoc.send_c_echo()
        for file_name in glob.glob(path_to_dataset+ '/*.dcm'):
            ds = dcmread(file_name)
            status = assoc.send_c_store(ds)
        assoc.release()
    else:
        raise ValueError('Association rejected, aborted or never connected')