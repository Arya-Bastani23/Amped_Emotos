#include <Arduino.h>
#include <Kattara.h>
#include <Off.h>
#include <Teleop.h>
#include <A_CAN.h>
#include <MotorController.h>

Kattara::Kattara() {
    IGNITION_PIN = 2;
    CAN_CS_PIN = 10;
    ignition = new Ignition(IGNITION_PIN);
    canBus = new A_CAN(CAN_CS_PIN);
    mc = new MotorController(canBus);
    teleop = new Teleop(mc);
    off = new Off(mc);
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
    Serial.println("one");
}

void Kattara::kattaraTestTwo() {
    //mc->sendRemThrottleCommand(0);
    Serial.println("two");
}