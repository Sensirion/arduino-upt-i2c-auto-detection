#include "SensorWrappers/Scd4x.h"
#include "SensirionCore.h"
#include "Sensirion_UPT_Core.h"

Scd4x::Scd4x(TwoWire& wire) : _wire(wire) {
    _metaData.deviceType.sensorType = SensorType::SCD4X;
    _metaData.platform = DevicePlatform::WIRED;
};

uint16_t Scd4x::start() {
    _driver.begin(_wire);
    return 0;
}

uint16_t Scd4x::measureAndWrite(Measurement measurements[],
                                const unsigned long timeStamp) {
    uint16_t co2;
    float temp;
    float humi;
    uint16_t error = _driver.readMeasurement(co2, temp, humi);
    if (error) {
        return error;
    }

    measurements[0].signalType = SignalType::CO2_PARTS_PER_MILLION;
    measurements[0].dataPoint.t_offset = timeStamp;
    measurements[0].dataPoint.value = static_cast<float>(co2);
    measurements[0].metaData = _metaData;

    measurements[1].signalType = SignalType::TEMPERATURE_DEGREES_CELSIUS;
    measurements[1].dataPoint.t_offset = timeStamp;
    measurements[1].dataPoint.value = temp;
    measurements[1].metaData = _metaData;

    measurements[2].signalType = SignalType::RELATIVE_HUMIDITY_PERCENTAGE;
    measurements[2].dataPoint.t_offset = timeStamp;
    measurements[2].dataPoint.value = humi;
    measurements[2].metaData = _metaData;

    return HighLevelError::NoError;
}

uint16_t Scd4x::initializationStep() {
    // stop potentially previously started measurement
    uint16_t error = _driver.stopPeriodicMeasurement();
    if (error) {
        return error;
    }
    // Sensor Serial no.
    uint16_t word0, word1, word2;
    error = _driver.getSerialNumber(word0, word1, word2);
    if (error) {
        return error;
    }
    uint64_t sensorID = 0;
    sensorID |=
        (static_cast<uint64_t>(word0) << 16 * 2) | (word1 << 16) | word2;

    _metaData.deviceID = sensorID;

    // Start Measurement
    error = _driver.startPeriodicMeasurement();
    return error;
}

SensorType Scd4x::getSensorType() const {
    return _metaData.deviceType.sensorType;
}

MetaData Scd4x::getMetaData() const {
    return _metaData;
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
    return reinterpret_cast<void*>(&_driver);
}
