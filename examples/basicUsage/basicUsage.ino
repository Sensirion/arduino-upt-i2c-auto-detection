/**
 * Basic Usage
 * This sketch shows the minimum required code to be able to access sensor
 * readings.
 *
 *  The circuit:
 *  A single Sensirion sensor should (at least) be connected for this sketch to
 *  give sensible output.
 */

#include "Sensirion_Sensor_Auto_Detection.h"

I2CAutoDetector i2CAutoDetector(Wire);
SensorManager sensorManager(i2CAutoDetector);
bool isEmpty(const MeasurementList**, size_t);
void printData(const MeasurementList**, size_t);

uint maxNumSensors;
const MeasurementList** pCurrentData;

void setup() {
    Serial.begin(115200);

    int sda_pin = 21;  // Default on esp32 boards
    int scl_pin = 22;
    Wire.begin(sda_pin, scl_pin);

    maxNumSensors = sensorManager.getMaxNumberOfSensors();
    pCurrentData = new const MeasurementList* [maxNumSensors] { nullptr };
}

void loop() {
    delay(500);
    /*
    Data retrieval:

    The library provides a hashmap of pointers to MeasurementList objects for
    each of the connected sensors. The referenced Data contains a collection of
    Measurements corresponding to the latest available readings from the sensor.
    The hashmap has a lower resolution than SensorType, because it treats
    sensors sharing the same I2C address as equals. Read out the detected
    sensorType from the metadata in the Measurements.
    */
    sensorManager.refreshAndGetSensorReadings(pCurrentData);
    // Print contents
    printData(pCurrentData, maxNumSensors);
    // Reset hashmap
    for (size_t i = 0; i < maxNumSensors; i++) {
        pCurrentData[i] = nullptr;
    }
}

void printData(const MeasurementList** data, size_t maxNumDataPacks) {
    if (isEmpty(data, maxNumDataPacks)) {
        Serial.println("No sensors seem to be connected.");
        return;
    }

    Serial.println("===========================================");
    for (size_t p = 0; p < maxNumDataPacks; p++) {
        const MeasurementList* dataPack = data[p];
        if (!dataPack) {
            continue;
        }
        Serial.println("-------------------------------------------");
        printMeasurementMetaData(dataPack->getMeasurement(0));
        Serial.println("-------------------------------------------");
        for (size_t i = 0; i < dataPack->getLength(); ++i) {
            printMeasurementWithoutMetaData(dataPack->getMeasurement(i));
        }
    }

    Serial.println();
}

bool isEmpty(const MeasurementList** data, size_t numDataPacks) {
    for (size_t p = 0; p < numDataPacks; p++) {
        const MeasurementList* dataPack = data[p];

        if (dataPack && data[p]->getLength() > 0) {
            return false;
        }
    }
    return true;
}
