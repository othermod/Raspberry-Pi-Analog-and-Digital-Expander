# Firmware for the Raspberry Pi Analog and Digital Expander

This README provides detailed instructions on how to connect your Raspberry Pi to the Analog and Digital Expander and flash the firmware.

## Hardware Connection

To connect your Raspberry Pi to the Expander, you'll need to make connections using both the CSI connector and 4 GPIO pins. Here are the steps:

1. Connect the CSI connector from the Raspberry Pi to the corresponding connector on the Expander.
2. Connect the 4 GPIO pins from the Raspberry Pi to the corresponding pins on the Expander.

## Flashing the Firmware

Once the hardware connection is complete, you can flash the firmware to the Expander. Here are the steps to do this using avrdude and the GPIO flashing method:

1. Install avrdude on your Raspberry Pi if it's not already installed. You can do this by running the following command: `sudo apt-get install avrdude`
2. Modify avrdude to allow gpio flashing and to specify the GPIO pins with`sudo nano /etc/avrdude.conf`


Find the following section of code
```
#programmer
#  id    = "linuxgpio";
#  desc  = "Use the Linux sysfs interface to bitbang GPIO lines";
#  type  = "linuxgpio";
#  reset = ?;
#  sck   = ?;
#  mosi  = ?;
#  miso  = ?;
#;
```
Remove the # before each line, and specify the GPIO pin to use for flashing
```
programmer
  id    = "linuxgpio";
  desc  = "Use the Linux sysfs interface to bitbang GPIO lines";
  type  = "linuxgpio";
  reset = 20;
  sck   = 21;
  mosi  = 22;
  miso  = 23;
;
```

3. Verify that avrdude can communicate with the atmega8.
```bash
sudo avrdude -p m8 -c linuxgpio
```
4. Navigate to the directory containing the firmware file.
5. Run the following command to flash the firmware: `avrdude -c gpio -p m8 -U flash:w:Firmware.hex`
6. Check for the chip to show up at I2C address 0x10

## Troubleshooting

If you encounter any issues during the hardware connection or firmware flashing process, please refer to the main README in the root directory of this repository for troubleshooting information. If you're still having trouble, feel free to open an issue in this repository with a detailed description of the problem and any error messages you're seeing.

## Conclusion

If you have any questions or need further assistance, don't hesitate to reach out.
