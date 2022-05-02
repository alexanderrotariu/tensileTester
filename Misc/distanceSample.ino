#include "HX711.h"

//Phsyical Implementation ULTRASONIC SENSOR
// Pin 5, yellow wire, ECHO
// Pin 6, green wire, TRIGGER

const int echoPin = 5;
const int trigPin = 6;

void setup() 
{
  // put your setup code here, to run once:
  Serial.begin(9600);

  //PinMode Declarations
  pinMode(echoPin, INPUT);
  pinMode(trigPin, OUTPUT);
}

void loop()
{
  //GETTING DISTANCE
  double total = 0.0;
  double samples[21];

  //POLLING THE SENSOR 20 TIMES WITH A 20MS DELAY
  for(int i = 0; i < 21; i++)
  {
      digitalWrite(trigPin, LOW);
      delayMicroseconds(2);

      //ACTIVE ! FOR 10 MS 
      digitalWrite(trigPin, HIGH);
      delayMicroseconds(10);
      digitalWrite(trigPin, LOW);

      //READ THE ECHO PIN RETURN THE SOUND WAVE TRAVEL TIME IN MS   
      long duration = pulseIn(echoPin, HIGH);

      //CALCULATE THE DISTANCE
      double distance = duration * 0.034 / 2;

      samples[i] = distance;

      delay(20);
  }

  //MEDIAN OF THE RANGE SAMPLES
  sort(samples, 21);
  double medianDistance = samples[10];

  Serial.println(medianDistance);
}

void sort(double samples[], int len)
{
  for(int i = 0; i < (len-1); i++)
  {
    for( int j = 0; j < (len - (i+1)); j++)
    {
      if(samples[j] > samples[j + 1])
      {
        int temp = samples[j];
        samples[j] = samples[j+1];
        samples[j+1] = temp;
      }
    }
  }
}
