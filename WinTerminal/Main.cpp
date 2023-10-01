#include "Data.h"
#include "../Lib/WinSerial.h"

Data dataSource;
HardwareSerial Serial;
SerialSubscriber serialSub;

void setup() 
{
    Serial.begin("COM3", 115200);
    serialSub.Xinit(&Serial);
}

void loop() 
{
    Communicator.Xchange();
}


int main()
{
    setup();
    while(1)
    {
        loop();
    }
}