#ifndef WIFISOCKET_H
#define WIFISOCKET_H

#include <WiFi.h>
#include <lwip/sockets.h>
#include <vector>
#define WIFI_CLIENT_SELECT_TIMEOUT_US    (10000000) // (1000000)

namespace TComm {
class WifiSocket : public StreamInterface {
public:
    WifiSocket() : client(nullptr), busy(false), nextIndexToWrite(0) {}

    bool Connect(std::string ssid, std::string password, std::string host, uint16_t port) 
    {
        this->ssid = ssid;
        this->password = password;
        this->host = host;
        this->port = port;
        maintainConnection = true;
        return MaintainConnection(ssid, password, host, port);
    }

    bool MaintainConnection(std::string ssid, std::string password, std::string host, uint16_t port) {
        if(WiFi.status() != WL_CONNECTED)
        {
            if(!wifiConnectionTimeout.Started()) // on first call and every following 5 seconds
            {
                // PRINT("------______---___--__(re)connecting wifi");
                WiFi.begin(ssid.c_str(), password.c_str());
            }
            wifiConnectionTimeout.Loop(); // loop every 5 seconds
        }
        else
        {
            if(!wifiConnected)
            {
                // PRINT("------______---___--__connected to wifi");
                wifiConnectionTimeout.Stop();
                wifiConnected = true;
            }
            if(!client) // upon first connect or if client was removed, regenerate it
            {
                client = new WiFiClient();
            }
            if(client)
            {
                if(!client->connected()) // whenever a client exists but isnt connected, attempt to connect
                {
                    // PRINT("disconnected from server------______---___--__");
                    clientConnected = false;
                    client->stop();
                    return client->connect(host.c_str(), port);
                }
                if(!clientConnected)
                {
                    // PRINT("------______---___--__connected to server");
                    clientConnected = true;
                }
                return true; // is or already was connected to wifi and client
            }
        }
        return false;
    }

    size_t write(const uint8_t* buf, size_t size) {
        if(clientConnected)
        {
            buffer.insert(buffer.end(), buf, buf + size);
            return size;
        }
        return 0;
    }

    int available() {
        if (clientConnected &&client) {
            return client->available();
        }
        return 0;
    }

    int read() {
        if (clientConnected && client) {
            return client->read();
        }
        return -1;
    }

    void stop() {
        if (client) {
            client->stop();
            delete client;
            client = nullptr;
            buffer.clear();
            writeBuffer.clear();
            busy = false;
            nextIndexToWrite = 0;
            clientConnected = false;
        }
    }

    void loop() {
        if(maintainConnection)
        {
            MaintainConnection(ssid, password, host, port);
        }
        if(clientConnected)
        {
            if (!busy && !buffer.empty()) {
                // Move data from buffer to writeBuffer
                writeBuffer.insert(writeBuffer.end(), buffer.begin(), buffer.end());
                buffer.clear();
                flushBuffer();
            } else if (busy) {
                flushBuffer();
            }
        }
    }

    ~WifiSocket() {
        stop();
    }

private:
    WiFiClient* client;
    std::vector<uint8_t> buffer;
    std::vector<uint8_t> writeBuffer;
    bool busy;
    size_t nextIndexToWrite;
    GenericLibrary::Timer wifiConnectionTimeout{5000};
    bool wifiConnected;
    bool clientConnected;
    std::string ssid;
    std::string password;
    std::string host;
    uint16_t port;
    bool maintainConnection;
    enum WRITESTATE
    {
        selectFileDestriptor,
        sendData,

    } writeState;

    void flushBuffer() {
        if (!client || writeBuffer.empty()) return;
        busy = true;
        size_t bytesRemaining = writeBuffer.size() - nextIndexToWrite;
        const uint8_t* buf = writeBuffer.data() + nextIndexToWrite;

        int socketFileDescriptor = client->fd();
        fd_set set;
        struct timeval tv;
        FD_ZERO(&set);
        FD_SET(socketFileDescriptor, &set);
        tv.tv_sec = 0;
        tv.tv_usec = WIFI_CLIENT_SELECT_TIMEOUT_US;

        if (select(socketFileDescriptor + 1, NULL, &set, NULL, &tv) > 0) {
            if (FD_ISSET(socketFileDescriptor, &set)) {
                int res = send(socketFileDescriptor, (void*)buf, bytesRemaining, MSG_DONTWAIT);
                if (res > 0) {
                    nextIndexToWrite += res;
                    if (nextIndexToWrite >= writeBuffer.size()) {
                        writeBuffer.clear();
                        // PRINT("...                          CHECK!");
                        nextIndexToWrite = 0;
                        busy = false;
                        return;
                    }
                } else if (res < 0 && errno != EAGAIN) {
                    stop();
                    return;
                }
            }
        }

        // If we haven't sent all data, keep busy flag true for next loop iteration
        busy = true;
    }
};
}

#endif // WIFISOCKET_H