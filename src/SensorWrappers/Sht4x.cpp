#include "SensorWrappers/Sht4x.h"
#include "SensirionCore.h"
#include "Sensirion_UPT_Core.h"

uint16_t Sht4x::start() {
    _driver.begin(_wire);
    return HighLevelError::NoError;
}

uint16_t Sht4x::measureAndWrite(Measurement measurements[],
                                const unsigned long timeStamp) {
    if (_sensorID == 0) {
        // Read sensor identifier at first measurement retrieval
        uint32_t serialNo;
        _driver.serialNumber(serialNo);
        _sensorID = static_cast<uint64_t>(serialNo);
    }

    float temp;
    float humi;
    uint16_t error = _driver.measureHighPrecision(temp, humi);
    if (error) {
        return error;
    }

    MetaData metaData;
    metaData.deviceID = _sensorID;
    metaData.deviceType.sensorType = _sensorType;
    metaData.platform = DevicePlatform::WIRED;

    measurements[0].signalType = SignalType::TEMPERATURE_DEGREES_CELSIUS;
    measurements[0].dataPoint.t_offset = timeStamp;
    measurements[0].dataPoint.value = temp;
    measurements[0].metaData = metaData;

    measurements[1].signalType = SignalType::RELATIVE_HUMIDITY_PERCENTAGE;
    measurements[1].dataPoint.t_offset = timeStamp;
    measurements[1].dataPoint.value = humi;
    measurements[1].metaData = metaData;

    return HighLevelError::NoError;
}

SensorType Sht4x::getSensorType() const {
    return _sensorType;
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
