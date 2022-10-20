#include "string.h"
#include <linux/i2c-dev.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <sys/ioctl.h>
#include <linux/uinput.h>
int numberOfJoysticks = 2;

#define SENSE_RESISTOR 100

// bit positions for each button
#define BTN_1_BIT_READING ((I2C_DATA.buttonsPortB >> 0x07) & 1)
#define BTN_2_BIT_READING ((I2C_DATA.buttonsPortB >> 0x06) & 1)
#define BTN_3_BIT_READING ((I2C_DATA.buttonsPortD >> 0x05) & 1)
#define BTN_4_BIT_READING ((I2C_DATA.buttonsPortD >> 0x06) & 1)
#define BTN_5_BIT_READING ((I2C_DATA.buttonsPortD >> 0x07) & 1)
#define BTN_6_BIT_READING ((I2C_DATA.buttonsPortB >> 0x00) & 1)
#define BTN_7_BIT_READING ((I2C_DATA.buttonsPortB >> 0x01) & 1)
#define BTN_8_BIT_READING ((I2C_DATA.buttonsPortB >> 0x02) & 1)

#define BTN_9_BIT_READING ((I2C_DATA.buttonsPortB >> 0x05) & 1)
#define BTN_10_BIT_READING ((I2C_DATA.buttonsPortB >> 0x04) & 1)
#define BTN_11_BIT_READING ((I2C_DATA.buttonsPortB >> 0x03) & 1)
#define BTN_12_BIT_READING ((I2C_DATA.buttonsPortD >> 0x00) & 1)
#define BTN_13_BIT_READING ((I2C_DATA.buttonsPortD >> 0x01) & 1)
#define BTN_14_BIT_READING ((I2C_DATA.buttonsPortD >> 0x02) & 1)
#define BTN_15_BIT_READING ((I2C_DATA.buttonsPortD >> 0x04) & 1)
#define BTN_16_BIT_READING ((I2C_DATA.buttonsPortD >> 0x03) & 1)

#define I2C_ADDRESS 0x10

typedef struct {
  uint8_t buttonsPortB; // button status
  uint8_t buttonsPortD; // button status
  uint8_t joystickLX; // button status
  uint8_t joystickLY; // button status
  uint8_t joystickRX; // button status
  uint8_t joystickRY; // button status
  uint8_t voltage; // can do all the math here and just give a voltage or %, making these a single byte or 2 bytes at most
  uint8_t amperage;
}
I2C_STRUCTURE;
I2C_STRUCTURE I2C_DATA; // create I2C data struct

int openI2C() {
  int file;
  char * filename = "/dev/i2c-0"; //specify which I2C bus to use
  if ((file = open(filename, O_RDWR)) < 0) {
    fprintf(stderr, "Failed to open the i2c bus"); /* ERROR HANDLING: you can check errno to see what went wrong */
    exit(1);
  }
  return file;
}

int createUInputDevice() {
  numberOfJoysticks = 2;
  int fd;
  fd = open("/dev/uinput", O_WRONLY | O_NDELAY);
  if (fd < 0) {
    fprintf(stderr, "Unable to create gamepad with uinput. Try running as sudo.\n");
    exit(1);
  }
  // device structure
  struct uinput_user_dev uidev;
  memset( & uidev, 0, sizeof(uidev));
  // init event
  ioctl(fd, UI_SET_EVBIT, EV_KEY);
  ioctl(fd, UI_SET_EVBIT, EV_REL);
  // button
  ioctl(fd, UI_SET_KEYBIT, BTN_TRIGGER_HAPPY1);
  ioctl(fd, UI_SET_KEYBIT, BTN_TRIGGER_HAPPY2);
  ioctl(fd, UI_SET_KEYBIT, BTN_TRIGGER_HAPPY3);
  ioctl(fd, UI_SET_KEYBIT, BTN_TRIGGER_HAPPY4);
  ioctl(fd, UI_SET_KEYBIT, BTN_TRIGGER_HAPPY5);
  ioctl(fd, UI_SET_KEYBIT, BTN_TRIGGER_HAPPY6);
  ioctl(fd, UI_SET_KEYBIT, BTN_TRIGGER_HAPPY7);
  ioctl(fd, UI_SET_KEYBIT, BTN_TRIGGER_HAPPY8);
  ioctl(fd, UI_SET_KEYBIT, BTN_TRIGGER_HAPPY9);
  ioctl(fd, UI_SET_KEYBIT, BTN_TRIGGER_HAPPY10);
  ioctl(fd, UI_SET_KEYBIT, BTN_TRIGGER_HAPPY11);
  ioctl(fd, UI_SET_KEYBIT, BTN_TRIGGER_HAPPY12);
  ioctl(fd, UI_SET_KEYBIT, BTN_TRIGGER_HAPPY13);
  ioctl(fd, UI_SET_KEYBIT, BTN_TRIGGER_HAPPY14);
  ioctl(fd, UI_SET_KEYBIT, BTN_TRIGGER_HAPPY15);
  ioctl(fd, UI_SET_KEYBIT, BTN_TRIGGER_HAPPY16);

  // axis
  if (numberOfJoysticks) {
    ioctl(fd, UI_SET_EVBIT, EV_ABS);
    // left joystick
    ioctl(fd, UI_SET_ABSBIT, ABS_X);
    uidev.absmin[ABS_X] = 0; // center position is 1650
    uidev.absmax[ABS_X] = 255; // center position is 1650
    uidev.absflat[ABS_X] = 10; // deadzone
    //uidev.absfuzz[ABS_X] = 0; // what does this do?
    ioctl(fd, UI_SET_ABSBIT, ABS_Y);
    uidev.absmin[ABS_Y] = 0; // center position is 1650
    uidev.absmax[ABS_Y] = 255; // center position is 1650
    uidev.absflat[ABS_Y] = 10; // deadzone
    //uidev.absfuzz[ABS_Y] = 0; // what does this do?
  }

  if (numberOfJoysticks == 2) {
    // right joystick
    ioctl(fd, UI_SET_ABSBIT, ABS_RX);
    uidev.absmin[ABS_RX] = 0; // center position is 1650
    uidev.absmax[ABS_RX] = 255; // center position is 1650
    uidev.absflat[ABS_RX] = 10; // deadzone
    //uidev.absfuzz[ABS_RX] = 0; // what does this do?
    ioctl(fd, UI_SET_ABSBIT, ABS_RY);
    uidev.absmin[ABS_RY] = 0; // center position is 1650
    uidev.absmax[ABS_RY] = 255; // center position is 1650
    uidev.absflat[ABS_RY] = 10; // deadzone
    //uidev.absfuzz[ABS_Y] = 0; // what does this do?
  }

  snprintf(uidev.name, UINPUT_MAX_NAME_SIZE, "othermod Gamepad");
  uidev.id.bustype = BUS_USB;
  uidev.id.vendor = 1;
  uidev.id.product = 5;
  uidev.id.version = 1;
  write(fd, & uidev, sizeof(uidev));
  if (ioctl(fd, UI_DEV_CREATE)) {
    fprintf(stderr, "Error while creating uinput device!\n");
    exit(1);
  }
  return fd;
}

