#include "I2CAutoDetector.h"
#include "Scd4x.h"
#include "SensirionSensorAutoDetection.h"

I2CAutoDetector i2CAutoDetector(Wire);
SensorManager sensorManager(i2CAutoDetector);

void setup() {
    Serial.begin(115200);
    Serial.println();
    Wire.begin();
    sensorManager.init();
}

void loop() {
    sensorManager.updateData();
    delay(500);
}
