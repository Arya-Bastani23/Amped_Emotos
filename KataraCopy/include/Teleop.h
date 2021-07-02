#ifndef TELEOP_h
#define TELEOP_h
    #include <Arduino.h>

class Teleop {
    private:
        int fixed;
    
    public:
        Teleop();
        void periodic();
};


#endif
