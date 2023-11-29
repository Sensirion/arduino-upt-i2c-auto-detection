/*
    Basic Usage

    This sketch shows the minimum required code to be able to
    access sensor readings.

    The circuit:
    A single Sensirion sensor should (at least) be connected
    for this sketch to give sensible output.
    */

#include "Sensirion_Sensor_Auto_Detection.h"

I2CAutoDetector i2CAutoDetector(Wire);
SensorManager sensorManager(i2CAutoDetector);
bool isEmpty(const Data**, size_t);
void printData(const Data**, size_t);

uint maxNumSensors;
const Data** pCurrentData;

void setup() {
    Serial.begin(115200);
    Wire.begin();

    maxNumSensors = sensorManager.getMaxNumberOfSensors();
    pCurrentData = new const Data*[maxNumSensors];
}

void loop() {
    delay(500);
    sensorManager.updateSensorList();
    sensorManager.updateStateMachines();
    /*
    Data retrieval:

    The library provides a hashmap of pointers to Data objects for each of the
    connected sensors. The referenced Data contains a collection of DataPoints
    corresponding to the latest available readings from the sensor.
    */
    sensorManager.getData(pCurrentData);
    // Print contents
    printData(pCurrentData, maxNumSensors);
}

void printData(const Data** data, size_t maxNumDataPacks) {
    if (isEmpty(data, maxNumDataPacks)) {
        Serial.println("No sensors seem to be connected.");
        return;
    }

    for (size_t p = 0; p < maxNumDataPacks; p++) {
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

bool isEmpty(const Data** data, size_t numDataPacks) {
    for (size_t p = 0; p < numDataPacks; p++) {
        const Data* dataPack = data[p];

        if (dataPack && data[p]->getLength() > 0) {
            return false;
        }
    }
    return true;
}
