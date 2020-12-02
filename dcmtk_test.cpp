
#include <iostream>
#include <iomanip>
#include <sstream>
#include <fstream>
#include "dcmtk/dcmdata/include/dcmtk/dcmdata/dctk.h"          /* for various dcmdata headers */
#include "dcmtk/dcmdata/include/dcmtk/dcmdata/cmdlnarg.h"      /* for prepareCmdLineArgs */
#include "dcmtk/dcmdata/include/dcmtk/dcmdata/dcuid.h"         /* for dcmtk version name */

#include "dcmtk/ofstd/include/dcmtk/ofstd/ofconapp.h"        /* for OFConsoleApplication */
#include "dcmtk/ofstd/include/dcmtk/ofstd/ofcmdln.h"         /* for OFCommandLine */

#include "dcmtk/oflog/include/dcmtk/oflog/oflog.h"           /* for OFLogger */
#include "dcmtk/dcmpstat/include/dcmtk/dcmpstat/dcmpstat.h"

#include "metadataEditor.hpp"
#include "metadataEditor.cpp"


using namespace std;

int main(){
    //std:: cout << "Hello" << '\n';
    int hh = 3;
    metadataEditor obj{hh};
    obj.print();

    DcmFileFormat image;
    OFString patientName;
    OFCondition status = image.loadFile("1-1copy.dcm");

    //DcmFileFormat fileformat;
    //OFCondition status = fileformat.loadFile("1-1copy.dcm");
    //if (status.good())   {
	//	cout << "File loaded" << endl;
    //}else{
    //cerr << "Error: cannot read DICOM file (" << status.text() << ")" << endl;
    //}
    //return 0;
}