#ifndef BASECOMMUNICATIONOBJECT_H
#define BASECOMMUNICATIONOBJECT_H
namespace TComm
{
class BaseCommunicationObject : public AbstractCommunicationObject
{
    public:
        BaseCommunicationObject(int deviceIndex = 1)
        {
            SetDeviceIndex(deviceIndex);
            RegisterObject();
        }
        ~BaseCommunicationObject()
        {
            UnregisterObject();
        }

        void SetDeviceIndex(int deviceIndex)
        {
            this->deviceIndex = deviceIndex;
        }

    protected:
        virtual void UnregisterObject()
        {
            communicator.RemoveCommunicationObject(deviceIndex, this);
        }
        virtual void RegisterObject()
        {
            objectIndex = communicator.AddCommunicationObject(deviceIndex, this);
        }
        void WriteData(CommunicationData commData)
        {
            communicator.WriteData(commData);
        }
        int objectIndex;
        int deviceIndex;
        

};
};
#endif // BASECOMMUNICATIONOBJECT_H