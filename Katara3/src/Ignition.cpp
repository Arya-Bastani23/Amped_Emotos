#include <Arduino.h>
#include <Ignition.h>

//Ignition Constructor
Ignition::Ignition(int digPin1) {
    inPin = digPin1;
    bike_on = false; //the current state of whether the bike is on or off
    ignitionButton = new Button(inPin);
}

void Ignition::update() {
    // if the input just went from LOW and HIGH and we've waited long enough
    // to ignore any noise on the circuit, toggle the output pin and remember
    // the time
    ignitionButton->update();
    bike_on = ignitionButton->isHigh();
}

bool Ignition::isBikeOn() {
    return bike_on;
}


