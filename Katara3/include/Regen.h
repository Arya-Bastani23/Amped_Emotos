#ifndef REGEN_h
#define REGEN_h
    #include <Arduino.h>
    #include <MotorController.h>
    #include <Throttle.h>

class Regen {
    long potentiometer_voltage;
    //long outputSpeed;
    //long outputTorque;
    //int throttleADCPin;
    Throttle* throttle;
    int regenADCPin;
    long potVoltage;
    //int maxKPH;
    MotorController* mc;
    //Constants
    //max torque is 86 Nm
    int maxTorque; 
    //5000 rpm
    int maxRPM; 

    public:
        Regen(int throttleADCPin1, int regenADCPin1, long maxPotVoltage1, 
                int maxKPH1, Throttle* throttle1, MotorController* mc1);
        //This will update all systems that rely on the regen
        //So it will take in the curret potentiometer values, do math
        //to figure out the motor output speed, then send the proper values
        //to the motor controller so it can move the bike 
        void update();
        // This will simple output the speed and is good practice so we 
        // we can't edit the outputSpeed outside of this class

        /**
        long getOutputSpeed() { return outputSpeed; }
        */
        // This will take the input from the ADC and update the outptuSpeed variable
        // Take into account ABS & Traction Control
        void updateSpeed();
        // Not sure how we're gonna implement this 
        void updateMCSpeed();
        
};

#endif