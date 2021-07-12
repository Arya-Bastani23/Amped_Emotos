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
        int CAN_CS_PIN;

        Ignition *ignition;
        Teleop *teleop;
        Off *off;
        A_CAN *canBus;
        MotorController *mc;

    public:
        Kattara();
        void kattaraTestOne();
        void kattaraTestTwo();
        void periodic();
};

#endif