#include "SensorWrappers/Sen5x.h"
#include "SensirionCore.h"
#include <map>

Sen5x::Sen5x(TwoWire& wire, uint16_t address) : 
    _wire(wire),
    _address{address},
    _metaData{sensirion::upt::core::SensorType::SEN5X()} {};

uint16_t Sen5x::start() {
    _driver.begin(_wire);
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

    error = _driver.readMeasuredValues(
        massConcentrationPm1p0, massConcentrationPm2p5, massConcentrationPm4p0,
        massConcentrationPm10p0, ambientHumidity, ambientTemperature, vocIndex,
        noxIndex);

    if (error) {
        return error;
    }

    measurements.emplace_back(_metaData, 
        sensirion::upt::core::SignalType::PM1P0_MICRO_GRAMM_PER_CUBIC_METER,
        sensirion::upt::core::DataPoint{timeStamp, massConcentrationPm1p0});

    measurements.emplace_back(_metaData, 
        sensirion::upt::core::SignalType::PM2P5_MICRO_GRAMM_PER_CUBIC_METER,
        sensirion::upt::core::DataPoint{timeStamp, massConcentrationPm2p5});

    measurements.emplace_back(_metaData, 
        sensirion::upt::core::SignalType::PM4P0_MICRO_GRAMM_PER_CUBIC_METER,
        sensirion::upt::core::DataPoint{timeStamp, massConcentrationPm4p0});

    measurements.emplace_back(_metaData, 
        sensirion::upt::core::SignalType::PM10P0_MICRO_GRAMM_PER_CUBIC_METER,
        sensirion::upt::core::DataPoint{timeStamp, massConcentrationPm10p0});



    // Verions 54, 55
    if (getDeviceType() == sensirion::upt::core::SensorType::SEN54() or
        getDeviceType() == sensirion::upt::core::SensorType::SEN55()) {

        measurements.emplace_back(_metaData, 
            sensirion::upt::core::SignalType::RELATIVE_HUMIDITY_PERCENTAGE,
            sensirion::upt::core::DataPoint{timeStamp, ambientHumidity});

        measurements.emplace_back(_metaData, 
            sensirion::upt::core::SignalType::TEMPERATURE_DEGREES_CELSIUS,
            sensirion::upt::core::DataPoint{timeStamp, ambientTemperature});

        measurements.emplace_back(_metaData, 
            sensirion::upt::core::SignalType::VOC_INDEX,
            sensirion::upt::core::DataPoint{timeStamp, vocIndex});

    }
    // Version 55
    if (getDeviceType() == sensirion::upt::core::SensorType::SEN55()) {

        measurements.emplace_back(_metaData, 
            sensirion::upt::core::SignalType::NOX_INDEX,
            sensirion::upt::core::DataPoint{timeStamp, noxIndex});

    }
    return HighLevelError::NoError;
}

uint16_t Sen5x::initializationStep() {
    // stop potentially previously started measurement
    uint16_t error = _driver.stopMeasurement();
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

    _metaData.deviceID = sensorID;

    // Start Measurement
    error = _driver.startMeasurement();
    if (error) {
        return error;
    }
    error = _determineSensorVersion();
    return error;
}

sensirion::upt::core::DeviceType Sen5x::getDeviceType() const {
    return _metaData.deviceType;
}

sensirion::upt::core::MetaData Sen5x::getMetaData() const {
    return _metaData;
}

size_t Sen5x::getNumberOfDataPoints() const {
    static std::map<sensirion::upt::core::DeviceType, size_t> deviceToSignalCount = {
        {sensirion::upt::core::SensorType::SEN5X(), 4},
        {sensirion::upt::core::SensorType::SEN50(), 4},
        {sensirion::upt::core::SensorType::SEN54(), 7},
        {sensirion::upt::core::SensorType::SEN55(), 8},
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

void* Sen5x::getDriver() {
    return reinterpret_cast<void*>(&_driver);
}

uint16_t Sen5x::_determineSensorVersion() {
    uint8_t sensorNameSize = 32;
    unsigned char sensorNameStr[sensorNameSize];
    uint16_t error = _driver.getProductName(sensorNameStr, sensorNameSize);

    if (error) {
        return error;
    }

    if (strcmp(reinterpret_cast<const char*>(sensorNameStr), "SEN50") == 0) {
        _metaData.deviceType = sensirion::upt::core::SensorType::SEN50();
    } else if (strcmp(reinterpret_cast<const char*>(sensorNameStr), "SEN54") ==
               0) {
        _metaData.deviceType = sensirion::upt::core::SensorType::SEN54();
    } else if (strcmp(reinterpret_cast<const char*>(sensorNameStr), "SEN55") ==
               0) {
        _metaData.deviceType = sensirion::upt::core::SensorType::SEN55();
    } else {
        _metaData.deviceType = sensirion::upt::core::SensorType::SEN5X();
    }
    return 0;
}
