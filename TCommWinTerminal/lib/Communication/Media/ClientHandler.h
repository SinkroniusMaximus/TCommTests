#ifndef CLIENTHANDLER_H
#define CLIENTHANDLER_H
#if defined(_WIN32) || defined(_WIN64)
    // windows only
    #include <winsock2.h>
    // #include <queue>
    // #include <iostream>
    // #include <functional> // Include for std::function
#elif defined(__linux__)
    #define INVALID_SOCKET -1
    #define SOCKET_ERROR -1
    #define SOCKET int
#endif
using namespace TComm;


class ClientHandler : public StreamInterface {
public:
    // Type definition for the callback function
    using CloseCallback = std::function<void(ClientHandler*)>;
    ClientHandler(SOCKET clientSocket, CloseCallback onClose)
        : ClientSocket(clientSocket), onClose(onClose) {}

    size_t write(const uint8_t* buf, size_t size) override {
        #if defined(_WIN32) || defined(_WIN64)
            int iResult = send(ClientSocket, reinterpret_cast<const char*>(buf), size, 0);
        #elif defined(__linux__)
            ssize_t iResult = ::send(ClientSocket, buf, size, 0); // Use send from <sys/socket.h>
        #endif
            if (iResult == SOCKET_ERROR) {
                ErrorResponse();
                return 0;
            }
            return iResult;
    }

    int available() override {
        if (ClientSocket == INVALID_SOCKET) {
            return 0;
        }
        int response;
        #if defined(_WIN32) || defined(_WIN64)
            u_long bytesAvailable;
            response = ioctlsocket(ClientSocket, FIONREAD, &bytesAvailable);
        #elif defined(__linux__)
            int bytesAvailable;
            response = ioctl(ClientSocket, FIONREAD, &bytesAvailable);
        #endif
        if(response < 0)
        {
            ErrorResponse();
            return 0;
        }
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
            #if defined(_WIN32) || defined(_WIN64)
                ssize_t iResult = static_cast<ssize_t>(recv(ClientSocket, recvbuf, sizeof(recvbuf), 0));
            #elif defined(__linux__)
                ssize_t iResult = ::recv(ClientSocket, recvbuf, sizeof(recvbuf), 0); // Use recv from <sys/socket.h>
            #endif
                if (iResult > 0) {
                    for (ssize_t i = 0; i < iResult; ++i) {
                        readBuffer.push(recvbuf[i]);
                    }
                    int byte = readBuffer.front();
                    readBuffer.pop();
                    return byte;
                } else if (iResult == 0) {
                    std::cout << "Connection closing...\n";
                    Close();
                } else {
                    ErrorResponse();
                }
        }
        return -1;  // No data available or error
    }

    void Close() {
        if (ClientSocket != INVALID_SOCKET) {
            #if defined(_WIN32) || defined(_WIN64)
                closesocket(ClientSocket);
            #elif defined(__linux__)
                ::close(ClientSocket);
            #endif
            ClientSocket = INVALID_SOCKET;
        }
    }

    bool IsClosed() const {
        return ClientSocket == INVALID_SOCKET;
    }

    void ErrorResponse()
    {
        #if defined(_WIN32) || defined(_WIN64)
            std::cerr << "Recv failed with error: " << WSAGetLastError() << std::endl;
            Close();
        #elif defined(__linux__)
            std::cerr << "Recv failed with error: " << strerror(errno) << std::endl;
            Close();
        #endif

    }

    ~ClientHandler() {
        PRINT("destructor");
        Close();
    }

private:
    SOCKET ClientSocket;
    std::queue<char> readBuffer;
    CloseCallback onClose; // Callback function to notify server on close
};

#endif // CLIENTHANDLER_H