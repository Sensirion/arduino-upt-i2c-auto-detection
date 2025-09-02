#include "SensorWrappers/Svm4x.h"
#include "SensirionCore.h"

namespace sensirion::upt::i2c_autodetect{

Svm4x::Svm4x(TwoWire& wire, uint16_t address) : _wire(wire), 
    _address{address},
    mMetaData{sensirion::upt::core::SVM41()} {};

uint16_t Svm4x::start() {
    _driver.begin(_wire);
    return 0;
}

uint16_t Svm4x::measureAndWrite(MeasurementList& measurements,
                                const unsigned long timeStamp) {
    float humidity;
    float temperature;
    float vocIndex;
    float noxIndex;
    uint16_t error =
        _driver.readMeasuredValues(humidity, temperature, vocIndex, noxIndex);
    if (error) {
        return error;
    }

    measurements.emplace_back(mMetaData, 
        sensirion::upt::core::SignalType::RELATIVE_HUMIDITY_PERCENTAGE,
        sensirion::upt::core::DataPoint{timeStamp, humidity});


    measurements.emplace_back(mMetaData, 
        sensirion::upt::core::SignalType::TEMPERATURE_DEGREES_CELSIUS,
        sensirion::upt::core::DataPoint{timeStamp, temperature});

    measurements.emplace_back(mMetaData, 
        sensirion::upt::core::SignalType::NOX_INDEX,
        sensirion::upt::core::DataPoint{timeStamp, noxIndex});


    return HighLevelError::NoError;
}

uint16_t Svm4x::initializationStep() {
    uint16_t error = _driver.stopMeasurement();
    if (error) {
        return error;
    }
    // Sensor Serial No.
    uint8_t serialNumber[32];
    uint8_t serialNumberSize = 32;
    error = _driver.getSerialNumber(serialNumber, serialNumberSize);
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

    mMetaData.deviceID = sensorID;

    // Start Measurement
    return _driver.startMeasurement();
}

sensirion::upt::core::DeviceType Svm4x::getDeviceType() const {
    return mMetaData.deviceType;
}

sensirion::upt::core::MetaData Svm4x::getMetaData() const {
    return mMetaData;
}

size_t Svm4x::getNumberOfDataPoints() const {
    return 4;
}

unsigned long Svm4x::getMinimumMeasurementIntervalMs() const {
    return 1000;
}

void* Svm4x::getDriver() {
    return reinterpret_cast<void*>(&_driver);
}
} // namespace sensirion::upt::i2c_autodetect 