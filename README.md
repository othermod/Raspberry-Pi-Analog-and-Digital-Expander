# Raspberry Pi Analog and Digital Expander

## Table of Contents
- [Overview](#overview)
- [Features](#features)
- [Prerequisites](#prerequisites)
- [How to Enable I2C on the Camera Connector](#how-to-enable-i2c-on-the-camera-connector)
- [Wiring the Board](#wiring-the-board)
- [Installation](#installation)
- [Running](#running)
- [Troubleshooting](#troubleshooting)
- [Contributing](#contributing)
- [License](#license)
- [Acknowledgements](#acknowledgements)
- [Contact](#contact)

## Overview
This repository contains two programs for interacting with a custom gamepad device connected to a Raspberry Pi via the I2C bus:
- `gamepad.c`: Emulates a virtual gamepad with up to 3 joysticks and 16 buttons, reading data from the connected device.
- `datareader.c`: Reads and displays data from the connected device in binary format.

## Features
- Emulate a gamepad with up to 3 joysticks and 16 buttons using `gamepad.c`.
- Read and display device data in binary format using `datareader.c`.
- Both programs interact with the connected device via the I2C bus.

## Prerequisites
- A Raspberry Pi with an available CSI connector (camera connector).
- The proper CSI cable for your version of Raspberry Pi.
- GCC compiler (for `gamepad.c` or `datareader.c`).

## How to Enable I2C on the Camera Connector
The I2C functionality must be enabled on the pins of the CSI connector to allow communication between the Raspberry Pi and the connected device. Here are the steps to enable I2C:

1. Open the Raspberry Pi configuration tool by running the following command in the terminal:
   ```
   sudo raspi-config
   ```
2. Navigate to the 'Interfacing Options' menu.
3. Select the 'I2C' option and enable it.
4. Exit the configuration tool and reboot your Raspberry Pi.

For Pi Zero and larger Pi boards, you may need to add the following lines to the `/boot/config.txt` file:

```
dtparam=i2c_vc=on
dtoverlay=i2c-rtc,ds1307
```

After adding these lines, save the file and reboot your Raspberry Pi. The I2C functionality should now be enabled on the camera connector.

## Wiring the Board
The module should be connected to the Raspberry Pi using the CSI connector. Ensure that the I2C functionality is enabled on the pins of the CSI connector. For detailed information about the layout of the board and its connection to the Raspberry Pi, please refer to the included PCB/schematics files in this repository. The device's I2C address is 0x10.

## Installation
Both programs are written in C and can be built using gcc or any other C compiler. Run the following commands to build the programs:

```
gcc -O3 gamepad.c -o gamepad
gcc datareader.c -o datareader
```

## Running
You can run `gamepad` using the following command:

```
sudo ./gamepad [number_of_joysticks]
```

The `number_of_joysticks` argument is optional. If not provided, it defaults to 3. Valid values are integers from 0 to 3.

You can run `datareader` using the following command:

```
sudo ./datareader [-d] [-a analog_channel]
```

The `-d` option displays the digital inputs. The `-a` option displays the specified analog channel. If no options are given, it displays all data.

## Troubleshooting
If you encounter any issues while using the project, please check the following:

- Ensure that the I2C functionality is enabled on your Raspberry Pi.
- Check your wiring to ensure that the device is correctly connected to the Raspberry Pi.
- Make sure that you have the correct permissions to run the programs. You may need to use `sudo` to run the programs.

If you're still having trouble, please open an issue in the GitHub repository.

## Contributing
We welcome contributions to this project. If you would like to contribute, please follow these steps:

1. Fork the repository.
2. Create a new branch for your changes.
3. Make your changes in your branch.
4. Submit a pull request with your changes.

Please ensure that your code follows the existing style to keep the project consistent.

## License
[Insert License Information Here]

## Acknowledgements
This project uses the Linux uinput system to create a virtual gamepad, and the Linux I2C-dev system to read gamepad data from an I2C device.

## Contact
If you have any questions or feedback, please contact us at [Insert Contact Information Here].
```
