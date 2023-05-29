# Raspberry Pi Analog and Digital Expander

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

## Wiring the Board
The module should be connected to the Raspberry Pi using the CSI connector. Ensure that the I2C functionality is enabled on the pins of the CSI connector.

For detailed information about the layout of the board and its connection to the Raspberry Pi, please refer to the included PCB/schematics files in this repository.

The device's I2C address is 0x10.

## Building
Both programs are written in C and can be built using gcc or any other C compiler.

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

## License


## Acknowledgements
This project uses the Linux uinput system to create a virtual gamepad, and the Linux I2C-dev system to read gamepad data from an I2C device.
