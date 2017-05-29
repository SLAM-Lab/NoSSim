//
// Generated file, do not edit! Created by opp_msgc 4.3 from ethernet/EtherMser.msg.
//

// Disable warnings about unused variables, empty switch stmts, etc:
#ifdef _MSC_VER
#  pragma warning(disable:4101)
#  pragma warning(disable:4065)
#endif

#include <iostream>
#include <sstream>
#include "EtherMser_m.h"

// Template rule which fires if a struct or class doesn't have operator<<
template<typename T>
std::ostream& operator<<(std::ostream& out,const T&) {return out;}

// Another default rule (prevents compiler from choosing base class' doPacking())
template<typename T>
void doPacking(cCommBuffer *, T& t) {
    throw cRuntimeError("Parsim error: no doPacking() function for type %s or its base class (check .msg and _m.cc/h files!)",opp_typename(typeid(t)));
}

template<typename T>
void doUnpacking(cCommBuffer *, T& t) {
    throw cRuntimeError("Parsim error: no doUnpacking() function for type %s or its base class (check .msg and _m.cc/h files!)",opp_typename(typeid(t)));
}




Register_Class(EtherMserReq);

EtherMserReq::EtherMserReq(const char *name, int kind) : cPacket(name,kind)
{
    this->requestId_var = 0;
    this->responseBytes_var = 0;
    fileBuffer_arraysize = 0;
    this->fileBuffer_var = 0;
}

EtherMserReq::EtherMserReq(const EtherMserReq& other) : cPacket(other)
{
    fileBuffer_arraysize = 0;
    this->fileBuffer_var = 0;
    copy(other);
}

EtherMserReq::~EtherMserReq()
{
    delete [] fileBuffer_var;
}

EtherMserReq& EtherMserReq::operator=(const EtherMserReq& other)
{
    if (this==&other) return *this;
    cPacket::operator=(other);
    copy(other);
    return *this;
}

void EtherMserReq::copy(const EtherMserReq& other)
{
    this->requestId_var = other.requestId_var;
    this->responseBytes_var = other.responseBytes_var;
    delete [] this->fileBuffer_var;
    this->fileBuffer_var = (other.fileBuffer_arraysize==0) ? NULL : new char[other.fileBuffer_arraysize];
    fileBuffer_arraysize = other.fileBuffer_arraysize;
    for (unsigned int i=0; i<fileBuffer_arraysize; i++)
        this->fileBuffer_var[i] = other.fileBuffer_var[i];
}

void EtherMserReq::parsimPack(cCommBuffer *b)
{
    cPacket::parsimPack(b);
    doPacking(b,this->requestId_var);
    doPacking(b,this->responseBytes_var);
    b->pack(fileBuffer_arraysize);
    doPacking(b,this->fileBuffer_var,fileBuffer_arraysize);
}

void EtherMserReq::parsimUnpack(cCommBuffer *b)
{
    cPacket::parsimUnpack(b);
    doUnpacking(b,this->requestId_var);
    doUnpacking(b,this->responseBytes_var);
    delete [] this->fileBuffer_var;
    b->unpack(fileBuffer_arraysize);
    if (fileBuffer_arraysize==0) {
        this->fileBuffer_var = 0;
    } else {
        this->fileBuffer_var = new char[fileBuffer_arraysize];
        doUnpacking(b,this->fileBuffer_var,fileBuffer_arraysize);
    }
}

long EtherMserReq::getRequestId() const
{
    return requestId_var;
}

void EtherMserReq::setRequestId(long requestId)
{
    this->requestId_var = requestId;
}

long EtherMserReq::getResponseBytes() const
{
    return responseBytes_var;
}

void EtherMserReq::setResponseBytes(long responseBytes)
{
    this->responseBytes_var = responseBytes;
}

void EtherMserReq::setFileBufferArraySize(unsigned int size)
{
    char *fileBuffer_var2 = (size==0) ? NULL : new char[size];
    unsigned int sz = fileBuffer_arraysize < size ? fileBuffer_arraysize : size;
    for (unsigned int i=0; i<sz; i++)
        fileBuffer_var2[i] = this->fileBuffer_var[i];
    for (unsigned int i=sz; i<size; i++)
        fileBuffer_var2[i] = 0;
    fileBuffer_arraysize = size;
    delete [] this->fileBuffer_var;
    this->fileBuffer_var = fileBuffer_var2;
}

