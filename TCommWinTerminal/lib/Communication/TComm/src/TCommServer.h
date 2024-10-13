// #ifndef TCOMMSERVER_H
// #define TCOMMSERVER_H

// namespace TComm
// {
// class TCommServer : public TCommBase
// {
// public:
//     TCommServer()
//     {
//         // register types, so when receiving device list
//         // it can generate a identical list in the server
//         TypeRegistry::RegisterType<int>();          // Xint
//         TypeRegistry::RegisterType<int32_t>();      // Xint32
//         TypeRegistry::RegisterType<uint8_t>();      // Xuint8
//         TypeRegistry::RegisterType<uint16_t>();     // Xuint16
//         TypeRegistry::RegisterType<float>();        // Xfloat
//         TypeRegistry::RegisterType<bool>();         // Xbool
//         TypeRegistry::RegisterType<std::string>();  // Xstring
//         deviceIndexManager.leaseIndex(0);// server index 0
//         deviceIndexManager.leaseIndex(0);// void device index 1
//     }
//     void Run()
//     {
//         // PRINT("server run");
//         communicator.Run();
//         HandleOverhead();
//     }
// private:
//     void HandleOverhead()
//     {
//         bool changeReceived = message.type.IsChangeReceived();
//         if(changeReceived)
//         {
//             PRINT("change received");
//             ;
//         }
//         switch(message.type)
//         {
//             case eNone:
//             {
//                 // PRINT("CCCC");
//                 if(subjectIsBeingInterrogated)
//                 {
//                     if(interruptedSequence)
//                     {
//                         message.type = sequenceMessageType;
//                         interruptedSequence = false;
//                     }
//                     else if(subjectDeviceListSize == 0)
//                     {
//                         // PRINT("CCDD");
//                         message.type = eRequestListSize;
//                         sequenceMessageType = message.type;
//                     }
//                     else if(subjectObjectIndex < subjectDeviceListSize)
//                     {
//                         // PRINT("DDDD");
//                         message.value = subjectObjectIndex;
//                         // message.type = eRequestNameOfObject;
//                         message.type = eRequestObject;
//                         sequenceMessageType = message.type;
//                     }
//                     else
//                     {
//                         PRINT("choose between end or request all data -------");
//                         PRINT(sequenceMessageType);
//                         PRINT(eRequestAllData);
//                         if(!(sequenceMessageType == eRequestAllData))
//                         {
//                             message.type = eRequestAllData;
//                             sequenceMessageType = message.type;
//                             PRINT("requesting all data");
//                         }
//                         else
//                         {
//                             message.type = eEndInterrogation;
//                             subjectIsBeingInterrogated = false;
//                             PRINT("ending interrogation");
//                         }
//                     }
//                 }
//                 break;
//             }
//             case eWhatsMyDeviceAddress:
//             {
//                 if(!subjectIsBeingInterrogated)
//                 {
//                     PRINT("whats my device?");
//                     PRINT(message.text1);
//                     subjectDeviceIndex = deviceIndexManager.leaseIndex(0);
//                     message.value = subjectDeviceIndex;
//                     subjectIsBeingInterrogated = true;
//                     subjectObjectIndex = 0;
//                     message.type = eYourDeviceAddressIs;
//                 }
//                 else
//                 {
//                     interruptedSequence = true;
//                     message.type = ePleaseBePatient;
//                 }
//                 break;
//             }
//             case eYourDeviceAddressIs:
//             {
//                 break;
//             }
//             case eRequestAllData:
//             {
//                 break;
//             }
//             case eRequestNameOfObject:
//             {
//                 break;
//             }
//             case eResponseNameOfObject:
//             {
//                 PRINT("response name");
//                 // PRINT(message->text);
//                 if (subjectIsBeingInterrogated) 
//                 {
//                     subjectObjectName = message.text1; 
//                     // subjectObjectIndex++;
//                     // PRINT("AAAA");
//                     message.type = eRequestPathOfObject;
//                     sequenceMessageType = message.type;
//                     break;
//                 }
//                 message.type = eNone;
//                 // PRINT("BBBB");
//                 break;
//             }
//             case eRequestPathOfObject:
//             {
//                 break;
//             }
//             case eResponsePathOfObject:
//             {
//                 PRINT("response path");
//                 PRINT(&message.text1);
//                 if (subjectIsBeingInterrogated) 
//                 {
//                     subjectObjectPath = message.text1;
//                     message.type = eRequestDataTypeOfObject;
//                     sequenceMessageType = message.type;
//                 }
//                 break;
//             }
//             case eRequestDataTypeOfObject:
//             {
//                 break;
//             }
//             case eResponseDataTypeOfObject:
//             {
//                 PRINT("response data type");
//                 PRINT(&message.text1);
//                 if(subjectIsBeingInterrogated)
//                 {
//                     TypeRegistry::CreateObject(message.text1->c_str(), subjectDeviceIndex);
//                     SetObjectName(subjectDeviceIndex, subjectObjectIndex, subjectObjectName.c_str());
//                     SetObjectPath(subjectDeviceIndex, subjectObjectIndex, subjectObjectPath.c_str());

