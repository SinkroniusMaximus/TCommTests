// #ifndef WEBSOCKETSERVER_H
// #define WEBSOCKETSERVER_H

// #include <winsock2.h>
// #include <ws2tcpip.h>
// #include <vector>
// #include <iostream>

// // Link with ws2_32.lib
// #pragma comment (lib, "Ws2_32.lib")

// #define DEFAULT_PORT "1234"
// using namespace TComm;
// class WebSocketServer {
// public:
//     WebSocketServer() : ListenSocket(INVALID_SOCKET) {
//         ZeroMemory(&hints, sizeof(hints));
//         hints.ai_family = AF_INET;
//         hints.ai_socktype = SOCK_STREAM;
//         hints.ai_protocol = IPPROTO_TCP;
//         hints.ai_flags = AI_PASSIVE;
//     }

//     bool begin() {
//         int iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
//         if (iResult != 0) {
//             std::cerr << "WSAStartup failed with error: " << iResult << std::endl;
//             return false;
//         }

//         iResult = getaddrinfo(NULL, DEFAULT_PORT, &hints, &result);
//         if (iResult != 0) {
//             std::cerr << "getaddrinfo failed with error: " << iResult << std::endl;
//             WSACleanup();
//             return false;
//         }

//         ListenSocket = socket(result->ai_family, result->ai_socktype, result->ai_protocol);
//         if (ListenSocket == INVALID_SOCKET) {
//             std::cerr << "Socket creation failed with error: " << WSAGetLastError() << std::endl;
//             freeaddrinfo(result);
//             WSACleanup();
//             return false;
//         }

//         iResult = bind(ListenSocket, result->ai_addr, (int)result->ai_addrlen);
//         if (iResult == SOCKET_ERROR) {
//             std::cerr << "Bind failed with error: " << WSAGetLastError() << std::endl;
//             freeaddrinfo(result);
//             closesocket(ListenSocket);
//             WSACleanup();
//             return false;
//         }

//         freeaddrinfo(result);

//         iResult = listen(ListenSocket, SOMAXCONN);
//         if (iResult == SOCKET_ERROR) {
//             std::cerr << "Listen failed with error: " << WSAGetLastError() << std::endl;
//             closesocket(ListenSocket);
//             WSACleanup();
//             return false;
//         }

//         return true;
//     }

//     void handleClients() {
//         // Accept new client connections
//         fd_set readfds;
//         FD_ZERO(&readfds);
//         FD_SET(ListenSocket, &readfds);
//         timeval timeout = { 0, 0 }; // non-blocking

//         int iResult = select(0, &readfds, NULL, NULL, &timeout);
//         if (iResult == SOCKET_ERROR) {
//             std::cerr << "Select failed with error: " << WSAGetLastError() << std::endl;
//             return;
//         }

//         if (FD_ISSET(ListenSocket, &readfds)) {
//             SOCKET ClientSocket = accept(ListenSocket, NULL, NULL);
//             if (ClientSocket == INVALID_SOCKET) {
//                 std::cerr << "Accept failed with error: " << WSAGetLastError() << std::endl;
//             } else {
//                 int flag = 1;
//                 setsockopt(ClientSocket, IPPROTO_TCP, TCP_NODELAY, (char *)&flag, sizeof(int));
//                 int bufferSize = 65536; // or any size that you find appropriate
//                 setsockopt(ClientSocket, SOL_SOCKET, SO_RCVBUF, (char *)&bufferSize, sizeof(bufferSize));
//                 setsockopt(ClientSocket, SOL_SOCKET, SO_SNDBUF, (char *)&bufferSize, sizeof(bufferSize));
//                 u_long mode = 1;
//                 ioctlsocket(ClientSocket, FIONBIO, &mode);
//                 ClientHandler* handler = new ClientHandler(ClientSocket);
//                 clientHandlers.push_back(handler);

//                 // Create and initialize a SerialSubscriber for this client
//                 TComm::SerialSubscriber* subscriber = new TComm::SerialSubscriber();
//                 subscriber->Xinit(handler);
//                 subscribers.push_back(subscriber);
//             }
//         }
//     }

//     void Run()
//     {
//         handleClients();
//     }

//     void close() {
//         for (ClientHandler* handler : clientHandlers) {
//             handler->Close();
//             delete handler;
//         }
//         clientHandlers.clear();

//         for (TComm::SerialSubscriber* subscriber : subscribers) {
//             delete subscriber;
//         }
//         subscribers.clear();

//         if (ListenSocket != INVALID_SOCKET) {
//             closesocket(ListenSocket);
//             ListenSocket = INVALID_SOCKET;
//         }

//         WSACleanup();
//     }

//     ~WebSocketServer() {
//         close();
//     }

// private:
//     WSADATA wsaData;
//     SOCKET ListenSocket;
//     std::vector<ClientHandler*> clientHandlers;
//     std::vector<TComm::SerialSubscriber*> subscribers;
//     struct addrinfo* result;
//     struct addrinfo hints;
// };

// #endif // WEBSOCKETSERVER_H

#ifndef WEBSOCKETSERVER_H
#define WEBSOCKETSERVER_H

#include <winsock2.h>
#include <ws2tcpip.h>
#include <vector>
#include <iostream>
#include <algorithm> // Include for std::remove_if

// Link with ws2_32.lib
#pragma comment (lib, "Ws2_32.lib")

#define DEFAULT_PORT "1234"
using namespace TComm;

class WebSocketServer {
public:
    WebSocketServer() : ListenSocket(INVALID_SOCKET) {
        ZeroMemory(&hints, sizeof(hints));
        hints.ai_family = AF_INET;
        hints.ai_socktype = SOCK_STREAM;
        hints.ai_protocol = IPPROTO_TCP;
        hints.ai_flags = AI_PASSIVE;
    }

