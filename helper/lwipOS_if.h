#include <systemc>

#ifndef LWIP_OS__H
#define LWIP_OS__H
class lwip_recv_if: virtual public sc_core::sc_interface
{
public:
    virtual int GetNode(int os_task_id) = 0;
    virtual int GetWeight(int os_task_id) = 0;
    virtual int GetSize(int os_task_id) = 0;
    virtual bool GetData(unsigned size, char* data, int os_task_id) = 0;
};

class lwip_send_if: virtual public sc_core::sc_interface
{
public:
    virtual void SetNode(int NodeID, int os_task_id) = 0;
    virtual void SetWeight(int weight, int os_task_id) = 0;
    virtual void SetSize(int size, int os_task_id) = 0;
    virtual void SetData(unsigned size, char* data, int os_task_id) = 0;
};
#endif
