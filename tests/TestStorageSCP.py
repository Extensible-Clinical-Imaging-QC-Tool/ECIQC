import sys

from pynetdicom import AE, debug_logger,evt
from pydicom import FileDataset
from TestStorageContext import add_context
from pathlib import Path

# Implement a handler for evt.EVT_C_STORE
class HandleStore():
    def __init__(self,save_path):
        self.save_path = Path(save_path).resolve() 
        if not self.save_path.exists():
            raise ValueError(f"The save path provided: {save_path} does not exist!")
        
    def handle_store(self,event):
        """Handle a C-STORE request event."""
        # Decode the C-STORE request's *Data Set* parameter to a pydicom Dataset
        ds = event.dataset

        
        # Add the File Meta Information
        ds.file_meta = event.file_meta

       

        # Save the dataset using the SOP Instance UID as the filename
        save_ds_path = Path.joinpath(self.save_path, f"{ds.SOPInstanceUID}.dcm")

        # Convert pydicom.dataset.Dataset to pydicom.dataset.FileDataset
        file_dataset = FileDataset(save_ds_path, {}, file_meta=ds.file_meta, preamble=b"\x00" * 128)
        file_dataset.save_as(save_ds_path)

        #ds.save_as(save_ds_path,write_like_original=True)

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