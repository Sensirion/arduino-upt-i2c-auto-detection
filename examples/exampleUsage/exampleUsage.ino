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

void setup() {
    Serial.begin(115200);
    Serial.println();
    Wire.begin();
    sensorManager.init();
    /** interval = 2 x loop delay
     *  Thus the data will only be updated every 2nd loop
     *  while being printed every loop.
     */
    sensorManager.setInterval(10000, SensorId::SCD4X);
    delay(100);
}

void loop() {
    delay(5000);
    AutoDetectorError error = sensorManager.updateData();
    if (error) {
        return;
    }
    const Data& currentData = sensorManager.getData();
    printData(currentData);
}
