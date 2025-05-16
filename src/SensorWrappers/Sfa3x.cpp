#include "SensorWrappers/Sfa3x.h"
#include "SensirionCore.h"

Sfa3x::Sfa3x(TwoWire& wire, uint16_t address) : _wire(wire), _address{address} {
    _metaData.deviceType.sensorType = SensorType::SFA3X;
    _metaData.platform = DevicePlatform::WIRED;
};

uint16_t Sfa3x::start() {
    _driver.begin(_wire, _address);
    return 0;
}

uint16_t Sfa3x::measureAndWrite(Measurement measurements[],
                                const unsigned long timeStamp) {
    float hcho;
    float humi;
    float temp;

    uint16_t error = _driver.readMeasuredValues(hcho, humi, temp);
    if (error) {
        return error;
    }

    measurements[0].signalType = SignalType::HCHO_PARTS_PER_BILLION;
    measurements[0].dataPoint.t_offset = timeStamp;
    measurements[0].dataPoint.value = hcho;
    measurements[0].metaData = _metaData;

    measurements[1].signalType = SignalType::RELATIVE_HUMIDITY_PERCENTAGE;
    measurements[1].dataPoint.t_offset = timeStamp;
    measurements[1].dataPoint.value = humi;
    measurements[1].metaData = _metaData;

    measurements[2].signalType = SignalType::TEMPERATURE_DEGREES_CELSIUS;
    measurements[2].dataPoint.t_offset = timeStamp;
    measurements[2].dataPoint.value = temp;
    measurements[2].metaData = _metaData;

    return HighLevelError::NoError;
}

uint16_t Sfa3x::initializationStep() {
    uint16_t error = _driver.stopMeasurement();
    if (error) {
        return error;
    }

    uint8_t serialNumberSize = 32;
    int8_t serialNumber[serialNumberSize];
    error = _driver.getDeviceMarking(serialNumber, serialNumberSize);
    if (error) {
        return error;
    }
    size_t actualLen = strlen((const char*)serialNumber);
    size_t numBytesToCopy = min(8, (int)actualLen);

    uint64_t sensorID = 0;
    for (int i = 0; i < numBytesToCopy - 1; i++) {
        sensorID |= (serialNumber[actualLen - numBytesToCopy - 1 + i]);
        sensorID = sensorID << 8;
    }
    sensorID |= serialNumber[actualLen - 1];

    _metaData.deviceID = sensorID;

    error = _driver.startContinuousMeasurement();
    return error;
}

SensorType Sfa3x::getSensorType() const {
    return _metaData.deviceType.sensorType;
}

MetaData Sfa3x::getMetaData() const {
    return _metaData;
}

size_t Sfa3x::getNumberOfDataPoints() const {
    return 3;
}

unsigned long Sfa3x::getMinimumMeasurementIntervalMs() const {
    return 5000;
}

void* Sfa3x::getDriver() {
    return reinterpret_cast<void*>(&_driver);
}