//                     int localIndex = GetListSize(subjectDeviceIndex) - 1;
//                     // PRINT("got list size");
//                     if( localIndex != subjectObjectIndex)
//                     {
//                         PRINT("Index error");
//                         PRINT("subject Device Index");
//                         PRINT(subjectDeviceIndex);
//                         PRINT("local index");
//                         PRINT(localIndex);
//                         PRINT("subject object index");
//                         PRINT(subjectObjectIndex);
//                         while(1)
//                         {
                            
//                         }
//                     }
//                     subjectObjectIndex++;
//                 }
//                 message.type = eNone;
//                 sequenceMessageType = message.type;
//                 break;
//             }
//             case eRequestObject:
//             {         
//                 break;
//             }
//             case eResponseObject:
//             {
//                 if(subjectIsBeingInterrogated)
//                 {
//                     PRINT("response object");
//                     PRINT(message.text1->c_str()); // name
//                     PRINT(message.text2->c_str()); // path
//                     PRINT(message.text3->c_str()); // datatype
//                     TypeRegistry::CreateObject(message.text3->c_str(), subjectDeviceIndex);
//                     SetObjectName(subjectDeviceIndex, subjectObjectIndex, message.text1->c_str());
//                     SetObjectPath(subjectDeviceIndex, subjectObjectIndex, message.text2->c_str());
//                     message.type = eNone;
//                     sequenceMessageType = message.type;
//                     subjectObjectIndex++;
//                 }
//                 break;
//             }
//             case eRequestListSize:
//             {
//                 // PRINT("requesting list size");
//                 break;
//             }
//             case eResponseListSize:
//             {
//                 PRINT("response list size");
//                 PRINT(&message.value);
//                 subjectDeviceListSize = message.value;
//                 message.type = eNone;
//                 break;
//             }
//             case eEndInterrogation:
//             {
//                 // PRINT("ended interrogation");
//                 break;
//             }
//             case ePleaseBePatient:
//             {
//                 break;
//             }
//         };
//         // PRINT("Finish overhead");
//     };
//     MESSAGE message{"TComm"};
//     std::string subjectObjectName;
//     std::string subjectObjectPath;
//     ResourceIndexManager deviceIndexManager;

//     bool subjectIsBeingInterrogated;
//     uint16_t subjectDeviceIndex;
//     uint16_t subjectDeviceListSize;
//     uint16_t subjectObjectIndex;
//     bool interruptedSequence;
//     MESSAGETYPE sequenceMessageType;
// };
// };
// #endif //TCOMMSERVER_H
#ifndef TCOMMSERVER_H
#define TCOMMSERVER_H

#include <unordered_map>

namespace TComm
{
class TCommServer : public TCommBase
{
public:
    TCommServer()
    {
        // Register types, so when receiving device list
        // it can generate an identical list in the server
        TypeRegistry::RegisterType<int>();          // Xint
        TypeRegistry::RegisterType<int32_t>();      // Xint32
        TypeRegistry::RegisterType<uint8_t>();      // Xuint8
        TypeRegistry::RegisterType<uint16_t>();     // Xuint16
        TypeRegistry::RegisterType<float>();        // Xfloat
        TypeRegistry::RegisterType<bool>();         // Xbool
        TypeRegistry::RegisterType<std::string>();  // Xstring
        deviceIndexManager.leaseIndex(0);// server index 0
        deviceIndexManager.leaseIndex(0);// void device index 1
    }

