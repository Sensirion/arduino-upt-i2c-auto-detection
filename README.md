# Arduino Sensor Auto Detection

Arduino Library for automatic detection of Sensirion sensors on an I2C Bus. It allows for easy read-out of the sensor data using an Arduino board.

## Supported sensors:

- SHT4X
- SCD4X
- SFA3X
- SVM40
- SEN44

## Getting Started

### Arduino

This library can be installed easily using the Arduino Library manager:
Start the [Arduino IDE](http://www.arduino.cc/en/main/software) and open
the Library Manager via

    Sketch => Include Library => Manage Libraries...

Search for the `Sensririon Sensor Auto Detection` library in the `Filter
your search...` field and install it by clicking the `install` button.

Alternatively, the library can also be added manually. To do this,
download the latest release from github as a .zip file via

    Code => Download Zip

and add it to the [Arduino IDE](http://www.arduino.cc/en/main/software) via

    Sketch => Include Library => Add .ZIP Library...

In both cases, don't forget to _install the dependencies_ listed below.

### PlatformIO

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

# Dependencies

These must be added to the [Arduino IDE](http://www.arduino.cc/en/main/software)
for this library to work. The installation process is analogous
to the one described above, but make sure to look at the respective
READMEs for more detail.

* [Sensirion Core](https://github.com/Sensirion/arduino-core)
* [SHT4x](https://github.com/Sensirion/arduino-i2c-sht4x)
* [SCD4x](https://github.com/Sensirion/arduino-i2c-scd4x)
* [SFA3x](https://github.com/Sensirion/arduino-i2c-sfa3x)
* [SVM40](https://github.com/Sensirion/arduino-i2c-svm40)
* [SEN44](https://github.com/Sensirion/arduino-i2c-sen44)

# Usage

For example code showing how to use the user API, look at examples/exampleUsage/exampleUsage.ino in this directory. It may also be opened from the Arduino IDE via

    File => Examples => Sensirion Sensor Auto Detection => exampleUsage

In any case, the following steps are essential.

Include the library:

    #include "Sensirion_Sensor_Auto_Detection.h"

Instantiate I2CAutoDetector and SensorManager globally (before void setup()):

    I2CAutoDetector i2CAutoDetector(Wire);
    SensorManager sensorManager(i2CAutoDetector);

Begin Serial, Wire and SensorManager in void setup():

    Serial.begin(115200);
    Wire.begin();
    sensorManager.begin();

Update and retrieve Data object in void loop():

    sensorManager.updateData();
    const Data& currentData = sensorManager.getData();

# Restrictions

- This library does not support more than one sensor of the same type at a time.
- Only one instance of the Data object is allowed, copies may not be made. In order to further process the DataPoints contained in the Data object, they should be copied out manually.
