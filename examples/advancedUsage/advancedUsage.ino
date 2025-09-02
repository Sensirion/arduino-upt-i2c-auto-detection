/*
Advanced Usage

This sketch demonstrates optional features of the library, namely retrieval of
the underlying Sensirion sensor driver from the SensorManager object for custom
initialization

The circuit:
Any number of sensirion (different) Sensirion sensors may be connected
to the I2C bus. The driver retrieval expects at least the SCD4X sensor to be
connected. (Attempts to call sensor actions while the sensor is not connected
causes a program crash)

Attempts to use this feature simultaneously with the autodetect library may
saturate the sensor with commands, thus throwing errors resulting in failure to
perform commands. Thus, only use the driver to perform actions not handled by
the sensor manager.
*/

#include "Arduino.h"
#include "SensirionI2cScd4x.h"
#include "Sensirion_upt_i2c_auto_detection.h"
#include <cmath>
#include "DefaultDriverConfig.h"

DefaultI2cDetector i2CAutoDetector(Wire);

SensorManager sensorManager(i2CAutoDetector);

/*
Pointer to sensor driver to be retrieved from sensorManager
To retrieve the drivers of other sensors, change:
    - the type of the pointer below
    - the SensorManager::getSensorDriver template argument
    - the sensorId argument
accordingly.
*/

SensirionI2cScd4x* pScd4xDriver = nullptr;
void printData(const ISensor::MeasurementList*[], size_t);

uint maxNumSensors;
const ISensor::MeasurementList** pCurrentData;
float t_incr = 0;

void setup() {
    Serial.begin(115200);

    int sda_pin = 21;  // Default on esp32 boards
    int scl_pin = 22;
    Wire.begin(sda_pin, scl_pin);

    maxNumSensors = DefaultI2cDetector::CONFIGURED_SENSORS;
    Wire.begin(sda_pin, scl_pin);
    sensorManager.refreshConnectedSensors();

    // Retrieval of sensor driver
    AutoDetectorError error = sensorManager.getSensorDriver<SensirionI2cScd4x>(
        pScd4xDriver, sensirion::upt::core::SCD4X());

    if (error != NO_ERROR) {
        Serial.print("Encountered error while getting sensor driver (code ");
        Serial.print(error);
        Serial.println(")");
    }

    // Set custom interval for sensor measurement update (default: 5s, lower
    // values are ignored)
    sensorManager.setInterval(7500, sensirion::upt::core::SCD4X());

    maxNumSensors = sensorManager.getMaxNumberOfSensors();
    pCurrentData = new const ISensor::MeasurementList* [maxNumSensors] { nullptr };
}

void loop() {
    delay(5000);

    if (pScd4xDriver) {
        // Perform some action with the driver

        //      CAUTION: WILL MODIFY SENSOR BEHAVIOUR AT HARDWARE LEVEL

        pScd4xDriver->setAmbientPressure(
            1000 + 500 * std::sin(t_incr * 2.0 * 3.1415 / 10.0 + 0.5 * 3.1415));
        // (to reset to 1 atm run the following line:
        // pScd4xDriver->setAmbientPressure(1013.25);

        t_incr++;

        // Data should update every second loop, because of the custom update
        // interval
        sensorManager.executeSensorCommunication();
        sensorManager.getSensorReadings(pCurrentData);
        printData(pCurrentData, maxNumSensors);
    } else {
        Serial.println(
            "Please connect a Sensirion SCD4X CO2 sensor on the i2c bus.");
        sensorManager.refreshConnectedSensors();
        sensorManager.getSensorDriver<SensirionI2cScd4x>(pScd4xDriver,
                                                         sensirion::upt::core::SCD4X());
    }
}

void printData(const ISensor::MeasurementList* data[], size_t numDataPacks) {
    Serial.println("===========================================");
    Serial.println("Data retrieved via sensor manager:");
    for (size_t p = 0; p < numDataPacks; p++) {
        const ISensor::MeasurementList* dataPack = data[p];
        if (!dataPack) {
            continue;
        }

        Serial.println("-------------------------------------------");
        printMeasurementMetaData((*dataPack)[0]);
        Serial.println("-------------------------------------------");
        for( const auto &m: *dataPack){
            printMeasurementWithoutMetaData(m);
        }
    }

    Serial.println();
}
