#include <Arduino.h>
#include <ILAS.h>
#include <Button.h>

    ILAS::ILAS(int topLimSwitchPin1, int botLimSwitchPin1, int PWMPin1, int ILASUpButDigPin1, int ILASDownButDigPin1) {
        //This is to initialize all the buttons
        TOP_LIM_SWITCH_DIG_PIN = topLimSwitchPin1;
        BOT_LIM_SWITCH_DIG_PIN = botLimSwitchPin1;
        PWM_PIN = PWMPin1;
        ILAS_UP_BUT_DIG_PIN = ILASUpButDigPin1;
        ILAS_DOWN_BUT_DIG_PIN = ILASDownButDigPin1;
        ILAS_SPEED = 175;
        pinMode(PWM_PIN, OUTPUT);
        pinMode(TOP_LIM_SWITCH_DIG_PIN, INPUT);
        pinMode(BOT_LIM_SWITCH_DIG_PIN, INPUT);
        // initialize the up and down buttons;
        upButton = new Button(ILAS_UP_BUT_DIG_PIN);
        downButton = new Button(ILASDownButDigPin1);
        //This is to zero the system and get the motor to hit the bottom
    }

    void ILAS::update() {
        int isTopOn = digitalRead(TOP_LIM_SWITCH_DIG_PIN) == LOW;
        int isBotOn = digitalRead(BOT_LIM_SWITCH_DIG_PIN) == LOW;
        if (upButton->isHigh() && !isTopOn) {
            analogWrite(PWM_PIN, ILAS_SPEED);
            return;
        }else if (downButton->isHigh() && !isBotOn) {
            analogWrite(PWM_PIN, -ILAS_SPEED);
            return;
        } else {
            analogWrite(PWM_PIN, 0);
            return;
        }

        /**
        if (digitalRead(LIMIT_SWITCH_DIGITAL_PIN) == LOW) {
            analogWrite(PWM_PIN, 0);
        } else {
            analogWrite(PWM_PIN, 255);
        }
        */
    }