    bool begin() {
        int iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
        if (iResult != 0) {
            std::cerr << "WSAStartup failed with error: " << iResult << std::endl;
            return false;
        }

        iResult = getaddrinfo(NULL, DEFAULT_PORT, &hints, &result);
        if (iResult != 0) {
            std::cerr << "getaddrinfo failed with error: " << iResult << std::endl;
            WSACleanup();
            return false;
        }

        ListenSocket = socket(result->ai_family, result->ai_socktype, result->ai_protocol);
        if (ListenSocket == INVALID_SOCKET) {
            std::cerr << "Socket creation failed with error: " << WSAGetLastError() << std::endl;
            freeaddrinfo(result);
            WSACleanup();
            return false;
        }

        iResult = bind(ListenSocket, result->ai_addr, (int)result->ai_addrlen);
        if (iResult == SOCKET_ERROR) {
            std::cerr << "Bind failed with error: " << WSAGetLastError() << std::endl;
            freeaddrinfo(result);
            closesocket(ListenSocket);
            WSACleanup();
            return false;
        }

        freeaddrinfo(result);

        iResult = listen(ListenSocket, SOMAXCONN);
        if (iResult == SOCKET_ERROR) {
            std::cerr << "Listen failed with error: " << WSAGetLastError() << std::endl;
            closesocket(ListenSocket);
            WSACleanup();
            return false;
        }

        return true;
    }

    void handleClients() {
        // Accept new client connections
        fd_set readfds;
        FD_ZERO(&readfds);
        FD_SET(ListenSocket, &readfds);
        timeval timeout = { 0, 0 }; // non-blocking

        int iResult = select(0, &readfds, NULL, NULL, &timeout);
        if (iResult == SOCKET_ERROR) {
            std::cerr << "Select failed with error: " << WSAGetLastError() << std::endl;
            return;
        }

        if (FD_ISSET(ListenSocket, &readfds)) {
            SOCKET ClientSocket = accept(ListenSocket, NULL, NULL);
            if (ClientSocket == INVALID_SOCKET) {
                std::cerr << "Accept failed with error: " << WSAGetLastError() << std::endl;
            } else {
                int flag = 1;
                setsockopt(ClientSocket, IPPROTO_TCP, TCP_NODELAY, (char *)&flag, sizeof(int));
                int bufferSize = 65536; // or any size that you find appropriate
                setsockopt(ClientSocket, SOL_SOCKET, SO_RCVBUF, (char *)&bufferSize, sizeof(bufferSize));
                setsockopt(ClientSocket, SOL_SOCKET, SO_SNDBUF, (char *)&bufferSize, sizeof(bufferSize));
                u_long mode = 1;
                ioctlsocket(ClientSocket, FIONBIO, &mode);
                ClientHandler* handler = new ClientHandler(ClientSocket, [this](ClientHandler* h) { this->RemoveClient(h); });
                clientHandlers.push_back(handler);

                // Create and initialize a SerialSubscriber for this client
                TComm::SerialSubscriber* subscriber = new TComm::SerialSubscriber();
                subscriber->Xinit(handler);
                subscribers.push_back(subscriber);
            }
        }
    }

    void Run() {
        handleClients();
        // Periodically check and remove closed client handlers
        CleanUpClients();
    }

    void Close() {
        for (ClientHandler* handler : clientHandlers) {
            handler->Close();
            delete handler;
        }
        clientHandlers.clear();

        for (TComm::SerialSubscriber* subscriber : subscribers) {
            delete subscriber;
        }
        subscribers.clear();

        if (ListenSocket != INVALID_SOCKET) {
            closesocket(ListenSocket);
            ListenSocket = INVALID_SOCKET;
        }

        WSACleanup();
    }

    ~WebSocketServer() {
        Close();
    }

private:
    WSADATA wsaData;
    SOCKET ListenSocket;
    std::vector<ClientHandler*> clientHandlers;
    std::vector<TComm::SerialSubscriber*> subscribers;
    struct addrinfo* result;
    struct addrinfo hints;

    // Function to remove a client handler and its corresponding subscriber
    void RemoveClient(ClientHandler* handler) {
        PRINT("remove client");
        // Remove the corresponding subscriber
        auto itSub = std::remove_if(subscribers.begin(), subscribers.end(), [handler](TComm::SerialSubscriber* subscriber) {
            return subscriber->GetPort() == handler;
        });
        if (itSub != subscribers.end()) {
            delete *itSub;
            subscribers.erase(itSub);
            PRINT("------------------------------- removed subscriber");
        }
        else
        {
            PRINT("_________________________________did not remove subscriber");
        }
        // Remove the client handler
        auto it = std::remove(clientHandlers.begin(), clientHandlers.end(), handler);
        if (it != clientHandlers.end()) {
            delete *it;
            clientHandlers.erase(it);
        }
    }

    // Function to periodically clean up closed clients
    void CleanUpClients() {
        clientHandlers.erase(
            std::remove_if(clientHandlers.begin(), clientHandlers.end(),
                           [this](ClientHandler* handler) {
                                if (handler->IsClosed()) {
                                    this->subscribers.erase(
                                    std::remove_if(this->subscribers.begin(), this->subscribers.end(),
                                                    [handler](TComm::SerialSubscriber* subscriber) {
                                                        if (subscriber->GetPort() == handler) {
                                                            delete subscriber;
                                                            return true;
                                                        }
                                                        return false;
                                                    }),
                                    this->subscribers.end());
                                    delete handler;
                                    return true;
                                }
                               return false;
                           }),
            clientHandlers.end());
    }
};

#endif // WEBSOCKETSERVER_H
