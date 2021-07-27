#include <Arduino.h>
#include <Throttle.h>
#include <MotorController.h>

//Throttle Constructor
Throttle::Throttle(int CAN_ID1, int throttleADCPin1, int regenADCPin1, long maxPotVoltage1, 
                   int maxKPH1, int CSIpin1, MotorController* mc1) {
    CAN_ID = CAN_ID1;
    throttleADCPin = throttleADCPin1;
    regenADCPin = regenADCPin1;
    CSIPin = CSIpin1;
    maxPotVoltage = maxPotVoltage1;
    maxKPH = maxKPH1;
    mc = mc1;
    maxTorque = 86; //max torque is 86 Nm
    maxRPM = 5000; //5000 rpm

}

void Throttle::updateSpeed() {
    //Get the analogue reading
    int sensorValue = analogRead(throttleADCPin);
    // Convert the analog reading (which goes from 0 - 1023) to a voltage (0 - 5V):
    long voltage = sensorValue * (maxPotVoltage / 1023.0);
    // Find the percent of 5V the voltage is and we will multiply that percentage by
    // our max speed to find the speed the user wants to go 
    long ratio = voltage / maxPotVoltage;
    //multiply the ratio by our maxKPH
    long newSpeed = ratio*maxKPH;
    //update the speed
    //If the regen is off then the new speed is the outputspeed, otherwise you need to 
    //subtract the regen speed
    if (isRegenOn()) {
        long bufferSpeed = newSpeed - regenAntiSpeed();
        //To make sure that we are only assigning zero or posisitve values to the outputSpeed
        //Add this if statement to set the speed to zero if the regen is larger than the throttle value
        if (bufferSpeed >= 0) {
            outputSpeed = bufferSpeed;
        } else {
            outputSpeed = 0;
        }
    } else {
        outputSpeed = newSpeed;
    }
}

long Throttle::regenAntiSpeed() {
    //Get the analogue reading
    int sensorValue = analogRead(regenADCPin);
    // Convert the analog reading (which goes from 0 - 1023) to a voltage (0 - 5V):
    long voltage = sensorValue * (maxPotVoltage / 1023.0);
    // Find the percent of 5V the voltage is and we will multiply that percentage by
    // our max speed to find the speed the user wants to go 
    long ratio = voltage / maxPotVoltage;
    //multiply the ratio by our maxKPH
    long regenSpeed = ratio*maxKPH;
    //update the speed
    return regenSpeed;
}

//return true if regen is on
bool Throttle::isRegenOn() {
    if (regenAntiSpeed() > 0) {
        return true;
    } else {
        return false;
    }
}

void Throttle::updateTorque() {
    //Constants
    long airDensity = 1.2;
    //coefficient of drag
    long cDrag = 0.8;
    //frontal area
    long area = 1.2;
    //coefficient of rolling resistance
    long cRoll = 0.02;
    //bike's mass
    long mass = 160;
    //gravity of the world
    long g = 9.8;
    //radius of wheel
    long radius = 0.2665;
    //velocity is calculated in m/s
    //Max RPM is 5,000
    //Torque is calculated in Newton-meters (Max is 86 Newton meters) 
    outputTorque = (((0.5)*(airDensity)*(cDrag)*(area)*(pow(outputSpeed,2))) + ((cRoll)*(mass)*(g)))*(radius);
}

void Throttle::updateMCSpeed() {
    //create the ratio's to send to the motor controller
    long rpmRatio = outputSpeed / maxKPH;
    long torqueRatio = outputTorque / maxTorque;
    mc->sendRemTorqueComm(rpmRatio);
    mc->sendRemTorqueComm(torqueRatio);
}

void Throttle::update() {
    updateSpeed();
    updateTorque();
    updateMCSpeed();
}