unsigned int EtherMserReq::getFileBufferArraySize() const
{
    return fileBuffer_arraysize;
}

char EtherMserReq::getFileBuffer(unsigned int k) const
{
    if (k>=fileBuffer_arraysize) throw cRuntimeError("Array of size %d indexed by %d", fileBuffer_arraysize, k);
    return fileBuffer_var[k];
}

void EtherMserReq::setFileBuffer(unsigned int k, char fileBuffer)
{
    if (k>=fileBuffer_arraysize) throw cRuntimeError("Array of size %d indexed by %d", fileBuffer_arraysize, k);
    this->fileBuffer_var[k] = fileBuffer;
}

class EtherMserReqDescriptor : public cClassDescriptor
{
  public:
    EtherMserReqDescriptor();
    virtual ~EtherMserReqDescriptor();

    virtual bool doesSupport(cObject *obj) const;
    virtual const char *getProperty(const char *propertyname) const;
    virtual int getFieldCount(void *object) const;
    virtual const char *getFieldName(void *object, int field) const;
    virtual int findField(void *object, const char *fieldName) const;
    virtual unsigned int getFieldTypeFlags(void *object, int field) const;
    virtual const char *getFieldTypeString(void *object, int field) const;
    virtual const char *getFieldProperty(void *object, int field, const char *propertyname) const;
    virtual int getArraySize(void *object, int field) const;

    virtual std::string getFieldAsString(void *object, int field, int i) const;
    virtual bool setFieldAsString(void *object, int field, int i, const char *value) const;

    virtual const char *getFieldStructName(void *object, int field) const;
    virtual void *getFieldStructPointer(void *object, int field, int i) const;
};

Register_ClassDescriptor(EtherMserReqDescriptor);

EtherMserReqDescriptor::EtherMserReqDescriptor() : cClassDescriptor("EtherMserReq", "cPacket")
{
}

EtherMserReqDescriptor::~EtherMserReqDescriptor()
{
}

bool EtherMserReqDescriptor::doesSupport(cObject *obj) const
{
    return dynamic_cast<EtherMserReq *>(obj)!=NULL;
}

const char *EtherMserReqDescriptor::getProperty(const char *propertyname) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    return basedesc ? basedesc->getProperty(propertyname) : NULL;
}

int EtherMserReqDescriptor::getFieldCount(void *object) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    return basedesc ? 3+basedesc->getFieldCount(object) : 3;
}

unsigned int EtherMserReqDescriptor::getFieldTypeFlags(void *object, int field) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount(object))
            return basedesc->getFieldTypeFlags(object, field);
        field -= basedesc->getFieldCount(object);
    }
    static unsigned int fieldTypeFlags[] = {
        FD_ISEDITABLE,
        FD_ISEDITABLE,
        FD_ISARRAY | FD_ISEDITABLE,
    };
    return (field>=0 && field<3) ? fieldTypeFlags[field] : 0;
}

const char *EtherMserReqDescriptor::getFieldName(void *object, int field) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount(object))
            return basedesc->getFieldName(object, field);
        field -= basedesc->getFieldCount(object);
    }
    static const char *fieldNames[] = {
        "requestId",
        "responseBytes",
        "fileBuffer",
    };
    return (field>=0 && field<3) ? fieldNames[field] : NULL;
}

int EtherMserReqDescriptor::findField(void *object, const char *fieldName) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    int base = basedesc ? basedesc->getFieldCount(object) : 0;
    if (fieldName[0]=='r' && strcmp(fieldName, "requestId")==0) return base+0;
    if (fieldName[0]=='r' && strcmp(fieldName, "responseBytes")==0) return base+1;
    if (fieldName[0]=='f' && strcmp(fieldName, "fileBuffer")==0) return base+2;
    return basedesc ? basedesc->findField(object, fieldName) : -1;
}

