#include "HX711.h"
#include <LiquidCrystal.h>

// CHANGE CALIBRATION CONDITIONS BASED ON BEST FIT LINE!!!
// For example, best fit equation is: y = mx + b.  (where x is raw sensor value, y is the value in grams
double calibration_YIntercept = 272700; // TODO: Need to define best fit y intercept //272700
double calibration_Slope = -432.7;  // TODO: Need to define best fit slope //-432.7
//-----------------------------------------------------------------------------

// TODO: -Show connecting potentiometer to adjust Y_intercept
//       -Button to show slope
//       -Show state machines to support calibration steps

// initialize the library by associating any needed LCD interface pin
// with the arduino pin number it is connected to
const int rs = 9, en = 4, d4 = 10, d5 = 11, d6 = 12, d7 = 13;
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);

const double nextWeight = 0.1; // how much to weigh the next sample as part of the moving average
const int DELAY_MS = 5; //how many milliseconds between samples

const bool STRICTLY_INCREASING = true;

bool firstCycle = true;

// HX711 circuit wiring
const int LOADCELL_DOUT_PIN =7;
const int LOADCELL_SCK_PIN = 8;

const int buttonPin = 3;

// Ultrasonic Range Sensor
const int ECHO_PIN = 5;
const int TRIG_PIN = 6;

double load, distance;
double prevDistance = 0.0;

int buttonState = 0;

typedef enum {START, WAIT_TILL_READY1, CALIBRATE_BASELINE, WAIT_TILL_READY2, CALIBRATE_1KG, WAIT_TILL_READY3, COLLECT_DATA, FINISHED} SystemState;
SystemState sysState = START;

int incomingByte = 0;

//Object representing load cell
HX711 scale;

void setup() {

  //Define BAUD rate (Async. Communication Speed)
  Serial.begin(9600);
  
  scale.begin(LOADCELL_DOUT_PIN, LOADCELL_SCK_PIN);

  pinMode(TRIG_PIN, OUTPUT); // Sets the trigPin as an OUTPUT
  pinMode(ECHO_PIN, INPUT); // Sets the echoPin as an INPUT

  //Display flie first row in CSV data standard (comma deliminator)
  //Serial.println("Distance (in cm), Load (in g), ");

  //SETUP THE BUTTON
  pinMode(buttonPin,INPUT);

  // set up the LCD's number of columns and rows:
  lcd.begin(16, 2);
  // Print a message to the LCD.
  lcd.setCursor(0,0);
  lcd.print("Calibration Mode");
  delay(5000);
  waitTillReady();
}


void ticFunc(){

  //State Transitions
  switch(sysState){
      case START:{
        sysState = WAIT_TILL_READY1;
        break;
      }
      case WAIT_TILL_READY1:{
        sysState = CALIBRATE_BASELINE;
        break;
      }
      case CALIBRATE_BASELINE:{
        sysState = WAIT_TILL_READY2;
        break;
      }
      case WAIT_TILL_READY2: 
        sysState = CALIBRATE_1KG;
        break;
      case CALIBRATE_1KG:{
        sysState = WAIT_TILL_READY3;
        break;
      }
     case WAIT_TILL_READY3: 
        sysState = COLLECT_DATA;
        break;
      
  }

  //State Actions
  switch(sysState){  
    
    case WAIT_TILL_READY1: {
        //Serial.println("Calibrate: place material in clamps switch the motor on until the material is barely in tension.");
        lcd.clear();
        lcd.setCursor(0,0);
        lcd.print("Tension Material");
        lcd.setCursor(0,1);
        lcd.print("In Clamps");
        delay(2000);
        waitTillReady();

        break;
    }
    case CALIBRATE_BASELINE:{
         distance = getDistance();
         load = getLoad();
        
         //Baseline values
        lcd.clear();
        lcd.setCursor(0,0);
        lcd.print("Calibrating");
        lcd.setCursor(0,1);
        lcd.print("Pls Wait");
        delay(5000);
        //Serial.println("Calibrating.... Continue to hold material in place....");
        for(int i=0;i<5;i++){
          updateDistance();
          updateLoad();
        }
        double baseDistance = distance;
        calibration_YIntercept = load;
  
        //Serial.print("Your accepted baseline values are: ");
        //Serial.print("Load (raw): ");
        //Serial.print(calibration_YIntercept);
        //Serial.print(" , distance (cm): ");
        //Serial.println(baseDistance);       
        break;
    }
    case WAIT_TILL_READY2: {
        lcd.clear();
        lcd.setCursor(0,0);
        lcd.print("Place 2kg");
        lcd.setCursor(0,1);
        lcd.print("On wood");
        delay(5000);
        //Serial.println("With the material in tension, place the 1kg weight on the bed.");
        waitTillReady();
        break;
    }
    case CALIBRATE_1KG:{
        load = getLoad();
        distance = getDistance();
        lcd.clear();
        lcd.setCursor(0,0);
        lcd.print("Calibrating");
        lcd.setCursor(0,1);
        lcd.print("Pls Wait");
        delay(5000);
        lcd.clear();
        //Serial.println("Calibrating.... Continue to hold material in place....");
        for(int i=0;i<20;i++){
          updateDistance();
          updateLoad();
        }    
        double deltaY = (load - calibration_YIntercept) ;
        double deltaX = 1000; //1kg
        double slope = deltaY / deltaX;
        calibration_Slope = slope;
        lcd.clear();   
        lcd.setCursor(0,0);
        lcd.print("Load: ");
        lcd.print(load);
        lcd.setCursor(0,1);
        lcd.print("Slope: ");
        lcd.print(slope);
        delay(5000);
        //Serial.print("Load: ");
        //Serial.println(load);
        //Serial.print("Slope: ");
        //Serial.println(slope);
        break;
    }
    case WAIT_TILL_READY3:{
      // Remove 1kg, then pull rope very slowly to start collecting stress-strain curve.
      //Serial.println("Calibrated!!");
      lcd.clear();
      lcd.setCursor(0,0);
      lcd.print("Calibrated!!");
      lcd.setCursor(0,1);
      lcd.print("Remove Weight");
      delay(5000);
      waitTillReady();
      break;
    }
    case COLLECT_DATA:
    {
      load = getLoad();
      if(firstCycle == true)
      {
        firstCycle = false;
        load = load - getLoad();
      }
       distance = getDistance();
       for(int i=0;i<10;i++)
       {
        updateDistance();
        updateLoad();
       }
      if ( !STRICTLY_INCREASING || distance > prevDistance){  
        //x =(y-b) /m 
        double x = (load - calibration_YIntercept) / calibration_Slope;
        printVals();
        prevDistance = distance;
      }
      break;
    }
  
  }
 
}


