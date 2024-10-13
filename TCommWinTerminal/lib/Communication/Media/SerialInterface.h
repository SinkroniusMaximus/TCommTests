#ifndef SERIALINTERFACE_H
#define SERIALINTERFACE_H

using namespace TComm;

struct SerialDeviceInfo 
{
    std::string portName;
    std::string friendlyName;
    std::string manufacturer;
    std::string vid;
    std::string pid;
    std::string serialNumber;
    std::string locationInformation;
};
class SerialInterface : Entity
{
public:
    CTRENTITY(SerialInterface) {
        configuration.selectedPort = -1;  // Ensure no random connection starts
    };

    void Run()
    {
        if (clockBits.ClockEdge_1hz() && configuration.refresh)
        {
            PRINT("updating serial devices");

            // Properly delete Xstring objects and clear the vector
            for (Xstring* deviceName : data.portSelection.deviceNames)
            {
                delete deviceName;
            }
            data.portSelection.deviceNames.clear();

            std::vector<SerialDeviceInfo> serialDevicesInfo = listAvailableSerialDevices();
            for (SerialDeviceInfo serialDevice : serialDevicesInfo)
            {
                PRINT(serialDevice.portName);
                PRINT(serialDevice.friendlyName);
                data.portSelection.deviceNames.push_back(new Xstring(serialDevice.friendlyName.c_str(), data.portSelection.GetPath().c_str(), 0));
                *data.portSelection.deviceNames.back() = serialDevice.portName;
            }
            configuration.refresh = false;
        }

        if (configuration.selectedPort >= 0 && configuration.selectedPort < data.portSelection.deviceNames.size() && configuration.connect)
        {
            Xstring* selectedDeviceName = data.portSelection.deviceNames.at(configuration.selectedPort);
            HardwareSerial* serialDevice = new HardwareSerial();
            SerialSubscriber* subscriber = new SerialSubscriber();
            subscriber->Xinit(serialDevice);
            serialDevice->begin(*selectedDeviceName, 512000);
            serialDevices.push_back(serialDevice);
            subscribers.push_back(subscriber);
            configuration.connect = false;
        }
    }

private:
    std::vector<HardwareSerial*> serialDevices;
    std::vector<SerialSubscriber*> subscribers;
    std::mutex devicesMutex;

    struct Configuration : Entity
    {
        CTRENTITY(Configuration) {}
        XINT(selectedPort, 0);  // Use integer for selected port index
        XBOOL(refresh, 0);
        XBOOL(connect, 0);
    } INSTENTITY(configuration);

    struct Data : Entity
    {
        CTRENTITY(Data) {};
        struct PortSelection : Entity
        {
            CTRENTITY(PortSelection) {};
            std::vector<Xstring*> deviceNames;
        } INSTENTITY(portSelection);
    } INSTENTITY(data);

    void checkAndAddDevice(const SerialDeviceInfo& device, std::vector<SerialDeviceInfo>& devices) {
        HANDLE hSerial = CreateFileA(
            device.portName.c_str(),
            GENERIC_READ | GENERIC_WRITE,
            0,
            0,
            OPEN_EXISTING,
            FILE_ATTRIBUTE_NORMAL,
            0);

        if (hSerial != INVALID_HANDLE_VALUE) {
            std::lock_guard<std::mutex> lock(devicesMutex);
            devices.push_back(device);
            CloseHandle(hSerial);
        } else {
            std::cerr << "Unable to open serial port: " << device.portName << std::endl;
        }
    }
    std::vector<SerialDeviceInfo> listAvailableSerialDevices() {
        std::vector<SerialDeviceInfo> devices;
        std::vector<std::future<void>> futures;
        const char* keys[] = {"SYSTEM\\CurrentControlSet\\Enum\\USB", "SYSTEM\\CurrentControlSet\\Enum\\BTHENUM"};

        for (const char* baseKey : keys) {
            HKEY hKey;
            if (RegOpenKeyExA(HKEY_LOCAL_MACHINE, baseKey, 0, KEY_READ, &hKey) != ERROR_SUCCESS) {
                std::cerr << "Error opening registry key: " << baseKey << std::endl;
                continue;
            }

            DWORD dwIndex = 0;
            char achKey[256];
            DWORD cchKey = sizeof(achKey);

            while (RegEnumKeyExA(hKey, dwIndex, achKey, &cchKey, NULL, NULL, NULL, NULL) == ERROR_SUCCESS) {
                HKEY hDeviceKey;
                if (RegOpenKeyExA(hKey, achKey, 0, KEY_READ, &hDeviceKey) != ERROR_SUCCESS) {
                    std::cerr << "Error opening device key." << std::endl;
                    dwIndex++;
                    cchKey = sizeof(achKey);
                    continue;
                }

                // Enumerate the subkeys under each device key
                DWORD dwDeviceIndex = 0;
                char achDeviceSubKey[256];
                DWORD cchDeviceSubKey = sizeof(achDeviceSubKey);

                while (RegEnumKeyExA(hDeviceKey, dwDeviceIndex, achDeviceSubKey, &cchDeviceSubKey, NULL, NULL, NULL, NULL) == ERROR_SUCCESS) {
                    HKEY hSubKey;
                    if (RegOpenKeyExA(hDeviceKey, achDeviceSubKey, 0, KEY_READ, &hSubKey) != ERROR_SUCCESS) {
                        dwDeviceIndex++;
                        cchDeviceSubKey = sizeof(achDeviceSubKey);
                        continue;
                    }

                    SerialDeviceInfo device;
                    if (getPortName(hSubKey, device.portName)) {
                        getDeviceInfo(hSubKey, device);
                        // Using lambda to wrap the function call correctly
                        futures.push_back(std::async(std::launch::async, [this, device, &devices]() {
                            checkAndAddDevice(device, devices);
                        }));
                    }

                    RegCloseKey(hSubKey);
                    dwDeviceIndex++;
                    cchDeviceSubKey = sizeof(achDeviceSubKey);
                }

                RegCloseKey(hDeviceKey);
                dwIndex++;
                cchKey = sizeof(achKey);
            }

            RegCloseKey(hKey);
        }

        // Wait for all async tasks to complete
        for (auto& future : futures) {
            future.get();
        }

        return devices;
    }

