
#if defined(_WIN32) || defined(_WIN64)
#ifndef WINSERIAL_H
#define WINSERIAL_H

namespace TComm
{
    class HardwareSerial : public StreamInterface
    {
    public:
        HardwareSerial() : serialHandle(INVALID_HANDLE_VALUE) {}
        ~HardwareSerial()
        {
            CloseHandle(serialHandle);
        }

        void begin(std::string comPort, DWORD baudrate)
        {
            begin(comPort, baudrate, 8, TWOSTOPBITS, NOPARITY);
        }

        void begin(std::string comPort, DWORD baudrate, BYTE byteSize, BYTE stopBits, BYTE parity)
        {
            CloseHandle(serialHandle);
            connected = false;
            std::cout << "opening serialport " << ("\\\\.\\" + comPort).c_str() << "\n";
            serialHandle = CreateFileA(("\\\\.\\" + comPort).c_str(), GENERIC_READ | GENERIC_WRITE, 0, 0, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);
            if (serialHandle == INVALID_HANDLE_VALUE)
            {
                if (GetLastError() == ERROR_FILE_NOT_FOUND)
                {
                    std::cout << "serial port not found.\n";
                }
                else
                {
                    std::cout << "any other error.\n";
                }
                return;
            }
            // Do some basic settings
            DCB serialParams = { 0 };
            serialParams.DCBlength = sizeof(serialParams);
            if (!GetCommState(serialHandle, &serialParams))
            {
                std::cout << "getcommstate handle error here\n";
            }
            serialParams.BaudRate = baudrate;
            serialParams.ByteSize = byteSize;
            serialParams.StopBits = stopBits;
            serialParams.Parity = parity;
            if (!SetCommState(serialHandle, &serialParams))
            {
                std::cout << "setcommstate handle error here\n";
            }
            // Set timeouts
            COMMTIMEOUTS timeout = { 0 };
            timeout.ReadIntervalTimeout = 5;
            timeout.ReadTotalTimeoutConstant = 5;
            timeout.ReadTotalTimeoutMultiplier = 5;
            timeout.WriteTotalTimeoutConstant = 5;
            timeout.WriteTotalTimeoutMultiplier = 1;
            if (!SetCommTimeouts(serialHandle, &timeout))
            {
                std::cout << "setcommtimeout handle error here\n";
            }
            std::cout << "setup comport done\n";
            connected = true;
        }

        int read()
        {
            return readBuffer[0];
        }

        size_t write(const uint8_t* buffer, size_t size)
        {
            if(!connected)
            {
                return (size_t)0;
            }
            if (!WriteFile(serialHandle, buffer, size, &writeSize, NULL))
            {   
                DWORD lastError = GetLastError();
                std::cout << "error nr " << lastError << "\n";
                if(lastError == 6)
                {
                    connected = false;
                }
                return (size_t)0;
            }
            return (size_t)writeSize;
        }

        int available()
        {
            if (connected && !ReadFile(serialHandle, readBuffer, sizeof(readBuffer), &readSize, NULL))
            {
                DWORD lastError = GetLastError();
                std::cout << "error nr " << lastError << "\n";
                if(lastError == 6)
                {
                    connected = false;
                }
            }
            return (int)readSize;
        }

    private:
        HANDLE serialHandle;
        byte readBuffer[1];
        DWORD readSize;
        DWORD writeSize;
        bool connected;

    };
};
#endif // WINSERIAL_H
#endif // _WIN32 | _WIN64