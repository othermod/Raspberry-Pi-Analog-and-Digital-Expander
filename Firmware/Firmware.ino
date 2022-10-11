//#include "pins_arduino.h"
#include <Wire.h>

#define I2C_ADDRESS 0x10

//Port B
#define SET_PORTB_PINS_AS_INPUTS DDRB &= B00000000
#define ENABLE_PULLUPS_ON_PORTB  PORTB = B11111111
#define READ_PORTB_PINS          PINB

//Port D
#define SET_PORTD_PINS_AS_INPUTS DDRD &= B00000000
#define ENABLE_PULLUPS_ON_PORTD  PORTD = B11111111
#define READ_PORTD_PINS          PIND

byte temporaryButtonDataA; //The button data is constantly changing and shifting. Store in temp variable and only copy to i2c data byte after all pins are read. This keeps incomplete data from being read by i2c.
byte temporaryButtonDataB; //To store parallel data

byte buttonDebounceA[8]; // button stays pressed until the bit is shifted out(5 10ms cycles) to debounce and to make sure the button press isn't missed
byte buttonDebounceB[8];

#define DEBOUNCE_CYCLES B00010000

struct I2C_Structure
{
  uint8_t buttonA; // button status
  uint8_t buttonB; // button status
  uint8_t joystickLX; // button status
  uint8_t joystickLY; // button status
  uint8_t joystickRX; // button status
  uint8_t joystickRY; // button status
  uint8_t voltage; // can do all the math here and just give a voltage or %, making these a single byte or 2 bytes at most
  uint8_t amperage;
  byte misc;
};

I2C_Structure I2C_data;

uint16_t tempVoltage;
uint16_t tempAmperage;

void requestEvent(){
  Wire.write((char*) &I2C_data, sizeof(I2C_data)); // send the data to the Pi
}

void readButtons(){
  //Pin registers are accessed directly. This reads all 8 GPIOs on each register with one command.
  I2C_data.buttonA = READ_PORTB_PINS;
  I2C_data.buttonB = READ_PORTD_PINS;
 // byte i;
  // for(i=0;i<8;i++) {
  //    if (true){ //if button is not pressed (GPIO high)
    //    buttonDebounceA[i] = buttonDebounceA[i] >> 1;
 //     } 
 //     else {
//        buttonDebounceA[i] = DEBOUNCE_CYCLES; 
//      }
      //pinState = buttonDebounceA[i];  //
      //temporaryButtonDataA = (temporaryButtonDataA<<1)|pinState; //Shift the bit into temporaryButtonDataA
  // }
    //I2C_data.buttonA = temporaryButtonDataA;
    //I2C_data.buttonB = temporaryButtonDataB;
   
}

void readADC(){
  I2C_data.joystickLX=analogRead(0)/4;
  I2C_data.joystickLY=analogRead(1)/4;
  I2C_data.joystickRX=analogRead(3)/4;
  I2C_data.joystickRY=analogRead(2)/4;
  tempVoltage = tempVoltage - (tempVoltage / 16) + analogRead(7); //rolling average of 16 readings
  I2C_data.voltage = tempVoltage / 16;
  
  tempAmperage = tempAmperage - (tempAmperage / 16) + analogRead(6); //rolling average of 16 readings
  I2C_data.amperage = tempAmperage / 16;
  //I2C_data.voltage = analogRead(2);
  //I2C_data.amperage = analogRead(3);
}

void setup() {
  SET_PORTB_PINS_AS_INPUTS;
  ENABLE_PULLUPS_ON_PORTB;
  SET_PORTD_PINS_AS_INPUTS;
  ENABLE_PULLUPS_ON_PORTD;
  Wire.begin(I2C_ADDRESS);  // join i2c bus
  Wire.onRequest(requestEvent); // register event
  //analogReference(INTERNAL); // use internal 1.1v reference for ADC
  tempVoltage = analogRead(2) * 16;
  tempAmperage = analogRead(3) * 16;
  
}
void loop() {
  uint16_t timeDelay=millis()+10; //why does a single byte only work for a moment and then freeze?
  readButtons();
  readADC();
  uint16_t timeNow=millis();
  delay(timeDelay-timeNow);
I2C_data.misc = B10010010;
}





   
