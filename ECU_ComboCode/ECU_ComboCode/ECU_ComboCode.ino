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
        delay(5000);
    }
    Serial.println("CAN BUS  Init OK!");

    //CAN Hardware filtering
    CAN.init_Mask(0, 0, 0x7fe);                         // there are 2 mask in mcp2515, you need to set both of them
    CAN.init_Mask(1, 0, 0x7fe);

    CAN.init_Filt(0, 0, 0x7E8);                         // there are 6 filter in mcp2515
    CAN.init_Filt(1, 0, 0x7E8);
    CAN.init_Filt(2, 0, 0x7E8);
    CAN.init_Filt(3, 0, 0x7E8);
    CAN.init_Filt(4, 0, 0x7E8);
    CAN.init_Filt(5, 0, 0x7E8);
}


void loop()
{
    unsigned char len = 0;
    unsigned char buf[8];

    if(CAN_MSGAVAIL == CAN.checkReceive())
    {
        CAN.readMsgBuf(&len, buf);

        unsigned int canId = CAN.getCanId();

        Serial.println("------------New CAN Frame Recieved-----------------");
        Serial.print("CANID: 0x");
        Serial.print(canId, HEX);
        Serial.print(" : DLC: ");
        Serial.print(len, DEC);
        Serial.print(" | DataBytes: ");

        for(int i = 0; i<len; i++)
        {
            Serial.print(buf[i], HEX);
            Serial.print("   ");            
        }
        Serial.println();
    }
}
