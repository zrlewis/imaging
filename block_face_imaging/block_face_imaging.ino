/*
 * Opto-isolated circuit for high resolution episcopic microscopy
 * by Zack Lewis
 * Apr. 2017
 * 
 * A trigger in the form of a tactile button is attached to a rotary microtome. 
 * The button activates the opto-isolator at a set point during the rotation of  
 * the microtome. The phototransistor (output) side of the opto-isolator fires
 * the camera shutter.
 * 
 */
 
#include <Wire.h>
#include <Adafruit_GFX.h>
#include "Adafruit_LEDBackpack.h"
#include <Button.h> //from https://github.com/JChristensen/Button

//telling arduino what type of LED matrix:
Adafruit_8x16minimatrix matrix = Adafruit_8x16minimatrix(); 

int STATUS_LED = 13; // use built in LED as an additional status light
int OPTO_PIN = 11;   // connect optocoupler pin 1 to arduino pin 11
//pins for the rgb LED:
int redPin = 5;
int greenPin = 6;
int bluePin = 7;
#define COMMON_ANODE //the LED has a common anode. Connect pin 2 to +5V

//initialize counter to 0
int counter = 0;

#define n 1                //number of images to take
#define preDelay 1000      //sets the delay after button is pressed and prior to image capture
                                //this allows block time to stop moving and prevents blur
                                
#define BUTTON_PIN 12      //Connect a tactile button switch from Arduino pin 12 to ground.
#define PULLUP true        //Can use the Arduino's internal pullup resistor. Here I've put a resistor in the circuit
#define INVERT false       //Since the pullup resistor will keep the pin high unless the
                           //switch is closed, this is negative logic, i.e. a high state
                           //means the button is NOT pressed. (Assuming a normally open switch.)
#define DEBOUNCE_MS 20     //A debounce time of 20 milliseconds usually works well for tactile button switches.

Button myBtn(BUTTON_PIN, PULLUP, INVERT, DEBOUNCE_MS);  //from Button library

void setup()
{
  Serial.begin(9600);
  pinMode(BUTTON_PIN, INPUT);
  pinMode(OPTO_PIN, OUTPUT);
  pinMode(STATUS_LED, OUTPUT);

//pinModes for the LED:
  pinMode(redPin, OUTPUT);
  pinMode(greenPin, OUTPUT);
  pinMode(bluePin, OUTPUT);  

  matrix.begin(0x70);  // pass in the address to the LED matrix
}

void loop() {
myBtn.read(); // read the state of the pushbutton value:
setColor(255, 0, 255); //set color of RGB LED to purple
 
  // check if the pushbutton was pressed.
  //            options: if(myBtn.isPressed()){
  //                     if(myBtn.pressedFor(500)){
  //                     if(myBtn.wasPressed()){
 
  if(myBtn.wasPressed()){
      setColor(0, 255, 0);  //change color of RGB LED to Green during imaging
      delay(preDelay);      //delay prior to imaging to reduce blur
      takeAnImage(n);       //fire the camera shutter
      writeMatrix();        //write the counter value to the LED matrix
     
      } 
}


void takeAnImage(int imageNumber){
 
  digitalWrite(STATUS_LED, HIGH);
  
  for(int i = 0; i<imageNumber; i++){
    
    counter++; //increment the counter

    //take an image by setting OPTO_PIN to HIGH
    digitalWrite(OPTO_PIN, HIGH);
    delay(100); //delay can be adjusted so that camera fires

    //print counter to serial monitor
    Serial.print("Counter: ");Serial.println(counter);

    //turn off STATUS_LED, turn off OPTO_PIN
    digitalWrite(STATUS_LED, LOW);
    digitalWrite(OPTO_PIN, LOW);
    
    //set RGB LED to aqua
    setColor(0, 255, 255); //aqua
    delay(500); //delay not actually needed

  }
}


//code below to display counter value on an LED matrix:

void writeMatrix(){
   
  matrix.setRotation(1);
  matrix.setTextSize(1);
  matrix.setTextWrap(false);  // we dont want text to wrap so it scrolls nicely
  matrix.setTextColor(LED_ON);

//printing to a matrix without scrolling. Will cut off part of number after 99
        
      matrix.clear();
      matrix.setCursor(0,0);
      matrix.print(counter); //could do counter/10 to display just a portion
      matrix.writeDisplay();


// scrolling functionality below. Doesn't work unless there is a delay, which impedes the button accuracy.  
//  for (int8_t x=0; x>=-64; x--) {
//   
//    matrix.clear();
//    matrix.setCursor(x,0);
//    matrix.print(counter);
//    matrix.writeDisplay();
//    
//    //delay(10); //any sort of delay here impedes the button accuracy
//  }
}


//code to set LED color:

void setColor(int red, int green, int blue)
{
  #ifdef COMMON_ANODE
    red = 255 - red;
    green = 255 - green;
    blue = 255 - blue;
  #endif
  analogWrite(redPin, red);
  analogWrite(greenPin, green);
  analogWrite(bluePin, blue);  
}