const char *EtherMserReqDescriptor::getFieldTypeString(void *object, int field) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount(object))
            return basedesc->getFieldTypeString(object, field);
        field -= basedesc->getFieldCount(object);
    }
    static const char *fieldTypeStrings[] = {
        "long",
        "long",
        "char",
    };
    return (field>=0 && field<3) ? fieldTypeStrings[field] : NULL;
}

const char *EtherMserReqDescriptor::getFieldProperty(void *object, int field, const char *propertyname) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount(object))
            return basedesc->getFieldProperty(object, field, propertyname);
        field -= basedesc->getFieldCount(object);
    }
    switch (field) {
        default: return NULL;
    }
}

int EtherMserReqDescriptor::getArraySize(void *object, int field) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount(object))
            return basedesc->getArraySize(object, field);
        field -= basedesc->getFieldCount(object);
    }
    EtherMserReq *pp = (EtherMserReq *)object; (void)pp;
    switch (field) {
        case 2: return pp->getFileBufferArraySize();
        default: return 0;
    }
}

std::string EtherMserReqDescriptor::getFieldAsString(void *object, int field, int i) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount(object))
            return basedesc->getFieldAsString(object,field,i);
        field -= basedesc->getFieldCount(object);
    }
    EtherMserReq *pp = (EtherMserReq *)object; (void)pp;
    switch (field) {
        case 0: return long2string(pp->getRequestId());
        case 1: return long2string(pp->getResponseBytes());
        case 2: return long2string(pp->getFileBuffer(i));
        default: return "";
    }
}

bool EtherMserReqDescriptor::setFieldAsString(void *object, int field, int i, const char *value) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount(object))
            return basedesc->setFieldAsString(object,field,i,value);
        field -= basedesc->getFieldCount(object);
    }
    EtherMserReq *pp = (EtherMserReq *)object; (void)pp;
    switch (field) {
        case 0: pp->setRequestId(string2long(value)); return true;
        case 1: pp->setResponseBytes(string2long(value)); return true;
        case 2: pp->setFileBuffer(i,string2long(value)); return true;
        default: return false;
    }
}

const char *EtherMserReqDescriptor::getFieldStructName(void *object, int field) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount(object))
            return basedesc->getFieldStructName(object, field);
        field -= basedesc->getFieldCount(object);
    }
    static const char *fieldStructNames[] = {
        NULL,
        NULL,
        NULL,
    };
    return (field>=0 && field<3) ? fieldStructNames[field] : NULL;
}

void *EtherMserReqDescriptor::getFieldStructPointer(void *object, int field, int i) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount(object))
            return basedesc->getFieldStructPointer(object, field, i);
        field -= basedesc->getFieldCount(object);
    }
    EtherMserReq *pp = (EtherMserReq *)object; (void)pp;
    switch (field) {
        default: return NULL;
    }
}

Register_Class(EtherMserResp);

EtherMserResp::EtherMserResp(const char *name, int kind) : cPacket(name,kind)
{
    this->requestId_var = 0;
    this->numFrames_var = 0;
    fileBuffer_arraysize = 0;
    this->fileBuffer_var = 0;
}

EtherMserResp::EtherMserResp(const EtherMserResp& other) : cPacket(other)
{
    fileBuffer_arraysize = 0;
    this->fileBuffer_var = 0;
    copy(other);
}

EtherMserResp::~EtherMserResp()
{
    delete [] fileBuffer_var;
}

EtherMserResp& EtherMserResp::operator=(const EtherMserResp& other)
{
    if (this==&other) return *this;
    cPacket::operator=(other);
    copy(other);
    return *this;
}

void EtherMserResp::copy(const EtherMserResp& other)
{
    this->requestId_var = other.requestId_var;
    this->numFrames_var = other.numFrames_var;
    delete [] this->fileBuffer_var;
    this->fileBuffer_var = (other.fileBuffer_arraysize==0) ? NULL : new char[other.fileBuffer_arraysize];
    fileBuffer_arraysize = other.fileBuffer_arraysize;
    for (unsigned int i=0; i<fileBuffer_arraysize; i++)
        this->fileBuffer_var[i] = other.fileBuffer_var[i];
}

void EtherMserResp::parsimPack(cCommBuffer *b)
{
    cPacket::parsimPack(b);
    doPacking(b,this->requestId_var);
    doPacking(b,this->numFrames_var);
    b->pack(fileBuffer_arraysize);
    doPacking(b,this->fileBuffer_var,fileBuffer_arraysize);
}

