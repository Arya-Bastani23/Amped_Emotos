#ifndef OFF_h
#define OFF_h
    #include <Arduino.h>
    #include <MotorController.h>

class Off {
    private:
        int placeHolder;
        MotorController *mc;
    public:
        Off(MotorController *mc1);
        void periodic();
};

#endif

