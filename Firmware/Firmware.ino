#include <Wire.h>

#define I2C_ADDRESS 0x10
// Digital Input Port B
#define SET_PORTB_PINS_AS_INPUTS DDRB &= B00000000
#define ENABLE_PULLUPS_ON_PORTB  PORTB = B11111111
#define READ_PORTB_PINS          PINB
// Digital Input Port D
#define SET_PORTD_PINS_AS_INPUTS DDRD &= B00000000
#define ENABLE_PULLUPS_ON_PORTD  PORTD = B11111111
#define READ_PORTD_PINS          PIND
#define DEBOUNCE_CYCLES 5 // keep the button pressed for this many loops. can be 0-255. each loop is 10ms

int averageAmperage = analogRead(6) * 16;
int averageVoltage = analogRead(7) * 16;

// to do:
// when the atmega begins, it should detect voltage based off voltage and amperage. wheh pi initializes, it switches to coloumb counting. create variable for capacity and allow pi to transmit it to the atmega

struct I2C_Structure { // define the structure layout for transmitting I2C data to the Raspberry Pi
  uint8_t buttonsPortB; // button status
  uint8_t buttonsPortD; // button status
  uint8_t joystickLX; // button status
  uint8_t joystickLY; // button status
  uint8_t joystickRX; // button status
  uint8_t joystickRY; // button status
  uint8_t voltage; // can do all the math here and just give a voltage or %, making these a single byte.
  uint8_t amperage;
};

I2C_Structure I2C_data; // create the structure for the I2C data

void requestEvent(){
  Wire.write((char*) &I2C_data, sizeof(I2C_data)); // send the data to the Pi
}

void readButtons(){
  //Pin registers are accessed directly. This reads all 8 GPIOs on each register with one command.
  byte readingB = ~READ_PORTB_PINS; // read the pins and invert them, so that a 1 means pushed
  byte readingD = ~READ_PORTD_PINS;
  uint8_t debouncePortB[8]; // button stays pressed for a few cycles to debounce and to make sure the button press isn't missed
  uint8_t debouncePortD[8];

  byte i;
  for(i=0;i<8;i++) {
    // for port B buttons
    if ((readingB >> i) & 1) {            // if this button is pressed
      debouncePortB[i] = DEBOUNCE_CYCLES; // begin the debounce function for this button
    }
    else {                                // if this button is not pressed
      if (debouncePortB[i]) {             // if debounce function is active ( > 0)
        debouncePortB[i]--;               // decrement the debounce function
        readingB = readingB|(1<<i);       // keep this pin pressed
      }
    }
    // for port D buttons
    if ((readingD >> i) & 1){             // if this button is pressed
      debouncePortD[i] = DEBOUNCE_CYCLES; // begin the debounce function for this button
    }
    else {                                // if this button is not pressed
      if (debouncePortD[i]) {             // if debounce function is active ( > 0)
        debouncePortD[i]--;               // decrement the debounce function
        readingD = readingD|(1<<i);       // keep this pin pressed
      }
    }
   }
    I2C_data.buttonsPortB = readingB; // copy the completed readings into the i2c variable to be read by the Raspberry Pi
    I2C_data.buttonsPortD = readingD;
}

void readJoysticks(){
  I2C_data.joystickLX=analogRead(0)/4; // read the ADCs, and reduce to 8 bits
  I2C_data.joystickLY=analogRead(1)/4;
  I2C_data.joystickRY=analogRead(2)/4;
  I2C_data.joystickRX=analogRead(3)/4;
}

void readBattery(){
  averageAmperage = averageAmperage - (averageAmperage / 16) + analogRead(6); //rolling average of 16 readings
  I2C_data.amperage = averageAmperage / 16;
  averageVoltage = averageVoltage - (averageVoltage / 16) + analogRead(7); //rolling average of 16 readings
  I2C_data.voltage = averageVoltage / 16;
}

void setup(){
  SET_PORTB_PINS_AS_INPUTS;
  ENABLE_PULLUPS_ON_PORTB;
  SET_PORTD_PINS_AS_INPUTS;
  ENABLE_PULLUPS_ON_PORTD;
  Wire.begin(I2C_ADDRESS);  // join i2c bus
  Wire.onRequest(requestEvent); // register event
}

void loop(){
  readButtons();
  readJoysticks();
  readBattery();
  delay(10);
}
