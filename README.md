# AltimeterEPS32TFT

An altimeter running on an ESP32 microcontroller, specially a TS-ESP32-S3.  The altimeter can record the following:

altitude, pressure, temperature, and acceleration on the x, y, and z axis

# Development Environment

Install ArduinoIDE
https://randomnerdtutorials.com/installing-esp32-arduino-ide-2-0/

Open ArduinoIDE

You will need to have the ESP32 board support version 2.0.14, anything higher than that may not work !!!

## Setup
In ArduinoIDE, under Tools->Manage Libraries, add the following libraries

Adafruit NeoPixel
ArduinoJson
BME280
Button2
LittleFS_esp2
SensorLib (used for the QMI8658 sensor)
TFT
TFT_eWidget

### Custom TFT_eSPI library
In C:\Users\<user>\OneDrive\Documents\Arduino\libraries on Windows get the custom code base.

git clone https://github.com/bdureau/TFT_eSPI.git

Open C:\Users\<user>\OneDrive\Documents\Arduino\libraries\TFT_eSPI\User_Setup_Select.h in a text editor

Comment out the following line

#include <User_Setup.h>

and uncomment the following line

#include <User_Setups/Setup400_Adafruit_Feather.h>

## Windows

AdurinoIDE defaults to open at the following location
C:\Users\<user>\OneDrive\Documents\Arduino

# Get Code
In a terminal

git clone https://github.com/thzero/AltimeterEPS32.git

# Build Code

Open C:\Users\<user>\OneDrive\Documents\Arduino\AltimeterEPS32 in ArduinoIDE

## SPIFFS

As the code uses LittleLFS, you need to make sure that the Partition Scheme is set to SPIFFS

In ArduinoIDE, set the Tools->Partition Scheme to "Default 4MB with Spiffs (1.2MB apps/1.5MB SPIFFS)"

## Attach Device

Hold the boot button when plugging in to enter boot loader 
Once connected, make sure you are using Adafruit Feather ESP32-S3 TFT board device in AdruionIDE

## Compile

Click the Verify button to see that it compiles succesfully

## Load

Click the Upload button to push the compiled code to the device

# Disclaimer

thZero is not responsible for any damage that could happen. The code is provided as it is.

