# CanberraGrammarSchool-OrienteeringDevice
Open-source Orienteering Device, originally designed in collaboration with Canberra Grammar School.

Once the device has finished initial V1 development, and the hardware and firmware verified as stable and appropriate, all design files will be transferred here.
This will allow continuous improvement, additions, and tweaks to the design.

## Hardware
For now, only the Schematic PDF and Bill of Materials CSV are given here.
These help with reference for any firmware development.

Once the design is proven completely, design files will be transferred here for future reference.

## Firmware
To develop, download **STM32CubeIDE** from here: https://www.st.com/en/development-tools/stm32cubeide.html
There are various installers for Windows, Linux, Mac, Debian, etc.
(Note: you will need to make an account with STMicroelectronics to download the installer)

Then, in the application, go to **File -> Open Projects from File System** and navigate to the cloned repo.

The firmware has proven quite challenging and really requires multiple user input, testing in the field, and long term verification.
Why?
The device performance and reliability is **critical**. 
1 packet lost in 100 is not acceptable. This could mean a player in the field loses their result from their SRR punch tag.

Here's an overview of all the features, and what has been identified:

### Slave
The slave has 4 high-level functions.

 * Take in input from a Red SRR
 * Take in input from a Blue SRR
 * Take in input from an Aux SRR
 * Take in input and transmit output through an XBee Module

Regarding the SRR input, this has been heavily tested, refined, and changed over time.
Additionally, XBee output from the Slaves has been heavily tested and proven on the bench.
Commands coming in for the slave have also been tested, but require standardising and testing further.

These commands can include:
 * Heartbeat
 * Ping
 * Set Channel (moved to V2 since there is no easy way to store this in non-volatile memory)
 
This code is currently ommited but will be added back in soon.
(I'm not good at using forks/branches, so I just have local archives of variant versions of code)

One niche feature which has also proven difficult is utilising the RSSI LEDs.
The way this needs to be implemented is using the XBee AT Command "DB".
But, this requires the module to enter command mode in order to actually send this command.
This involves the workflow:

 * Entering command mode with "+++"
 * Waiting 3-4 seconds
 * Reading in UART and succesfully receiving/checking for an "OK" ACK signal from the XBee
 * Transmitting the "DB" command
 * Reading back the data (which is in ASCII text, not a byte value)
 * Converting this text to a byte (which isn't trivial and needs a helper function, since the text can be 1-3 digits such as "6", "89", or "112")
 * Exit Command Mode
 * Determining the amount of LEDs to blink (note that this reading is in dB, so it is not linear - so it should be 1 LED for 100+, 2 LED for 90+, and 3 LED for 80+ as an example)
 
Within this time frame, it's possible a packet could be received.
It also means there's a 3-4 second delay between when a packet is received and when the RSSI can be blinked/indicated on LEDs.
This isn't really nice and doesn't perform well in the field.

Consider a Master device receiving punches from players out in the field.
1 Player punches, the Slave sends the packet -> to the master. 
The master begins to enter a command mode, and during the 3-5 second window a second player punches the same/a different slave.
That packet would then be missed. So the system is flawed.
I haven't thought of a resolution to this to-date.
The XBee module's GPIOs can be configured to display RSSI (I think) and could be connected to the LEDs, instead of the STM32.
But this requires a new revision of hardware, and further R&D into verifying that is possible, and what commands are required to tell the XBee to do that.

### Master
The master has 2 high-level functions.

 * Take in input and transmit output through an XBee Module
 * Take in input and transmit output through a UART-USB cable/adaptor to a PC
 
The input coming in from/out to an XBee module can be:
 * Feedback from Ping Data
 * Information from Punch Data
 
 The input coming in from/out to the PC can be:
 * Set channel commmand
 * Trigger ping command
 * Anything else?
 
 Currently, I'm having some syncrhonisation problems with processing and managing incoming XBee data.
 As a result, it's hard to make any traction on other features.
 
 The PC commands are trivial.
 These can be 1 byte commands terminated with newline (e.g. "P\n" for Ping).
 The firmware is ready to accept this sort of input, but not handle the details and actions until we resolve XBee <-> XBee comms issues.
 
 ### Both
 One feature both devices share is battery percentage.
 The chip that provides this information is the BQ24771 which sends this over I2C.
 I have used this chip on 10+ designs in the last 4 years, it is my favourite.
 On the current protoypes, though, I haven't been able to get data off.
 I'm not sure what's wrong and what is happening. It could be an issue with the I2C configuration, pull-up resistor values, or speed of the bus.
 This will take some time to resolve, too.
 
 # Software
 While I (Scott W - Xentronics) haven't done any of the software that interfaces to the devices, it's important that that is documented here too.
 
 As well as this, for development and general use, I recommend RealTerm: https://sourceforge.net/projects/realterm/
 
 I have used lots of Serial Terminals in my time.
 Some are great for ASCII command terminal style use, some are great for graphing - and this one is great for using Hex and ASCII in the same system.
 This system uses both a lot and interchangebly, so it's a good choice.
 
 Note (this took me ages to work out): You may have to use "Run as Administrator" to stop an error from being raised in Windows 10
