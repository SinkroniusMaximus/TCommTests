#ifndef STREAMINTERFACE_H
#define STREAMINTERFACE_H

namespace TComm {

class StreamInterface {
public:
    virtual ~StreamInterface() {}
    virtual size_t write(const uint8_t* buf, size_t size) = 0;
    virtual int available() = 0;
    virtual int read() = 0;
};
};

#endif // STREAMINTERFACE_H
