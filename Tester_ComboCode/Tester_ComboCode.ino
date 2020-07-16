#include <SPI.h>
#include <mcp_can.h>

//Function prototypes
char DecodeCharToNum(char inchr);
void ProcessNewInCmd();

//Structure declarations.
struct FrameCommandFormat_TAG
{
  boolean IsNewPDU;
  boolean FrameType;
  unsigned int CanID;
  unsigned int PduLength;
  unsigned char DataBytes[8];
}FrameCommand;


/* Macros for the code */
#define SERIALBAUDRATE 9600

#define FRAMETYPE_TX (boolean)1
#define FRAMETYPE_RX (boolean)0

#define TRUE (boolean)1
#define FALSE (boolean)0

const int spiCSPin = 10;  //CS pin chosen is pin 10 for SPI communication. 
int ledHIGH    = 1;
int ledLOW     = 0;

/* Global Variables Declarations*/
unsigned char InputCommand[100];
unsigned int InCmdLen;
boolean NewCommandRecieved;

MCP_CAN CAN(spiCSPin);

void setup()
{
    Serial.begin(SERIALBAUDRATE);

    while (CAN_OK != CAN.begin(CAN_500KBPS))
    {
        Serial.println("CAN BUS init Failed");
        delay(100);
    }
    Serial.println("CAN BUS Shield Init OK!");
}


    
void loop()
{   
  Serial.println("In loop");
  //Process the New Input command if any
  if (NewCommandRecieved == TRUE)
  {
    ProcessNewInCmd();
    CAN.sendMsgBuf(FrameCommand.CanID, 0, FrameCommand.PduLength, FrameCommand.DataBytes);
    NewCommandRecieved = FALSE;
  }
}

/*
  SerialEvent occurs whenever a new data comes in the hardware serial RX. This
  routine is run between each time loop() runs, so using delay inside loop can
  delay response. Multiple bytes of data may be available.
*/
void serialEvent()
{
  
  if (Serial.available())
  {     //Serial.println(millis());
        InCmdLen = Serial.readBytesUntil('\n', InputCommand, 100); //read Serial    
        NewCommandRecieved = TRUE;
        //Serial.println(millis());
     }    
     
}

void ProcessNewInCmd()
{
  unsigned char idx,DBidx;
  if (InputCommand[0] == '#'){
    /* Frame command format
    STarts with symbol and then CANID, PDU Length and Databytes seperated by colon (:)
    For example if Can Id is 0x07E0 and 6 databytes are there then the command will be like below (Note that inside quoation means ascii value
    '#' ':' 07 224 ':' 06 ':' 01 02 03 04 05 06 ';'*/
    Serial.println("New Frame Command is recieved.");
    
    FrameCommand.FrameType = FRAMETYPE_TX;
    FrameCommand.CanID = ((unsigned int)(DecodeCharToNum(InputCommand[1])) << 8) + ((DecodeCharToNum(InputCommand[2])) << 4) + (DecodeCharToNum(InputCommand[3]));
    FrameCommand.PduLength = ((unsigned int)(DecodeCharToNum(InputCommand[5])) << 8) + ((DecodeCharToNum(InputCommand[6])) << 4) + (DecodeCharToNum(InputCommand[7]));

    DBidx = 0;
    for (idx = 9; (InputCommand[idx] != ';'); idx+=2) 
    {
      FrameCommand.DataBytes[DBidx] = ((DecodeCharToNum(InputCommand[idx])) << 4) + (DecodeCharToNum(InputCommand[idx + 1]));
      DBidx++;
    }
    FrameCommand.IsNewPDU = TRUE;
    //UDSHandler.Trigger_TX_Frame();
    /*
    Serial.print("CANID is ");
    Serial.println(FrameCommand.CanID);

    Serial.print("DLC is ");
    Serial.println(FrameCommand.PduLength);

    Serial.println("Databytes are ");
    for (idx = 0; idx < FrameCommand.PduLength; idx++){
    Serial.print(FrameCommand.DataBytes[idx]);
    Serial.print("  ");
    }   */
    
  }
  
  else {
    Serial.println("New Command is Invalid Command");
  }
  //Serial.println(millis());
}

char DecodeCharToNum(char inchr)
{
  char outnum;
  if ((inchr >= '0') && (inchr <= '9'))
  {
    outnum = inchr - '0';
  }
  else if ((inchr >= 'A') && (inchr <= 'F'))
  {
    outnum = 10 + inchr - 'A';
  }
  else 
  {
    //The control should not reach this else conditio ever.
    outnum = 0xFF;
  }
  
  return(outnum);
}
