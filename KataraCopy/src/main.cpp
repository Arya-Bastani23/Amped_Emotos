#include <Arduino.h>
#include <Kattara.h>

/**
 * All the internal math on the system will be done in the metric system, because
 * most of the sensors we are using will use the metric system, on the back end for
 * the driver display we will do a conversion to display MPH instead of KPH
 */

Kattara *katty = new Kattara();
INT8U output;

void setup() {
  // put your setup code here, to run once:
  output = katty->kattaraTest();
}

void loop() {
  // put your main code here, to run repeatedly:
  //katty->periodic();
  Serial.println(katty->kattaraTest());
  
}