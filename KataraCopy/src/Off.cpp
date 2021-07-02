#include <Arduino.h>
#include <Off.h>

Off::Off() {
    placeHolder = 1;
    pinMode(13, OUTPUT);

}

void Off::periodic() {
    digitalWrite(13, LOW);    // turn the LED off by making the voltage LOW
}


