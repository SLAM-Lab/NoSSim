
#include <iostream>
#include <sstream>
#include "OmnetIf_pkt.h"



OmnetIf_pkt::OmnetIf_pkt()
{
    fileBuffer_arraysize = 0;
    this->fileBuffer = NULL;
}


OmnetIf_pkt::~OmnetIf_pkt()
{
    delete [] fileBuffer;
}

void OmnetIf_pkt::setFileBufferArraySize(unsigned int size)
{
    char *fileBuffer_var2 = (size==0) ? NULL : new char[size];
    unsigned int sz = fileBuffer_arraysize < size ? fileBuffer_arraysize : size;
    for (unsigned int i=0; i<sz; i++)
        fileBuffer_var2[i] = this->fileBuffer[i];
    for (unsigned int i=sz; i<size; i++)
        fileBuffer_var2[i] = 0;
    fileBuffer_arraysize = size;
    delete [] this->fileBuffer;
    this->fileBuffer = fileBuffer_var2;
}

unsigned int OmnetIf_pkt::getFileBufferArraySize() const
{
    return fileBuffer_arraysize;
}

char OmnetIf_pkt::getFileBuffer(unsigned int k) const
{
    return fileBuffer[k];
}

void OmnetIf_pkt::setFileBuffer(unsigned int k, char fileBuffer)
{
    this->fileBuffer[k] = fileBuffer;
}


