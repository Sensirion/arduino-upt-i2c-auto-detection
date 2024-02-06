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
void printMeasurement(const Measurement&);

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

    The library provides a hashmap of pointers to MeasurementList objects for each
    of the connected sensors. The referenced Data contains a collection of
    DataPoints corresponding to the latest available readings from the sensor.
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

    for (size_t p = 0; p < maxNumDataPacks; p++) {
        const MeasurementList* dataPack = data[p];
        if (!dataPack) {
            continue;
        }

        for (size_t i = 0; i < dataPack->getLength(); ++i) {
            printMeasurement(dataPack->getMeasurement(i));
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

void printMeasurement(const Measurement &measurement) {
    // Get device and platform descriptive labels
    const char *platformLbl = devicePlatformLabel(
        measurement.metadata.platform, measurement.metadata.devicetype);
    const char *deviceLbl = deviceLabel(measurement.metadata.platform,
                                          measurement.metadata.devicetype);

    // Get deviceID in string representation
    char deviceIDStr[64];
    if (measurement.metadata.platform == DevicePlatform::BLE) {
        sprintf(deviceIDStr, "0x%llx", measurement.metadata.deviceID);
    } else {
        sprintf(deviceIDStr, "%llu", measurement.metadata.deviceID);
    }

    Serial.printf("\nShowing Measurement:\n");

    Serial.printf("  Data Point:\n");
    Serial.printf("    Measured at:\t%lus\n",
                  measurement.datapoint.t_offset / 1000);
    Serial.printf("    Value:\t\t");
    switch (measurement.signaltype) {
    case SignalType::TEMPERATURE_DEGREES_CELSIUS:
    case SignalType::RELATIVE_HUMIDITY_PERCENTAGE:
    case SignalType::VELOCITY_METERS_PER_SECOND:
        Serial.printf("%.1f\n", measurement.datapoint.value);
        break;
    case SignalType::CO2_PARTS_PER_MILLION:
    case SignalType::HCHO_PARTS_PER_BILLION:
    case SignalType::PM1P0_MICRO_GRAMM_PER_CUBIC_METER:
    case SignalType::PM2P5_MICRO_GRAMM_PER_CUBIC_METER:
    case SignalType::PM4P0_MICRO_GRAMM_PER_CUBIC_METER:
    case SignalType::PM10P0_MICRO_GRAMM_PER_CUBIC_METER:
    case SignalType::VOC_INDEX:
    case SignalType::NOX_INDEX:
    case SignalType::GAS_CONCENTRATION:
        Serial.printf("%i\n", static_cast<int>(measurement.datapoint.value));
        break;
    default:
        Serial.printf("%i\n", static_cast<int>(measurement.datapoint.value));
        break;
    }

    Serial.printf("  SignalType:\n");
    Serial.printf("    Physical Quantity:\t%s\n",
                  quantityOf(measurement.signaltype));
    Serial.printf("    Units:\t\t%s\n", unitOf(measurement.signaltype));

    Serial.printf("  Metadata:\n");
    Serial.printf("    Platform:\t\t%s\n", platformLbl);
    Serial.printf("    deviceID:\t\t%s\n", deviceIDStr);
    Serial.printf("    Device Type:\t%s\n", deviceLbl);

    return;
}
