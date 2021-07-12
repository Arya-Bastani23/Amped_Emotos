#ifndef THROTTLE_h
#define THROTTLE_h
    #include <Arduino.h>
    #include <A_CAN.h>

class Throttle {
    private:
        long potentiometer_voltage;
        long outputSpeed;
        int CAN_ID;
        int ADCPin;
        int CSIPin;
        long potVoltage;
        int maxKPH;
        A_CAN* canBus;

    public:
        Throttle(int CAN_ID1, int ADCPin1, long potVoltage1, 
                 int maxKPH1, int CSIpin1, A_CAN* canBus1);
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
        void messageMotorController();

};

#endif
