#ifndef ILAS_h
#define ILAS_h
    #include <Arduino.h>
    #include <Button.h>

class ILAS {
    private:
        //pin number of the limit switch
        int TOP_LIM_SWITCH_DIG_PIN;
        int BOT_LIM_SWITCH_DIG_PIN;
        int PWM_PIN;
        int ILAS_UP_BUT_DIG_PIN;
        int ILAS_DOWN_BUT_DIG_PIN;
        int ILAS_SPEED;
        Button* upButton;
        Button* downButton;

    public:
        ILAS(int topLimSwitchPin1, int botLimSwitchPin1, int PWMPin1,
             int ILASUpButDigPin1, int ILASDownButDigPin1);
        void update();
};

#endif
