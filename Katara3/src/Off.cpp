#include <Arduino.h>
#include <Off.h>

Off::Off(MotorController *mc1) {
    mc = mc1;
    placeHolder = 1;
    pinMode(13, OUTPUT);

}

/** This function is called periodically when the bike is "off", runs all the functions
 * that are needed when the bike is in an "off" modes.
 */
void Off::periodic() {
    digitalWrite(13, LOW);    // turn the LED off by making the voltage LOW
    //mc->disbleMotor();
}


