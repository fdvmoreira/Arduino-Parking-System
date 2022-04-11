#include <IRremote.hpp>									      // IR controller
#include <Servo.h>										        // Motor
#include <LiquidCrystal.h>								    // LCD

// LCD
const int RS=12, EN=11, D4=5, D5=4, D6=3, D7=2;			// Pins used by LCD 
LiquidCrystal lcd(RS, EN, D4, D5, D6, D7);				  // Initialise LCD
// motor for garage door
Servo servo;
const int MOTOR_PIN = 8;
int motorPosition = 0;
// LEDs
const int RED =7 ,GREEN = 9;
// IR Receiver
const int IR_RECEIVE_PIN = 6;
// CONTROLLER BUTTONS
const int UP = 10, DOWN = 8;
// Ultrasoud sensor for detecting parked vehicle
const int PARKING_SENSOR = 13;
// PIR Sensor for detectiing approximating vehicles
const int APPROXIMATION_SENSOR = 10;

// declare the function to detect parking status
bool isOccupied(const float& garageHeight=200.0f);

// Runs Once to initialise the project
void setup()
{
  // Set up the LCD's number of columns and rows
  lcd.begin(16,2);
  lcd.write("vacant");
  // Attach motor to pin 8
  servo.attach(MOTOR_PIN);
  servo.write(motorPosition);
  // RGB LED
  pinMode(GREEN,OUTPUT);
  pinMode(RED,OUTPUT);
  turnLEDOn(RED); // default light
  // PIR sensor
  pinMode(APPROXIMATION_SENSOR, INPUT);
  // Serial Output enable
  Serial.begin(9600);
  // init IR Receiver
  IrReceiver.begin(IR_RECEIVE_PIN, ENABLE_LED_FEEDBACK);
}

// *************************** Main Loop ********************************
void loop()
{
  
  
  // if a vehicle is near the doors, open it
  if(isVehicleApproximating() && (!isOccupied())){
    openDoor();
    delay(4000);
    shutDoor();
  }

  // if no vehicle in the car park the LED stays GREEN, else RED
  if(!isOccupied()){
    turnLEDOn(GREEN);
    displayMessage("vacant",0,0);
  }else{
    turnLEDOn(RED);
    displayMessage("Occupied",0,0);
  }
  
  // listen for inputs from the remote controller
  if(IrReceiver.decode()){
    uint16_t button = IrReceiver.decodedIRData.command;
    // Serial.println(button);
    // IrReceiver.printIRResultShort(&Serial);
    IrReceiver.resume();
    
    switch(button){
      case UP:			// open door
          openDoor();
      break;
      case DOWN:			// close door
          shutDoor();
      break;
      default:;
    }
  } 
}
// ************************** END Main Loop *****************************


// Open the door
void openDoor(){
  if(isDoorShut()){
    
    for(motorPosition; motorPosition<90; motorPosition++){
      servo.write(motorPosition);
    }
    displayMessage("Door Open",0,1);
  }
}

// Shut the door
void shutDoor(){
  if(isDoorOpen()){
    
    for(motorPosition; motorPosition>0; motorPosition--){
      servo.write(motorPosition);
    }
    displayMessage("Door Closed",0,1);
  }
}

// detect if any movement is in fron of the door
bool isVehicleApproximating(){
  int val = digitalRead(APPROXIMATION_SENSOR);
  //Serial.println(val);
  return (bool)val;
}

// if the current height of the garage is less than its normal heights
// that means that a car, motor, or any other vehicle is in the parking spot
// currentHeight - the height of the spot at current time
// garageHeight - default height of the garage
bool isOccupied(const float& garageHeight){
  
  // parking sensor
  int duration = ping(PARKING_SENSOR);
  const float currentHeight = (duration * 0.034) /2;
  //Serial.println(currentHeight);
  
  return(currentHeight < garageHeight);
}

// check is door open
bool isDoorOpen(){return motorPosition > 1;}

// check if door is shut
bool isDoorShut(){ return motorPosition < 1;}

// display message
void displayMessage(const String& msg, const int& col, const int& row){
  //lcd.clear();
  lcd.setCursor(col,row);
  lcd.print(msg);
  // fill the rest of the line with empty spaces
  if(msg.length()<16){
    for(int i = msg.length();i<16;i++){
      lcd.print(' ');  
    }  
  }
}

// Turn LEDs ON and OFF
void turnLEDOn(const int& led){
  switch(led){
    case RED:
    	digitalWrite(GREEN, LOW);
    	digitalWrite(RED, HIGH);
    break;
    case GREEN:
    	digitalWrite(RED, LOW);
    	digitalWrite(GREEN, HIGH);
    break;
    defaul:;
  }
}

 // Initiate and capture Ping))) Ultrasonic Distance Sensor's round trip echo time.
 // pin - Digital I/O pin connected to Ping)))
 // Returns:   duration - The echo time in microseconds
 
long ping(const int& pin)
{
  long duration;                                   // Variables for calculating distance
  
  // Turn the pin mode to OUTPUT, send signal and wait for 10 microseconds
  pinMode(pin, OUTPUT);                            
  digitalWrite(pin, HIGH);                         
  delayMicroseconds(10);
                              
  digitalWrite(pin, LOW);                          
  pinMode(pin, INPUT);                             // Set I/O pin to input
  duration = pulseIn(pin, HIGH);                   // Measure echo time pulse from Ping)))
  // delay(500);                                   // hold for half second
  return duration;                                 // Return pulse duration
}
