#include <stdio.h>
#include <stdlib.h>
#include <linux/i2c-dev.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include <string.h>
#include <unistd.h>
#include <linux/uinput.h>
#include <stdint.h>

#define I2C_DEVICE "/dev/i2c-1"
#define I2C_ADDR 0x10
#define GAMEPAD_UPDATE_INTERVAL_MS 10

void check(int ret, char *msg) {
    if(ret < 0) {
        perror(msg);
        exit(-1);
    }
}

int main() {
    struct uinput_user_dev uidev;
    int uinput_fd = open("/dev/uinput", O_WRONLY | O_NONBLOCK);
    check(uinput_fd, "open /dev/uinput");

    // Enable key events
    ioctl(uinput_fd, UI_SET_EVBIT, EV_KEY);
    // Enable absolute events for joysticks
    ioctl(uinput_fd, UI_SET_EVBIT, EV_ABS);

    // Register all the gamepad buttons
    for(int i = BTN_TRIGGER_HAPPY1; i < BTN_TRIGGER_HAPPY1+16; i++) {
        ioctl(uinput_fd, UI_SET_KEYBIT, i);
    }

    // Register joystick axes
    for(int i = ABS_X; i <= ABS_RZ; i++) {
        ioctl(uinput_fd, UI_SET_ABSBIT, i);
        uidev.absmin[i] = 0;
        uidev.absmax[i] = 255;
    }

    memset(&uidev, 0, sizeof(uidev));
    snprintf(uidev.name, UINPUT_MAX_NAME_SIZE, "Raspberry Pi Virtual Gamepad");
    uidev.id.bustype = BUS_USB;
    uidev.id.vendor = 0x1;
    uidev.id.product = 0x1;
    uidev.id.version = 1;

    write(uinput_fd, &uidev, sizeof(uidev));
    ioctl(uinput_fd, UI_DEV_CREATE);

    int i2c_fd = open(I2C_DEVICE, O_RDWR);
    check(i2c_fd, "open i2c device");
    ioctl(i2c_fd, I2C_SLAVE, I2C_ADDR);

    while(1) {
        uint8_t data[8];
        int ret = read(i2c_fd, &data, 8);
        //check(ret, "read i2c data");

        uint16_t buttons = data[0] | (data[1] << 8);
        for(int i = 0; i < 16; i++) {
            struct input_event ev;
            memset(&ev, 0, sizeof(ev));
            ev.type = EV_KEY;
            ev.code = BTN_TRIGGER_HAPPY1+i;
            ev.value = (buttons & (1 << i)) != 0;
            write(uinput_fd, &ev, sizeof(ev));
        }

        int axes[] = {ABS_X, ABS_Y, ABS_RX, ABS_RY, ABS_Z, ABS_RZ};
        for(int i = 0; i < 6; i++) {
            struct input_event ev;
            memset(&ev, 0, sizeof(ev));
            ev.type = EV_ABS;
            ev.code = axes[i];
            ev.value = data[i+2];
            write(uinput_fd, &ev, sizeof(ev));
        }

        struct input_event syn;
        memset(&syn, 0, sizeof(syn));
        syn.type = EV_SYN;
        syn.code = SYN_REPORT;
        syn.value = 0;
        write(uinput_fd, &syn, sizeof(syn));

        usleep(GAMEPAD_UPDATE_INTERVAL_MS * 1000);
    }

    ioctl(uinput_fd, UI_DEV_DESTROY);
    close(uinput_fd);
    close(i2c_fd);
    return 0;
}
