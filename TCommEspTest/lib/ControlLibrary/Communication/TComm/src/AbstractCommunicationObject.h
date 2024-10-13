#ifndef ABSTRACTCOMMUNICATIONOBJECT_H
#define ABSTRACTCOMMUNICATIONOBJECT_H

namespace TComm {

class AbstractCommunicationObject //Abstract Communication object
{
    public:
        virtual void Update() {}
        virtual void Inject(CommunicationData commData) {}
        virtual const size_t GetSize() const { return 0; }
        virtual void SetName(const char* name) {}
        virtual const char* GetName() const { return "abstractName"; }
        virtual void SetPath(const char* path) {}
        virtual const char* GetPath() const { return "abstractPath"; }
        virtual CommunicationData GetData() { CommunicationData data; return data; }
        virtual void SetDeviceIndex(int deviceIndex) {}
        virtual const std::type_info* GetType() { return &typeid(this); }
        virtual void SetChanged() {}
        virtual EnumUpdateInterval GetUpdateInterval() { return EnumUpdateInterval::eOnChange; }
};
};

#endif // ABSTRACTCOMMUNICATIONOBJECT_H