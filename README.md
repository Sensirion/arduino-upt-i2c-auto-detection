# Arduino Sensor Auto Detection

Arduino Library for automatic detection of Sensirion sensors on an I2C Bus. It allows for easy read-out of the sensor data using an Arduino board.

# Supported sensors:

- SHT4X
- SCD4X
- SFA3X
- SVM40
- SEN44

# Installation

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

Include library and I2CAutoDetector:

    #include "I2CAutoDetector.h"
    #include "SensirionSensorAutoDetection.h"

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
- Instances of the class Data may only be handled by reference, copy consturctor and assignment operator are deleted. Of course one may copy out the DataPoints manually from the reference.
