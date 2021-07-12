#include <Arduino.h>
#include <Teleop.h>
#include <A_CAN.h>
#include <MotorController.h>

Teleop:: Teleop(MotorController *mc1) {
    //pinMode(13, OUTPUT);
    mc = mc1;
}

/** This function is called periodically during operator control, runs all the functions
 * that are needed during user operated modes.
 */
void Teleop:: periodic() {
    digitalWrite(13, HIGH);   // turn the LED on (HIGH is the voltage level)
    mc->enableMotor();
}