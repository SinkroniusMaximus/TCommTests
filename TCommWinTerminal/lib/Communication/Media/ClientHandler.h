#ifndef CLIENTHANDLER_H
#define CLIENTHANDLER_H

#include <winsock2.h>
#include <queue>
#include <iostream>
#include <functional> // Include for std::function

using namespace TComm;

class ClientHandler : public StreamInterface {
public:
    // Type definition for the callback function
    using CloseCallback = std::function<void(ClientHandler*)>;

    ClientHandler(SOCKET clientSocket, CloseCallback onClose)
        : ClientSocket(clientSocket), onClose(onClose) {}

    size_t write(const uint8_t* buf, size_t size) override {
        // PRINT("Here");
        int iResult = send(ClientSocket, reinterpret_cast<const char*>(buf), size, 0);
        if (iResult == SOCKET_ERROR) {
            std::cerr << "Send failed with error: " << WSAGetLastError() << std::endl;
            Close();
            return 0;
        }
        return iResult;
    }

    int available() override {
        if (ClientSocket == INVALID_SOCKET) {
            return 0;
        }
        u_long bytesAvailable;
        ioctlsocket(ClientSocket, FIONREAD, &bytesAvailable);
        return static_cast<int>(bytesAvailable + readBuffer.size());
    }

    int read() override {
        if (!readBuffer.empty()) {
            int byte = readBuffer.front();
            readBuffer.pop();
            return byte;
        }

        if (available() > 0) {
            char recvbuf[512];
            int iResult = recv(ClientSocket, recvbuf, sizeof(recvbuf), 0);
            if (iResult > 0) {
                for (int i = 0; i < iResult; ++i) {
                    readBuffer.push(recvbuf[i]);
                }
                int byte = readBuffer.front();
                readBuffer.pop();
                return byte;
            } else if (iResult == 0) {
                std::cout << "Connection closing...\n";
                Close();
            } else {
                std::cerr << "Recv failed with error: " << WSAGetLastError() << std::endl;
                Close();
            }
        }
        return -1;  // No data available or error
    }

    void Close() {
        if (ClientSocket != INVALID_SOCKET) {
            closesocket(ClientSocket);
            ClientSocket = INVALID_SOCKET;
        }
    }

    bool IsClosed() const {
        return ClientSocket == INVALID_SOCKET;
    }

    ~ClientHandler() {
        PRINT("destructor");
        if (ClientSocket != INVALID_SOCKET) {
            closesocket(ClientSocket);
            ClientSocket = INVALID_SOCKET;
        }
    }

private:
    SOCKET ClientSocket;
    std::queue<char> readBuffer;
    CloseCallback onClose; // Callback function to notify server on close
};

#endif // CLIENTHANDLER_H