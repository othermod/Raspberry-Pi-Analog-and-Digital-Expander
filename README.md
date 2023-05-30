# Raspberry Pi Analog and Digital Expander

## Table of Contents
- [Overview](#overview)
- [Features](#features)
- [Prerequisites](#prerequisites)
- [Hardware Installation](#hardware-installation)
- [Software Setup](#software-setup)
- [Troubleshooting](#troubleshooting)

## Overview
This repository contains the PCB files for an atmega-based Analog and Digital I2C expander for the Raspberry Pi. The PCB connects to the Raspberry Pi using the CSI camera connector.

I included two programs for interacting with the Expander:
- Create a gamepad with up to 3 joysticks and 16 buttons using `gamepad.c`.
- Read and display device data in binary format using `datareader.c`.

## Prerequisites
- A Raspberry Pi with an available CSI connector.
- The othermod Analog and Digital Expander
- The proper CSI cable for your version of Raspberry Pi.
- GCC compiler (for `gamepad.c` or `datareader.c`).

## Hardware Installation
The module connects to the Raspberry Pi using the CSI connector.

[Image of Pi Zero Connector and Pi 3 Connector]

## Software Setup

### Automatic Installation
If you intend to use this as a gamepad, you can run the setup script. [coming soon]

### Manual Installation

### Enable I2C on the Camera Connector
Start by enabling I2C on the system. Here are the steps if you haven't already done it:

1. Open the Raspberry Pi configuration tool by running the following command in the terminal:
   ```
   sudo raspi-config
   ```
2. Navigate to the 'Interfacing Options' menu.
3. Select the 'I2C' option and enable it.
4. Exit the configuration tool.

Now I2C must be enabled on the CSI connector. Add the following line to `/boot/config.txt` file:
```
dtparam=i2c_vc=on
```

After a reboot, I2C should now be enabled.

If you aren't sure which I2C bus the camera connector is using, run the following commands to build and run the scanning program:
```
gcc scan.c -o scan
./scan
```
Now build the programs.
```
gcc -O3 gamepad.c -o gamepad
gcc datareader.c -o datareader
```
You can run `gamepad` using the following command:
```
sudo ./gamepad [number_of_joysticks]
```

The `number_of_joysticks` argument is optional. If not provided, it defaults to 3. Valid values are integers from 0 to 3.

You can run `datareader` using the following command:

```
sudo ./datareader [-d] [-a analog_channel]
```

The `-d` option displays only the digital inputs. The `-a` option displays the specified analog channel. If no options are given, it displays all data.

## Troubleshooting

## Acknowledgements
This project uses the Linux uinput system to create a virtual gamepad, and the Linux I2C-dev system to read gamepad data from an I2C device.

## Contact
If you have any questions or feedback, please contact us at [Insert Contact Information Here].
```
