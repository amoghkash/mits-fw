# Communication over Console

The device has a built in console with which you can communicate. This means that by just plugging in and opening the serial port, you can talk with the device and control it. This is especially useful since the device doesn't have a visual interface with which you can communicate with.

## How to Connect?

### Mac & Linux

To connect over USB/Serial to the device, we are going to use Picocom. Picocom is a tool that is used to connect to the device and communicate with it.

To install Picocom on Mac, you can simply type `brew install picocom` and for linux it is `sudo apt-get install picocom`.

To connect to the device, find the port that the device is connected to. For Mac, type in `ls /dev/cu*` and copy the port that starts with "/dev/cu.usbmodem". After this, type in `picocom --b 115200 /dev/cu.usbmodemXXXXX` with the X's being the number from before.

You can now communicate with the device over USB.

### Windows

To communicate with the device on Windows, we are going to use TeraTerm. TeraTerm is a free serial communication software for Windows. To connect to a device, open Tera Term, create a new session, pick the COM port that the device is on, select the Baud rate as 115200 and then hit connect. This should connect you to the device.