void EtherMserResp::parsimUnpack(cCommBuffer *b)
{
    cPacket::parsimUnpack(b);
    doUnpacking(b,this->requestId_var);
    doUnpacking(b,this->numFrames_var);
    delete [] this->fileBuffer_var;
    b->unpack(fileBuffer_arraysize);
    if (fileBuffer_arraysize==0) {
        this->fileBuffer_var = 0;
    } else {
        this->fileBuffer_var = new char[fileBuffer_arraysize];
        doUnpacking(b,this->fileBuffer_var,fileBuffer_arraysize);
    }
}

int EtherMserResp::getRequestId() const
{
    return requestId_var;
}

void EtherMserResp::setRequestId(int requestId)
{
    this->requestId_var = requestId;
}

int EtherMserResp::getNumFrames() const
{
    return numFrames_var;
}

void EtherMserResp::setNumFrames(int numFrames)
{
    this->numFrames_var = numFrames;
}

void EtherMserResp::setFileBufferArraySize(unsigned int size)
{
    char *fileBuffer_var2 = (size==0) ? NULL : new char[size];
    unsigned int sz = fileBuffer_arraysize < size ? fileBuffer_arraysize : size;
    for (unsigned int i=0; i<sz; i++)
        fileBuffer_var2[i] = this->fileBuffer_var[i];
    for (unsigned int i=sz; i<size; i++)
        fileBuffer_var2[i] = 0;
    fileBuffer_arraysize = size;
    delete [] this->fileBuffer_var;
    this->fileBuffer_var = fileBuffer_var2;
}

unsigned int EtherMserResp::getFileBufferArraySize() const
{
    return fileBuffer_arraysize;
}

char EtherMserResp::getFileBuffer(unsigned int k) const
{
    if (k>=fileBuffer_arraysize) throw cRuntimeError("Array of size %d indexed by %d", fileBuffer_arraysize, k);
    return fileBuffer_var[k];
}

void EtherMserResp::setFileBuffer(unsigned int k, char fileBuffer)
{
    if (k>=fileBuffer_arraysize) throw cRuntimeError("Array of size %d indexed by %d", fileBuffer_arraysize, k);
    this->fileBuffer_var[k] = fileBuffer;
}

class EtherMserRespDescriptor : public cClassDescriptor
{
  public:
    EtherMserRespDescriptor();
    virtual ~EtherMserRespDescriptor();

    virtual bool doesSupport(cObject *obj) const;
    virtual const char *getProperty(const char *propertyname) const;
    virtual int getFieldCount(void *object) const;
    virtual const char *getFieldName(void *object, int field) const;
    virtual int findField(void *object, const char *fieldName) const;
    virtual unsigned int getFieldTypeFlags(void *object, int field) const;
    virtual const char *getFieldTypeString(void *object, int field) const;
    virtual const char *getFieldProperty(void *object, int field, const char *propertyname) const;
    virtual int getArraySize(void *object, int field) const;

    virtual std::string getFieldAsString(void *object, int field, int i) const;
    virtual bool setFieldAsString(void *object, int field, int i, const char *value) const;

    virtual const char *getFieldStructName(void *object, int field) const;
    virtual void *getFieldStructPointer(void *object, int field, int i) const;
};

Register_ClassDescriptor(EtherMserRespDescriptor);

EtherMserRespDescriptor::EtherMserRespDescriptor() : cClassDescriptor("EtherMserResp", "cPacket")
{
}

EtherMserRespDescriptor::~EtherMserRespDescriptor()
{
}

bool EtherMserRespDescriptor::doesSupport(cObject *obj) const
{
    return dynamic_cast<EtherMserResp *>(obj)!=NULL;
}

const char *EtherMserRespDescriptor::getProperty(const char *propertyname) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    return basedesc ? basedesc->getProperty(propertyname) : NULL;
}

int EtherMserRespDescriptor::getFieldCount(void *object) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    return basedesc ? 3+basedesc->getFieldCount(object) : 3;
}

