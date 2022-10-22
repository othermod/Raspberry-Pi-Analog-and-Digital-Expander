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
#include "imageGraphics.h"
#include "imageLayer.h"
#include <math.h>
#include "bcm_host.h"
#include <assert.h>

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

// create colors ( format is: red, green, blue, opacity)
static RGBA8_T clearColor = { 0,    0,    0,    0};
static RGBA8_T green =      { 0,    255,  0,    255};
static RGBA8_T red =        { 255,  0,    0,    255};
static RGBA8_T orange =     { 255,  127,  0,    255};
static RGBA8_T white =      { 255,  255,  255,  255};
static RGBA8_T black =      { 0,    0,    0,    255};

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

int numberOfJoysticks = 0;
bool batteryEnabled = 0;

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
    uidev.absmin[ABS_RX] = 0; // center position is 127
    uidev.absmax[ABS_RX] = 255;
    uidev.absflat[ABS_RX] = 10; // deadzone
    //uidev.absfuzz[ABS_RX] = 0; // what does this do?
    ioctl(fd, UI_SET_ABSBIT, ABS_RY);
    uidev.absmin[ABS_RY] = 0; // center position is 127
    uidev.absmax[ABS_RY] = 255;
    uidev.absflat[ABS_RY] = 10; // deadzone
    //uidev.absfuzz[ABS_Y] = 0; // what does this do?
  }

  snprintf(uidev.name, UINPUT_MAX_NAME_SIZE, "othermod Gamepad");
  uidev.id.bustype = BUS_USB; // is the id stuff needed?
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

void drawBattery(IMAGE_LAYER_T * batteryLayer) {
  int batteryData = 19;
  int  isCharging = 1;
  IMAGE_T * image = & (batteryLayer -> image);
  //clearImageRGB(image, & clearColor); //the image doesn't need to be erased because the same pixels are being used and colors are changing
  RGBA8_T * batteryColor;
  batteryColor = & green;
  if (batteryData < 20) { // sets color depending on battery level
    batteryColor = & orange;
  }
  if (batteryData < 10) {
    batteryColor = & red;
  }
  // draw the battery outline and fill with color
  imageBoxFilledRGB(image, 1, 0, 30, 14, & white);
  imageBoxFilledRGB(image, 0, 4, 2, 10, & white);
  imageBoxFilledRGB(image, 2, 1, 29, 13, & black);
  imageBoxFilledRGB(image, 1, 5, 3, 9, & black);
  imageBoxFilledRGB(image, 28 - batteryData / 4, 2, 28, 12, batteryColor);
  if (isCharging) {
    RGBA8_T * boltColor;
    if (isCharging == 2) {
      boltColor = & green;
    } else {
      boltColor = & white;
    }
    // draw the lightning bolt to show that the battery is charging
    imageBoxFilledRGB(image, 15, 3, 16, 11, boltColor);
    imageBoxFilledRGB(image, 12, 4, 14, 6, boltColor);
    imageBoxFilledRGB(image, 10, 6, 11, 8, boltColor);
    imageBoxFilledRGB(image, 7, 7, 9, 8, boltColor);
    imageBoxFilledRGB(image, 17, 8, 19, 10, boltColor);
    imageBoxFilledRGB(image, 20, 7, 21, 8, boltColor);
    imageBoxFilledRGB(image, 22, 6, 24, 7, boltColor);
    imageBoxFilledRGB(image, 14, 2, 17, 2, & black);
    imageBoxFilledRGB(image, 17, 2, 17, 7, & black);
    imageBoxFilledRGB(image, 18, 7, 19, 7, & black);
    imageBoxFilledRGB(image, 19, 6, 21, 6, & black);
    imageBoxFilledRGB(image, 21, 5, 25, 5, & black);
    imageBoxFilledRGB(image, 25, 6, 25, 7, & black);
    imageBoxFilledRGB(image, 24, 7, 24, 8, & black);
    imageBoxFilledRGB(image, 23, 8, 22, 8, & black);
    imageBoxFilledRGB(image, 22, 9, 20, 9, & black);
    imageBoxFilledRGB(image, 20, 10, 19, 10, & black);
    imageBoxFilledRGB(image, 19, 11, 17, 11, & black);
    imageBoxFilledRGB(image, 17, 12, 14, 12, & black);
    imageBoxFilledRGB(image, 14, 11, 14, 8, & black);
    imageBoxFilledRGB(image, 14, 7, 12, 7, & black);
    imageBoxFilledRGB(image, 12, 8, 10, 8, & black);
    imageBoxFilledRGB(image, 10, 9, 6, 9, & black);
    imageBoxFilledRGB(image, 6, 8, 6, 8, & black);
    imageBoxFilledRGB(image, 6, 7, 7, 7, & black);
    imageBoxFilledRGB(image, 7, 6, 9, 6, & black);
    imageBoxFilledRGB(image, 9, 5, 11, 5, & black);
    imageBoxFilledRGB(image, 11, 4, 12, 4, & black);
    imageBoxFilledRGB(image, 12, 3, 14, 3, & black);
  }
  changeSourceAndUpdateImageLayer(batteryLayer);
}

