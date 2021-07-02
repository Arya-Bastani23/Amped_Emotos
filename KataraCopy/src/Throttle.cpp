#include <Arduino.h>
#include <Throttle.h>
#include <A_CAN.h>

//Throttle Constructor
Throttle::Throttle(int CAN_ID1, int ADCPin1, long potVoltage1, 
                   int maxKPH1, int CSIpin1, A_CAN* canBus1) {
    CAN_ID = CAN_ID1;
    ADCPin = ADCPin1;
    CSIPin = CSIpin1;
    potVoltage = potVoltage1;
    maxKPH = maxKPH1;
    canBus = canBus1;
    
}

void Throttle::updateSpeed() {
    //Get the analogue reading
    int sensorValue = analogRead(ADCPin);
    // Convert the analog reading (which goes from 0 - 1023) to a voltage (0 - 5V):
    long voltage = sensorValue * (potVoltage / 1023.0);
    // Find the percent of 5V the voltage is and we will multiply that percentage by
        // our max speed to find the speed the user wants to go 
    long ratio = voltage / potVoltage;
    //multiply the ratio by our maxKPH
    long newSpeed = ratio*maxKPH;
    //update the speed
    outputSpeed = newSpeed;
}

void Throttle::messageMotorController() {
    
}



void Throttle::update() {
    updateSpeed();
    messageMotorController();
}

