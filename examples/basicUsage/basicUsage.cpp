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

void setup() {
    Serial.begin(115200);
    Wire.begin();
    sensorManager.begin();
}

void loop() {
    delay(500);
    sensorManager.updateData();
    // Get Data object contatining DataPoints
    const Data& currentData = sensorManager.getData();
    // Access first dataPoint
    DataPoint dp = currentData.dataPoints[0];
    // Print DataPoint value
    Serial.println(dp.value);
}
