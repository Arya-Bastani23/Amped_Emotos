#ifndef BUTTON_h
#define BUTTON_h
    #include <Arduino.h>

class Button {
    private:
        bool is_high;
        int inPin;
        int reading; //the current reading of the input pin
        int lastButtonState; //the previous reading of the input pin
        long time; // the last time the output pin was toggled
        long debounceDelay; // the debounce time, increase if the output flickers
        unsigned long lastDebounceTime;
        int buttonState;

    public:
        //initialize the button
        Button(int digPin1);
        //return whether bike is on or not
        bool isHigh();
        //update the ignition state
        void update();
    };
    #endif