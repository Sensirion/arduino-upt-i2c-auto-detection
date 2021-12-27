#include "Data.h"
#include "I2CAutoDetector.h"
#include "Scd4x.h"
#include "SensirionSensorAutoDetection.h"

void printData(const Data& data) {
    for (size_t i = 0; i < data.getLength(); ++i) {
        const DataPoint& dp = data.dataPoints[i];
        Serial.print(sensorIdStr[static_cast<int>(dp.id)]);
        Serial.print(": ");
        Serial.print(dp.value);
        Serial.print(" ");
        Serial.print(unitStr[static_cast<int>(dp.unit)]);
        Serial.print(" @");
        Serial.println(dp.timeStamp);
    }
    Serial.println();
}

I2CAutoDetector i2CAutoDetector(Wire);
SensorManager sensorManager(i2CAutoDetector);
SensirionI2CScd4x* pScd4xDriver = nullptr;

void setup() {
    Serial.begin(115200);
    Serial.println();
    Wire.begin();
    sensorManager.init();
    delay(5000);
}

void loop() {
    delay(1000);
    // Get data via sensorManager
    AutoDetectorError error = sensorManager.updateData();
    if (error) {
        return;
    }
    const Data& currentData = sensorManager.getData();
    printData(currentData);
}
