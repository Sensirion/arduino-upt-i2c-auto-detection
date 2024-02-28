#include "SensorWrappers/Sht4x.h"
#include "SensirionCore.h"
#include "Sensirion_UPT_Core.h"

Sht4x::Sht4x(TwoWire& wire) : _wire(wire) {
    _metaData.deviceType.sensorType = SensorType::SHT4X;
    _metaData.platform = DevicePlatform::WIRED;
};

uint16_t Sht4x::start() {
    _driver.begin(_wire);
    return HighLevelError::NoError;
}

uint16_t Sht4x::measureAndWrite(Measurement measurements[],
                                const unsigned long timeStamp) {
    float temp;
    float humi;
    uint16_t error = _driver.measureHighPrecision(temp, humi);
    if (error) {
        return error;
    }

    measurements[0].signalType = SignalType::TEMPERATURE_DEGREES_CELSIUS;
    measurements[0].dataPoint.t_offset = timeStamp;
    measurements[0].dataPoint.value = temp;
    measurements[0].metaData = _metaData;

    measurements[1].signalType = SignalType::RELATIVE_HUMIDITY_PERCENTAGE;
    measurements[1].dataPoint.t_offset = timeStamp;
    measurements[1].dataPoint.value = humi;
    measurements[1].metaData = _metaData;

    return HighLevelError::NoError;
}

uint16_t Sht4x::initializationStep() {
    uint32_t serialNo;
    uint16_t error = _driver.serialNumber(serialNo);
    if (error) {
        return error;
    }
    _metaData.deviceID = static_cast<uint64_t>(serialNo);
    return error;
}

SensorType Sht4x::getSensorType() const {
    return _metaData.deviceType.sensorType;
}

MetaData Sht4x::getMetaData() const {
    return _metaData;
}

size_t Sht4x::getNumberOfDataPoints() const {
    return 2;
}

unsigned long Sht4x::getMinimumMeasurementIntervalMs() const {
    return 9;
}

void* Sht4x::getDriver() {
    return reinterpret_cast<void*>(&_driver);
}
