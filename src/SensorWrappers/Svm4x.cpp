#include "SensorWrappers/Svm4x.h"
#include "SensirionCore.h"

namespace sensirion::upt::i2c_autodetect{

Svm4x::Svm4x(TwoWire& wire, uint16_t address) : mWire(wire), 
    mAddress{address},
    mMetaData{core::SVM41()} {};

uint16_t Svm4x::start() {
    mDriver.begin(mWire);
    return 0;
}

uint16_t Svm4x::measureAndWrite(MeasurementList& measurements,
                                const unsigned long timeStamp) {
    float humidity;
    float temperature;
    float vocIndex;
    float noxIndex;
    uint16_t error =
        mDriver.readMeasuredValues(humidity, temperature, vocIndex, noxIndex);
    if (error) {
        return error;
    }

    measurements.emplace_back(mMetaData, 
        core::SignalType::RELATIVE_HUMIDITY_PERCENTAGE,
        core::DataPoint{timeStamp, humidity});


    measurements.emplace_back(mMetaData, 
        core::SignalType::TEMPERATURE_DEGREES_CELSIUS,
        core::DataPoint{timeStamp, temperature});

    measurements.emplace_back(mMetaData, 
        core::SignalType::NOX_INDEX,
        core::DataPoint{timeStamp, noxIndex});


    return HighLevelError::NoError;
}

uint16_t Svm4x::initializationStep() {
    uint16_t error = mDriver.stopMeasurement();
    if (error) {
        return error;
    }
    // Sensor Serial No.
    uint8_t serialNumber[32];
    uint8_t serialNumberSize = 32;
    error = mDriver.getSerialNumber(serialNumber, serialNumberSize);
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
    return mDriver.startMeasurement();
}

core::DeviceType Svm4x::getDeviceType() const {
    return mMetaData.deviceType;
}

core::MetaData Svm4x::getMetaData() const {
    return mMetaData;
}

size_t Svm4x::getNumberOfDataPoints() const {
    return 4;
}

uint8_t Svm4x::getI2CAddress() const {
    return mAddress;
};

unsigned long Svm4x::getMinimumMeasurementIntervalMs() const {
    return 1000;
}

void* Svm4x::getDriver() {
    return reinterpret_cast<void*>(&mDriver);
}
} // namespace sensirion::upt::i2c_autodetect 