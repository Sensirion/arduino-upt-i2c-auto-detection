/**
 * Basic Usage
 * This sketch shows the minimum required code to be able to access sensor
 * readings.
 *
 *  The circuit:
 *  A single Sensirion sensor should (at least) be connected for this sketch to
 *  give sensible output.
 */

#include "Sensirion_upt_i2c_auto_detection.h"
#include "DefaultDriverConfig.h"

DefaultI2cDetector i2CAutoDetector(Wire);

SensorManager sensorManager(i2CAutoDetector);
bool isEmpty(const ISensor::MeasurementList*[], size_t);
void printData(const ISensor::MeasurementList*[], size_t);

uint maxNumSensors;
const ISensor::MeasurementList** pCurrentData;

void setup() {
    
    Serial.begin(115200);

    int sda_pin = 21;  // Default on esp32 boards
    int scl_pin = 22;
    Wire.begin(sda_pin, scl_pin);

    maxNumSensors = DefaultI2cDetector::CONFIGURED_SENSORS;
    pCurrentData = new const ISensor::MeasurementList* [maxNumSensors] { nullptr };
    Serial.print("Number of configured sensors: "); 
    Serial.println(DefaultI2cDetector::CONFIGURED_SENSORS);

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

}

void printData(const ISensor::MeasurementList* data[], size_t maxNumDataPacks) {
    if (isEmpty(data, maxNumDataPacks)) {
        Serial.println("No sensors seem to be connected.");
        return;
    }

    Serial.println("===========================================");
    for (size_t p = 0; p < maxNumDataPacks; p++) {
        const ISensor::MeasurementList* dataPack = data[p];
        if (!dataPack) {
            continue;
        }
        Serial.println("-------------------------------------------");
        printMeasurementMetaData((*dataPack)[0]);
        Serial.println("-------------------------------------------");
        for (auto& m: *dataPack){
            printMeasurementWithoutMetaData(m);
        }
        
    }

    Serial.println();
}

bool isEmpty(const ISensor::MeasurementList* data[], size_t numDataPacks) {
    for (size_t p = 0; p < numDataPacks; p++) {
        const ISensor::MeasurementList* dataPack = data[p];

        if (dataPack && dataPack->size() > 0) {
            return false;
        }
    }
    return true;
}
