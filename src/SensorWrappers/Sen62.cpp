#include "SensorWrappers/Sen62.h"
#include "SensirionCore.h"

namespace sensirion::upt::i2c_autodetect {

Sen62::Sen62(TwoWire& wire, uint16_t address)
    : mWire(wire), mAddress{address}, mMetaData{SEN62} {};

uint16_t Sen62::start() {
    mDriver.begin(mWire, mAddress);
    return 0;
}

uint16_t Sen62::measureAndWrite(MeasurementList& measurements,
                                const unsigned long timeStamp) {
    uint16_t error = 0;

    // Read Measurement
    float massConcentrationPm1p0 = 0.0;
    float massConcentrationPm2p5 = 0.0;
    float massConcentrationPm4p0 = 0.0;
    float massConcentrationPm10p0 = 0.0;
    float humidity = 0.0;
    float temperature = 0.0;

    error = mDriver.readMeasuredValues(
        massConcentrationPm1p0, massConcentrationPm2p5, massConcentrationPm4p0,
        massConcentrationPm10p0, humidity, temperature);

    if (error) {
        return error;
    }

    measurements.emplace_back(
        mMetaData, core::SignalType::PM1P0_MICRO_GRAMM_PER_CUBIC_METER,
        core::DataPoint{timeStamp, massConcentrationPm1p0});

    measurements.emplace_back(
        mMetaData, core::SignalType::PM2P5_MICRO_GRAMM_PER_CUBIC_METER,
        core::DataPoint{timeStamp, massConcentrationPm2p5});

    measurements.emplace_back(
        mMetaData, core::SignalType::PM4P0_MICRO_GRAMM_PER_CUBIC_METER,
        core::DataPoint{timeStamp, massConcentrationPm4p0});

    measurements.emplace_back(
        mMetaData, core::SignalType::PM10P0_MICRO_GRAMM_PER_CUBIC_METER,
        core::DataPoint{timeStamp, massConcentrationPm10p0});

    measurements.emplace_back(mMetaData,
                              core::SignalType::RELATIVE_HUMIDITY_PERCENTAGE,
                              core::DataPoint{timeStamp, humidity});

    measurements.emplace_back(mMetaData,
                              core::SignalType::TEMPERATURE_DEGREES_CELSIUS,
                              core::DataPoint{timeStamp, temperature});

    return HighLevelError::NoError;
}

uint16_t Sen62::initializationStep() {
    // Reset the device to ensure a known state
    uint16_t error = mDriver.deviceReset();
    if (error) {
        return error;
    }

    // Get sensor unique ID (last 8 chars of serial no.)
    constexpr uint16_t serialNumberSize = 32;
    int8_t serialNumber[serialNumberSize] = {0};
    error = mDriver.getSerialNumber(serialNumber, serialNumberSize);
    if (error) {
        return error;
    }
    mMetaData.deviceID = extractSensorId(serialNumber, serialNumberSize);

    // Start Measurement
    error = mDriver.startContinuousMeasurement();

    return error;
}

core::DeviceType Sen62::getDeviceType() const {
    return mMetaData.deviceType;
}

core::MetaData Sen62::getMetaData() const {
    return mMetaData;
}

size_t Sen62::getNumberOfDataPoints() const {
    return 6;
}

unsigned long Sen62::getMinimumMeasurementIntervalMs() const {
    return 1000;
}

uint8_t Sen62::getI2CAddress() const {
    return mAddress;
};

bool Sen62::probe() {
    constexpr uint8_t sensorNameSize = 32;
    int8_t sensorName[sensorNameSize] = {0};
    // reset device before probing
    mDriver.deviceReset();
    const uint16_t error = mDriver.getProductName(sensorName, sensorNameSize);
    return !error &&
           strncmp(reinterpret_cast<const char*>(sensorName), "SEN62",
                   sensorNameSize) == 0;
}

void* Sen62::getDriver() {
    return &mDriver;
}

}  // namespace sensirion::upt::i2c_autodetect
