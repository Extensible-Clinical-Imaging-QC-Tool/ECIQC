import sys

from pynetdicom import AE, debug_logger,evt
from TestStorageContext import add_context

# Implement a handler for evt.EVT_C_STORE
class HandleStore():
    def __init__(self,save_path):
        self.save_path = save_path 
    def handle_store(self,event):
        """Handle a C-STORE request event."""
        # Decode the C-STORE request's *Data Set* parameter to a pydicom Dataset
        ds = event.dataset

        # Add the File Meta Information
        ds.file_meta = event.file_meta

        # Save the dataset using the SOP Instance UID as the filename
        ds.save_as(f"{save_path}{ds.SOPInstanceUID}.dcm")

        # Return a 'Success' status
        return 0x0000


if __name__ == '__main__':
   debug_logger()
   ae = AE()
   add_context(ae,'SCP')
   port_number = int(sys.argv[1])
   save_path = sys.argv[2]
   handlers = [(evt.EVT_C_STORE, HandleStore(save_path).handle_store)]
   ae.start_server(("localhost", port_number), evt_handlers=handlers)