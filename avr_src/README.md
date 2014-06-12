Contents:

- application_notes: Selection of application notes supplied by Atmel,
    explaining how to use various features of the XMEGA microcontroller
    Starting point: writing_C_code.pdf (explains e.g. register naming scheme)
- board_layout: schematic and layout of the test board in eagle format 
    (.sch and .brd) and as images (.png)
- datasheets: Datasheets of the hardware components. XMAEGA.pdf is a general
    details all the registers and functions of the XMEGA family. XMEGA_A3.pdf
    is specific to the chip used in this project.
- software_AVR_Studio: example code, which illustrates most of the
    functionalities that we need. Organized as AVR Studio project
- software_WinAVR_Linux: same code, organized for usage with WinAVR or with
    Linux.


Software:

The main program is xmega_test.c. It contains 2 functions:
- init: initializes the master clock, the USART/RS232 serial communication 
        port and the GPIO (general pupose input/output) pins.
- main: calls init and initialization functions for the peripheral HW, blinks
        the onboard LED three times to visually indicate that the chip was 
        successfully reset and issues a welcome message to the serial
        interface.
        Finally it enters an infinite loop, in which it monitors pushing of 
        the 4 buttons and displays pushbutton activity on the LEDs. Then it
        checks for command input from the RS232 port. The following commands
        are implemented:
        - "v": displays software version information
        - "D <c> <v>": sets the DAC output channel <c> to the value <v>
        - "a <c>": reads the ADC channel <c>
        - "S": sweeps DAC channel 3 through its full range, read ADC channel 7
          (assuming they are connected to each other) and stores the ADC value
          in the SRAM memory. When the sweep is finished, it read the values
          back from memory and dumps them to the RS232 interface
        - "R": reset the microcontroller
        The commands are numbered 1-4. The number of the last command executed
        is displayed on the LEDs. In addition, the LEDs also indicate the
        progress during the sweep command.

Peripherals (DAC, ADC, etc.) are controlled by the code in separate files, 
usually two: <peripheral>.c and <peripheral>.h. <peripheral>.c contains the
control code (driver) for the peripheral, <peripheral>.h contains configuration
parameters.
To work with the software, install WinAVR or AVR Studio (under Windows) or the 
correspondig tools (under Linux).
When working in AVR Studio you have to open the project (also called solution)
file 
intro_board/software_AVR_Studio/xmega_test.avrsln.
From within the development environment, you can edit the code and recompile
it. To program the uC, use
Tools>AVR Programming>Apply>Memories>Program to load the executable code
intro_board/software_AVR_Studio/xmega_test/Debug/xmega_test.hex
with the ISP-programmer connected to USB and the Crumb board.
Under Linux you can just type "make program" in the directory
intro_board/software_WinAVR_Linux
to compile the code and load it to the uC.


Hardware:
The main units are the XMEGA microcontroller, the DAC7615 DAC, the LTC1859
ADC, and the MC23K256 SRAM memory. Communication between the uC and the three
other devices is done via SPI. Direct status information can be obtained by a
line of 8 LEDs or by monitoring any location on the board with a DVM or
oscilloscope. 4 Testpoints give comfortable oscilloscope access to the SPI
communication between the ADC and the uC. DAC channel 3 and ADC channel 7 each
have an SMA connector and can be connected to each other to monitor the DAC
output by the ADC.

Connections (see connections.jpg):
Power connections are +V_S (red), GND (black), and -V_S (blue), where 
7V <= V_S <= 15V. The RS232 interface goes through an RS232-USB converter.
Thus, it plugs into the PC's USB port, but shows up as a serial (COMx or
/dev/ttyUSBx) connection. The programming adapter (AVRISP mkII) plugs into the
PC's USB and connects to the 6-pin connector on the Crumb-board. The coding
nose of the connector points away from the quartz's metal housing. Connecting
the two SMA connectors next to the DAC and the ADC connects DAC channel 3 with
ADC channel 7. More peripherals can be connected to the free connectors.
Consult schematic for details.

RS232-interface:
Communication between uC and PC goes through an RS232 serial interface. Use
HTerm, cutecom or similar on the PC side. Configuration:
Baudrate: 115200, 8 data bits, no parity, 1 stop bit (8N1), no handshake.
Command line terminator is LF ('\n', 0x10). 



#####################################

Include user to group dialout 188

In this example our Linux userid is user and we are on the server called machine.

Our USB device is called /dev/ttyUSB0.

Check the current permissions and owner/group of the device.

[user@machine ~]$ ls -la /dev/ttyUSB0

crw-rw----. 1 root dialout 188, 0 Apr 3 21:16 /dev/ttyUSB0

For this configuration, the owner is root, the group is dialout_and both the owner/group have _read/write permissions.

What you need to do is make your login userid part of the group associated with the USB device.

For this case, we add the group dialout to our userid user using the usermod command. This command requires root privileges to run.

[user@machine ~]$ sudo usermod -a -G dialout user

You will need to log out then log back in and now you should have access to the device.
#####################################

Include a new rule to run Make program without sudo.

error:"avrdude: usb_open(): cannot read serial number "error sending control message: Operation not permitted""

look for the device:
Disconnect.
tail -f /var/log/syslog
Connect.
write down the adress /sys/devices/pci000\:00/0000\:00:\1d.0/usb2/2-1/2-1.3
Then:
cat /sys/devices/pci0000\:00/0000\:00\:1d.0/usb2/2-1/2-1.3/idProduct
2104
cat /sys/devices/pci0000\:00/0000\:00\:1d.0/usb2/2-1/2-1.3/idVendor
03eb

Now add the id's from the device 

sudo gedit /etc/udev/rules.d/10-atmega.rules

# AVRISP mkII 
ATTR{idVendor}=="03eb", ATTR{idProduct}=="2104", GROUP="users", MODE="0666" 
#####################################

cutecom 

DAC 12 bits / 4 chnls. [-2.5,2.5] ==> [0,4095] 
Canal instalado: 3

ADC 16 / 8 chnls. [-5,5] ==> [-32768,32768]
Canal instalado: 7