#ifndef STORAGEMODE_H
#define STORAGEMODE_H


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

    /** modes for generating subdirectories
     */
    enum E_DirectoryGenerationMode
    {
        /// do not generate any subdirectories
        DGM_NoSubdirectory,
        /// generated subdirectories based on the Series Date (0008,0021)
        DGM_SeriesDate,
        /// default value
        DGM_Default = DGM_NoSubdirectory
    };

    /** modes for generating filenames
     */
    enum E_FilenameGenerationMode
    {
        /// generate filename from SOP Instance UID (0008,0018)
        FGM_SOPInstanceUID,
        /// generate unique filename based on new UID
        FGM_UniqueFromNewUID,
        /// generate short pseudo-random unique filename
        FGM_ShortUniquePseudoRandom,
        /// generate filename from current system time
        FGM_CurrentSystemTime,
        /// default value
        FGM_Default = FGM_SOPInstanceUID
    };




#endif // STORAGEMODE_H