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
bool isEmpty(const MeasurementList**, size_t);
void printData(const MeasurementList**, size_t);
void printMetaData(const MetaData& metaData);
void printMeasurementDataPointAndSignalType(const Measurement&);

uint maxNumSensors;
const MeasurementList** pCurrentData;

void setup() {
    Serial.begin(115200);
    Wire.begin();

    maxNumSensors = sensorManager.getMaxNumberOfSensors();
    pCurrentData = new const MeasurementList* [maxNumSensors] { nullptr };
}

void loop() {
    delay(500);
    /*
    Data retrieval:

    The library provides a hashmap of pointers to MeasurementList objects for
    each of the connected sensors. The referenced Data contains a collection of
    Measurements corresponding to the latest available readings from the sensor.
    The hashmap has a lower resolution than SensorType, because it treats
    sensors sharing the same I2C address as equals. Read out the detected
    sensorType from the metadata in the Measurements.
    */
    sensorManager.refreshAndGetSensorReadings(pCurrentData);
    // Print contents
    printData(pCurrentData, maxNumSensors);
    // Reset hashmap
    for (size_t i = 0; i < maxNumSensors; i++) {
        pCurrentData[i] = nullptr;
    }
}

void printData(const MeasurementList** data, size_t maxNumDataPacks) {
    if (isEmpty(data, maxNumDataPacks)) {
        Serial.println("No sensors seem to be connected.");
        return;
    }

    Serial.println("===========================================");
    for (size_t p = 0; p < maxNumDataPacks; p++) {
        const MeasurementList* dataPack = data[p];
        if (!dataPack) {
            continue;
        }

        printMetaData(dataPack->getMeasurement(0).metaData);
        for (size_t i = 0; i < dataPack->getLength(); ++i) {
            printMeasurementDataPointAndSignalType(dataPack->getMeasurement(i));
        }
    }

    Serial.println();
}

bool isEmpty(const MeasurementList** data, size_t numDataPacks) {
    for (size_t p = 0; p < numDataPacks; p++) {
        const MeasurementList* dataPack = data[p];

        if (dataPack && data[p]->getLength() > 0) {
            return false;
        }
    }
    return true;
}

void printMetaData(const MetaData& metaData) {
    // Get device and platform descriptive labels
    const char* platformLbl =
        devicePlatformLabel(metaData.platform, metaData.deviceType);
    const char* deviceLbl = deviceLabel(metaData.platform, metaData.deviceType);

    // Get deviceID in string representation
    char deviceIDStr[64];
    if (metaData.platform == DevicePlatform::BLE) {
        sprintf(deviceIDStr, "0x%llx", metaData.deviceID);
    } else {
        sprintf(deviceIDStr, "%llu", metaData.deviceID);
    }

    Serial.println("-------------------------------------------");
    Serial.printf("Device metadata:\n");
    Serial.printf("  Platform:\t\t%s\n", platformLbl);
    Serial.printf("  DeviceID:\t\t%s\n", deviceIDStr);
    Serial.printf("  Device Type:\t\t%s\n", deviceLbl);
    Serial.println("-------------------------------------------");
}

void printMeasurementDataPointAndSignalType(const Measurement& measurement) {
    Serial.printf("\nShowing Measurement:\n");
    Serial.printf("  Data Point:\n");
    Serial.printf("    Measured at:\t%lus\n",
                  measurement.dataPoint.t_offset / 1000);
    Serial.printf("    Value:\t\t");
    switch (measurement.signalType) {
        case SignalType::TEMPERATURE_DEGREES_CELSIUS:
        case SignalType::RELATIVE_HUMIDITY_PERCENTAGE:
        case SignalType::VELOCITY_METERS_PER_SECOND:
        case SignalType::GAS_CONCENTRATION_VOLUME_PERCENTAGE:
            Serial.printf("%.1f\n", measurement.dataPoint.value);
            break;
        case SignalType::CO2_PARTS_PER_MILLION:
        case SignalType::HCHO_PARTS_PER_BILLION:
        case SignalType::PM1P0_MICRO_GRAMM_PER_CUBIC_METER:
        case SignalType::PM2P5_MICRO_GRAMM_PER_CUBIC_METER:
        case SignalType::PM4P0_MICRO_GRAMM_PER_CUBIC_METER:
        case SignalType::PM10P0_MICRO_GRAMM_PER_CUBIC_METER:
        case SignalType::RAW_VOC_INDEX:
        case SignalType::RAW_NOX_INDEX:
        case SignalType::VOC_INDEX:
        case SignalType::NOX_INDEX:
            Serial.printf("%i\n",
                          static_cast<int>(measurement.dataPoint.value));
            break;
        default:
            Serial.printf("%i\n",
                          static_cast<int>(measurement.dataPoint.value));
            break;
    }

    Serial.printf("  SignalType:\n");
    Serial.printf("    Physical Quantity:\t%s\n",
                  quantityOf(measurement.signalType));
    Serial.printf("    Units:\t\t%s\n", unitOf(measurement.signalType));

    return;
}
