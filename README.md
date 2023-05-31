
# Raspberry Pi Analog and Digital Expander

## Table of Contents

- [Overview](#overview)
- [Prerequisites](#prerequisites)
- [Hardware Installation](#hardware-installation)
- [Software Setup](#software-setup)
- [Troubleshooting](#troubleshooting)

## Overview

This repository contains the PCB files and software for an atmega-based Analog and Digital I2C expander for the Raspberry Pi. The PCB connects to the Raspberry Pi using the CSI camera connector.

The board provides a computationally efficient way to create inputs for a gamepad. The atmega8 constantly scans all 6 analog inputs and 16 digital inputs, and handles it debouncing. The Raspberry Pi can request all of the input data and create game inputs from the data 60 times a second with minimal CPU usage.

While the device was primarily designed with gamepad input in mind, it has a variety of other potential uses due to its ability to expand the Raspberry Pi's analog and digital inputs.

Included in this repository are two programs for interacting with the Expander:

- `gamepad.c`: A gamepad with up to 3 joysticks and 16 buttons.
- `datareader.c`: A simple program that will read and display device data in binary format.

## Prerequisites

- A Raspberry Pi with an available CSI connector.
- The othermod Analog and Digital Expander.
- The proper CSI cable for your version of Raspberry Pi.
- GCC compiler (for `gamepad.c` or `datareader.c`).

## Hardware Installation

The module connects to the Raspberry Pi using the CSI connector.

## Software Setup

### Automatic Installation

If you intend to use `gamepad` or `datareader`, you can run the setup script to enable I2C and compile them. Copy the files from the repository's Driver folder to your Raspberry Pi, and run the following command in the terminal:
```bash
sudo bash setup.sh
```

### Manual Installation

#### Enable I2C

Start by enabling I2C on the system. Here are the steps if you haven't already done it:

1. Open the Raspberry Pi configuration tool by running the following command in the terminal:

```bash
sudo raspi-config
```

2. Navigate to the 'Interfacing Options' menu.
3. Select the 'I2C' option and enable it.
4. Exit the configuration tool.

#### Enable I2C on the Camera Connector

Now I2C must be enabled on the CSI connector. Add the following line to `/boot/config.txt` file:

```bash
dtoverlay=i2c_vc
```

After a reboot, I2C should now be enabled on the connector.

#### Scan for the I2C Device

If you aren't sure which I2C bus the camera connector is using, run the following commands to build and run the scanning program:

```bash
gcc scan.c -o scan
./scan
```

#### Compile and Run the Driver

Before building and running the programs, you need to verify that they have the correct I2C interface defined under `#define I2C_DEVICE`. It is `/dev/i2c-0` by default, and you may need to change it to match your situation. Once that is done, you can build the programs.

```bash
gcc -O3 gamepad.c -o gamepad
gcc datareader.c -o datareader
```

You can run `gamepad` using the following command:

```bash
sudo ./gamepad [number_of_joysticks]
```

The `number_of_joysticks` argument is optional. If not provided, it defaults to 3. Valid values are integers from 0 to 3.

You can run `datareader` using the following command:

```bash
./datareader [-d] [-a analog_channel]
```

The `-d` option displays only the digital inputs. The `-a` option displays the specified analog channel. If no options are given, it displays all 8 bytes of data.

## Troubleshooting

If you encounter any issues while setting up or using the Raspberry Pi Analog and Digital Expander, here are some troubleshooting steps:

1. **Check the I2C Connection**: Ensure that the I2C connection is properly enabled and configured. You can use the `scan.c` program included in this repository to verify that the correct I2C bus is being used.

2. **Check the CSI Cable**: Ensure that the CSI cable is properly seated into the connectors at both ends.

3. **Check the Compiler**: If you're having issues compiling `gamepad.c` or `datareader.c`, ensure that the GCC compiler is properly installed and updated.

4. **Check the I2C Device Path**: If the programs are not working as expected, verify that the correct I2C device path is defined in the `#define I2C_DEVICE` line of the program. The default is `/dev/i2c-0`, but this may need to be changed depending on your setup.

If you're still having issues after trying these steps, please open an issue in this repository with a detailed description of the problem and any error messages you're seeing.
```
