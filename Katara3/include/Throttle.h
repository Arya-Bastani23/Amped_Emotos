#ifndef THROTTLE_h
#define THROTTLE_h
    #include <Arduino.h>
    #include <MotorController.h>

class Throttle {
    private:
        long potentiometer_voltage;
        long outputSpeed;
        long outputTorque;
        int CAN_ID;
        int throttleADCPin;
        int regenADCPin;
        int CSIPin;
        long maxPotVoltage;
        int maxKPH;
        MotorController* mc;
        //Constants
        //max torque is 86 Nm
        int maxTorque; 
        //5000 rpm
        int maxRPM; 

        


    public:
        Throttle(int CAN_ID1, int throttleADCPin1, int regenADCPin1, long maxPotVoltage1, 
                int maxKPH1, int CSIpin1, MotorController* mc1);
        //This will update all systems that rely on the throttle
        //So it will take in the curret potentiometer values, do math
        //to figure out the motor output speed, then send the proper values
        //to the motor controller so it can move the bike 
        void update();
        // This will simple output the speed and is good practice so we 
        // we can't edit the outputSpeed outside of this class
        long getOutputSpeed() { return outputSpeed; }
        // This will take the input from the ADC and update the outptuSpeed variable
        // Take into account ABS & Traction Control
        void updateSpeed();
        // Not sure how we're gonna implement this 
        void updateMCSpeed();
        //update torque given the velocity of the bike
        void updateTorque();
        //outputs the counteracting speed that the regen lever is exerting on the bike
        long regenAntiSpeed();
        //returns if the Regen braking is being used
        bool isRegenOn();
};

#endif
