#ifndef KATTARA_h
#define KATTARA_h
    #include <Arduino.h>
    #include <Ignition.h>
    #include <Off.h>
    #include <Teleop.h>
    #include <A_CAN.h>
    #include <MotorController.h>

class Kattara {
    private:
        int IGNITION_PIN;
        Ignition *ignition;
        Teleop *teleop;
        Off *off;

    public:
        Kattara();
        INT8U kattaraTest();
        void periodic();
};

#endif