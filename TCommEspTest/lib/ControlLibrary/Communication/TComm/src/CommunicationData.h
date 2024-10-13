#ifndef COMMUNICATIONDATA_H
#define COMMUNICATIONDATA_H

namespace TComm {
    class CommunicationData
    {
    public:
        byte* buffer;
        uint16_t deviceIndex;
        uint16_t dataSize;
        uint16_t objectIndex;
    };
};

#endif //COMMUNICATIONDATA_H