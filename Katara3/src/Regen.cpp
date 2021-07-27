#include <Arduino.h>
#include <Regen.h>

Regen::Regen(int throttleADCPin1, int regenADCPin1, long maxPotVoltage1, 
                int maxKPH1, Throttle* throttle1, MotorController* mc1) {
    throttle = throttle1;
    
    }


void Regen::updateSpeed() {
    //update the breaking
}

void Regen::updateMCSpeed() {

}

void Regen::update() {
    //Check if the throttle is being pushed, because if it is we will subract the values of regen from the 
    //Throttle instead of actually breaking
    if (throttle->getOutputSpeed() > 0) {
        return;
    }
    //Otherwise regeneratively brake normally
    
}