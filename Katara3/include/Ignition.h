#ifndef IGNITION_h
#define IGNITION_h
    #include <Arduino.h>
    #include <Button.h>

class Ignition {
    private:
        //state of bike
        bool bike_on;
        int inPin;
        Button *ignitionButton;

    public:
        //initialize the button
        Ignition(int digPin1);
        //return whether bike is on or not
        bool isBikeOn();
        //update the ignition state
        void update();
};

#endif
