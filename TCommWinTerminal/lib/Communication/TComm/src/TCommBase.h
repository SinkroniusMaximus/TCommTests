#ifndef TCOMMBASE_H
#define TCOMMBASE_H

namespace TComm
{
    class TCommBase
    {
        protected:
            void SetDeviceIndex(uint16_t deviceIndex)
            {
                // use this method as client only
                communicator.UpdateDeviceIndex(1, deviceIndex);
            }
            void RemoveDevice(uint16_t deviceIndex)
            {
                communicator.RemoveDevice(deviceIndex);
            }

            const std::type_info* GetObjectType(uint16_t deviceIndex, uint16_t objectIndex)
            {
                return communicator.GetCommunicationObject(deviceIndex, objectIndex)->GetType();
            }
            std::string GetObjectName(uint16_t deviceIndex, uint16_t objectIndex)
            {
                return std::string(communicator.GetCommunicationObject(deviceIndex, objectIndex)->GetName());

            }
            std::string GetObjectPath(uint16_t deviceIndex, uint16_t objectIndex)
            {
                return std::string(communicator.GetCommunicationObject(deviceIndex, objectIndex)->GetPath());
            }
            void SetObjectName(uint16_t deviceIndex, uint16_t objectIndex, const char* name)
            {
                communicator.GetCommunicationObject(deviceIndex, objectIndex)->SetName(name);
            }
            void SetObjectPath(uint16_t deviceIndex, uint16_t objectIndex, const char* path)
            {
                communicator.GetCommunicationObject(deviceIndex, objectIndex)->SetPath(path);
            }

            const int GetListSize(uint16_t deviceIndex)
            {
                // PRINT("GetListSize");
                return communicator.GetObjectListSize(deviceIndex);
            }
            void RegisterDevice(uint16_t deviceIndex)
            {
                communicator.RegisterDevice(deviceIndex);
            }

            std::string getMacAddress() 
            {
                std::string macAddress;

                #ifdef ARDUINO
                    // ESP32 specific code to retrieve MAC address
                    uint8_t mac[6];
                    esp_read_mac(mac, ESP_MAC_WIFI_STA);
                    char macStr[18];
                    sprintf(macStr, "%02X:%02X:%02X:%02X:%02X:%02X", mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);
                    macAddress = macStr;
                #elif defined(_WIN32) || defined(_WIN64)
                    // Windows specific code to retrieve MAC address
                    IP_ADAPTER_INFO AdapterInfo[16];
                    DWORD dwBufLen = sizeof(AdapterInfo);
                    DWORD dwStatus = GetAdaptersInfo(AdapterInfo, &dwBufLen);
                    if (dwStatus != ERROR_SUCCESS) return "";

                    PIP_ADAPTER_INFO pAdapterInfo = AdapterInfo;
                    macAddress = std::to_string(pAdapterInfo->Address[0]);
                    for (int i = 1; i < pAdapterInfo->AddressLength; i++) {
                        macAddress += ":" + std::to_string(pAdapterInfo->Address[i]);
                    }
                #elif defined(__linux__)
                    // Linux specific code to retrieve MAC address
                    int sock = socket(AF_INET, SOCK_DGRAM, IPPROTO_IP);
                    if (sock < 0) return "";

                    struct ifreq ifr;
                    std::strcpy(ifr.ifr_name, "eth0"); // Change this to the appropriate network interface
                    if (ioctl(sock, SIOCGIFHWADDR, &ifr) < 0) {
                        close(sock);
                        return "";
                    }
                    close(sock);

                    uint8_t *mac = (uint8_t *)ifr.ifr_hwaddr.sa_data;
                    char macStr[18];
                    sprintf(macStr, "%02X:%02X:%02X:%02X:%02X:%02X", mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);
                    macAddress = macStr;
                #endif

                return macAddress;
            }

            enum MESSAGETYPE
            {
                eNone,                  // no action
                eWhatsMyDeviceAddress,  // value = 0, text = random ID; request from client
                eYourDeviceAddressIs,   // value = assigned device address, text = the same random ID as the client   
                eRequestAllData,        // avalanche all the content on the list, request from server
                eRequestNameOfObject,     // value = data index, text1 = ''; request from server
                eResponseNameOfObject,    // value = data index, text1 = name of data; response from client
                eRequestPathOfObject,     // value = data index, text1 = ''; request from server
                eResponsePathOfObject,    // value = data index, text1 = path of data; response from client
                eRequestDataTypeOfObject, // value = data index, text1 = ''; request from server
                eResponseDataTypeOfObject,    // value = data index, text1 = datatype; response from client
                eRequestObject,             // value = data index, text1 = ''; request a complete object
                eResponseObject,            // value = data index, text1 = name, text2 = path, text3 = datatype; response from client
                eRequestListSize,       // value = 0, text1 = ''; request from server
                eResponseListSize,       // value = data list size, text1 = '' response from client
                eEndInterrogation,       // notify client the interrogation is over
                ePleaseBePatient         // tell client to await its turn
            };
            public:
            struct MESSAGE
            {
                MESSAGE(std::string path) 
                {
                    std::string fullPath = path + "\\message\\";
                    type.SetPath(fullPath.c_str());
                    type.SetUpdateInterval(eOnChange);
                    value.SetPath(fullPath.c_str());
                    value.SetUpdateInterval(eOnChange);
                    text1.SetPath(fullPath.c_str());
                    text1.SetUpdateInterval(eOnChange);
                    text2.SetPath(fullPath.c_str());
                    text2.SetUpdateInterval(eOnChange);
                    text3.SetPath(fullPath.c_str());
                    text3.SetUpdateInterval(eOnChange);
                    text4.SetPath(fullPath.c_str());
                    text4.SetUpdateInterval(eOnChange);
                    text5.SetPath(fullPath.c_str());
                    text5.SetUpdateInterval(eOnChange);
                }
                Xint value{"value", 0};
                Xstring text1{"text1", 0};
                Xstring text2{"text2", 0};
                Xstring text3{"text3", 0};
                Xstring text4{"text4", 0};
                Xstring text5{"text5", 0};
                TemplateCommunicationObject<MESSAGETYPE> type{"type", 0};
            };

    };
};

#endif // TCOMMBASE_H