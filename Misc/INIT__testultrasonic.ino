const int pingPin = 7; // Trigger Pin of Ultrasonic Sensor
const int echoPin = 6; // Echo Pin of Ultrasonic Sensor

void setup() {
   Serial.begin(9600); // Starting Serial Terminal
}

void loop() {
   double duration, millimeters, cmAvg, inAvg; 
   double inchesTotal = 0;
   double centimetersTotal = 0 ;
   pinMode(pingPin, OUTPUT);
   digitalWrite(pingPin, LOW);
   delayMicroseconds(2);
   digitalWrite(pingPin, HIGH);
   delayMicroseconds(10);
   digitalWrite(pingPin, LOW);
   pinMode(echoPin, INPUT);
   duration = pulseIn(echoPin, HIGH);

  for(int i = 0; i < 10; i++){
    double tempIN = duration / 74 / 2;
    double tempCM = duration / 29 / 2;
    inchesTotal = tempIN + inchesTotal;
    centimetersTotal = tempCM + centimetersTotal;
  }
    cmAvg = centimetersTotal /10;
    inAvg = inchesTotal/10;
    millimeters = centimetersTotal *10;
    
   Serial.print(inAvg);
   Serial.print("in, ");
   Serial.print(cmAvg);
   Serial.print("cm,  ");
   Serial.print(millimeters);
   Serial.print("mm");
   Serial.println();
   delay(500);

}
