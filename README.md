# Arduino Sensor Auto Detection

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
- SGP41. The SGP41 arduino I2C driver returns raw VOC and NOX values, as opposed to the SEN5X sensors, which internally feed the raw values through Sensirions gas index algorithm and returns a gas index in the range of [0, 500].
- STC3X. The STC3X requires a conditioning phase of up to 10 seconds (this library considers 8 sufficient), during which the value of the datapoints will be UNDEFINED/0.

## Getting Started

### Recommended Hardware

This project was developed and tested on Espressif [ESP32 DevKitC](https://www.espressif.com/en/products/devkits/esp32-devkitc) hardware (see e.g. [ESP32-DevKitC-32D](https://www.digikey.com/en/products/detail/espressif-systems/ESP32-DEVKITC-32D/9356990)).
The ESP32 and all sensors need to be connected to the I2C bus. On the ESP32, the SDA pin (data) is 21 and the SCL pin (clock) is 22.

### Arduino

This library can be installed easily using the Arduino Library manager:
Start the [Arduino IDE](http://www.arduino.cc/en/main/software) and open the Library Manager via

    Sketch => Include Library => Manage Libraries...

Search for the `Sensririon Sensor Auto Detection` library in the `Filter your search...` field and install it by clicking the `install` button.

Alternatively, the library can also be added manually. To do this, download the latest release from github as a .zip file via

    Code => Download Zip

and add it to the [Arduino IDE](http://www.arduino.cc/en/main/software) via

    Sketch => Include Library => Add .ZIP Library...

In both cases, don't forget to _install the dependencies_ listed below. Platformio should take care of this automatically, these dependencies being listed in the `platformio.ini` file.

### PlatformIO
An often more straightforward alternative to the Arduino IDE is the PlatformIO framework. It is integrated well into Visual Studio Code on all platforms and can also be used via a terminal, which is the recommended approach on Linux/Unix systems and is detailed in the following.

Install PlatformIO Core by following the [official instructions](https://docs.platformio.org/en/latest/core/installation/methods/index.html).

To install this library (in `~/.platformio/lib`), execute the following command in a terminal.
```bash
pio lib --global install "Sensirion_Sensor_Auto_Detection"
```
To test an example (e.g `basicUsage`), use the corresponding platformio environment from the project directory:
```bash
cd ~/.platformio/lib/Sensirion_Sensor_Auto_Detection/
```
As of november 2, 2023 platformIO does not correctly convert `.ino` files to `.cpp` files in subdirectories of the project. We must do so manually with the following commands, 
for platformio does not compile .ino files.
```bash
cp examples/basicUsage/basicUsage.ino examples/basicUsage/basicUsage.cpp
pio run -e basicUsage -t upload
```
and start the Serial monitor.
```bash
pio device monitor
```
Available environments are `basicUsage`and `advancedUsage`.
In case you're using some other board, it is recommended you create a new environment in the `platformio.ini` file, using the existing environments as a template.

In order to use this library in your project, add it to the `lib_deps` in your `platformio.ini` file and include `Sensirion_Sensor_Auto_Detection.h`.

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

For example code showing how to use the user API, look at examples/exampleUsage/exampleUsage.ino in this directory. It may also be opened from the Arduino IDE via

    File => Examples => Sensirion Sensor Auto Detection => exampleUsage

In any case, the following steps are essential.

Include the library:
```cpp
    #include "Sensirion_Sensor_Auto_Detection.h"
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

Still in void setup(), fetch the maximum number of simultaneously connected sensors and allocate memory for a hashmap:
```cpp
    maxNumSensors = sensorManager.getMaxNumberOfSensors();
    pCurrentData = new const DataPointList* [maxNumSensors] { nullptr };
```

Update and retrieve Data object in void loop():
```cpp
    sensorManager.refreshAndGetSensorReadings(pCurrentData);
```

Alternatively, more control over the different routines (bus scan for new sensors, state machine update and reading retrieval) is provided through respective functions. The following snippet is equivalent to the above single function call:
```cpp
    sensorManager.refreshConnectedSensors();
    sensorManager.executeSensorCommunication();
    sensorManager.getSensorReadings(pCurrentData);
```

The sensor manager handles command dispatch to the detected sensors, to make sure minimum intervals between commands are respected. Data readout is decoupled from sensor state machine updates, but only the last recorded measurement is available. Note: some sensors have a decay time, after which a conditioning procedure must be executed before readings are available (eg. SGP41). This decay time must not be exceeded inbetween ```cpp SensorManager::updateStateMachines()``` function calls, else SensorManager is never able to provide a measurement for these sensors.
If three 3 consecutive errors occur while trying to read the data ror a sensor, the sensor is considered lost and not read out anymore in the following to save resources.

# Limitations

- This library does not support more than one sensor of the same type at a time.
- Only a single bus is supported. Multiple SensorManager instances must be used to get data from sensors connected to different buses. We suggest using TwoWire instead of the Arduino standard ```Wire.h``` in this case, since it does not support multiple I2C buses.
- As UPT Core, a dependency of this library, uses C++ Standard Template Library (STL), it won't run on most Arduino boards by default. ESP32s or other boards that support the STL will work more smoothly.