    void printAvailableSerialDevices() 
    {
        std::vector<SerialDeviceInfo> devices = listAvailableSerialDevices();
        std::cout << "Available Serial Devices:\n";
        for (const auto& device : devices) 
        {
            std::cout << "Port Name: " << device.portName << std::endl;
            std::cout << "Friendly Name: " << device.friendlyName << std::endl;
            std::cout << "Manufacturer: " << device.manufacturer << std::endl;
            std::cout << "VID: " << device.vid << std::endl;
            std::cout << "PID: " << device.pid << std::endl;
            std::cout << "Serial Number: " << device.serialNumber << std::endl;
            std::cout << "Location Information: " << device.locationInformation << std::endl;
            std::cout << std::endl;
        }
    }
    
    bool getPortName(HKEY hKey, std::string& portName) 
    {
        HKEY hDeviceParametersKey;
        if (RegOpenKeyExA(hKey, "Device Parameters", 0, KEY_READ, &hDeviceParametersKey) == ERROR_SUCCESS) 
        {
            char buffer[256];
            DWORD bufferSize = sizeof(buffer);
            if (RegQueryValueExA(hDeviceParametersKey, "PortName", NULL, NULL, reinterpret_cast<LPBYTE>(buffer), &bufferSize) == ERROR_SUCCESS) 
            {
                portName = buffer;
                RegCloseKey(hDeviceParametersKey);
                return true;
            }
            RegCloseKey(hDeviceParametersKey);
        }
        return false;
    }

    void getDeviceInfo(HKEY hKey, SerialDeviceInfo& device) 
    {
        char buffer[256];
        DWORD bufferSize = sizeof(buffer);

        // Retrieve FriendlyName
        bufferSize = sizeof(buffer);
        if (RegQueryValueExA(hKey, "FriendlyName", NULL, NULL, reinterpret_cast<LPBYTE>(buffer), &bufferSize) == ERROR_SUCCESS) 
        {
            device.friendlyName = buffer;
        }

        // Retrieve Manufacturer
        bufferSize = sizeof(buffer);
        if (RegQueryValueExA(hKey, "Mfg", NULL, NULL, reinterpret_cast<LPBYTE>(buffer), &bufferSize) == ERROR_SUCCESS) 
        {
            device.manufacturer = buffer;
        }

        // Retrieve HardwareID for VID and PID
        bufferSize = sizeof(buffer);
        if (RegQueryValueExA(hKey, "HardwareID", NULL, NULL, reinterpret_cast<LPBYTE>(buffer), &bufferSize) == ERROR_SUCCESS) 
        {
            std::string hardwareId = buffer;
            size_t vidPos = hardwareId.find("VID_");
            size_t pidPos = hardwareId.find("PID_");
            if (vidPos != std::string::npos && pidPos != std::string::npos) 
            {
                device.vid = hardwareId.substr(vidPos + 4, 4);
                device.pid = hardwareId.substr(pidPos + 4, 4);
            }
        }

        // Retrieve SerialNumber
        bufferSize = sizeof(buffer);
        if (RegQueryValueExA(hKey, "SerialNumber", NULL, NULL, reinterpret_cast<LPBYTE>(buffer), &bufferSize) == ERROR_SUCCESS) 
        {
            device.serialNumber = buffer;
        }

        // Retrieve LocationInformation
        bufferSize = sizeof(buffer);
        if (RegQueryValueExA(hKey, "LocationInformation", NULL, NULL, reinterpret_cast<LPBYTE>(buffer), &bufferSize) == ERROR_SUCCESS) 
        {
            device.locationInformation = buffer;
        }
    }
};

#endif // SERIALINTERFACE_H