void loop() {

  ticFunc();
  delay(DELAY_MS); 
}


void printVals(){
  Serial.print(distance);
  Serial.print('\t');
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("Distance: ");
  lcd.print(distance);
  lcd.print(" m");

  double loadF = convertLoadToGrams(load);
  Serial.println(loadF);

  lcd.setCursor(0,1);
  lcd.print("Load: ");
  lcd.print(loadF);
  lcd.print(" N");
  delay(500);
}


//Rolling average of distance (10% weight for each new sample)
void updateDistance(){
  double currDist = getDistance();
  distance = ((1.0-nextWeight)*distance)+(nextWeight*currDist);
}


//Returns the current distance of the ultrasonic range sensor
double getDistance(){
  
  digitalWrite(TRIG_PIN, LOW);
  delayMicroseconds(2);
      
  // Sets the trigPin HIGH (ACTIVE) for 10 microseconds
  digitalWrite(TRIG_PIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG_PIN, LOW);
      
  // Reads the echoPin, returns the sound wave travel time in microseconds
  double duration = pulseIn(ECHO_PIN, HIGH);
      
  // Calculating the distance
  double dist = (duration * 0.034) / 2.0; // Speed of sound wave divided by 2 (go and back)
  dist = dist - 14;
  return dist;
}


double updateLoad(){ 
  double currLoad = getLoad();
  load = ((1.0-nextWeight)*load)+(nextWeight*currLoad);
}



void waitTillReady()
{

  switch(sysState)
  {
    case WAIT_TILL_READY3:
    
      lcd.clear();  
      lcd.setCursor(0,0);
      lcd.print("Remove Weight");
      lcd.setCursor(0,1);
      lcd.print("Press Button");
    
    default:
      lcd.clear();  
      lcd.setCursor(0,0);
      lcd.print("Press Button");
      lcd.setCursor(0,1);
      lcd.print("ONCE!!!!");

    break;
  }
  //if(sysState == WAIT_TILL_READY3)
  //{
  //  //Serial.println("Calibrated!! Remove the 1kg weight and press the button when ready and flip the switch to begin pulling up");
  //  lcd.clear();  
  //  lcd.setCursor(0,0);
  //  lcd.print("Remove Weight");
  //}
  //else
  //{
  //  //Serial.println("PRESS BUTTON WHEN READY");
  //  lcd.clear();  
  //  lcd.setCursor(0,0);
  //  lcd.print("Press Button");
  //}

  while(buttonState == 0)
  {
    buttonState = digitalRead(buttonPin);
  }
  buttonState = 0;
}

double convertLoadToGrams(double l){
    //x =(y-b) /m 
    double x = (load - calibration_YIntercept) / calibration_Slope;
    x = x * 0.00981;
    return x;
}

double getLoad(){
  double avg = scale.read();

  //double diff = avg - calibration_YIntercept;
  //double grams = diff / calibration_Slope;
  
  //y = mx + b (where y is the value in grams and x is the raw value) 
  //  double grams = (calibration_Slope)*avg + calibration_YIntercept;
  
  return avg;
  //return grams;
}
