Author: G33kAtWork (https://github.com/G33KatWork)

# ISD2100 flash library - ISD Soundchip / sound recorder

This ISD2100 flash-library is neither complete nor extensively tested.
Take it as it is and use it at your own risk!!

The firmware was developed on an AT90USB162 (Olimex AVR-USB-162), but similar AVRs supported by LUFA may be compatible as well.


## Requirements
* AT90USB162 or similar @ 3V3
* LUFA-120730 or later (http://www.fourwalledcubicle.com/LUFA.php)
* python-usb version 1.0


## Pinouts
```
 PIN  AT90USB162  ISD2100
  SS   PB0 (14)      (3)
 SCK   PB1 (15)   GPIO1 (2)
MOSI   PB2 (16)   GPIO0 (4)
MISO   PB3 (17)   GPIO2 (1)
 BSY   PB4 (18)   GPIO4 (12)
```

## Usage
```
# python isd2100.py
status              Shows status
int                 Shows interrupt status
pwrup               Powerup device
pwrdwn              Powerdown device
reset               Reset device
id                  Shows device id
read                Dumps device
write               Writes device
flush               Flushs device
voice               Plays voice prompt
voicerg             Plays voice prompt in register
macro               Plays voice macro
macrorg             Plays voice macro in register
stop                Stops play
help                Prints help
```


## Thanks/Credits
Thx to all who provided infos and of whom i may have borrowed code/ideas from.
