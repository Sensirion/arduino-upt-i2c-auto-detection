#include "I2CAutoDetector.h"
#include "Scd4x.h"
#include "SensirionSensorAutoDetection.h"

SensorList sl;
I2CAutoDetector i2CAutoDetector(Wire);

void setup() {
    Serial.begin(115200);
    Serial.println();
    Wire.begin();
    i2CAutoDetector.findSensors(sl);
}

void loop() {
}
