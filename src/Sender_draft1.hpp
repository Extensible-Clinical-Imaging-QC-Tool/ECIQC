#ifndef SENDER_H_
#define SENDER_H_

#include <iostream>
#include <dcmtk/ofstd/ofstd.h> 
#include <dcmtk/dcmnet/dstorscu.h> 

class Sender {

    public:

    /** Constructor. */
    //Sender();

    OFBool process(const char* opt_peer, const char* opt_peerTitle);


    /** Destructor. */
    //virtual ~Sender();

    //OFBool process(OFString str);
    
    OFCondition result;

    /** Set Peer Host Name.
     *  @param opt_peer
     */ 
    //void PeerHostName(OFString opt_peer);

    /** Set Peer AE Title. 
     *  @param optPeerTitle
     */ 
    //void PeerAETitle(OFString opt_peerTitle);

    /** Set Peer Port  
     *  @param opt_port 
     */ 
    //void PeerPort(Uint16  opt_port);
    


};

#endif // SENDER_H_