#ifndef SERIALADAPTER_H
#define SERIALADAPTER_H

namespace TComm
{

#include <HardwareSerial.h>

class SerialAdapter : public StreamInterface {
public:
    SerialAdapter(HardwareSerial* serial) : serial(serial) {}

    size_t write(const uint8_t* buf, size_t size) override {
        return serial->write(buf, size);
    }

    int available() override {
        return serial->available();
    }

    int read() override {
        return serial->read();
    }

private:
    HardwareSerial* serial;
};
};
#endif // SERIALADAPTER_H