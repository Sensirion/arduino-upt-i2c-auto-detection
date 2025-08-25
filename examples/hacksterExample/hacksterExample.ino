/*
Example code for the hackster.io article showcasing how to use this library:
https://www.hackster.io/sensirion-software/easily-read-out-sensirion-sensor-measurements-on-arduino-9c1862
*/

#include "Arduino.h"
#include "Sensirion_upt_i2c_auto_detection.h"
#include "Wire.h"
#include "DefaultDriverConfig.h"

using namespace sensirion::upt::core;

DefaultI2cDetector i2CAutoDetector(Wire);
    
SensorManager sensorManager(i2CAutoDetector);

int maxNumSensors;
const ISensor::MeasurementList **dataPointers;

void setup() {
  Serial.println("setup");
  // 1.1 Initialize serial port at 115200 baud
  Serial.begin(115200);

  // 1.2 Initialize the I2C bus on pins 21 (SDA) and 22 (SCL)
  int sda_pin = 21;
  int scl_pin = 22;
  Wire.begin(sda_pin, scl_pin);

  // 1.3 Build the "reverse PO Box" we'll use to retrieve sensor data
  maxNumSensors = maxNumSensors = DefaultI2cDetector::CONFIGURED_SENSORS;
  dataPointers = new const ISensor::MeasurementList *[maxNumSensors] { nullptr };

};

void loop() {

  // 2.1 Task SensorManager to fetch sensor data
  sensorManager.refreshAndGetSensorReadings(dataPointers);

  // 2.2 Peek into the non-empty mailboxes and show their contents
  for (int i = 0; i < maxNumSensors; i++) {
    if (dataPointers[i] != nullptr) {
      const ISensor::MeasurementList& measurementList = *dataPointers[i];
      Serial.print("Showing measurements for sensor ");
      deviceLabel(measurementList[0].metaData.deviceType);
      for( const auto &measurement: measurementList){
        Serial.print("Measured ");
        Serial.print(quantityOf(measurement.signalType));
        Serial.print(": ");
        Serial.print(measurement.dataPoint.value);
        Serial.print(" ");
        Serial.println(unitOf(measurement.signalType));

      }
      Serial.println();
    }
  }

  // 2.3 Clear all mailboxes
  for (int i = 0; i < maxNumSensors; i++) {
    dataPointers[i] = nullptr;
  }

  // 2.4 Wait for 1000 milliseconds
  delay(1000);
};
