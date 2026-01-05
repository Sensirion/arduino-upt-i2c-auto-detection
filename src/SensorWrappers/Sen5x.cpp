#include "SensorWrappers/Sen5x.h"
#include "SensirionCore.h"
#include <map>

namespace sensirion::upt::i2c_autodetect{

Sen5x::Sen5x(TwoWire& wire, uint16_t address) : 
    mWire(wire),
    mAddress{address},
    mMetadata{core::SEN5X()} {};

uint16_t Sen5x::start() {
    mDriver.begin(mWire);
    return 0;
}

uint16_t Sen5x::measureAndWrite(MeasurementList& measurements,
                                const unsigned long timeStamp) {
    uint16_t error = 0;

    // Read Measurement
    float massConcentrationPm1p0;
    float massConcentrationPm2p5;
    float massConcentrationPm4p0;
    float massConcentrationPm10p0;
    float ambientHumidity;
    float ambientTemperature;
    float vocIndex;
    float noxIndex;

    error = mDriver.readMeasuredValues(
        massConcentrationPm1p0, massConcentrationPm2p5, massConcentrationPm4p0,
        massConcentrationPm10p0, ambientHumidity, ambientTemperature, vocIndex,
        noxIndex);

    if (error) {
        return error;
    }

    measurements.emplace_back(mMetadata, 
        core::SignalType::PM1P0_MICRO_GRAMM_PER_CUBIC_METER,
        core::DataPoint{timeStamp, massConcentrationPm1p0});

    measurements.emplace_back(mMetadata, 
        core::SignalType::PM2P5_MICRO_GRAMM_PER_CUBIC_METER,
        core::DataPoint{timeStamp, massConcentrationPm2p5});

    measurements.emplace_back(mMetadata, 
        core::SignalType::PM4P0_MICRO_GRAMM_PER_CUBIC_METER,
        core::DataPoint{timeStamp, massConcentrationPm4p0});

    measurements.emplace_back(mMetadata, 
        core::SignalType::PM10P0_MICRO_GRAMM_PER_CUBIC_METER,
        core::DataPoint{timeStamp, massConcentrationPm10p0});



    // Verions 54, 55
    if (getDeviceType() == core::SEN54() or
        getDeviceType() == core::SEN55()) {

        measurements.emplace_back(mMetadata, 
            core::SignalType::RELATIVE_HUMIDITY_PERCENTAGE,
            core::DataPoint{timeStamp, ambientHumidity});

        measurements.emplace_back(mMetadata, 
            core::SignalType::TEMPERATURE_DEGREES_CELSIUS,
            core::DataPoint{timeStamp, ambientTemperature});

        measurements.emplace_back(mMetadata, 
            core::SignalType::VOC_INDEX,
            core::DataPoint{timeStamp, vocIndex});

    }
    // Version 55
    if (getDeviceType() == core::SEN55()) {

        measurements.emplace_back(mMetadata, 
            core::SignalType::NOX_INDEX,
            core::DataPoint{timeStamp, noxIndex});

    }
    return HighLevelError::NoError;
}

uint16_t Sen5x::initializationStep() {
    // stop potentially previously started measurement
    uint16_t error = mDriver.stopMeasurement();
    if (error) {
        return error;
    }

    // Get sensor version (SEN50/SEN54/SEN55)
    error = _determineSensorVersion();
    if (error) {
        return error;
    }

    // Get sensor unique ID (last 8 chars of serial no.)
    uint8_t serialNumberSize = 32;
    unsigned char serialNumber[serialNumberSize];
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

    mMetadata.deviceID = sensorID;

    // Start Measurement
    error = mDriver.startMeasurement();
    if (error) {
        return error;
    }
    error = _determineSensorVersion();
    return error;
}

core::DeviceType Sen5x::getDeviceType() const {
    return mMetadata.deviceType;
}

core::MetaData Sen5x::getMetaData() const {
    return mMetadata;
}

size_t Sen5x::getNumberOfDataPoints() const {
    static std::map<core::DeviceType, size_t> deviceToSignalCount = {
        {core::SEN5X(), 4},
        {core::SEN50(), 4},
        {core::SEN54(), 7},
        {core::SEN55(), 8},
    };
    const auto iter = deviceToSignalCount.find(getDeviceType());
    if (iter == deviceToSignalCount.cend()){
        return 0;
    }
    return iter->second;
}

unsigned long Sen5x::getMinimumMeasurementIntervalMs() const {
    return 1000;
}

bool Sen5x::probe() {
    return _determineSensorVersion() == HighLevelError::NoError;
}

void* Sen5x::getDriver() {
    return reinterpret_cast<void*>(&mDriver);
}

uint8_t Sen5x::getI2CAddress() const {
    return mAddress;
};

uint16_t Sen5x::_determineSensorVersion() {
    uint8_t sensorNameSize = 32;
    unsigned char sensorNameStr[sensorNameSize];
    uint16_t error = mDriver.getProductName(sensorNameStr, sensorNameSize);

    if (error) {
        return error;
    }

    if (strcmp(reinterpret_cast<const char*>(sensorNameStr), "SEN50") == 0) {
        mMetadata.deviceType = core::SEN50();
    } else if (strcmp(reinterpret_cast<const char*>(sensorNameStr), "SEN54") ==
               0) {
        mMetadata.deviceType = core::SEN54();
    } else if (strcmp(reinterpret_cast<const char*>(sensorNameStr), "SEN55") ==
               0) {
        mMetadata.deviceType = core::SEN55();
    } else {
        mMetadata.deviceType = core::SEN5X();
    }
    return 0;
}
} // namespace sensirion::upt::i2c_autodetect 
