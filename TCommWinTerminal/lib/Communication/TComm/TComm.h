#ifndef TCOMM_H
#define TCOMM_H

#include "src/Common.h"
#include "src/StreamInterface.h"
#if defined(_WIN32) || defined(_WIN64)
  #include "src/WinSerial.h"
#elif defined(__linux__)
  #include "src/LinuxSerial.h"
#endif

#include "src/EnumUpdateInterval.h"
#include "src/CommunicationData.h"
#include "src/AbstractCommunicationObject.h"
#include "src/AbstractSubscriber.h"


#include "src/Communicator.h"

#include "src/BaseSubscriber.h"
#include "src/BaseCommunicationObject.h"
#include "src/TemplateCommunicationObject.h"
#include "src/DummyCommunicationObject.h"

#include "src/ByteSerializer.h"
#include "src/SerialSubscriber.h"

#include "src/TypeRegistry.h"

namespace TComm
{
  typedef TemplateCommunicationObject<int> Xint;
  typedef TemplateCommunicationObject<int32_t> Xint32;
  typedef TemplateCommunicationObject<uint8_t> Xuint8;
  typedef TemplateCommunicationObject<uint16_t> Xuint16;
  typedef TemplateCommunicationObject<float> Xfloat;
  typedef TemplateCommunicationObject<bool> Xbool;
  typedef TemplateCommunicationObject<std::string> Xstring;
};
#include "src/TCommBase.h"
#include "src/TCommClient.h"
#include "src/TCommServer.h"

#endif //TCOMM_H