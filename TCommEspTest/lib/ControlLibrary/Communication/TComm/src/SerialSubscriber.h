#ifndef SERIALSUBSCRIBER_H
#define SERIALSUBSCRIBER_H

namespace TComm {

class SerialSubscriber : ByteSerializer, BaseSubscriber // Subscription of serial port to the communicator
{
    public:  
        void Xinit(StreamInterface* newPort) 
        { 
            port = newPort;
        }

        StreamInterface* GetPort() const {
            return port;
        }

        void Write(CommunicationData data) override 
        {
            byte toSend[data.dataSize + 8];
            Serialize(data, toSend);
            if(port)
            {
                port->write(toSend, sizeof(toSend));
            }
        }
        
        void Read() override 
        {
            if(port)
            {
                while(port->available())
                {
                    CommunicationData* data = Deserialize(port->read());
                    if(data)
                    {
                        InjectCommObject(*data, this);
                    }
                }
            }
        }
    private:
        StreamInterface* port;
};
};
#endif //SERIALSUBSCRIBER
