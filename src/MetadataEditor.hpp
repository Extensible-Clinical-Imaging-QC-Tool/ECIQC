#ifndef METADATAEDITOR_H_
#define METADATAEDITOR_H_
#include<iostream>

class MetadataEditor {

OFString fpath;

public:
    int x;
    
    MetadataEditor(OFString file_path, int xx = 23);

    void print();

    void readFile();

};

#endif // METADATAEDITOR_H_