    void Run()
    {
        // PRINT("server run");
        communicator.Run();
        HandleOverhead();
    }

private:
    struct DeviceData
    {
        std::string name;
        std::string path;
        std::string dataType;
    };

    void HandleOverhead()
    {
        bool changeReceived = message.type.IsChangeReceived();
        if (changeReceived)
        {
            ;
            // PRINT("change received");
        }

        switch (message.type)
        {
            case eNone:
            {
                if (subjectIsBeingInterrogated)
                {
                    if (interruptedSequence)
                    {
                        message.type = sequenceMessageType;
                        interruptedSequence = false;
                    }
                    else if (subjectDeviceListSize == 0)
                    {
                        message.type = eRequestListSize;
                        sequenceMessageType = message.type;
                    }
                    else if (subjectObjectIndex < subjectDeviceListSize)
                    {
                        message.value = subjectObjectIndex;
                        message.type = eRequestObject;
                        sequenceMessageType = message.type;
                    }
                    else
                    {
                        PRINT("choose between end or request all data -------");
                        PRINT(sequenceMessageType);
                        PRINT(eRequestAllData);
                        if (!(sequenceMessageType == eRequestAllData))
                        {
                            message.type = eRequestAllData;
                            sequenceMessageType = message.type;
                            PRINT("requesting all data");
                        }
                        else
                        {
                            message.type = eEndInterrogation;
                            subjectIsBeingInterrogated = false;
                            PRINT("ending interrogation");
                        }
                    }
                }
                break;
            }
            case eWhatsMyDeviceAddress:
            {
                if (!subjectIsBeingInterrogated)
                {   
                    if(message.text1.IsChangeReceived())
                    {
                        // PRINT("whats my device?");
                        // PRINT(message.text1->c_str());
                        subjectDeviceIndex = GetOrCreateDeviceIndex(message.text1);
                        message.value = subjectDeviceIndex;
                        subjectIsBeingInterrogated = true;
                        subjectObjectIndex = 0;
                        subjectDeviceListSize = 0;
                        message.type = eYourDeviceAddressIs;
                    }
                }
                else
                {
                    interruptedSequence = true;
                    message.type = ePleaseBePatient;
                }
                break;
            }
            case eYourDeviceAddressIs:
            case eRequestAllData:
            case eRequestNameOfObject:
            case eRequestPathOfObject:
            case eRequestDataTypeOfObject:
            case eRequestObject:
            case eRequestListSize:
            case eEndInterrogation:
            case ePleaseBePatient:
                break;

            case eResponseObject:
            {
                if (subjectIsBeingInterrogated)
                {
                    DeviceData data = {message.text1->c_str(), message.text2->c_str(), message.text3->c_str()};
                    // PRINT("--___--___--___capturing");
                    // PRINT(message.text1->c_str());
                    // PRINT(message.text2->c_str());
                    // PRINT(message.text3->c_str());
                    tempDeviceData.push_back(data);
                    message.type = eNone;
                    sequenceMessageType = message.type;
                    subjectObjectIndex++;

                    if (subjectObjectIndex >= subjectDeviceListSize)
                    {
                        HandleReceivedDeviceData();
                    }
                }
                break;
            }
            case eResponseListSize:
            {
                PRINT("response list size");
                PRINT(message.value.Get());
                subjectDeviceListSize = message.value;
                message.type = eNone;
                break;
            }
            case eResponseNameOfObject:
            {
                if (subjectIsBeingInterrogated)
                {
                    subjectObjectName = message.text1;
                    message.type = eRequestPathOfObject;
                    sequenceMessageType = message.type;
                }
                else
                {
                    message.type = eNone;
                }
                break;
            }
            case eResponsePathOfObject:
            {
                if (subjectIsBeingInterrogated)
                {
                    subjectObjectPath = message.text1;
                    message.type = eRequestDataTypeOfObject;
                    sequenceMessageType = message.type;
                }
                break;
            }
            case eResponseDataTypeOfObject:
            {
                if (subjectIsBeingInterrogated)
                {
                    TypeRegistry::CreateObject(message.text1->c_str(), subjectDeviceIndex);
                    SetObjectName(subjectDeviceIndex, subjectObjectIndex, subjectObjectName.c_str());
                    SetObjectPath(subjectDeviceIndex, subjectObjectIndex, subjectObjectPath.c_str());

                    int localIndex = GetListSize(subjectDeviceIndex) - 1;
                    if (localIndex != subjectObjectIndex)
                    {
                        PRINT("Index error");
                        PRINT("subject Device Index");
                        PRINT(subjectDeviceIndex);
                        PRINT("local index");
                        PRINT(localIndex);
                        PRINT("subject object index");
                        PRINT(subjectObjectIndex);
                        while (1)
                        {
                        }
                    }
                    subjectObjectIndex++;
                }
                message.type = eNone;
                sequenceMessageType = message.type;
                break;
            }
        }
    }