void clearLayer(IMAGE_LAYER_T * layer) {
  IMAGE_T * image = & (layer -> image);
  clearImageRGB(image, & clearColor);
  changeSourceAndUpdateImageLayer(layer);
}

int main(int argc, char * argv[]) {
  int ctr;
     for( ctr=0; ctr < argc; ctr++ ) {
        if (!strcmp("-battery", argv[ctr])) {
         batteryEnabled = 1;
        }
        if (!strcmp("-joystick", argv[ctr])) {
         numberOfJoysticks = 1;
       }
        if (!strcmp("-joysticks", argv[ctr])) {
          numberOfJoysticks = 2;
        }
     }
  if (batteryEnabled) {printf("Battery Overlay Enabled\n");}
  if (numberOfJoysticks) {printf("Joysticks Enabled: %d\n", numberOfJoysticks);}
  int I2CFile = openI2C(); // open I2C device
  if (ioctl(I2CFile, I2C_SLAVE, I2C_ADDRESS) < 0) { // initialize communication
    fprintf(stderr, "I2C: Failed to acquire bus access/talk to slave 0x%x\n", I2C_ADDRESS);
    return 0;
  }

if (batteryEnabled) {
  // set up the overlay
  uint32_t displayNumber = 0;
  bcm_host_init();
  DISPMANX_DISPLAY_HANDLE_T display
    = vc_dispmanx_display_open(displayNumber);
  assert(display != 0);
  DISPMANX_MODEINFO_T info;
  int result = vc_dispmanx_display_get_info(display, & info);
  assert(result == 0);
  static int layer = 100000;
  IMAGE_LAYER_T batteryLayer;
  initImageLayer( & batteryLayer,
    31, // battery image width
    15, // battery image height
    VC_IMAGE_RGBA16);
  createResourceImageLayer( & batteryLayer, layer);
  DISPMANX_UPDATE_HANDLE_T update = vc_dispmanx_update_start(0);
  assert(update != 0);
  int xOffset = info.width - 31;
  int yOffset = 0;
  addElementImageLayerOffset( & batteryLayer, xOffset, yOffset, display, update);
  result = vc_dispmanx_update_submit_sync(update);
  assert(result == 0);
  drawBattery(& batteryLayer); // this is only done if something changes on the OSD
}


  int virtualGamepad = createUInputDevice(); // create uinput device
  updateButtons(virtualGamepad);

  uint8_t previousPortB = 0;
  uint8_t previousPortD = 0;
  while (1) {

    if (read(I2CFile, &I2C_DATA, sizeof(I2C_STRUCTURE)) != sizeof(I2C_STRUCTURE)) { // read the atmega
      printf("Controller is not detected on the I2C bus.\n");
      sleep(1);
    }
    else {

      bool updateUinput = 0;
      if ((previousPortB!=I2C_DATA.buttonsPortB) | (previousPortD!=I2C_DATA.buttonsPortD)) { // only update the buttons if something changed
        updateButtons(virtualGamepad);
        updateUinput = 1;
      }
      if(numberOfJoysticks) { // only update the joysticks if they are enabled
        updateJoystick(virtualGamepad);
        updateUinput = 1;
      }
      if (updateUinput) { // only update the gamepad when something changes
        emit(virtualGamepad, EV_SYN, SYN_REPORT, 0);
      }
      previousPortB = I2C_DATA.buttonsPortB;
      previousPortD = I2C_DATA.buttonsPortD;
    }

    usleep(16666);
  }
close(I2CFile); // close i2c
}
