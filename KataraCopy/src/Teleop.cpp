#include <Arduino.h>
#include <Teleop.h>


Teleop:: Teleop() {
    pinMode(13, OUTPUT);
}

void Teleop:: periodic() {
    digitalWrite(13, HIGH);   // turn the LED on (HIGH is the voltage level)
    
}