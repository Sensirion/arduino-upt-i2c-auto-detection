/*
Advanced Usage

This sketch demonstrates optional features of the library, namely retrieval of
the underlying Sensirion sensor driver from the SensorManager object for custom
initialization

The circuit:
Any number of sensirion (different) Sensirion sensors may be connected
to the I2C bus. The driver retrieval expects at least the SCD4X sensor to be
connected.

Attempts to use this feature simultaneously with the autodetect library may
saturate the sensor with commands, thus throwing errors resulting in failure to
perform commands. Thus, only use the driver to perform actions not handled by
the sensor manager.
*/

#include "Arduino.h"
#include "SensirionI2CScd4x.h"
#include "Sensirion_Sensor_Auto_Detection.h"
#include "Sensirion_UPT_Core.h"
#include <cmath>

I2CAutoDetector i2CAutoDetector(Wire);
SensorManager sensorManager(i2CAutoDetector);

// Pointer to sensor driver to be retrieved from sensorManager
// To retrieve the drivers of other sensors, change:
// - the type of the pointer below
// - the SensorManager::getSensorDriver template argument
// - the sensorId argument
// accordingly.

SensirionI2CScd4x* pScd4xDriver = nullptr;
void printData(const Data**, size_t);
float t_incr = 0;

void setup() {
    Serial.begin(115200);
    Wire.begin();
    sensorManager.begin();

    // Retrieval of sensor driver
    AutoDetectorError error = sensorManager.getSensorDriver<SensirionI2CScd4x>(
        pScd4xDriver, SensorID::SCD4X);

    // Set custom interval for sensor measurement update (default: 5s, lower
    // values are ignored)
    sensorManager.setInterval(7500, SensorID::SCD4X);
}

void loop() {
    delay(5000);

    // Perform some action with the driver

    //      CAUTION: WILL MODIFY SENSOR BEHAVIOUR AT HARDWARE LEVEL

    pScd4xDriver->setAmbientPressure(
        1000 + 500 * std::sin(t_incr * 2.0 * 3.1415 / 10.0 + 0.5 * 3.1415));
    // (to reset to 1 atm run the following line:
    // pScd4xDriver->setAmbientPressure(1013.25);

    t_incr++;

    // Data should update every second loop, because of the custom update
    // interval
    sensorManager.updateStateMachines();
    const Data* currentData[sensorManager.getMaxNumberOfSensors()];
    sensorManager.getData(currentData);
    printData(currentData, sensorManager.getMaxNumberOfSensors());
}

void printData(const Data** data, size_t numDataPacks) {
    Serial.println("Data retrieved via sensor manager:");
    for (size_t p = 0; p < numDataPacks; p++) {
        const Data* dataPack = data[p];
        if (!dataPack) {
            continue;
        }

        for (size_t i = 0; i < dataPack->getLength(); ++i) {
            const DataPoint& dp = dataPack->getDataPoint(i);
            // Get SensorId string using SensorId enum as index
            Serial.print(dp.sourceDevice.c_str());
            Serial.print("-");
            Serial.print(quantityOf(dp.signalType).c_str());
            Serial.print(":\t ");
            Serial.print(dp.value);
            Serial.print(" ");
            // Get Unit string using Unit enum as index
            Serial.print(unitOf(dp.signalType).c_str());
            Serial.print(" \t@");
            Serial.println(dp.timeStamp);
        }
    }

    Serial.println();
}