unsigned int EtherMserRespDescriptor::getFieldTypeFlags(void *object, int field) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount(object))
            return basedesc->getFieldTypeFlags(object, field);
        field -= basedesc->getFieldCount(object);
    }
    static unsigned int fieldTypeFlags[] = {
        FD_ISEDITABLE,
        FD_ISEDITABLE,
        FD_ISARRAY | FD_ISEDITABLE,
    };
    return (field>=0 && field<3) ? fieldTypeFlags[field] : 0;
}

const char *EtherMserRespDescriptor::getFieldName(void *object, int field) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount(object))
            return basedesc->getFieldName(object, field);
        field -= basedesc->getFieldCount(object);
    }
    static const char *fieldNames[] = {
        "requestId",
        "numFrames",
        "fileBuffer",
    };
    return (field>=0 && field<3) ? fieldNames[field] : NULL;
}

int EtherMserRespDescriptor::findField(void *object, const char *fieldName) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    int base = basedesc ? basedesc->getFieldCount(object) : 0;
    if (fieldName[0]=='r' && strcmp(fieldName, "requestId")==0) return base+0;
    if (fieldName[0]=='n' && strcmp(fieldName, "numFrames")==0) return base+1;
    if (fieldName[0]=='f' && strcmp(fieldName, "fileBuffer")==0) return base+2;
    return basedesc ? basedesc->findField(object, fieldName) : -1;
}

const char *EtherMserRespDescriptor::getFieldTypeString(void *object, int field) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount(object))
            return basedesc->getFieldTypeString(object, field);
        field -= basedesc->getFieldCount(object);
    }
    static const char *fieldTypeStrings[] = {
        "int",
        "int",
        "char",
    };
    return (field>=0 && field<3) ? fieldTypeStrings[field] : NULL;
}

const char *EtherMserRespDescriptor::getFieldProperty(void *object, int field, const char *propertyname) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount(object))
            return basedesc->getFieldProperty(object, field, propertyname);
        field -= basedesc->getFieldCount(object);
    }
    switch (field) {
        default: return NULL;
    }
}

int EtherMserRespDescriptor::getArraySize(void *object, int field) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount(object))
            return basedesc->getArraySize(object, field);
        field -= basedesc->getFieldCount(object);
    }
    EtherMserResp *pp = (EtherMserResp *)object; (void)pp;
    switch (field) {
        case 2: return pp->getFileBufferArraySize();
        default: return 0;
    }
}

std::string EtherMserRespDescriptor::getFieldAsString(void *object, int field, int i) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount(object))
            return basedesc->getFieldAsString(object,field,i);
        field -= basedesc->getFieldCount(object);
    }
    EtherMserResp *pp = (EtherMserResp *)object; (void)pp;
    switch (field) {
        case 0: return long2string(pp->getRequestId());
        case 1: return long2string(pp->getNumFrames());
        case 2: return long2string(pp->getFileBuffer(i));
        default: return "";
    }
}

bool EtherMserRespDescriptor::setFieldAsString(void *object, int field, int i, const char *value) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount(object))
            return basedesc->setFieldAsString(object,field,i,value);
        field -= basedesc->getFieldCount(object);
    }
    EtherMserResp *pp = (EtherMserResp *)object; (void)pp;
    switch (field) {
        case 0: pp->setRequestId(string2long(value)); return true;
        case 1: pp->setNumFrames(string2long(value)); return true;
        case 2: pp->setFileBuffer(i,string2long(value)); return true;
        default: return false;
    }
}

const char *EtherMserRespDescriptor::getFieldStructName(void *object, int field) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount(object))
            return basedesc->getFieldStructName(object, field);
        field -= basedesc->getFieldCount(object);
    }
    static const char *fieldStructNames[] = {
        NULL,
        NULL,
        NULL,
    };
    return (field>=0 && field<3) ? fieldStructNames[field] : NULL;
}

void *EtherMserRespDescriptor::getFieldStructPointer(void *object, int field, int i) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount(object))
            return basedesc->getFieldStructPointer(object, field, i);
        field -= basedesc->getFieldCount(object);
    }
    EtherMserResp *pp = (EtherMserResp *)object; (void)pp;
    switch (field) {
        default: return NULL;
    }
}


