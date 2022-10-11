raspi-gpio set 40 op dl
sleep .5
raspi-gpio set 40 op dh
sleep .5
./twiboot -a 0x29 -d /dev/i2c-0 -w flash:Firmware.ino.eightanaloginputs.hex
raspi-gpio set 40 ip pu
