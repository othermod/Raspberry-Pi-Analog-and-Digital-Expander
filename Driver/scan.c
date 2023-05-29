#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <linux/i2c-dev.h>
#include <sys/ioctl.h>
#include <errno.h>
#include <stdbool.h>

#define ADE_ADDRESS 0x10

bool checkDeviceOnBus(const char* bus, int addr) {
    int file;
    char buf[1] = {0};

    if ((file = open(bus, O_RDWR)) < 0) {
        //printf("Failed to open bus %s.\n", bus);
        return 0;
    }

    if (ioctl(file, I2C_SLAVE, addr) < 0) {
        //printf("Failed to acquire bus access or talk to slave.\n");
        close(file);
        return 0;
    }

    // Try to read a byte from the device
    if (read(file, buf, 1) < 0) {
        //printf("Nothing found on %s.\n", bus);
        return 0;
    } else {
        //printf("Device was found at address 0x%x on %s.\n", addr, bus);
        return 1;
    }
    close(file);
}

int main() {
  bool foundADE;
  bool foundAnything = 0;
    for (int i = 0; i < 23; i++) { // typically there are 2 buses: /dev/i2c-0 and /dev/i2c-1
        char filename[20];
        sprintf(filename, "/dev/i2c-%d", i);
        foundADE = checkDeviceOnBus(filename, ADE_ADDRESS);
        if (foundADE) {
          printf("Found the module on i2c-%d\n", i);
          foundAnything = 1;
        }
    }
    if (!foundAnything) {
      printf("The module was not during scanning. Check your wiring.\n");
    }
    return 0;
}
