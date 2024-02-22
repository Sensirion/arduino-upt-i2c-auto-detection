#include "SensorWrappers/Sfa3x.h"
#include "SensirionCore.h"

uint16_t Sfa3x::start() {
    _driver.begin(_wire);
    return 0;
}

uint16_t Sfa3x::measureAndWrite(Measurement measurements[],
                                const unsigned long timeStamp) {
    int16_t hcho;
    int16_t humi;
    int16_t temp;

    uint16_t error = _driver.readMeasuredValues(hcho, humi, temp);
    if (error) {
        return error;
    }

    MetaData metaData;
    metaData.deviceID = _sensorID;
    metaData.deviceType.sensorType = _sensorType;
    metaData.platform = DevicePlatform::WIRED;

    measurements[0].signalType = SignalType::HCHO_PARTS_PER_BILLION;
    measurements[0].dataPoint.t_offset = timeStamp;
    measurements[0].dataPoint.value = static_cast<float>(hcho) / 5.0f;
    measurements[0].metaData = metaData;

    measurements[1].signalType = SignalType::RELATIVE_HUMIDITY_PERCENTAGE;
    measurements[1].dataPoint.t_offset = timeStamp;
    measurements[1].dataPoint.value = static_cast<float>(humi) / 100.0f;
    measurements[1].metaData = metaData;

    measurements[2].signalType = SignalType::TEMPERATURE_DEGREES_CELSIUS;
    measurements[2].dataPoint.t_offset = timeStamp;
    measurements[2].dataPoint.value = static_cast<float>(temp) / 200.0f;
    measurements[2].metaData = metaData;

    return HighLevelError::NoError;
}

uint16_t Sfa3x::initializationStep() {
    uint16_t error = _driver.stopMeasurement();
    if (error) {
        return error;
    }

    uint8_t serialNumberSize = 32;
    unsigned char serialNumber[serialNumberSize];
    error = _driver.getDeviceMarking(serialNumber, serialNumberSize);
    if (error) {
        return error;
    }
    size_t actualLen = strlen((const char*)serialNumber);
    size_t numBytesToCopy = min(8, (int)actualLen);

    _sensorID = 0;
    for (int i = 0; i < numBytesToCopy - 1; i++) {
        _sensorID |= (serialNumber[actualLen - numBytesToCopy - 1 + i]);
        _sensorID = _sensorID << 8;
    }
    _sensorID |= serialNumber[actualLen - 1];

    error = _driver.startContinuousMeasurement();
    return error;
}

SensorType Sfa3x::getSensorType() const {
    return _sensorType;
}

size_t Sfa3x::getNumberOfDataPoints() const {
    return 3;
}

unsigned long Sfa3x::getMinimumMeasurementIntervalMs() const {
    return 5000;
}

bool Sfa3x::requiresInitializationStep() const {
    return true;
}

void* Sfa3x::getDriver() {
    return reinterpret_cast<void*>(&_driver);
}
