#ifndef GENERICLIBRARY_COMMON_H
#define GENERICLIBRARY_COMMON_H
    #ifdef ARDUINO
        #include <Arduino.h>
    #endif

    #include <memory>
    #include <string>
    #include "Timer.h"
    #include "ClockBits.h"
    #include "Entity.h"
    #include "ResourceIndexManager.h"
    #include "..\Communication\TComm\TComm.h"
    #include "AbstractConnector.h"
    #include "BaseConnector.h"
    #include "Connector.h"

    namespace GenericLibrary
    {
        #ifdef ARDUINO
            #define PRINT(formattedString)  \
            Serial.println(formattedString);
        #elif defined(_WIN32) || defined(_WIN64) || defined(__linux__)
            typedef std::string String;
            #define PRINT(formattedString)  \
                std::cout << formattedString << std::endl; \
                std::cout.flush();
        #endif
        #define CTRENTITY(className) className(Entity* parent, const std::string& name) : Entity(parent, name)
        #define INSTENTITY(name) name{this, #name};

        // Common macro for defining variables inside TComm
        #define TCOMM_VAR(type, name, ...) TComm::type name{#name, [this]() { return GetPath(); }, ##__VA_ARGS__}

        // Specific macros using the common TCOMM_VAR macro
        #define XBOOL(name, ...) TCOMM_VAR(Xbool, name, ##__VA_ARGS__)
        #define XFLOAT(name, ...) TCOMM_VAR(Xfloat, name, ##__VA_ARGS__)
        #define XINT(name, ...) TCOMM_VAR(Xint, name, ##__VA_ARGS__)
        #define XINT32(name, ...) TCOMM_VAR(Xint32, name, ##__VA_ARGS__)
        #define XUINT8(name, ...) TCOMM_VAR(Xuint8, name, ##__VA_ARGS__)
        #define XUINT16(name, ...) TCOMM_VAR(Xuint16, name, ##__VA_ARGS__)
        #define XSTRING(name, ...) TCOMM_VAR(Xstring, name, ##__VA_ARGS__)
        #define XTEMPLATE(name, ...) TComm::TemplateCommunicationObject<T> name{#name, [this]() { return GetPath(); }, ##__VA_ARGS__}
    }
#endif //GENERICLIBRARY_COMMON_H