# Arduino Sensor Auto Detection

Arduino Library for automatic detection of Sensirion sensors on an I2C Bus. It allows for easy read-out of the data of Sensirion Sensors using an ESP32 board.

## Supported sensors:

- SCD30
- SCD4X
- SEN44
- SEN5X
- SFA3X
- SGP41
- SHT4X
- STC3X
- SVM4X

### Sensor Oddities
- SGP41. The SGP41 arduino I2C driver returns raw VOC and NOX values, as opposed to the SEN5X sensors, which internally feed the raw values through Sensirions gas index algorithm and returns a gas index in the range of [0, 500].
- STC3X. The STC3X requires an initialization phase of 10 seconds, during which the value of the datapoints will be 0
- SEN44/5X. As both the 44 and 5X versions share the same I2C address (which is used to detect the sensor), only one version may be used at a time. You will have to select the appropriate version by uncommenting it in the src/DriverConfig.h file.

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

In both cases, don't forget to _install the dependencies_ listed below.

### PlatformIO
An often more straightforward alternative to the Arduino IDE is the PlatformIO framework. It is integrated well into Visual Studio Code on all platforms and can also be used via a terminal, which is the recommended approach on Linux/Unix systems and is detailed in the following.

Install PlatformIO Core by following the [official instructions](https://docs.platformio.org/en/latest/core/installation/methods/index.html).

To install this library (in `~/.platformio/lib`), execute the following command in a terminal.
```bash
pio lib --global install "Sensirion_Sensor_Auto_Detection"
```
To test an example (e.g `basicUsage`), navigate to the example directory,
```bash
cd ~/.platformio/lib/Sensirion_Sensor_Auto_Detection/examples/basicUsage
```
compile and Upload
```bash
pio run -t upload
```
and start the Serial monitor.
```bash
pio device monitor
```

In order to use this library in your project, add it to the `lib_deps` in your `platformio.ini` file and include `Sensirion_Sensor_Auto_Detection.h`.

## Dependencies

This library uses the following dependencies.

* [Sensirion Core](https://github.com/Sensirion/arduino-core)
* [SCD30](https://github.com/Sensirion/arduino-i2c-scd30)
* [SCD4X](https://github.com/Sensirion/arduino-i2c-scd4x)
* [SEN44](https://github.com/Sensirion/arduino-i2c-sen44)
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
Begin Serial, Wire and SensorManager in void setup():

```cpp
    Serial.begin(115200);
    Wire.begin();
    sensorManager.begin();
```
Update and retrieve Data object in void loop():
```cpp
    sensorManager.updateData();
    const Data& currentData = sensorManager.getData();
```
Sensordata is only read out at specific measurement intervals, which are more accurate with higher loop frequency. The 'SensorManager::getData()' method should thus be called with high frequency and long time delays in the main loop should be avoided. If three 3 consecutive errors occurr while trying to read the data ror a sensor, the sensor is considered lost and not read out anymore in the following to save resources.

# Limitations

- This library does not support more than one sensor of the same type at a time.
- The I2C bus is only scanned on startup, so in order to add a new sensor, the board needs to be restarted.
- As UPT Core, a dependency of this library, uses C++ Standard Template Library (STL), it won't run on most Arduino boards by default. ESP32s or other boards that support the STL will work more smoothly.
- Only one instance of the Data object is allowed, copies may not be made. In order to further process the DataPoints contained in the Data object, they should be copied out manually.
