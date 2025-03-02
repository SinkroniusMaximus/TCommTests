#include <ControlLibrary.h>
using namespace TComm;
using namespace GenericLibrary;
#include "Modules\CM\ControlModule.h"
#include "Modules\EM\EquipmentModule.h"
TCommClient client;
Entity root{nullptr,"root"};
SerialSubscriber serialSub;
WifiSocket wifiSocket;
// const char* ssid = "WifiNetwork";           // Replace with wifi network name
// const char* password = "WifiPassword";      // Replace with actual password
// const char* host = "ThisIsMyComputerName";  // Replace with your server 'computer name' to connect to
// const uint16_t port = 1234;
EquipmentModule equipmentModule{nullptr,"equipmentModule"};
SerialAdapter serialAdapter(&Serial);

void setup() {
  Serial.begin(230400);//115200);
  // wifiSocket.Connect(ssid, password, host, port);
  // serialSub.Xinit(&wifiSocket);
  serialSub.Xinit(&serialAdapter);
}

void loop() {
    clockBits.Run();
    client.Run();
    wifiSocket.loop();
    equipmentModule.Run();
}