#ifndef STORAGEMODE_H
#define STORAGEMODE_H

namespace StorageMode
{
    /** modes specifying whether and how to store the received datasets
     */
    enum E_DatasetStorageMode
    {
        /// receive dataset in memory, perform some conversions and store it to file
        DGM_StoreToFile,
        /// receive dataset directly to file, i.e. write data exactly as received
        DGM_StoreBitPreserving,
        /// receive dataset in memory, but do not store it to file
        DSM_Ignore,
        /// default value
        DSM_Default = DSM_Ignore
    };
} // namespace StorageMode



#endif // STORAGEMODE_H