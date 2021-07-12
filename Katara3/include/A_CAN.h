#ifndef A_CAN_h
#define A_CAN_h
    #include <Arduino.h>
    #include <mcp_can.h>

class A_CAN {
    private:
        //CS Pin for the SPI connection to the mpu2515
        int CSIPin;
        MCP_CAN* mcp;
        

    public:
        //initialize the Can bus
        A_CAN(int CSPin1);
        //Create and send a message to a node on the CAN bus
        byte sendMessage(INT32U id, INT8U ext, INT8U len, INT8U buf[8]);
        
};

#endif
