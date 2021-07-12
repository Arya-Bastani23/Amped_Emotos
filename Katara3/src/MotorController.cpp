#include <Arduino.h>
#include <MotorController.h>
#include <A_CAN.h>

MotorController::MotorController(A_CAN *canBus1) {
    canBus = canBus1;

}
/** This will be used in order to write SDO messages to the motor controller, 
 *  it is the template that specific messages such as torque communication use.
 */
void MotorController::SDOWrite(int numBytestoWrite, INT8U *data) {
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
    byte sndStat = canBus->sendMessage(canHeader, rtr, length, data);

    /** THIS PRINT STATEMENT SHOULD BE REMOVED EVENTUALLY!!!
     * WE JUST NEED IT NOW TO SEE IF THE BUS IS RUNNING CORRECTLY */
    if(sndStat == CAN_OK){
    Serial.println("Message Sent Successfully!");
    Serial.println(data[1]);

  } else {
    Serial.println("Error Sending Message...");
  }
 }

void MotorController::SDORead(INT8U *data) {
    INT32U canHeader;
    INT32U messageID = 0x600;
    INT32U nodeID = 0x2A;
    canHeader = messageID + nodeID;
    INT8U rtr = 0x0;
    INT8U length = 0x8;
    //Byte zero
    INT8U command = 0x2B;
    data[0] = command;
    canBus->sendMessage(canHeader, rtr, length, data);
}

void MotorController::SDORErrorResponse(){

}

//This will enable the motor/motor controller to be on an ON state
void MotorController::enableMotor() {
    //To enable the motor all we need to do is send a remote state command with the value 2
    sendRemStateCommand(2);
}

//This will set the motor/motor controller to an idle state
void MotorController::idleMotor() {
    //To enable the motor all we need to do is send a remote state command with the value 1
    sendRemStateCommand(1);
}

//This will disable the motor/motor controller to be on an OFF state
void MotorController::disableMotor() {
    //To enable the motor all we need to do is send a remote state command with the value 0
    sendRemStateCommand(0);
}

//The sends a remoteStateCommand
void MotorController::sendRemStateCommand(int command) {
    //For documented code look at the sendRemTorqueComm(int torquePercent) function
    INT32U remStateCommAddress = 493;
    INT8U *data = new INT8U[8];
    data[1] = calcLowerIndex(remStateCommAddress);
    data[2] = calcUpperIndex(remStateCommAddress);
    data[3] = calcSubIndex(remStateCommAddress);
    INT8U byteCommand = (INT8U) command;
    data[4] = byteCommand;
    SDOWrite(1, data);
    delete data;
}

//The sends a remoteThrottleCommand
void MotorController::sendRemThrottleCommand(int throttleVoltage) {
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



//The sends a remoteStateCommand
void MotorController::setControlCommandSource(int command) {
    //For documented code look at the sendRemTorqueComm(int torquePercent) function
    INT32U remStateCommAddress = 208;
    INT8U *data = new INT8U[8];
    data[1] = calcLowerIndex(remStateCommAddress);
    data[2] = calcUpperIndex(remStateCommAddress);
    data[3] = calcSubIndex(remStateCommAddress);
    INT8U byteCommand = (INT8U) command;
    data[4] = byteCommand;
    SDOWrite(1, data);
    delete data;
}

//This sends a message to set the new torque value as a percentage of the maximum possible torque
void MotorController::sendRemTorqueComm(int torquePercent) {
    // This is the address of the the remTorqueComm in the object dictionary
    INT32U remTorqueCommAddress = 494;
    //Create data object
    INT8U *data = new INT8U[8];
    //We now need to fill out the values in the data array
    //Byte 0 is decided, by the SDO message sender
    //Byte [1,2] are made up of the index of the object in the dictionary
    //The lower index goes firt then the upper index
    data[1] = calcLowerIndex(remTorqueCommAddress);
    data[2] = calcUpperIndex(remTorqueCommAddress);
    //The 3rd byte of data is the subindex of the object
    data[3] = calcSubIndex(remTorqueCommAddress);
    //Remote Torque Commands only take the new torque value so that will take bytes 4 & 5
    //First calculate the proper values
    //The scale is 40.96, so we need to multiply by that and round to get rid of decimal
    INT32U torque = round(torquePercent * 40.96);
    //Now we need to two individual bytes that make up the torque
    INT8U lowerTorque = torque >> 8;
    torque = torque << 24;
    INT8U upperTorque = torque >> 24;
    //Now we put the lowerTorque in byte 4, and upperTorque in byte 5
    data[4] = lowerTorque;
    data[5] = upperTorque;
    //Now that we have properly set up the data path we can send an SDO message with it;
    //Because the torque value takes up two bytes we fill that with two, and the second input with the data
    SDOWrite(2, data);
    delete data;
}

//This sets the maximum brake current based as a percentage of the maximum possible braking current
void MotorController::setRemMaxBrakeCurrent(int currentPercent) {
    //For documented code look at the sendRemTorqueComm(int torquePercent) function
    INT32U remMaxBrakeCurrentAddress = 492;
    INT8U *data = new INT8U[8];
    data[1] = calcLowerIndex(remMaxBrakeCurrentAddress);
    data[2] = calcUpperIndex(remMaxBrakeCurrentAddress);
    data[3] = calcSubIndex(remMaxBrakeCurrentAddress);
    //CHECK IF THE 40.96 is the corrent scale for setting the remote max braking current
    INT32U current = round(currentPercent * 40.96);
    INT8U lowerCurrent = current >> 8;
    current = current << 24;
    INT8U upperCurrent = current >> 24;
    data[4] = lowerCurrent;
    data[5] = upperCurrent;
    SDOWrite(2, data);
    delete data;
}

//This sets the maximum motoring current based as a percentage of the maximum possible motoring current
void MotorController::setRemMaxMotoringCurrent(int currentPercent) {
    //For documented code look at the sendRemTorqueComm(int torquePercent) function
    INT32U remMaxMotoringCurrentAddress = 492;
    INT8U *data = new INT8U[8];
    data[1] = calcLowerIndex(remMaxMotoringCurrentAddress);
    data[2] = calcUpperIndex(remMaxMotoringCurrentAddress);
    data[3] = calcSubIndex(remMaxMotoringCurrentAddress);
    //CHECK IF THE 40.96 is the corrent scale for setting the remote max braking current
    INT32U current = round(currentPercent * 40.96);
    INT8U lowerCurrent = current >> 8;
    current = current << 24;
    INT8U upperCurrent = current >> 24;
    data[4] = lowerCurrent;
    data[5] = upperCurrent;
    SDOWrite(2, data);
    delete data;
}




//-----------------PRIVATE HELPER FUNCTIONS-----------------



INT8U MotorController::calcUpperIndex(INT32U address) {
    //This equation was gained from the SDO documentation, and is the standard way to find the index
    //from the address of a command
    INT32U x = floor(address/64) + 8192;
    //Now this value should on;y be a max of 16 bits long and we just want the upper index which will
    //be in the second 8 bits so we need to bit shift right by 8
    INT8U output = x >> 8;
    //Now we only want to store the first byte and output that so we store the value in an INT8U
    return output;
}

INT8U MotorController::calcLowerIndex(INT32U address) {
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

INT8U MotorController::calcSubIndex(INT32U address) {
    INT8U output = (address%64) + 1;
    return output;
}


