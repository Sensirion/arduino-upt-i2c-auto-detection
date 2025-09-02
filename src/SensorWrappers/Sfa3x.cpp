#include "SensorWrappers/Sfa3x.h"
#include "SensirionCore.h"

namespace sensirion::upt::i2c_autodetect{

using namespace sensirion::upt::core;

Sfa3x::Sfa3x(TwoWire& wire, uint16_t address) : _wire(wire), _address{address},
    mMetadata{core::SFA3X()}{};

uint16_t Sfa3x::start() {
    _driver.begin(_wire, _address);
    return 0;
}

uint16_t Sfa3x::measureAndWrite(MeasurementList& measurements,
                                const unsigned long timeStamp) {
    float hcho;
    float humi;
    float temperature;

    uint16_t error = _driver.readMeasuredValues(hcho, humi, temperature);
    if (error) {
        return error;
    }
    measurements.emplace_back(mMetadata, 
        core::SignalType::HCHO_PARTS_PER_BILLION,
        core::DataPoint{timeStamp, hcho});

    measurements.emplace_back(mMetadata, 
        core::SignalType::RELATIVE_HUMIDITY_PERCENTAGE,
        core::DataPoint{timeStamp, humi});    

    measurements.emplace_back(mMetadata, 
        core::SignalType::TEMPERATURE_DEGREES_CELSIUS,
        core::DataPoint{timeStamp, temperature});

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

    mMetadata.deviceID = sensorID;

    error = _driver.startContinuousMeasurement();
    return error;
}

core::DeviceType Sfa3x::getDeviceType() const {
    return mMetadata.deviceType;
}

core::MetaData Sfa3x::getMetaData() const {
    return mMetadata;
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
} // namespace sensirion::upt::i2c_autodetect 

