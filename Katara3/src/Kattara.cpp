#include <Arduino.h>
#include <Kattara.h>
#include <Off.h>
#include <Teleop.h>
#include <A_CAN.h>
#include <MotorController.h>

Kattara::Kattara() {
    //IGNITION_PIN = 2;
    CAN_CS_PIN = 10;
    ILAS_PWM_PIN = 5;
    ILAS_TOP_LIM_PIN = 4;
    ILAS_BOT_LIM_PIN = 2;
    ILAS_UP_BUT_PIN = 0;
    ILAS_DOWN_BUT_PIN = 1;
    ignition = new Ignition(IGNITION_PIN);
    //canBus = new A_CAN(CAN_CS_PIN);
    mc = new MotorController(canBus);
    teleop = new Teleop(mc);
    off = new Off(mc);
    ily = new ILAS(ILAS_TOP_LIM_PIN, ILAS_BOT_LIM_PIN, 
    ILAS_PWM_PIN, ILAS_UP_BUT_PIN, ILAS_DOWN_BUT_PIN);

    //mc->setControlCommandSource(0);
}

/**This is the periodic script that will run everything on the bike. */
void Kattara::periodic() {
    ignition->update();
    if (ignition->isBikeOn()) {
        teleop->periodic();
    } else {
        off->periodic();
    }
}


//THIS IS A TEST BASE TO RUN WHEN YOU HAVE TEST CODE YOU NEED TO RUN
void Kattara::kattaraTestOne() {
    //mc->sendRemThrottleCommand(1);
    //Serial.println("one");
    ily->update();
    //Serial.println("fuck");
    //ignition->update();
}

void Kattara::kattaraTestTwo() {
    //mc->sendRemThrottleCommand(0);
    Serial.println("two");
}