void emit(int virtualGamepad, int type, int code, int val) {
  struct input_event ie;
  ie.type = type;
  ie.code = code;
  ie.value = val;
  /* timestamp values below are ignored */
  ie.time.tv_sec = 0;
  ie.time.tv_usec = 0;
  write(virtualGamepad, & ie, sizeof(ie));
}

void updateButtons(int virtualGamepad) {
  // update button event
  emit(virtualGamepad, EV_KEY, BTN_TRIGGER_HAPPY1, BTN_1_BIT_READING);
  emit(virtualGamepad, EV_KEY, BTN_TRIGGER_HAPPY2, BTN_2_BIT_READING);
  emit(virtualGamepad, EV_KEY, BTN_TRIGGER_HAPPY3, BTN_3_BIT_READING);
  emit(virtualGamepad, EV_KEY, BTN_TRIGGER_HAPPY4, BTN_4_BIT_READING);
  emit(virtualGamepad, EV_KEY, BTN_TRIGGER_HAPPY5, BTN_5_BIT_READING);
  emit(virtualGamepad, EV_KEY, BTN_TRIGGER_HAPPY6, BTN_6_BIT_READING);
  emit(virtualGamepad, EV_KEY, BTN_TRIGGER_HAPPY7, BTN_7_BIT_READING);
  emit(virtualGamepad, EV_KEY, BTN_TRIGGER_HAPPY8, BTN_8_BIT_READING);
  emit(virtualGamepad, EV_KEY, BTN_TRIGGER_HAPPY9, BTN_9_BIT_READING);
  emit(virtualGamepad, EV_KEY, BTN_TRIGGER_HAPPY10, BTN_10_BIT_READING);
  emit(virtualGamepad, EV_KEY, BTN_TRIGGER_HAPPY11, BTN_11_BIT_READING);
  emit(virtualGamepad, EV_KEY, BTN_TRIGGER_HAPPY12, BTN_12_BIT_READING);
  emit(virtualGamepad, EV_KEY, BTN_TRIGGER_HAPPY13, BTN_13_BIT_READING);
  emit(virtualGamepad, EV_KEY, BTN_TRIGGER_HAPPY14, BTN_14_BIT_READING);
  emit(virtualGamepad, EV_KEY, BTN_TRIGGER_HAPPY15, BTN_15_BIT_READING);
  emit(virtualGamepad, EV_KEY, BTN_TRIGGER_HAPPY16, BTN_16_BIT_READING);
}

void updateJoystick(int virtualGamepad) {
  // update joystick
  emit(virtualGamepad, EV_ABS, ABS_X, I2C_DATA.joystickLX);
  emit(virtualGamepad, EV_ABS, ABS_Y, I2C_DATA.joystickLX);
  if (numberOfJoysticks == 2) {
    emit(virtualGamepad, EV_ABS, ABS_RX, I2C_DATA.joystickRX);
    emit(virtualGamepad, EV_ABS, ABS_RY, I2C_DATA.joystickRY);
  }
}

int main(int argc, char * argv[]) {
  int I2CFile = openI2C(); // open I2C device
  if (ioctl(I2CFile, I2C_SLAVE, I2C_ADDRESS) < 0) { // initialize communication
    fprintf(stderr, "I2C: Failed to acquire bus access/talk to slave 0x%x\n", I2C_ADDRESS);
    return 0;
  }

  int virtualGamepad = createUInputDevice(); // create uinput device
  updateButtons(virtualGamepad);
  uint8_t previousPortB;
  uint8_t previousPortD;
  while (1) {
    if (read(I2CFile, &I2C_DATA, sizeof(I2C_STRUCTURE)) != sizeof(I2C_STRUCTURE)) { // read the atmega
      printf("Controller is not detected on the I2C bus.\n");
      sleep(1);
    } else {
      if ((previousPortB!=I2C_DATA.buttonsPortB) | (previousPortD!=I2C_DATA.buttonsPortD)) {
        updateButtons(virtualGamepad);
      }
      updateJoystick(virtualGamepad); // this needs its own section, or needs to constantly happen
      emit(virtualGamepad, EV_SYN, SYN_REPORT, 0); //make this happen whenever something changes
      previousPortB = I2C_DATA.buttonsPortB;
      previousPortD = I2C_DATA.buttonsPortD;
    }

    usleep(16666);
  }
close(I2CFile); // close file

}
