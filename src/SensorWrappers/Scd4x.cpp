#include "SensorWrappers/Scd4x.h"
#include "SensirionCore.h"
#include "Sensirion_UPT_Core.h"

namespace sensirion::upt::i2c_autodetect{

Scd4x::Scd4x(TwoWire& wire, const uint16_t address) : mWire(wire), 
    mAddress{address},
    mMetadata{core::SCD4X()}{};

uint16_t Scd4x::start() {
    mDriver.begin(mWire, mAddress);
    return 0;
}

uint16_t Scd4x::measureAndWrite(MeasurementList& measurements,
                                const unsigned long timeStamp) {
    uint16_t co2;
    float temp;
    float humi;
    uint16_t error = mDriver.readMeasurement(co2, temp, humi);
    if (error) {
        return error;
    }

    measurements.emplace_back(mMetadata, 
        core::SignalType::CO2_PARTS_PER_MILLION,
        core::DataPoint{timeStamp, static_cast<float>(co2)});

    measurements.emplace_back(mMetadata, 
        core::SignalType::TEMPERATURE_DEGREES_CELSIUS,
        core::DataPoint{timeStamp, temp});        

    measurements.emplace_back(mMetadata, 
        core::SignalType::RELATIVE_HUMIDITY_PERCENTAGE,
        core::DataPoint{timeStamp, humi});      

    return HighLevelError::NoError;
}

uint16_t Scd4x::initializationStep() {
    // stop potentially previously started measurement
    uint16_t error = mDriver.stopPeriodicMeasurement();
    if (error) {
        return error;
    }
    // Sensor Serial no.
    uint64_t serialNumber;
    error = mDriver.getSerialNumber(serialNumber);
    if (error) {
        return error;
    }

    mMetadata.deviceID = serialNumber;

    // Start Measurement
    error = mDriver.startPeriodicMeasurement();
    return error;
}

core::DeviceType Scd4x::getDeviceType() const {

    return mMetadata.deviceType;
}

core::MetaData Scd4x::getMetaData() const {
    return mMetadata;
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
    return std::addressof(mDriver);
}
} // namespace sensirion::upt::i2c_autodetect 