#include <Arduino.h>
#include <A_CAN.h>
#include <mcp_can.h>

//Ignition Constructor
A_CAN::A_CAN(int digPin1) {
    mcp = new MCP_CAN(CSIPin);
    if(mcp->begin(MCP_ANY, CAN_500KBPS, MCP_16MHZ) == CAN_OK) {
        Serial.println("MCP2515 Initialized Successfully!");
    }
    else {
        Serial.println("Error Initializing MCP2515...");
    }
    mcp->setMode(MCP_NORMAL);
}
  

void A_CAN::sendMessage(INT32U id, INT8U ext, INT8U len, INT8U buf[8]) {
    mcp->sendMsgBuf(id, ext, len, buf);
}


