#include "SensorWrappers/Stcc4.h"
#include "SensirionCore.h"
#include "Sensirion_UPT_Core.h"

namespace sensirion::upt::i2c_autodetect{

Stcc4::Stcc4(TwoWire& wire, uint16_t address) : mWire(wire), 
    mAddress{address}, mMetadata{core::STCC4()} {
};

uint16_t Stcc4::start() {
    mDriver.begin(mWire, mAddress);
    return 0;
}

uint16_t Stcc4::measureAndWrite(MeasurementList& measurements,
                                const unsigned long timeStamp) {
    int16_t co2;
    float temperatureValue;
    float relativeHumidityValue;
    uint16_t sensorStatus; // Required by API but not used in this implementation

    uint16_t error =
        mDriver.readMeasurement(co2, temperatureValue, relativeHumidityValue, sensorStatus);
    if (error) {
        return error;
    }

    measurements.emplace_back(mMetadata, 
        core::SignalType::CO2_PARTS_PER_MILLION,
        core::DataPoint{timeStamp, static_cast<float>(co2)});

    measurements.emplace_back(mMetadata, 
        core::SignalType::TEMPERATURE_DEGREES_CELSIUS,
        core::DataPoint{timeStamp, temperatureValue});

    measurements.emplace_back(mMetadata, 
        core::SignalType::RELATIVE_HUMIDITY_PERCENTAGE,
        core::DataPoint{timeStamp, relativeHumidityValue});


    return HighLevelError::NoError;
}

uint16_t Stcc4::initializationStep() {
    // stop potentially previously started measurement
    uint16_t error = mDriver.stopContinuousMeasurement();
    if (error) {
        return error;
    }

    uint32_t productId;
    uint64_t serialNumber;
    error = mDriver.getProductId(productId, serialNumber);
    if (error) {
        return error;
    }

    mMetadata.deviceID = serialNumber;

    error = mDriver.startContinuousMeasurement();
    return error;
}

core::DeviceType Stcc4::getDeviceType() const {
    return mMetadata.deviceType;
}

core::MetaData Stcc4::getMetaData() const {
    return mMetadata;
}

size_t Stcc4::getNumberOfDataPoints() const {
    return 3;
}

uint8_t Stcc4::getI2CAddress() const {
    return mAddress;
};

unsigned long Stcc4::getMinimumMeasurementIntervalMs() const {
    return 1000;
}

void* Stcc4::getDriver() {
    return reinterpret_cast<void*>(&mDriver);
}
} // namespace sensirion::upt::i2c_autodetect 
