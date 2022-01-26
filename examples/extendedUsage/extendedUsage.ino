/*
    Extended Usage

    This sketch demonstrates optional features of the library, namely:
        - Retrieval of the underlying Sensirion sensor driver from the
        SensorManager object for custom initialization
        - Setting of sampling interval for individual sensors
        - Acquisition of sensor reading metadata (name, unit, timestamp).
        This is implemented in the printData function.

    The circuit:
    Any number of sensirion (different) Sensirion sensors may be connected
    to the I2C bus. The driver retrieval expects the SCD4X sensor to be
    connected.
    */

#include "I2CAutoDetector.h"
#include "SensirionSensorAutoDetection.h"

I2CAutoDetector i2CAutoDetector(Wire);
SensorManager sensorManager(i2CAutoDetector);

// Pointer to sensor driver to be retrieved from sensorManager
// To retrieve the drivers of other sensors, change:
// - the type of the pointer below
// - the SensorManager::getSensorDriver template argument
// - the sensorId argument
// accordingly.
SensirionI2CScd4x* pScd4xDriver = nullptr;

void setup() {

    Serial.begin(115200);
    Serial.println();
    Wire.begin();
    sensorManager.begin();

    // Retrieval of sensor driver
    AutoDetectorError error = sensorManager.getSensorDriver<SensirionI2CScd4x>(
        pScd4xDriver, SensorId::SCD4X);
    if (error) {
        Serial.println("Scd4x driver retrieval failed!");
    } else {
        Serial.println("SCD4X driver retrieval succeeded!\n");
        delay(5000);  // Wait for SCD4X to finish measuring
        uint16_t co2;
        float temperature;
        float humidity;
        pScd4xDriver->readMeasurement(co2, temperature, humidity);
        Serial.println("Data retrieved directly from sensor driver:");
        Serial.print("co2: ");
        Serial.println(co2);
        Serial.print("temperature: ");
        Serial.println(temperature);
        Serial.print("humidity: ");
        Serial.println(humidity);
        Serial.println();
    }

    delay(5000);  // Wait for SCD4X to finish measuring

    // Setting the SCD4X measurement interval
    sensorManager.setInterval(10000, SensorId::SCD4X);
}

void loop() {
    delay(500);
    sensorManager.updateData();
    const Data& currentData = sensorManager.getData();
    printData(currentData);
}

void printData(const Data& data) {
    for (size_t i = 0; i < data.getLength(); ++i) {
        const DataPoint& dp = data.dataPoints[i];
        // Get SensorId string using SensorId enum as index
        Serial.print(sensorIdStr[static_cast<int>(dp.id)]);
        Serial.print(": ");
        Serial.print(dp.value);
        Serial.print(" ");
        // Get Unit string using Unit enum as index
        Serial.print(unitStr[static_cast<int>(dp.unit)]);
        Serial.print(" @");
        Serial.println(dp.timeStamp);
    }
    Serial.println();
}