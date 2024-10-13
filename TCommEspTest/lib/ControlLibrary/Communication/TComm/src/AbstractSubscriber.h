#ifndef ABSTRACTSUBSCRIBER_H
#define ABSTRACTSUBSCRIBER_H

namespace TComm {

class AbstractSubscriber // Abstract subscription object
{
    public:
        virtual void Write(CommunicationData data) = 0;
        virtual void Read() = 0;
        virtual void RegisterDevice(uint16_t deviceIndex)
        {
            
        };
};
};

#endif // ABSTRACTSUBSCRIBER_H