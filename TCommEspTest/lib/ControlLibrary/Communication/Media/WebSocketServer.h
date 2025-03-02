#ifndef WEBSOCKETSERVER_H
#define WEBSOCKETSERVER_H

#if defined(_WIN32) || defined(_WIN64)
    #include <winsock2.h>
    #include <ws2tcpip.h>
    #pragma comment (lib, "Ws2_32.lib")
    #define CLOSE_SOCKET closesocket
    #define GET_LAST_ERROR WSAGetLastError()
#elif defined(__linux__)
    #define INVALID_SOCKET -1
    #define SOCKET_ERROR -1
    #define SOCKET int
    #define CLOSE_SOCKET ::close
    #define GET_LAST_ERROR errno
#endif

#define DEFAULT_PORT "1234"

using namespace TComm;

class WebSocketServer {
public:
    WebSocketServer() : ListenSocket(INVALID_SOCKET) {
        memset(&hints, 0, sizeof(hints));
        hints.ai_family = AF_INET;
        hints.ai_socktype = SOCK_STREAM;
        hints.ai_protocol = IPPROTO_TCP;
        hints.ai_flags = AI_PASSIVE;
    }

    bool begin() {
        #if defined(_WIN32) || defined(_WIN64)
            if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
                std::cerr << "WSAStartup failed\n";
                return false;
            }
        #endif

        if (getaddrinfo(nullptr, DEFAULT_PORT, &hints, &result) != 0) {
            std::cerr << "getaddrinfo failed\n";
            cleanup();
            return false;
        }

        ListenSocket = socket(result->ai_family, result->ai_socktype, result->ai_protocol);
        if (ListenSocket == INVALID_SOCKET) {
            std::cerr << "Socket creation failed\n";
            cleanup();
            return false;
        }

        if (bind(ListenSocket, result->ai_addr, result->ai_addrlen) == SOCKET_ERROR) {
            std::cerr << "Bind failed\n";
            cleanup();
            return false;
        }

        if (listen(ListenSocket, SOMAXCONN) == SOCKET_ERROR) {
            std::cerr << "Listen failed\n";
            cleanup();
            return false;
        }

        return true;
    }

    void handleClients() {
        fd_set readfds;
        FD_ZERO(&readfds);
        FD_SET(ListenSocket, &readfds);
        timeval timeout = { 0, 0 };

        int nfds = ListenSocket + 1; // For Linux compatibility
        if (select(nfds, &readfds, nullptr, nullptr, &timeout) > 0) {
            if (FD_ISSET(ListenSocket, &readfds)) {
                SOCKET ClientSocket = accept(ListenSocket, nullptr, nullptr);
                if (ClientSocket != INVALID_SOCKET) {
                    setNonBlocking(ClientSocket);
                    auto* handler = new ClientHandler(ClientSocket, [this](ClientHandler* h) { RemoveClient(h); });
                    clientHandlers.push_back(handler);

                    auto* subscriber = new TComm::SerialSubscriber();
                    subscriber->Xinit(handler);
                    subscribers.push_back(subscriber);
                } else {
                    std::cerr << "Accept failed\n";
                }
            }
        }
    }

    void Run() {
        handleClients();
        // Periodically check and remove closed client handlers
        CleanUpClients();
    }

    void Close() {
        for (auto* handler : clientHandlers) {
            handler->Close();
            delete handler;
        }
        clientHandlers.clear();

        for (auto* subscriber : subscribers) {
            delete subscriber;
        }
        subscribers.clear();

        if (ListenSocket != INVALID_SOCKET) {
            CLOSE_SOCKET(ListenSocket);
            ListenSocket = INVALID_SOCKET;
        }

        #if defined(_WIN32) || defined(_WIN64)
            WSACleanup();
        #endif
    }

    ~WebSocketServer() {
        Close();
    }

private:
    #if defined(_WIN32) || defined(_WIN64)
        WSADATA wsaData;
    #endif
    SOCKET ListenSocket;
    struct addrinfo* result = nullptr;
    struct addrinfo hints;

    std::vector<ClientHandler*> clientHandlers;
    std::vector<TComm::SerialSubscriber*> subscribers;

    void setNonBlocking(SOCKET socket) {
        #if defined(_WIN32) || defined(_WIN64)
            u_long mode = 1;
            ioctlsocket(socket, FIONBIO, &mode);
        #elif defined(__linux__)
            int flags = fcntl(socket, F_GETFL, 0);
            fcntl(socket, F_SETFL, flags | O_NONBLOCK);
        #endif
    }

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

    void cleanup() {
        if (result) {
            freeaddrinfo(result);
            result = nullptr;
        }
        if (ListenSocket != INVALID_SOCKET) {
            CLOSE_SOCKET(ListenSocket);
        }
        #if defined(_WIN32) || defined(_WIN64)
            WSACleanup();
        #endif
    }
};

#endif // WEBSOCKETSERVER_H