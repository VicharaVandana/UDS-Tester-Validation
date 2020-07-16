#include <SPI.h>
#include "mcp_can.h"

const int spiCSPin = 10;
const int ledPin = 2;
boolean ledON = 1;

MCP_CAN CAN(spiCSPin);

/* Macros for the code */
#define SERIALBAUDRATE 9600

void setup()
{
    Serial.begin(SERIALBAUDRATE);
    

    while (CAN_OK != CAN.begin(CAN_500KBPS))
    {
        Serial.println("CAN BUS Init Failed");
        delay(100);
    }
    Serial.println("CAN BUS  Init OK!");
}


void loop()
{
    unsigned char len = 0;
    unsigned char buf[8];

    if(CAN_MSGAVAIL == CAN.checkReceive())
    {
        CAN.readMsgBuf(&len, buf);

        unsigned int canId = CAN.getCanId();

        Serial.println("------------New Frame Recieved-----------------");
        Serial.print("CANID: 0x");
        Serial.print(canId, HEX);
        Serial.print(" : DLC: ");
        Serial.print(len, DEC);
        Serial.print(" | DataBytes: ");

        for(int i = 0; i<len; i++)
        {
            Serial.print(buf[i], HEX);
            Serial.print("\t");            
        }
        Serial.println();
    }
}
