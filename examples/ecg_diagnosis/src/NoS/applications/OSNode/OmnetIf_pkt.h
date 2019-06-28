#ifndef _OMNETIF_PKT_H_ // _OMNETIF_PKT_H_
#define _OMNETIF_PKT_H_ //_OMNETIF_PKT_H_
#include <iostream>
#include <sstream>

class OmnetIf_pkt
{



  public:
    char *fileBuffer; // array ptr
    unsigned int fileBuffer_arraysize;
    int DestNode;

    OmnetIf_pkt();
    ~OmnetIf_pkt();
    void setFileBufferArraySize(unsigned int size);
    unsigned int getFileBufferArraySize() const;
    char getFileBuffer(unsigned int k) const;
    void setFileBuffer(unsigned int k, char fileBuffer);
};







#endif // _OMNETIF_PKT_H_
