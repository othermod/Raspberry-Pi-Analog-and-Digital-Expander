#include <stdio.h>
#include <stdlib.h>
#include <linux/i2c-dev.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include <unistd.h>
#include <getopt.h>

#define DEVICE_ADDR 0x10

void display_data_in_binary(unsigned char* data, int start, int length) {
    for(int i = start; i < start + length; i++) {
        for (int j = 7; j >= 0; --j) {
            printf("%d", (data[i] >> j) & 1);
        }
    }
    printf("\n");
}

int main(int argc, char *argv[]) {
    int file;
    char *filename = "/dev/i2c-1"; // change bus number according to your setup
    if ((file = open(filename, O_RDWR)) < 0) {
        printf("Failed to open the i2c bus\n");
        exit(1);
    }

    if (ioctl(file, I2C_SLAVE, DEVICE_ADDR) < 0) {
        printf("Failed to acquire bus access and/or talk to slave.\n");
        exit(1);
    }

    unsigned char data[8] = {0};
    if (read(file, data, 8) != 8) {
        printf("Failed to read from the i2c bus.\n");
    } else {
        int c;
        int digit_opt = 0;
        int analog_opt = -1;
        while ((c = getopt(argc, argv, "da:")) != -1) {
            switch (c) {
                case 'd':
                    digit_opt = 1;
                    break;
                case 'a':
                    analog_opt = atoi(optarg);
                    break;
                default:
                    printf("Unknown option `-%c'.\n", c);
                    exit(1);
            }
        }

        if(digit_opt) {
            display_data_in_binary(data, 0, 2);
        }

        if(analog_opt >= 0) {
            if(analog_opt > 6) analog_opt = 6;
            display_data_in_binary(data, 2 + analog_opt - 1, 1);
        }

        if(digit_opt == 0 && analog_opt < 0) {
            display_data_in_binary(data, 0, 8);
        }
    }

    return 0;
}
