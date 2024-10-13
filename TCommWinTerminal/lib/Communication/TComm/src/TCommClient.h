#ifndef TCOMMCLIENT_H
#define TCOMMCLIENT_H
// #include <iostream>
namespace TComm
{
class TCommClient : TCommBase
{
public:
    void Run()
    {
        communicator.Run();
        HandleOverhead();
    }
private:
    void HandleOverhead()
    {
        bool changeReceived = message.type.IsChangeReceived();
        switch(message.type)
        {
            case eNone:
            {
                if(deviceIndex == 0 && !awaitTurn)
                {
                    message.text1 = getMacAddress();
                    message.text1.SetChanged();
                    // PRINT("mac id sent");
                    // PRINT(message.text1->c_str());
                    message.type = eWhatsMyDeviceAddress;
                }
                if(deviceIsBeingInterrogated)
                {
                    ;
                    // PRINT("awaiting commands");
                }
                break;
            }
            case eWhatsMyDeviceAddress:
            {
                responseTimeout.Run();
                responseTimeout.Start();
                if(responseTimeout.Done())
                {
                    // PRINT("timeout");
                    responseTimeout.Stop();
                    message.type = eNone;
                }
                break;
            }
            case eYourDeviceAddressIs:
            {
                // PRINT("your dev id is");
                // PRINT(getMacAddress().c_str());
                // PRINT(message.text1->c_str());
                std::string text1 = message.text1;
                if(text1 == getMacAddress())
                {
                    // PRINT("being interrogated");
                    deviceIndex = message.value;
                    SetDeviceIndex(deviceIndex);
                    deviceIsBeingInterrogated = true;
                    // SetDeviceFilter(true, 0);
                    message.type = eNone; 
                }
                break;
            }
            case eRequestAllData:
            {
                if(deviceIsBeingInterrogated)
                {
                    communicator.SetSendAllData();
                    message.type = eNone;
                }
                break;
            }
            case eRequestDataTypeOfObject:
            {
                if(deviceIsBeingInterrogated)
                {
                    // get the typeinfo of object at index given in message->value
                    // take the name of that an demangle it so it can be recognized by the server
                    // and copy that to the response message->text
                    auto objectType = GetObjectType(deviceIndex, message.value);
                    if(objectType)
                    {
                        message.text1 = TypeRegistry::Demangle(objectType->name());    
                    }
                    else
                    {
                        message.text1 = std::string("wrong typeinfo");
                    }
                    message.type = eResponseDataTypeOfObject;
                }
                break;
            }
            case eResponseDataTypeOfObject:
            {

                break;
            }
            case eRequestNameOfObject:
            {
                if(deviceIsBeingInterrogated)
                {
                    message.text1 = GetObjectName(deviceIndex, message.value);
                    message.type = eResponseNameOfObject;
                }
                break;
            }
            case eResponseNameOfObject:
            {
                break;
            }
            case eRequestPathOfObject:
            {
                if(deviceIsBeingInterrogated)
                {
                    message.text1 = GetObjectPath(deviceIndex, message.value);
                    message.type = eResponsePathOfObject;
                }
                break;
            }
            case eResponsePathOfObject:
            {
                break;
            }
            case eRequestObject:
            {
                if(deviceIsBeingInterrogated)
                {        
                    message.text1 = GetObjectName(deviceIndex, message.value);
                    message.text2 = GetObjectPath(deviceIndex, message.value);
                    auto objectType = GetObjectType(deviceIndex, message.value);
                    if(objectType)
                    {
                        message.text3 = TypeRegistry::Demangle(objectType->name());    
                    }
                    else
                    {
                        message.text3 = std::string("wrong typeinfo");
                    }
                    message.type = eResponseObject;
                }
                break;
            }
            case eResponseObject:
            {
                break;
            }
            case eRequestListSize:
            {
                if(deviceIsBeingInterrogated)
                {
                    message.value = GetListSize(deviceIndex);
                    message.type = eResponseListSize;
                }
                break;
            }
            case eResponseListSize:
            {
                break;
            }
            case eEndInterrogation:
            {
                if(deviceIsBeingInterrogated)
                {
                    message.type = eNone;
                }
                deviceIsBeingInterrogated = false;
                // SetDeviceFilter(false, 0);
                awaitTurn = false;
                break;
            }
            case ePleaseBePatient:
            {
                if(!deviceIsBeingInterrogated)
                {
                    awaitTurn = true;
                    message.type = eNone;
                }
                break;
            }
        };
    };
    Timer responseTimeout{1000};
    MESSAGE message{"TComm"};
    uint16_t deviceIndex;
    bool deviceIsBeingInterrogated;
    bool awaitTurn;
};
};

using namespace TComm;


#endif // TCOMMCLIENT_H