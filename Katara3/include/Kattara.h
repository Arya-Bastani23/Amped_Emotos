#ifndef KATTARA_h
#define KATTARA_h
    #include <Arduino.h>
    #include <Ignition.h>
    #include <Off.h>
    #include <Teleop.h>
    #include <A_CAN.h>
    #include <MotorController.h>
    #include <ILAS.h>

class Kattara {
    private:
        int IGNITION_PIN;
        int CAN_CS_PIN;
        int ILAS_TOP_LIM_PIN;
        int ILAS_BOT_LIM_PIN;
        int ILAS_PWM_PIN;
        int ILAS_UP_BUT_PIN;
        int ILAS_DOWN_BUT_PIN;


        Ignition *ignition;
        Teleop *teleop;
        Off *off;
        A_CAN *canBus;
        MotorController *mc;
        ILAS *ily;

    public:
        Kattara();
        void kattaraTestOne();
        void kattaraTestTwo();
        void periodic();
};

#endif