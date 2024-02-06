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
void printData(const MeasurementList**, size_t);
void printMeasurement(const Measurement&);

uint maxNumSensors;
const MeasurementList** pCurrentData;
float t_incr = 0;

void setup() {
    Serial.begin(115200);
    Wire.begin();
    sensorManager.refreshConnectedSensors();

    // Retrieval of sensor driver
    AutoDetectorError error = sensorManager.getSensorDriver<SensirionI2CScd4x>(
        pScd4xDriver, SensorType::SCD4X);

    // Set custom interval for sensor measurement update (default: 5s, lower
    // values are ignored)
    sensorManager.setInterval(7500, SensorType::SCD4X);

    maxNumSensors = sensorManager.getMaxNumberOfSensors();
    pCurrentData = new const MeasurementList* [maxNumSensors] { nullptr };
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
        sensorManager.getSensorDriver<SensirionI2CScd4x>(pScd4xDriver,
                                                         SensorType::SCD4X);
    }
}

void printData(const MeasurementList** data, size_t numDataPacks) {
    Serial.println("Data retrieved via sensor manager:");
    for (size_t p = 0; p < numDataPacks; p++) {
        const MeasurementList* dataPack = data[p];
        if (!dataPack) {
            continue;
        }

        for (size_t i = 0; i < dataPack->getLength(); ++i) {
            printMeasurement(dataPack->getMeasurement(i));
        }
    }

    Serial.println();
}

void printMeasurement(const Measurement &measurement) {
    // Get device and platform descriptive labels
    const char *platformLbl = devicePlatformLabel(
        measurement.metadata.platform, measurement.metadata.devicetype);
    const char *deviceLbl = deviceLabel(measurement.metadata.platform,
                                          measurement.metadata.devicetype);

    // Get deviceID in string representation
    char deviceIDStr[64];
    if (measurement.metadata.platform == DevicePlatform::BLE) {
        sprintf(deviceIDStr, "0x%llx", measurement.metadata.deviceID);
    } else {
        sprintf(deviceIDStr, "%llu", measurement.metadata.deviceID);
    }

    Serial.printf("\nShowing Measurement:\n");

    Serial.printf("  Data Point:\n");
    Serial.printf("    Measured at:\t%lus\n",
                  measurement.datapoint.t_offset / 1000);
    Serial.printf("    Value:\t\t");
    switch (measurement.signaltype) {
    case SignalType::TEMPERATURE_DEGREES_CELSIUS:
    case SignalType::RELATIVE_HUMIDITY_PERCENTAGE:
    case SignalType::VELOCITY_METERS_PER_SECOND:
        Serial.printf("%.1f\n", measurement.datapoint.value);
        break;
    case SignalType::CO2_PARTS_PER_MILLION:
    case SignalType::HCHO_PARTS_PER_BILLION:
    case SignalType::PM1P0_MICRO_GRAMM_PER_CUBIC_METER:
    case SignalType::PM2P5_MICRO_GRAMM_PER_CUBIC_METER:
    case SignalType::PM4P0_MICRO_GRAMM_PER_CUBIC_METER:
    case SignalType::PM10P0_MICRO_GRAMM_PER_CUBIC_METER:
    case SignalType::VOC_INDEX:
    case SignalType::NOX_INDEX:
    case SignalType::GAS_CONCENTRATION:
        Serial.printf("%i\n", static_cast<int>(measurement.datapoint.value));
        break;
    default:
        Serial.printf("%i\n", static_cast<int>(measurement.datapoint.value));
        break;
    }

    Serial.printf("  SignalType:\n");
    Serial.printf("    Physical Quantity:\t%s\n",
                  quantityOf(measurement.signaltype));
    Serial.printf("    Units:\t\t%s\n", unitOf(measurement.signaltype));

    Serial.printf("  Metadata:\n");
    Serial.printf("    Platform:\t\t%s\n", platformLbl);
    Serial.printf("    deviceID:\t\t%s\n", deviceIDStr);
    Serial.printf("    Device Type:\t%s\n", deviceLbl);

    return;
}
