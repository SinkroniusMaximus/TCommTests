#include "lib\System\Common.h"
#include "lib\Communication\Media\UASubscriber.h"
#include "lib\Communication\Media\ClientHandler.h"
#include "lib\Communication\Media\WebSocketServer.h"
#include "lib\Communication\Media\SerialInterface.h"

TCommServer server;
opcua::Server uaserver;
UASubscriber uaSubscriber;
WebSocketServer webSocketServer;

Entity tCommVerse{nullptr,"tCommVerse"};
SerialInterface serialInterface{&tCommVerse, "serialInterface"};

int main()
{
    if (webSocketServer.begin()) {
        std::cout << "Server started and listening for connections" << std::endl;
    } else {
        std::cout << "Failed to start server" << std::endl;
    }
    uaserver.setApplicationName("TCommVerse");
    /// Set application URI, default: `urn:open62541.server.application`.
    uaserver.setApplicationUri("urn:TCommVerse.server.application");
    uaSubscriber.Xinit(&uaserver);
    while(1)
    {
        clockBits.Run();
        server.Run();
        serialInterface.Run();
        webSocketServer.Run();
    }
}