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
  //output = katty->kattaraTest();
  //pinMode(LED_BUILTIN, OUTPUT);
  SerialUSB.begin(9600); 
  while(!SerialUSB);
  SerialUSB.print("Hello World");


}

void loop() {
  // put your main code here, to run repeatedly:
  //katty->periodic();
  //Serial.println(katty->kattaraTest());
  //katty->kattaraTestOne();
  //delay(1000);
  //katty->kattaraTestTwo();

  digitalWrite(LED_BUILTIN, HIGH);   // turn the LED on (HIGH is the voltage level)
  delay(1000);                       // wait for a second
  digitalWrite(LED_BUILTIN, LOW);    // turn the LED off by making the voltage LOW
  delay(1000);                       // wait for a second
  
}