
#include "I2CAutoDetector.h"
#include "SensirionSensorAutoDetection.h"

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
    DataPoint dp;
    if(currentData.dataPoints[0]) {
        dp = currentData.dataPoints[0];
    }
}
