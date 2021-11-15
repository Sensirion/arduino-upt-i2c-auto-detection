#include "Scd4x.h"
#include "SensirionSensorAutoDetection.h"
#include "I2CAutoDetector.h"


I2CAutoDetector detector(Wire);
SensorManager sm(detector);

Scd4x scd(Wire);

void setup() {
    Serial.begin(115200);
    Serial.println();
    Wire.begin();
}

void loop() {
    Serial.println("loop");
    delay(1000);
}
