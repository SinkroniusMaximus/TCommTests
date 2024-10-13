#ifndef COMMUNICATOR_H
#define COMMUNICATOR_H

namespace TComm {

static class Communicator// Communicator 
{
    friend class ByteSerializer;
    friend class BaseCommunicationObject;
    friend class BaseSubscriber;
    friend class TCommBase;
    public:
        void Run() 
        {
            // PRINT("Read data");
            ReadData();
            // PRINT("update comm objects");
            UpdateCommObjects();
        }        
        void SetSendAllData()
        {
            sendAllData = true;            
        }
    protected:
        void WriteData(const CommunicationData& commData, AbstractSubscriber* excludedSubscriber = nullptr) 
        {
            for (auto& subscriber : subList)
            {
                if (subscriber != excludedSubscriber)
                {
                    subscriber->Write(commData);
                }
            }
        }

        void InjectCommObject(CommunicationData commData, AbstractSubscriber* excludedSubscriber)
        {
            // PRINT("injecting");
            // PRINT("device index " << commData.deviceIndex);
            // PRINT("object index " << commData.objectIndex);
            // PRINT("datasize " << commData.dataSize);
            auto deviceEntry = deviceObjectMap.find(commData.deviceIndex);
            if (deviceEntry != deviceObjectMap.end()) 
            {
                // PRINT("device found");
                
                if(deviceEntry->second.size() > commData.objectIndex)
                {
                    // PRINT("object found");
                    auto objectEntry = deviceEntry->second.at(commData.objectIndex);                    
                    objectEntry->Inject(commData);
                }
                else
                {
                    ;
                    // PRINT("object not found");
                }
            }
            else
            {
                // PRINT("device not found");
            }
            WriteData(commData, excludedSubscriber);
        }
        int AddCommunicationObject(uint16_t deviceIndex, std::shared_ptr<AbstractCommunicationObject> communicationObject)
        {
            deviceObjectMap[deviceIndex].push_back(communicationObject);
            deviceChangeMap[deviceIndex] = true;
            return deviceObjectMap[deviceIndex].size() - 1;
        }

        int AddCommunicationObject(uint16_t deviceIndex, AbstractCommunicationObject* communicationObject) {
            deviceObjectMap[deviceIndex].push_back(std::shared_ptr<AbstractCommunicationObject>(communicationObject, [](AbstractCommunicationObject*) {}));
            deviceChangeMap[deviceIndex] = true;
            return deviceObjectMap[deviceIndex].size() - 1;
        }

        void RemoveCommunicationObject(uint16_t deviceIndex, AbstractCommunicationObject* communicationObject) {
            auto& objects = deviceObjectMap[deviceIndex];
            objects.erase(std::remove_if(objects.begin(), objects.end(),
                [communicationObject](const std::shared_ptr<AbstractCommunicationObject>& obj) {
                    return obj.get() == communicationObject;
                }), objects.end());
            deviceChangeMap[deviceIndex] = true;
        }

        void AddSubscriber(AbstractSubscriber* subscriber)
        {
            subList.push_back(subscriber);
        }
        
        void RemoveSubscriber(AbstractSubscriber* subscriber)
        {
            subList.erase(std::remove_if(subList.begin(), subList.end(),
            [subscriber](const AbstractSubscriber* sub) {
                return sub == subscriber;
            }), subList.end());
        }

        std::shared_ptr<AbstractCommunicationObject> GetCommunicationObject(uint16_t deviceIndex, uint16_t objectIndex)
        {
            auto deviceEntry = deviceObjectMap.find(deviceIndex);
            if (deviceEntry != deviceObjectMap.end()) 
            {
                if(deviceEntry->second.size() > objectIndex)
                {
                    auto objectEntry = deviceEntry->second.at(objectIndex); 
                    return objectEntry;
                }
            }
            std::shared_ptr<AbstractCommunicationObject> aco = std::make_shared<AbstractCommunicationObject>();
            return aco;
        }
        
