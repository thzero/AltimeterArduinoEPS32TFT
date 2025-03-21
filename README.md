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

## Features

An altimeter running on an ESP32 microcontroller, specially a TS-ESP32-S3.  The altimeter can record the following:

* Serial Interface
   * Follows the [BearConsole2](https://github.com/bdureau/BearConsole2) standards.
   * Implemented commands
      * a - output to serial all flight logs
      * e - erase all recorded flight logs
      * l - output to seriali a list of all flight logs
      * n - output to serial the number of recorded flight logs
      * r<n> output to serial data for the specified flight log
      * t - toggle flight log telemetry
      * x - delete last recorded flight log
   * Additional commands
      * ae - expanded output to serial all flight logs
      * h - help
      * lj - output to seriali a list of all flight logs - json - DEV only
      * re<#> expanded output to serial data for the flight log #
      * z - reindex recorded flight log index
   * DEV only commands
      * i - i2c scanner
      * s - start simulation with simulator profile 1
      * s<#> - start simulation with simulator profile #
      * st - stop simulation
* WiFi Access Point
   * A WiFi access point can be used to access the web interface
* Web interface
   * The web interface is acessible on 192.168.5.1.
   * The web interface has the following features
      * Device Information
      * Settings Information
      * Settings Updates
         * Samples/second Ascent
         * Samples/second Descent
         * Samples/second Ground
         * Launch Detect Height (meters)
         * Samples/second Ascent
         * WiFi SSID
         * WiFi Password (forthcoming)
      * Flight Logs
      * Download of Flight Logs
      * Erase Flight Logs (all or last)
      * Upload of new device images (forthcoming)
* TFT
   * Display of Flight Log information (forthcoming)
   * Graphs of Flight Log (forthcoming)

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
* BMP280 (dvarrel ver 1.0.3)
* Button2
* LittleFS_esp2
* SensorLib (used for the QMI8658 sensor)
* TFT
* TFT_eWidget
* TFT_eSPI library
  * See Custom TFT_eSPI library section
* ESPAsyncWebServer
  * See ESPAsyncWebServer library section
* AsyncTCP
  * See AsyncTCP library section

### SPIFFS
As the code uses LittleLFS, make sure that the Partition Scheme is set to SPIFFS.  In ArduinoIDE, set the Tools->Partition Scheme to "Default 4MB with Spiffs (1.2MB apps/1.5MB SPIFFS)".

### AsyncTCP

The AsyncTCP library is not available to install through the Arduino Library Manager in ArduinoIDE, use these steps to install.

Go to https://github.com/ESP32Async/AsyncTCP
Click on Releases and download the latest release as a zip file.
In ArduinoIDE, Sketch > Include Library > Add .zip Library and select the ESPAsyncWebServer library that was downloaded.

### Custom TFT_eSPI library
Using a custom version of the TFT_eSPI that was tweaked by [bdureau](https://github.com/bdureau) to work with this board.

Open a terminal console, navigate to the default working folder for Arudino projects, and execute the following to pull down the custom code base:

``
git clone https://github.com/bdureau/TFT_eSPI.git
``

#### Windows
ArdunioIDE Sketch Folder location is at: C:\Users\\[user>]\OneDrive\Documents\Arduino\libraries

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

### ESPAsyncWebServer

The ESPAsyncWebServer library is not available to install through the Arduino Library Manager in ArduinoIDE, use these steps to install.

Go to https://github.com/ESP32Async/ESPAsyncWebServer
Click on Releases and download the latest release as a zip file.
In ArduinoIDE, Sketch > Include Library > Add .zip Library and select the ESPAsyncWebServer library that was downloaded.

### Filesystem Uploader
To upload the web server files in the data directly, ythe ESP32 Uploader Plugin must be installed in ArduinoIDE.

#### Installation 
ArduinoIDE 2.X: Install [ESP32 LittleFS Uploader](https://github.com/earlephilhower/arduino-littlefs-upload).

* Go to the Uploader's releases page (https://github.com/earlephilhower/arduino-littlefs-upload/releases).
  * Download .vsix file

* Windows
  * IN Windows Explorer, type C:\Users\\[user>]\.arduinoIDE and open that directory.
* Mac OSX
  * In Finder, type ~/.arduinoIDE/ and open that directory.
* Create a new folder called plugins, if a folder by that name is not already present.
* Move the .vsix file to the plugins folder.
  * Remove any other previous versions of the same plugin.
* Restart or open the Arduino IDE 2. 

To check if the plugin was successfully installed, press [Ctrl] + [Shift] + [P] to open the command palette, and search for a command with the name of 'Upload Little FS to Pico/ESP8266/ESP32'.

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
  * DEBUG_COMMANDS
    * Enable debug output for commands.
  * DEBUG_FLIGHT_LOGGER
    * Enable debug output for the flight logger.
  * DEBUG_INTERNAL
    * Enable internal debug output.
  * DEBUG_SENSOR
    * Enable sensor debug output.
  * DEBUG_SIM
    * Enable debug output for the simulator. Only available in DEV mode.
  * DEBUG_SIM_CONFIG
    * Enable debug output for the simulator configuration. Only available in DEV mode.
  * DEBUG_SIM_OUTPUT
    * Enable debug output for the simulator output. Only available in DEV mode.

* DEV
  * Enable to build with dev utilities.
    * Enables the WiFi network on startup.
  * DEV_SIM
    * Enables the "lightweight" flight simulation capability for testing.

## Upload Files

* Shutdown the Serial Monitor if opened, otherwise the upload tool will fail.
* Press [Ctrl] + [Shift] + [P] on Windows or [⌘] + [Shift] + [P] on Mac OSX to open the command palette. 
* Search for the 'Upload LittleFS to Pico/ESP8266/ESP32' command and click on it.
  * After a few seconds, a "Completed upload." message should be shown which indicate that the files were successfully uploaded to the SPIFFS filesystem.

### Errors

If following error message “ERROR: No port specified, check IDE menus“ is shown...

* Restart the Arduino IDE
* Validate that the Tools->Port has been selected as the same one used to upload the Sketch with.
* Retry the Upload tool

### Issues

Uploading files via the method will wipe out any stored flight logs.  This is due to the way the tool creates an file system image offline, then uploads
the file system image in its entirity to the device.

* Make sure to have downloaded any flight logs before doing an upload.

### Potential Resolutions

* Pull flight logs off device and include in the file system image.
* Alternate way to upload files to device.
  * Whether image updates are via browser or serial, file updates would need to be also uploadded.

## Load

Click the Upload button to push the compiled code to the device.

# DEV Mode

Developer mode is enabled by uncommenting the DEV define in the constants.h file.  See Constants section for more detials.

## Simulator

A lightweigth simulator is including when building in DEV mode.   This is NOT a full fideltiy simulator, just enough to try and test the baseline state machine 
under various rocketry conditions.

### Enabling

To enable the simulator, both the DEV and DEV_SIM defines in the constants.h file must be uncommented.

### Simulator Profiles

The simulator has a series of profiles that are stored in the /data/sim.json file.

### Using the Simulator

You can use the simualotor via the serial interface with the following commands.  The simulator will run and producer serial output and trigger flight events,
based on altitude only, within the altimeter. 

* s - start simulation with simulator profile 1
* s<#> - start simulation with simulator profile #
* st - stop simulation

# Disclaimer

thZero is not responsible for any damage that could happen. The code is provided as it is.