    uint16_t GetOrCreateDeviceIndex(const std::string& macId)
    {
        auto it = macIdToDeviceIndex.find(macId);
        if (it != macIdToDeviceIndex.end())
        {
            // PRINT("existing device index found");
            // PRINT(it->second);
            return it->second;
        }
        else
        {
            // PRINT("create new device index");
            uint16_t newDeviceIndex = deviceIndexManager.leaseIndex(0);
            macIdToDeviceIndex[macId] = newDeviceIndex;
            // PRINT(newDeviceIndex);
            return newDeviceIndex;
        }
    }

    void HandleReceivedDeviceData()
    {
        bool hasChanges = false;
        bool listSizeChanged = tempDeviceData.size() != GetListSize(subjectDeviceIndex);
        if(listSizeChanged)
        {
            hasChanges = true;
        }
        else
        {
            for (size_t i = 0; i < tempDeviceData.size(); ++i)
            {
                // check object type
                std::string objectType = TypeRegistry::Demangle(GetObjectType(subjectDeviceIndex, i)->name());
                bool objectTypeChanged = tempDeviceData[i].dataType != objectType;
                std::string objectName = GetObjectName(subjectDeviceIndex, i);
                bool objectNameChanged = tempDeviceData[i].name != objectName;
                std::string objectPath = GetObjectPath(subjectDeviceIndex, i);
                bool objectPathChanged = tempDeviceData[i].path != objectPath;
                if (objectTypeChanged || objectNameChanged || objectPathChanged)
                {
                    hasChanges = true;
                    break;
                }
            }
        }

        if (hasChanges)
        {
            RemoveDevice(subjectDeviceIndex);
            for (size_t i = 0; i < tempDeviceData.size(); ++i)
            {
                CreateAndRegisterObject(tempDeviceData[i], subjectDeviceIndex, i);
            }
        }
        else
        {
            // PRINT("dont change a winning team");
        }

        tempDeviceData.clear();
    }

    void CreateAndRegisterObject(const DeviceData& data, uint16_t deviceIndex, uint16_t objectIndex)
    {
        TypeRegistry::CreateObject(data.dataType.c_str(), deviceIndex);
        SetObjectName(deviceIndex, objectIndex, data.name.c_str());
        std::string path = "device" + std::to_string(deviceIndex) + "\\" + data.path;
        SetObjectPath(deviceIndex, objectIndex, path.c_str());
    }

    std::unordered_map<std::string, uint16_t> macIdToDeviceIndex;
    std::vector<DeviceData> tempDeviceData;
    MESSAGE message{"TComm"};
    std::string subjectObjectName;
    std::string subjectObjectPath;
    ResourceIndexManager deviceIndexManager;
    
    bool subjectIsBeingInterrogated = false;
    uint16_t subjectDeviceIndex = 0;
    uint16_t subjectDeviceListSize = 0;
    uint16_t subjectObjectIndex = 0;
    bool interruptedSequence = false;
    MESSAGETYPE sequenceMessageType;
};
}
#endif // TCOMMSERVER_H