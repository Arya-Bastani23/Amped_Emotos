#include <Arduino.h>
#include <Button.h>

Button::Button(int digPin1) {
    inPin = digPin1;
    is_high = false; //the current state of whether the bike is on or off
    lastButtonState = LOW; //the previous reading of the input pin
    pinMode(inPin, INPUT);

    //Here we will be keeping track of time in order to ignore debounce and noise
    time = 0; // the last time the output pin was toggled
    debounceDelay = 50.0; // the debounce time, increase if the output flickers
}


bool Button::isHigh() {
    return is_high;
}

void Button::update() {
    int reading = digitalRead(inPin);

    // check to see if you just pressed the button
    // (i.e. the input went from LOW to HIGH), and you've waited long enough
    // since the last press to ignore any noise:

    // If the switch changed, due to noise or pressing:
    if (reading != lastButtonState) {
        // reset the debouncing timer
        lastDebounceTime = millis();
    }

    if (abs((millis() - lastDebounceTime)) > abs(debounceDelay)) {
        // whatever the reading is at, it's been there for longer than the debounce
        // delay, so take it as the actual current state:

        // if the button state has changed:
        if (reading != buttonState) {
        buttonState = reading;

            // only toggle the bike mode if the new button state is HIGH
            if (buttonState == HIGH) {
                is_high = HIGH;
            } else {
                is_high = LOW;
            }

            /** IF you want this to act like a switch replace the top if statement with
             * if (buttonState == HIGH) {
                is_high = !is_high;
            }
            */
        }
    }

    // save the reading. Next time through the loop, it'll be the lastButtonState:
    lastButtonState = reading;
    
}