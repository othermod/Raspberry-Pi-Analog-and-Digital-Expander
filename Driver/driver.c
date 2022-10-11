#include <linux/i2c-dev.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <sys/ioctl.h>

#define SENSE_RESISTOR 100

#define BYTE_TO_BINARY_PATTERN "%c%c%c%c%c%c%c%c"
#define BYTE_TO_BINARY(byte)  \
  (byte & 0x80 ? '1' : '0'), \
  (byte & 0x40 ? '1' : '0'), \
  (byte & 0x20 ? '1' : '0'), \
  (byte & 0x10 ? '1' : '0'), \
  (byte & 0x08 ? '1' : '0'), \
  (byte & 0x04 ? '1' : '0'), \
  (byte & 0x02 ? '1' : '0'), \
  (byte & 0x01 ? '1' : '0')

#define I2C_ADDRESS 0x10

typedef struct {
  uint8_t buttonA; // button status
  uint8_t buttonB; // button status
  uint8_t joystickLX; // button status
  uint8_t joystickLY; // button status
  uint8_t joystickRX; // button status
  uint8_t joystickRY; // button status
  uint8_t voltage; // can do all the math here and just give a voltage or %, making these a single byte or 2 bytes at most
  uint8_t amperage;
  uint8_t misc;
}
I2C_STRUCTURE;


int openI2C() {
  int file;
  char * filename = "/dev/i2c-0"; //specify which I2C bus to use
  if ((file = open(filename, O_RDWR)) < 0) {
    fprintf(stderr, "Failed to open the i2c bus"); /* ERROR HANDLING: you can check errno to see what went wrong */
    exit(1);
  }
  return file;
}

int main(int argc, char * argv[]) {
  int I2CFile = openI2C(); // open I2C device
  if (ioctl(I2CFile, I2C_SLAVE, I2C_ADDRESS) < 0) { // initialize communication
    fprintf(stderr, "I2C: Failed to acquire bus access/talk to slave 0x%x\n", I2C_ADDRESS);
    return 0;
  }

  I2C_STRUCTURE I2C_DATA; // create I2C data struct

  while (1) {
    if (read(I2CFile, &I2C_DATA, sizeof(I2C_STRUCTURE)) != sizeof(I2C_STRUCTURE)) { // read the atmega
      printf("Controller is not detected on the I2C bus.\n");
      sleep(1);
    } else {
      printf("\nButtonA: "BYTE_TO_BINARY_PATTERN, BYTE_TO_BINARY(I2C_DATA.buttonA));
      printf("\nButtonB: "BYTE_TO_BINARY_PATTERN, BYTE_TO_BINARY(I2C_DATA.buttonB));
      printf("\njoystickLX: "BYTE_TO_BINARY_PATTERN, BYTE_TO_BINARY(I2C_DATA.joystickLX));
      printf("\njoystickLY: "BYTE_TO_BINARY_PATTERN, BYTE_TO_BINARY(I2C_DATA.joystickLY));
      printf("\njoystickRX: "BYTE_TO_BINARY_PATTERN, BYTE_TO_BINARY(I2C_DATA.joystickRX));
      printf("\njoystickRX: "BYTE_TO_BINARY_PATTERN, BYTE_TO_BINARY(I2C_DATA.joystickRX));
      printf("\nMisc: "BYTE_TO_BINARY_PATTERN, BYTE_TO_BINARY(I2C_DATA.misc));
      uint16_t readVoltage=I2C_DATA.voltage*3300/1024;
      uint16_t readAmperage;
      if (I2C_DATA.amperage < 50) { //just dealing with 8 bit rollover
        readAmperage=(I2C_DATA.amperage+255)*3300/1024;
      } else {
        readAmperage=I2C_DATA.amperage*3300/1024;
      }

      int voltageDifference = readVoltage-readAmperage;
      int amperage = voltageDifference * 1000 * (160 / 150) / SENSE_RESISTOR;  //the 160/150 accounts for the 150/10 voltage divider. Make sure the numerator and denomonator are correct
      int calculatedVoltage = readVoltage * 16 + amperage / 10;
      int rawVoltage = readVoltage * 16;
      printf("\nreadVoltageADC: %d", I2C_DATA.voltage);
      printf("\nreadAmperageADC: %d", I2C_DATA.amperage + 255 * (I2C_DATA.amperage < 50)); // the last part is just for the 8 bit rollover. just tinkering
      printf("\nreadVoltage: %d", readVoltage);
      printf("\nreadAmperage: %d", readAmperage);
      printf("\nvoltageDifference: %d", voltageDifference);
      printf("\namperage: %d", -amperage);
      printf("\ncalculatedVoltage: %d", calculatedVoltage);
      printf("\nrawVoltage: %d", rawVoltage);
      printf("\n");
    }
    usleep(200000);
  }
close(I2CFile); // close file

}
