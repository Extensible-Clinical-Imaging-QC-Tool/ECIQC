from pydicom.uid import *
from pynetdicom.presentation import AllStoragePresentationContexts
from pynetdicom.sop_class import *
import pynetdicom

def get_abstract_syntax(support_all_syntaxes):
    if support_all_syntaxes:
        return AllStoragePresentationContexts
    else:
        return [
            CTImageStorage,
            MRImageStorage,
            DigitalXRayImageStorageForPresentation,
            SecondaryCaptureImageStorage,
            UltrasoundMultiFrameImageStorage,
            "1.2.840.10008.1.1"

    ]

def get_transfer_syntax(support_all_syntaxes):
    if support_all_syntaxes:
        return pynetdicom._globals.ALL_TRANSFER_SYNTAXES
    else:
        return [
                ImplicitVRLittleEndian,
                ExplicitVRLittleEndian,
                JPEGBaseline8Bit,
                ]

def add_context(ae,service_type='SCP',support_all_syntaxes=False):
    
    transfer_syntax = get_transfer_syntax(support_all_syntaxes)
    if service_type == 'SCP':
            for abstract_syntax in get_abstract_syntax(support_all_syntaxes):
                if abstract_syntax == UltrasoundMultiFrameImageStorage:
                     ae.add_supported_context(abstract_syntax,JPEGBaseline8Bit)
                else:
                    ae.add_supported_context(abstract_syntax,ImplicitVRLittleEndian)
    elif service_type == 'SCU':
            for abstract_syntax in get_abstract_syntax(support_all_syntaxes):
                if abstract_syntax == UltrasoundMultiFrameImageStorage:
                     ae.add_requested_context(abstract_syntax,JPEGBaseline8Bit)
                else:
                    ae.add_requested_context(abstract_syntax,ImplicitVRLittleEndian)
    else:
        raise ValueError('Invalid service type!')
    
    #ae.supported_contexts = AllStoragePresentationContexts