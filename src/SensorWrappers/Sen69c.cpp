#include "SensorWrappers/Sen69c.h"
#include "SensirionCore.h"

namespace sensirion::upt::i2c_autodetect {

Sen69c::Sen69c(TwoWire& wire, uint16_t address)
    : mWire(wire), mAddress{address}, mMetaData{SEN69C} {};

uint16_t Sen69c::start() {
    mDriver.begin(mWire, mAddress);
    return 0;
}

uint16_t Sen69c::measureAndWrite(MeasurementList& measurements,
                                 const unsigned long timeStamp) {
    uint16_t error = 0;

    // Read Measurement
    float massConcentrationPm1p0 = 0.0;
    float massConcentrationPm2p5 = 0.0;
    float massConcentrationPm4p0 = 0.0;
    float massConcentrationPm10p0 = 0.0;
    float humidity = 0.0;
    float temperature = 0.0;
    float vocIndex = 0.0;
    float noxIndex = 0.0;
    float hcho = 0.0;
    int16_t co2 = 0;

    error = mDriver.readMeasuredValues(
        massConcentrationPm1p0, massConcentrationPm2p5, massConcentrationPm4p0,
        massConcentrationPm10p0, humidity, temperature, vocIndex, noxIndex,
        hcho, co2);

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

    measurements.emplace_back(mMetaData, core::SignalType::VOC_INDEX,
                              core::DataPoint{timeStamp, vocIndex});

    measurements.emplace_back(mMetaData, core::SignalType::NOX_INDEX,
                              core::DataPoint{timeStamp, noxIndex});

    measurements.emplace_back(mMetaData,
                              core::SignalType::HCHO_PARTS_PER_BILLION,
                              core::DataPoint{timeStamp, hcho});

    measurements.emplace_back(
        mMetaData, core::SignalType::CO2_PARTS_PER_MILLION,
        core::DataPoint{timeStamp, static_cast<float>(co2)});

    return HighLevelError::NoError;
}

uint16_t Sen69c::initializationStep() {
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

core::DeviceType Sen69c::getDeviceType() const {
    return mMetaData.deviceType;
}

core::MetaData Sen69c::getMetaData() const {
    return mMetaData;
}

size_t Sen69c::getNumberOfDataPoints() const {
    return 10;
}

unsigned long Sen69c::getMinimumMeasurementIntervalMs() const {
    return 1000;
}

uint8_t Sen69c::getI2CAddress() const {
    return mAddress;
};

bool Sen69c::probe() {
    std::basic_string<int8_t> sensorNameStr(32, '\0');
    // reset device before probing
    mDriver.deviceReset();
    const uint16_t error =
        mDriver.getProductName(sensorNameStr.data(), sensorNameStr.capacity());
    return !error && sensorNameStr == reinterpret_cast<const int8_t*>("SEN69C");
}

void* Sen69c::getDriver() {
    return &mDriver;
}

}  // namespace sensirion::upt::i2c_autodetect
