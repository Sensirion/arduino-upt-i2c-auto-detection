#include "I2CAutoDetector.h"
#include "Scd4x.h"
#include "SensirionSensorAutoDetection.h"

SensorList sl;
I2CAutoDetector* pDetector;
SensorManager* pSm;

Scd4x scd(Wire);
int i;
void setup() {
    Serial.begin(115200);
    Serial.println();
    Wire.begin();
    pDetector = new I2CAutoDetector(Wire);
    pSm = new SensorManager(*pDetector);
    pDetector->findSensors(sl);
}

void loop() {
    Serial.println("loop");
    delay(1000);
    i++;
}
