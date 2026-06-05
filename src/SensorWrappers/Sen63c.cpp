#include "SensorWrappers/Sen63c.h"
#include "SensirionCore.h"

namespace sensirion::upt::i2c_autodetect {

Sen63c::Sen63c(TwoWire& wire, uint16_t address)
    : mWire(wire), mAddress{address}, mMetaData{SEN63C} {};

uint16_t Sen63c::start() {
    mDriver.begin(mWire, mAddress);
    return 0;
}

uint16_t Sen63c::measureAndWrite(MeasurementList& measurements,
                                const unsigned long timeStamp) {
    uint16_t error = 0;

    // Read Measurement
    float massConcentrationPm1p0 = 0.0;
    float massConcentrationPm2p5 = 0.0;
    float massConcentrationPm4p0 = 0.0;
    float massConcentrationPm10p0 = 0.0;
    float humidity = 0.0;
    float temperature = 0.0;
    int16_t co2 = 0.0;

    error = mDriver.readMeasuredValues(
        massConcentrationPm1p0, massConcentrationPm2p5, massConcentrationPm4p0,
        massConcentrationPm10p0, humidity, temperature, co2);

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

    // Filter out 0xFFFF CO2 value
    if (co2 == 65535) {
        co2 = 0;
    }

    measurements.emplace_back(
        mMetaData, core::SignalType::CO2_PARTS_PER_MILLION,
        core::DataPoint{timeStamp, static_cast<float>(co2)});

    return HighLevelError::NoError;
}

uint16_t Sen63c::initializationStep() {
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

core::DeviceType Sen63c::getDeviceType() const {
    return mMetaData.deviceType;
}

core::MetaData Sen63c::getMetaData() const {
    return mMetaData;
}

size_t Sen63c::getNumberOfDataPoints() const {
    return 7;
}

unsigned long Sen63c::getMinimumMeasurementIntervalMs() const {
    return 1000;
}

uint8_t Sen63c::getI2CAddress() const {
    return mAddress;
};

bool Sen63c::probe() {
    constexpr uint8_t productTypeSize = 32;
    int8_t productType[productTypeSize] = {0};
    // reset device before probing
    mDriver.deviceReset();
    const uint16_t error = mDriver.getProductType(productType, productTypeSize);
    uint32_t productTypeNum =
        strtoul(reinterpret_cast<const char*>(productType), nullptr, 16);
    return !error && productTypeNum == 0x00085700;
}

void* Sen63c::getDriver() {
    return &mDriver;
}

}  // namespace sensirion::upt::i2c_autodetect