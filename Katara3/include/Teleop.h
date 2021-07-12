#ifndef TELEOP_h
#define TELEOP_h
    #include <Arduino.h>
    #include <MotorController.h>

class Teleop {
    private:
        int fixed;
        MotorController *mc;
    
    public:
        Teleop(MotorController *mc1);
        void periodic();
};


#endif
