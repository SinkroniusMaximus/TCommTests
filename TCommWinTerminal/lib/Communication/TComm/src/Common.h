#ifndef COMMON_H
#define COMMON_H
        #include <vector>
        #include <typeinfo>
        #include <cxxabi.h>
        #include <cstring>
        #include <unordered_map>
        #include <memory>
        #ifdef ARDUINO
            #include <Arduino.h>
            #include "esp_system.h"  // Include for ESP32-specific functions
        #elif defined(_WIN32) || defined(_WIN64)
        // Windows-specific includes
            #include <iostream>
            #include <winsock2.h>
            #define WINDOWS_LEAN_AND_MEAN
            #include <windows.h>
            #include <cstdint>
            #include <setupapi.h>
            #include <iphlpapi.h>
            #pragma comment(lib, "iphlpapi.lib")
            #pragma comment(lib, "ws2_32.lib")

            // C++ Standard Library includes
            #include <functional>
            #include <thread>
            #include <mutex>
            #include <future>
            #include <vector>
            #include <string>
            #include <map>
            #include <sstream>
        #elif defined(__linux__)
            #include <iostream>
            #include <dirent.h>
            #include <fcntl.h>
            #include <unistd.h>
            #include <termios.h>
        #endif
        
        #include "../../../System/ClockBits.h"
        #include "../../../System/Timer.h"
        #include "../../../System/ResourceIndexManager.h"
        namespace TComm
        {
            using GenericLibrary::Timer;
            using GenericLibrary::clockBits;
            #ifdef ARDUINO
                void PRINT(String formattedString) 
                {
                    Serial.println(formattedString);
                }
            #endif
            #if defined(_WIN32) || defined(_WIN64)
                typedef std::string String;
                 #define PRINT(formattedString)  \
                 std::cout << formattedString << std::endl;  \
                 std::cout.flush();

                // void PRINT(const std::string& formattedString) 
                // {
                //     std::cout << formattedString << std::endl;
                //     std::cout.flush();
                // }
            #endif
        }
#endif //COMMON_H