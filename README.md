# AltimeterEPS32TFT

An altimeter running on an ESP32 microcontroller, specially a TS-ESP32-S3.  The altimeter can record the following:

* Altitude
* Humidity
   * Not available on the BME280 sensor
* Pressure
* Temperature
* Acceleration on the 
  * X axis
  * Y axis
  * Z axis

# Development Environment

## Install Git

https://git-scm.com

## Install ArduinoIDE

https://randomnerdtutorials.com/installing-esp32-arduino-ide-2-0/

### Windows

AdurinoIDE projects default to opening in the following location:

C:\Users\\[user>]\OneDrive\Documents\Arduino

## ArduinoIDE Setup

### ESP32 Support

In ArduinoIDE, Boards Manager install the version 2.0.14 version of the ESP32 support library by Expressif Systems.

#### Issue
Any version higher than version 2.0.14 causes core dumps even on the simplest programs.

### Libraries

In ArduinoIDE, under Tools->Manage Libraries, add the following libraries

* Adafruit NeoPixel
* ArduinoJson
* BME280
* Button2
* LittleFS_esp2
* SensorLib (used for the QMI8658 sensor)
* TFT
* TFT_eWidget
* TFT_eSPI library
  * See Custom TFT_eSPI library section

### SPIFFS
As the code uses LittleLFS, make sure that the Partition Scheme is set to SPIFFS.  In ArduinoIDE, set the Tools->Partition Scheme to "Default 4MB with Spiffs (1.2MB apps/1.5MB SPIFFS)".

### Custom TFT_eSPI library
Using a custom version of the TFT_eSPI that was tweaked by [bdureau](https://github.com/bdureau) to work with this board.

Open a terminal console, navigate to the default working folder for Arudino projects, and execute the following to pull down the custom code base:

``
git clone https://github.com/bdureau/TFT_eSPI.git
``

#### Windows
Location is C:\Users\\[user>]\OneDrive\Documents\Arduino\libraries on Windows.

#### Tweak Code

Open C:\Users\\[user>]\OneDrive\Documents\Arduino\libraries\TFT_eSPI\User_Setup_Select.h in a text editor and perform the following edits.

Replace the following line:

``
#include <User_Setup.h>
``

with this line:

``
// #include <User_Setup.h>
``

Replace the following line:

// #include <User_Setups/Setup400_Adafruit_Feather.h>

with this line:

``
#include <User_Setups/Setup400_Adafruit_Feather.h>
``

## Attach Device

Hold the boot button when plugging in to enter boot loader.
Once connected, make sure you are using Adafruit Feather ESP32-S3 TFT board device in AdruionIDE.

# Get the Code
In a terminal console, execute the clone command:

``
git clone https://github.com/thzero/AltimeterEPS32.git
``

# Build Code

In AdruinoIDE, open the AltimeterEPS32.ino file.

## Windows
The location is at C:\Users\\[user]\OneDrive\Documents\Arduino\AltimeterEPS32.


## Compile

Click the Verify button to see that it compiles succesfully.

### Compliation Directives

* USE_SLEEP_MODE
  * Use to enable or disable the sleep mode.

* SAMPLES_RATE
  * Sample rate are based on Eggtimer Quantum documentation
    * https://eggtimerrocketry.com/wp-content/uploads/2024/02/Eggtimer-Quantum-1_09G.pdf, page 25
  * SAMPLE_RATE_GROUND
    * Adjust the sample rate, in Hz, when the altimeter is on the ground.
  * SAMPLE_RATE_AIRBORNE_ASCENT
    * Adjust the sample rate, in Hz, when the altimeter is airborne and ascending.
  * SAMPLE_RATE_AIRBORNE_DESCENT
    * Adjust the sample rate, in Hz, when the altimeter is airborne and descending.

* KALMAN
  * Enable KALMAN debug output.
  * KALMAN_ALTITUDE
    * Enable altitude Kalman filter debug output.
  * KALMAN_PRESSURE
    * Enable pressure Kalman filter debug output.
  * KALMAN_TEMPERATURE
    * Enable pressure Kalman filter debug output.

* DEBUG
  * Toggle the debug output.
  * DEBUG_INTERNAL
    * Enable internal debug output.
  * DEBUG_SENSOR
    * Enable sensor debug output.

* DEV
  * Enable to build with dev utilities.
  * DEV_SIM
    * Enables the "lightweight" flight simulation capability for testing.

## Load

Click the Upload button to push the compiled code to the device.

# Disclaimer

thZero is not responsible for any damage that could happen. The code is provided as it is.