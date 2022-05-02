#include "HX711.h"

const int pingPin = 7; // Trigger Pin of Ultrasonic Sensor
const int echoPin = 6; // Echo Pin of Ultrasonic Sensor
const int loadCellOut = 6;
const int loadCellSck = 5;
const int calibrationNum = 0;

HX711 scale;

void setup() {
   Serial.begin(9600); // Starting Serial Terminal
   Serial.println("Scale Demo Begin:");

   scale.begin(loadCellSck, loadCellOut);
   
   scale.set_scale(482.506);
   scale.tare();

   Serial.pr-=intln("Readings");
}

void loop() {
   
   Serial.print("Reading: ");
   Serial.print(scale.get_units(10), 1);
   Serial.println(" g");
   
   delay(500);
}

double microsecondsToInches(double microseconds) {
   return microseconds / 74 / 2;
}

double microsecondsToCentimeters(double microseconds) {
   return microseconds / 29 / 2;
}
