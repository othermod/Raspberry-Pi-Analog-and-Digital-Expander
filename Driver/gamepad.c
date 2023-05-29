#include <stdio.h>
#include <stdlib.h>
#include <linux/i2c-dev.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include <string.h>
#include <unistd.h>
#include <linux/uinput.h>
#include <stdint.h>

// Constants for the device and joystick configuration
#define I2C_DEVICE "/dev/i2c-0"
#define I2C_ADDR 0x10
#define GAMEPAD_UPDATE_INTERVAL_MS 16
#define DEFAULT_JOYSTICKS 3
#define JOYSTICK_AXES 2 // Each joystick has 2 axes

// Helper function to handle errors
void check(int ret, char *msg) {
    if(ret < 0) {
        perror(msg);
        exit(-1);
    }
}

int main(int argc, char *argv[]) {
    // Handle the joystick count argument
    int joystick_count = DEFAULT_JOYSTICKS;
    if(argc > 1) {
        joystick_count = atoi(argv[1]);
        if(joystick_count < 0 || joystick_count > 3) {
            printf("Invalid number of joysticks. Please enter a number between 0 and 3.\n");
            return -1;
        }
    }

    // Initialize uinput device
    struct uinput_user_dev uidev;
    int uinput_fd = open("/dev/uinput", O_WRONLY | O_NONBLOCK);
    check(uinput_fd, "open /dev/uinput");

    // Enable key and absolute events
    ioctl(uinput_fd, UI_SET_EVBIT, EV_KEY);
    ioctl(uinput_fd, UI_SET_EVBIT, EV_ABS);

    // Register all the gamepad buttons
    for(int i = BTN_TRIGGER_HAPPY1; i < BTN_TRIGGER_HAPPY1+16; i++) {
        ioctl(uinput_fd, UI_SET_KEYBIT, i);
    }

    // Register the axes for the joysticks
    int axes[] = {ABS_X, ABS_Y, ABS_RX, ABS_RY, ABS_Z, ABS_RZ};
    for(int i = 0; i < joystick_count * JOYSTICK_AXES; i++) {
        ioctl(uinput_fd, UI_SET_ABSBIT, axes[i]);
        uidev.absmin[axes[i]] = 0;
        uidev.absmax[axes[i]] = 255;
    }

    // Set up the uinput device
    memset(&uidev, 0, sizeof(uidev));
    snprintf(uidev.name, UINPUT_MAX_NAME_SIZE, "Raspberry Pi Virtual Gamepad");
    uidev.id.bustype = BUS_USB;
    uidev.id.vendor = 0x1;
    uidev.id.product = 0x1;
    uidev.id.version = 1;

    // Create the uinput device
    write(uinput_fd, &uidev, sizeof(uidev));
    ioctl(uinput_fd, UI_DEV_CREATE);

    // Initialize I2C
    int i2c_fd = open(I2C_DEVICE, O_RDWR);
    check(i2c_fd, "open i2c device");
    ioctl(i2c_fd, I2C_SLAVE, I2C_ADDR);

    // Main loop
    while(1) {
        // Read data from I2C
        uint8_t data[8];
        int ret = read(i2c_fd, &data, 8);
        check(ret, "read i2c data");

        // Process button data
        uint16_t buttons = data[0] | (data[1] << 8);
        for(int i = 0; i < 16; i++) {
            struct input_event ev;
            memset(&ev, 0, sizeof(ev));
            ev.type = EV_KEY;
            ev.code = BTN_TRIGGER_HAPPY1+i;
            ev.value = (buttons & (1 << i)) != 0;
            write(uinput_fd, &ev, sizeof(ev));
        }

        // Process joystick data
        for(int i = 0; i < joystick_count * JOYSTICK_AXES; i++) {
            struct input_event ev;
            memset(&ev, 0, sizeof(ev));
            ev.type = EV_ABS;
            ev.code = axes[i];
            ev.value = data[i+2];
            write(uinput_fd, &ev, sizeof(ev));
        }

        // Send sync event
        struct input_event syn;
        memset(&syn, 0, sizeof(syn));
        syn.type = EV_SYN;
        syn.code = SYN_REPORT;
        syn.value = 0;
        write(uinput_fd, &syn, sizeof(syn));

        // Sleep for a while
        usleep(GAMEPAD_UPDATE_INTERVAL_MS * 1000);
    }

    // Cleanup
    ioctl(uinput_fd, UI_DEV_DESTROY);
    close(uinput_fd);
    close(i2c_fd);

    return 0;
}
