# Arduino Sensirion UPT I2C Auto Detection

Arduino Library for automatic detection of Sensirion sensors on an I2C Bus. It allows for easy read-out of the data of Sensirion Sensors using an ESP32 board.

## Supported sensors:

- SCD30
- SCD4X
- SEN5X
- SFA3X
- SGP41
- SHT4X
- STC3X
- SVM4X

### Sensor Oddities
- SCD30. This sensor has a particular method of retrieveing measurements, which typically enters a waiting loop until the measurement is ready. To avoid this blocking call, the library uses an alternative method, that sometimes yields I2C errors. These errors are not fatal and can be ignored.
- SGP41. The SGP41 arduino I2C driver returns raw VOC and NOX values, as opposed to the SEN5X sensors, which internally feed the raw values through [Sensirion's gas index algorithm](https://github.com/Sensirion/gas-index-algorithm) and returns a gas index in the range of [0, 500].
- STC3X. The STC3X requires a conditioning phase of up to 10 seconds (this library considers 8 sufficient), during which the value of the data points will be UNDEFINED/0.
- SVM40. The SVM40 Evaluation Kit Board is deprecated and **not** supported by sensor autodetection.

## Getting Started

### Recommended Hardware

This project was developed and tested on Espressif [ESP32 DevKitC](https://www.espressif.com/en/products/devkits/esp32-devkitc) hardware (see e.g. [ESP32-DevKitC-32D](https://www.digikey.com/en/products/detail/espressif-systems/ESP32-DEVKITC-32D/9356990)).
The ESP32 and all sensors need to be connected to the I2C bus. On the ESP32, the SDA pin (data) is 21 and the SCL pin (clock) is 22.

### Arduino
Install the software from the [official website](https://www.arduino.cc/en/software) and read [this short tutorial](https://docs.arduino.cc/software/ide-v2/tutorials/getting-started-ide-v2/) to get an introduction to the IDE.
Next, select your board and port in the Board Manager by following [these instructions](https://support.arduino.cc/hc/en-us/articles/4406856349970-Select-board-and-port-in-Arduino-IDE).

This library can be installed easily using the Arduino Library manager:
Start the [Arduino IDE](http://www.arduino.cc/en/main/software) and open the Library Manager via

    Sketch => Include Library => Manage Libraries...

Search for the `Sensirion UPT I2C Auto Detection` library in the `Filter your search...` field and install it by clicking the `install` button. Make sure to click "Install All", lest you'll have to manually search for and install the following:
* [Sensirion Arduino Core](https://www.arduino.cc/reference/en/libraries/sensirion-core/)
* [Sensirion UPT Core](https://www.arduino.cc/reference/en/libraries/sensirion-upt-core/)
* [Sensirion I2C SCD4x](https://www.arduino.cc/reference/en/libraries/sensirion-i2c-scd4x/)
* [Sensirion I2C SFA3x](https://www.arduino.cc/reference/en/libraries/sensirion-i2c-sfa3x/)
* [Sensirion I2C SVM4x](https://www.arduino.cc/reference/en/libraries/sensirion-i2c-svm4x/)
* [Sensirion I2C SHT4x](https://www.arduino.cc/reference/en/libraries/sensirion-i2c-sht4x/)
* [Sensirion I2C SEN5x](https://www.arduino.cc/reference/en/libraries/sensirion-i2c-sen5x/)
* [Sensirion I2C SCD30](https://www.arduino.cc/reference/en/libraries/sensirion-i2c-scd30/)
* [Sensirion I2C SGP41](https://www.arduino.cc/reference/en/libraries/sensirion-i2c-sgp41/)
* [Sensirion I2C STC3x](https://www.arduino.cc/reference/en/libraries/sensirion-i2c-stc3x/)

Alternatively, the library can also be added manually. To do this, download the latest release from github as a .zip file via

    Code => Download Zip

and add it to the [Arduino IDE](http://www.arduino.cc/en/main/software) via

    Sketch => Include Library => Add .ZIP Library...

In this second case, you'll have to also add all dependencies the same way (links to all .zip files given below in the Dependencies section).

### PlatformIO
An often more straightforward alternative to the Arduino IDE is the PlatformIO framework, which is the recommended approach on Linux/Unix systems and is detailed in the following.

The most straight-forward way to use [PlatformIO](https://platformio.org/platformio-ide) is as an extension to Microsoft's [Visual Studio Code](https://code.visualstudio.com/), you'll find it easily among the extensions available for it. Please refer to the official installation instructions [here](https://platformio.org/install/ide?install=vscode).

To use the library, add the following dependencies to your `platformio.ini`'s `lib_deps`:

```control
lib_deps =
    Sensirion/Sensirion UPT I2C Auto Detection
```
PlatformIO will automatically fetch the latest version of the dependencies during the build process.

Alternatively, to install this library in your project environment execute the following command in a terminal:
```bash
pio pkg install --library "Sensirion/Sensirion UPT I2C Auto Detection"
```

To test the default example (`basicUsage`), use the following platformio command from the project's root directory (the one containing the `platformio.ini` file):
```bash
pio run -e basicUsage -t upload
```
and start the Serial monitor with
```bash
pio device monitor
```
The second example can be run by specifying the build environement `advancedUsage`, as such:
```bash
pio run -e advancedUsage -t upload && pio device monitor
```
Available environments are `basicUsage`, `advancedUsage` and `hacksterExample`. Environment `basicUsage` is default and will be used if you omit the `-e` flag.
Please refer to the README in `examples/hacksterExample` for more information about this environment, as it is designed to accompany an example published to [Hackster.io](https://www.hackster.io/sensirion-software/easily-read-out-sensirion-sensor-measurements-on-arduino-9c1862).

In case you're using some other board, it is recommended you create a new environment in the `platformio.ini` file, using the existing environments as a template. Find your `board` parameter [here](https://docs.platformio.org/en/latest/boards/index.html).

## Dependencies

This library uses the following dependencies.

* [Sensirion Core](https://github.com/Sensirion/arduino-core)
* [Sensirion UPT Core](https://github.com/Sensirion/upt-core)
* [SCD30](https://github.com/Sensirion/arduino-i2c-scd30)
* [SCD4X](https://github.com/Sensirion/arduino-i2c-scd4x)
* [SEN5X](https://github.com/Sensirion/arduino-i2c-sen5x)
* [SFA3X](https://github.com/Sensirion/arduino-i2c-sfa3x)
* [SGP41](https://github.com/Sensirion/arduino-i2c-sgp41)
* [SHT4X](https://github.com/Sensirion/arduino-i2c-sht4x)
* [SVM40](https://github.com/Sensirion/arduino-i2c-svm40)
* [STC3X](https://github.com/Sensirion/arduino-i2c-stc3x)

In case automatic dependency installation doesn't work, you will have to install them manually.

## Usage

For example code showing how to use the user API, look at `examples/basicUsage/basicUsage.ino` in this directory. It may also be opened from the Arduino IDE via

    File => Examples => Sensirion Sensor Auto Detection => basicUsage

In any case, the following steps are essential.

Include the library:
```cpp
    #include "Sensirion_upt_i2c_auto_detection.h"
```
Instantiate I2CAutoDetector and SensorManager globally (before void setup()):

```cpp
    I2CAutoDetector i2CAutoDetector(Wire);
    SensorManager sensorManager(i2CAutoDetector);
```
Begin Serial, Wire in void setup():

```cpp
    Serial.begin(115200);
    Wire.begin();
```

Still in `void setup()`, fetch the maximum number of simultaneously connected sensors and allocate memory for the hashmap:
```cpp
    maxNumSensors = sensorManager.getMaxNumberOfSensors();
    pCurrentData = new const DataPointList* [maxNumSensors] { nullptr };
```

Update and retrieve Data object in `void loop()`:
```cpp
    sensorManager.refreshAndGetSensorReadings(pCurrentData);
```

Alternatively, more control over the different routines (bus scan for new sensors, state machine update and reading retrieval) is provided through respective functions. The following snippet is equivalent to the above single function call:
```cpp
    sensorManager.refreshConnectedSensors();
    sensorManager.executeSensorCommunication();
    sensorManager.getSensorReadings(pCurrentData);
```

The sensor manager handles command dispatch to the detected sensors, to make sure minimum intervals between commands are respected. Data readout is decoupled from sensor state machine updates, but only the last recorded measurement is available. Note: some sensors have a decay time, after which a conditioning procedure must be executed before readings are available (eg. SGP41). This decay time must not be exceeded inbetween ```cpp SensorManager::updateStateMachines()``` function calls, else `SensorManager` is never able to provide a measurement for these sensors.
If three 3 consecutive errors occur while trying to read the data ror a sensor, the sensor is considered lost and not read out anymore in the following to save resources.

If all is well, you should see the output as such in the device monitor:
```control
Data retrieved via sensor manager:
-------------------------------------------
  Metadata:
    Platform:           WIRED
    deviceID:           23194551925592
    Device Type:        SCD4X
-------------------------------------------
  Data Point:
    Measured at:        30s
    Value:              1300
  SignalType:
    Physical Quantity:  CO2
    Units:              ppm
  Data Point:
    Measured at:        30s
    Value:              25.3
  SignalType:
    Physical Quantity:  T
    Units:              degC
  Data Point:
    Measured at:        30s
    Value:              40.0
  SignalType:
    Physical Quantity:  RH
    Units:              %
```

# Limitations

- This library does not support more than one sensor of the same type at a time.
- Only a single bus is supported. Multiple SensorManager instances must be used to get data from sensors connected to different buses. We suggest using TwoWire instead of the Arduino standard ```Wire.h``` in this case, since it does not support multiple I2C buses.
- As UPT Core, a dependency of this library, uses C++ Standard Template Library (STL), it won't run on most Arduino boards by default. ESP32s or other boards that support the STL will work more smoothly.
