// CAN Send Example
//

#include <mcp_can.h>
#include <SPI.h>

MCP_CAN* mcp = new MCP_CAN(3);


INT32U addy = 495;


INT8U calcUpperIndex(INT32U address) {
    //This equation was gained from the SDO documentation, and is the standard way to find the index
    //from the address of a command
    INT32U x = floor(address/64) + 8192;
    //Now this value should on;y be a max of 16 bits long and we just want the upper index which will
    //be in the second 8 bits so we need to bit shift right by 8
    INT8U output = x >> 8;
    //Now we only want to store the first byte and output that so we store the value in an INT8U
    return output;
}

INT8U calcLowerIndex(INT32U address) {
    //This equation was gained from the SDO documentation, and is the standard way to find the index
    //from the address of a command
    INT32U x = floor(address/64) + 8192;
    //Now this value should on;y be a max of 16 bits long and we just want the upper index which will
    //be in the second 8 bits so we need to bit shift right by 8
    x = x << 24;
    INT8U output = x >> 24;
    //Now we only want to store the first byte and output that so we store the value in an INT8U
    return output;
}

INT8U calcSubIndex(INT32U address) {
    INT8U output = (address%64) + 1;
    return output;
}

void SDOWrite(int numBytestoWrite, INT8U *data) {
    //
    INT32U canHeader;
    INT32U messageID = 0x600;
    INT32U nodeID = 0x2A;
    canHeader = messageID + nodeID;
    INT8U rtr = 0x0;
    INT8U length = 0x8;
    //Byte zero
    //Command Code is dependant on how many bytes of information we are trying to send
    //The switch statement will assign the correct value to command based on if we are sending 1-4 bytes
    INT8U command;

    switch(numBytestoWrite) {
        case 1:
            command = 0x2F;
        case 2:
            command = 0x2B;
        case 3:
            command = 0x27;
        case 4:
            command = 0x23;
    }
    data[0] = command;
    byte sndStat = mcp->sendMsgBuf(canHeader, rtr, length, data);

    /** THIS PRINT STATEMENT SHOULD BE REMOVED EVENTUALLY!!!
     * WE JUST NEED IT NOW TO SEE IF THE BUS IS RUNNING CORRECTLY */
    if(sndStat == CAN_OK){
    Serial.println("Message Sent Successfully!");
    Serial.println(data[1]);

  } else {
    Serial.println("Error Sending Message...");
  }
 }

 void sendRemThrottleCommand(int throttleVoltage) {
    //For documented code look at the sendRemTorqueComm(int torquePercent) function
    INT32U remThrottleCommAddress = 495;
    INT8U *data = new INT8U[8];
    data[1] = calcLowerIndex(remThrottleCommAddress);
    data[2] = calcUpperIndex(remThrottleCommAddress);
    data[3] = calcSubIndex(remThrottleCommAddress);
    INT32U voltage = (INT32U) throttleVoltage;
    INT8U lowerVoltage = voltage >> 8;
    voltage = voltage << 24;
    INT8U upperVoltage = voltage >> 24;
    data[4] = lowerVoltage;
    data[5] = upperVoltage;
    SDOWrite(2, data);
    delete data;
}

void setup()
{
  Serial.begin(115200);

  // Initialize MCP2515 running at 16MHz with a baudrate of 500kb/s and the masks and filters disabled.
  if(mcp->begin(MCP_ANY, CAN_500KBPS, MCP_16MHZ) == CAN_OK) Serial.println("MCP2515 Initialized Successfully!");
  else Serial.println("Error Initializing MCP2515...");

  mcp->setMode(MCP_NORMAL);   // Change to normal mode to allow messages to be transmitted
}

byte data[8] = {0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07};

void loop()
{
  sendRemThrottleCommand(1);
}
/*********************************************************************************************************
  END FILE
*********************************************************************************************************/
