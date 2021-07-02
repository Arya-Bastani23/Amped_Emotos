#include <Arduino.h>
#include <Kattara.h>
#include <Off.h>
#include <Teleop.h>
#include <A_CAN.h>
#include <MotorController.h>

Kattara::Kattara() {
    IGNITION_PIN = 2;
    ignition = new Ignition(IGNITION_PIN);
    teleop = new Teleop();
    off = new Off();
}


void Kattara::periodic() {
    ignition->update();
    if (ignition->isBikeOn()) {
        teleop->periodic();
    } else {
        off->periodic();
    }
}

INT8U Kattara::kattaraTest() {
    INT32U input = 494;
    INT32U x = floor(input/64) + 8192;
    //Now this value should on;y be a max of 16 bits long and we just want the upper index which will
    //be in the second 8 bits so we need to bit shift right by 8
    
    //Now we only want to store the first byte and output that so we store the value in an INT8U
    INT8U output = x >> 8;
    return output; 
}