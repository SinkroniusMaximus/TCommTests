#ifndef TEMPLATECOMMUNICATIONOBJECT_H
#define TEMPLATECOMMUNICATIONOBJECT_H

namespace TComm {
template <class T>
class TemplateCommunicationObject : public BaseCommunicationObject //Template Communication object
{
    public:
        TemplateCommunicationObject(int deviceIndex = 1) : BaseCommunicationObject(deviceIndex)
        {             
            Initialize(nullptr, nullptr);
        }
        TemplateCommunicationObject(const char* name, int deviceIndex = 1) : BaseCommunicationObject(deviceIndex)
        { 
            Initialize(name, nullptr);
        }

        TemplateCommunicationObject(const char* name, std::function<std::string()> getParentPath, int deviceIndex = 1) : BaseCommunicationObject(deviceIndex)
        {
            Initialize(name, getParentPath().c_str());
        }

        TemplateCommunicationObject(const char* name, const char* path, int deviceIndex = 1) : BaseCommunicationObject(deviceIndex)
        {
            Initialize(name, path);
        }

        ~TemplateCommunicationObject() 
        {
        }

        TemplateCommunicationObject<T>& operator = (const T& input) 
        { 
            value = input; 
            return *this;
        }

        TemplateCommunicationObject<T>& operator = (const TemplateCommunicationObject<T>& input) 
        { 
            value = input.value; 
            return *this;
        }

        operator T&()
        { 
            return value; 
        }

        operator T*()
        { 
            return &value; 
        }

        T* operator ->() 
        { 
            return &value; 
        }

        void Set(T input) 
        { 
            value = input;
        }

        T Get() 
        { 
            return value;
        }

        CommunicationData GetData() override
        {
            CommunicationData commData;
            if constexpr (std::is_same_v<T, std::string>) 
            {
                // Allocate memory for the buffer
                commData.buffer = new byte[value.size() + 1]; // +1 for null terminator
                // Copy string data including null terminator
                std::memcpy(commData.buffer, value.c_str(), value.size() + 1);
                commData.dataSize = value.size() + 1; // Include null terminator size
            } else 
            {
                commData.buffer = reinterpret_cast<byte*>(&value);
                commData.dataSize = sizeof(T);
            }
            commData.objectIndex = objectIndex;
            commData.deviceIndex = deviceIndex;
            return commData;
        }

        const size_t GetSize() const
        {
            return sizeof(value);
        }

        bool IsChangeReceived()
        {
            bool isChangeReceived = changeReceived;
            changeReceived = false;
            return isChangeReceived;
        }

        void Update() override 
        { 
            if(changed || !(value == valueOld)) 
            {
                // PRINT("writing data: ");
                // PRINT(objectName);
                // PRINT(objectPath);
                // PRINT(value);
                CommunicationData commData = GetData();
                // on change writeData
                WriteData(commData);
                // PRINT("on to the next");
                valueOld = value;
                changed = false;
                if constexpr (std::is_same_v<T, std::string>) {
                    delete[] commData.buffer; // Free allocated buffer for strings
                }
            } 
        }

        void Inject(CommunicationData commData) override 
        {
            valueOld = value;
            if constexpr (std::is_same_v<T, std::string>) 
            {
                value = std::string(reinterpret_cast<char*>(commData.buffer), commData.dataSize);
            } else 
            {
                std::memcpy(&value, commData.buffer, sizeof(T));
            }
            // PRINT("name");
            // PRINT(objectName);
            // PRINT("path");
            // PRINT(objectPath);
            // PRINT("size");
            // PRINT(commData.dataSize);

            // PRINT("new injected value ------");
            // PRINT(value);
            changeReceived = !(value == valueOld);
            if(changeReceived)
            {
                ;
                // PRINT("changed:");
            }
            valueOld = value;
        }

        void SetChanged() { changed = true; }

        void SetName(const char* name) 
        {
            objectName = TruncateString(name, MAX_NAME_LENGTH);
        }

        const char* GetName() const 
        {
            return objectName.c_str();
        }

        void SetPath(const char* path) 
        {
            objectPath = TruncateString(path, MAX_PATH_LENGTH);
        }

        const char* GetPath() const 
        {
            return objectPath.c_str();
        }

        const std::type_info* GetType()
        {
            return &typeid(T);
        }

        void SetUpdateInterval(EnumUpdateInterval updateInterval)
        {
            this->updateInterval = updateInterval;
        }

        EnumUpdateInterval GetUpdateInterval()
        {
            return updateInterval;
        }

    private:
        T value;
        T valueOld;
        bool changed;
        bool changeReceived;
        std::string objectName;
        std::string objectPath;
        static const size_t MAX_PATH_LENGTH = 99;
        static const size_t MAX_NAME_LENGTH = 99;
        EnumUpdateInterval updateInterval;

        void Initialize(const char* name, const char* path) 
        {
            if (name) 
            {
                SetName(name);
            }
            if (path) 
            {
                SetPath(path);
            }
            SetUpdateInterval(eClock_5hz);
            // Initialization logic here
            if constexpr (std::is_arithmetic_v<T> || std::is_same_v<T, bool>) 
            {
                value = 0;
                valueOld = 0;
            } 
            else if constexpr (std::is_default_constructible_v<T>) 
            {
                value = T();
                valueOld = T();
            }
        }

        std::string TruncateString(const char* value, size_t maxLength) {
            if (value == nullptr) {
                return "";
            }

            std::string str(value);
            if (str.length() > maxLength) {
                str = str.substr(0, maxLength);
            }

            return str;
        }
};
};

#endif //TEMPLATECOMMUNICATIONOBJECT_H