        size_t GetObjectListSize(uint16_t deviceIndex)
        {
            return deviceObjectMap[deviceIndex].size();
        }

        // Updating device index by removing the old key and inserting the new key
        void UpdateDeviceIndex(uint16_t oldDeviceIndex, uint16_t newDeviceIndex) 
        {
            auto deviceEntry = deviceObjectMap.find(oldDeviceIndex);
            if (deviceEntry != deviceObjectMap.end()) 
            {
                const std::vector<std::shared_ptr<AbstractCommunicationObject>>& objectList = deviceEntry->second;
                for (std::shared_ptr<AbstractCommunicationObject> commObject : objectList) 
                {
                    commObject->SetDeviceIndex(newDeviceIndex);
                }
                deviceObjectMap[newDeviceIndex] = std::move(deviceEntry->second);
                deviceObjectMap.erase(deviceEntry);
            }
            deviceChangeMap[oldDeviceIndex] = true;
            deviceChangeMap[newDeviceIndex] = true;
        }

        void RemoveDevice(uint16_t deviceIndex)
        {
            auto deviceEntry = deviceObjectMap.find(deviceIndex);
            if (deviceEntry != deviceObjectMap.end()) 
            {
                deviceObjectMap.erase(deviceEntry);
                deviceChangeMap[deviceIndex] = true;
            }
        }
        
        void RegisterDevice(uint16_t deviceIndex)
        {          
            for(AbstractSubscriber* subscriber : subList)
            { 
                subscriber->RegisterDevice(deviceIndex); 
            } // notify the subscriber of change in a device
        }

        std::unordered_map<uint16_t, std::vector<std::shared_ptr<AbstractCommunicationObject>>> deviceObjectMap; // Communication object list per device
        std::unordered_map<uint16_t, bool> deviceChangeMap;
        std::vector<AbstractSubscriber*> subList; //  Subscriber object list

    private:
        void ReadData()
        {
            for(AbstractSubscriber* subscriber : subList)
            { 
                subscriber->Read(); 
            } // read incoming data

        }

        void UpdateCommObjects()
        {
            for (auto& deviceChanged : deviceChangeMap)
            {
                if(deviceChanged.second)
                {
                    PRINT("device changed, registring..");
                    PRINT(deviceChanged.first);
                    RegisterDevice(deviceChanged.first);
                    deviceChanged.second = false;
                }
            }

            for (const auto& deviceEntry : deviceObjectMap) 
            {
                // uint16_t deviceIndex = deviceEntry.first;
                const std::vector<std::shared_ptr<AbstractCommunicationObject>>& objectList = deviceEntry.second;
                for (std::shared_ptr<AbstractCommunicationObject> commObject : objectList) 
                {
                    if(sendAllData)
                    {
                        commObject->SetChanged(); //force sending all data or on device change
                    }
                    if(IsTimeToUpdate(commObject->GetUpdateInterval()) || sendAllData)
                    {
                        commObject->Update(); 
                    }
                }
            } // on change write
            sendAllData = false;
        }

        bool IsTimeToUpdate(EnumUpdateInterval updateInterval)
        {
            switch(updateInterval)
            {
                case eOnChange:
                    return true;
                case eClock_10hz:
                    return clockBits.ClockEdge_10hz();
                case eClock_5hz:
                    return clockBits.ClockEdge_5hz();
                case eClock_2_5hz:
                    return clockBits.ClockEdge_2_5hz();
                case eClock_2hz:
                    return clockBits.ClockEdge_2hz();
                case eClock_1_hz:
                    return clockBits.ClockEdge_1hz();
                case eClock_0_625hz:
                    return clockBits.ClockEdge_0_625hz();
                case eClock_0_5hz:
                    return clockBits.ClockEdge_0_5hz();
            };
            return false;
        }
        int i;
        bool sendAllData;
} communicator;
};

#endif //COMMUNICATOR
