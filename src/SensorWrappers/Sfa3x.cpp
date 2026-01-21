#include "SensorWrappers/Sfa3x.h"
#include "SensirionCore.h"

namespace sensirion::upt::i2c_autodetect {

using namespace sensirion::upt::core;

Sfa3x::Sfa3x(TwoWire& wire, uint16_t address)
    : mWire(wire), mAddress{address}, mMetadata{SFA3X()} {};

uint16_t Sfa3x::start() {
    mDriver.begin(mWire, mAddress);
    return 0;
}

uint16_t Sfa3x::measureAndWrite(MeasurementList& measurements,
                                const unsigned long timeStamp) {
    float hcho;
    float humi;
    float temperature;

    const uint16_t error = mDriver.readMeasuredValues(hcho, humi, temperature);
    if (error) {
        return error;
    }
    measurements.emplace_back(mMetadata, SignalType::HCHO_PARTS_PER_BILLION,
                              DataPoint{timeStamp, hcho});

    measurements.emplace_back(mMetadata,
                              SignalType::RELATIVE_HUMIDITY_PERCENTAGE,
                              DataPoint{timeStamp, humi});

    measurements.emplace_back(mMetadata,
                              SignalType::TEMPERATURE_DEGREES_CELSIUS,
                              DataPoint{timeStamp, temperature});

    return NoError;
}

uint16_t Sfa3x::initializationStep() {
    uint16_t error = mDriver.deviceReset();
    if (error) {
        return error;
    }

    constexpr uint8_t serialNumberSize = 32;
    int8_t serialNumber[serialNumberSize];
    error = mDriver.getDeviceMarking(serialNumber, serialNumberSize);
    if (error) {
        return error;
    }
    const size_t actualLen =
        strlen(reinterpret_cast<const char*>(serialNumber));
    const size_t numBytesToCopy = min(8, static_cast<int>(actualLen));

    uint64_t sensorID = 0;
    for (int i = 0; i < numBytesToCopy - 1; i++) {
        sensorID |= (serialNumber[actualLen - numBytesToCopy - 1 + i]);
        sensorID = sensorID << 8;
    }
    sensorID |= serialNumber[actualLen - 1];

    mMetadata.deviceID = sensorID;

    error = mDriver.startContinuousMeasurement();
    return error;
}

DeviceType Sfa3x::getDeviceType() const {
    return mMetadata.deviceType;
}

MetaData Sfa3x::getMetaData() const {
    return mMetadata;
}

size_t Sfa3x::getNumberOfDataPoints() const {
    return 3;
}

uint8_t Sfa3x::getI2CAddress() const {
    return mAddress;
};

unsigned long Sfa3x::getMinimumMeasurementIntervalMs() const {
    return 5000;
}

bool Sfa3x::probe() {
    constexpr uint8_t markingSize = 32;
    int8_t markingStr[markingSize];
    // reset device before probing
    mDriver.deviceReset();
    const uint16_t error = mDriver.getDeviceMarking(markingStr, markingSize);
    return (error == NoError);
}

void* Sfa3x::getDriver() {
    return &mDriver;
}
}  // namespace sensirion::upt::i2c_autodetect
