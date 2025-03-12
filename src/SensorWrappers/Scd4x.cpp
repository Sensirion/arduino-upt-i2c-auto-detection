#include "SensorWrappers/Scd4x.h"
#include "SensirionCore.h"
#include "Sensirion_UPT_Core.h"

Scd4x::Scd4x(TwoWire& wire) : mWire(wire) {
    mMetadata.deviceType.sensorType = SensorType::SCD4X;
    mMetadata.platform = DevicePlatform::WIRED;
};

uint16_t Scd4x::start() {
    mDriver.begin(mWire, SCD40_I2C_ADDR_62);
    return 0;
}

uint16_t Scd4x::measureAndWrite(Measurement measurements[],
                                const unsigned long timeStamp) {
    uint16_t co2;
    float temp;
    float humi;
    uint16_t error = mDriver.readMeasurement(co2, temp, humi);
    if (error) {
        return error;
    }

    measurements[0].signalType = SignalType::CO2_PARTS_PER_MILLION;
    measurements[0].dataPoint.t_offset = timeStamp;
    measurements[0].dataPoint.value = static_cast<float>(co2);
    measurements[0].metaData = mMetadata;

    measurements[1].signalType = SignalType::TEMPERATURE_DEGREES_CELSIUS;
    measurements[1].dataPoint.t_offset = timeStamp;
    measurements[1].dataPoint.value = temp;
    measurements[1].metaData = mMetadata;

    measurements[2].signalType = SignalType::RELATIVE_HUMIDITY_PERCENTAGE;
    measurements[2].dataPoint.t_offset = timeStamp;
    measurements[2].dataPoint.value = humi;
    measurements[2].metaData = mMetadata;

    return HighLevelError::NoError;
}

uint16_t Scd4x::initializationStep() {
    // stop potentially previously started measurement
    uint16_t error = mDriver.stopPeriodicMeasurement();
    if (error) {
        return error;
    }
    // Sensor Serial no.
    uint64_t serialNumber;
    error = mDriver.getSerialNumber(serialNumber);
    if (error) {
        return error;
    }

    mMetadata.deviceID = serialNumber;

    // Start Measurement
    error = mDriver.startPeriodicMeasurement();
    return error;
}

SensorType Scd4x::getSensorType() const {
    return mMetadata.deviceType.sensorType;
}

MetaData Scd4x::getMetaData() const {
    return mMetadata;
}

size_t Scd4x::getNumberOfDataPoints() const {
    return 3;
}

unsigned long Scd4x::getMinimumMeasurementIntervalMs() const {
    return 5000;
}

unsigned long Scd4x::getInitializationIntervalMs() const {
    // Sensor does not produce measurements for ~12s after
    // startPeriodicMeasurement() is called
    return 12 * 1000;
}

void* Scd4x::getDriver() {
    return reinterpret_cast<void*>(&mDriver);
}
