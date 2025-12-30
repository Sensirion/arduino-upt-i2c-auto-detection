#include "SensorWrappers/Sht4x.h"
#include "SensirionCore.h"
#include "Sensirion_UPT_Core.h"

namespace sensirion::upt::i2c_autodetect{

using namespace sensirion::upt::core;

Sht4x::Sht4x(TwoWire& wire, uint16_t address) : mWire(wire), mAddress{address},
    mMetadata{core::SHT4X()}{};

uint16_t Sht4x::start() {
    mDriver.begin(mWire, mAddress);
    return HighLevelError::NoError;
}

uint16_t Sht4x::measureAndWrite(MeasurementList& measurements,
                                const unsigned long timeStamp) {
    float temperature;
    float humi;
    uint16_t error = mDriver.measureHighPrecision(temperature, humi);
    if (error) {
        return error;
    }

    measurements.emplace_back(mMetadata, 
        core::SignalType::TEMPERATURE_DEGREES_CELSIUS,
        core::DataPoint{timeStamp, temperature});

    measurements.emplace_back(mMetadata, 
        core::SignalType::RELATIVE_HUMIDITY_PERCENTAGE,
        core::DataPoint{timeStamp, humi});

    return HighLevelError::NoError;
}

uint16_t Sht4x::initializationStep() {
    uint32_t serialNo;
    uint16_t error = mDriver.serialNumber(serialNo);
    if (error) {
        return error;
    }
    mMetadata.deviceID = static_cast<uint64_t>(serialNo);
    return error;
}

DeviceType Sht4x::getDeviceType() const {
    return mMetadata.deviceType;
}

MetaData Sht4x::getMetaData() const {
    return mMetadata;
}

size_t Sht4x::getNumberOfDataPoints() const {
    return 2;
}

unsigned long Sht4x::getMinimumMeasurementIntervalMs() const {
    return 9;
}

void* Sht4x::getDriver() {
    return reinterpret_cast<void*>(&mDriver);
}

uint8_t Sht4x::getI2CAddress() const {
    return mAddress;
};

bool Sht4x::probe() {
    // Use serial number as a probe method
    uint32_t serial;
    uint16_t error = mDriver.serialNumber(serial);
    return (error == HighLevelError::NoError);
}
} // namespace sensirion::upt::i2c_autodetect 