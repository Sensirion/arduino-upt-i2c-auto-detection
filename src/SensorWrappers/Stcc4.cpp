#include "SensorWrappers/Stcc4.h"
#include "SensirionCore.h"
#include "Sensirion_UPT_Core.h"

Stcc4::Stcc4(TwoWire& wire, uint16_t address) : _wire(wire), _address{address} {
    _metaData.deviceType.sensorType = SensorType::STCC4;
    _metaData.platform = DevicePlatform::WIRED;
};

uint16_t Stcc4::start() {
    _driver.begin(_wire, _address);
    return 0;
}

uint16_t Stcc4::measureAndWrite(Measurement measurements[],
                                const unsigned long timeStamp) {
    int16_t co2;
    float temperatureValue;
    float relativeHumidityValue;
    uint16_t sensorStatus; // Required by API but not used in this implementation

    uint16_t error =
        _driver.readMeasurement(co2, temperatureValue, relativeHumidityValue, sensorStatus);
    if (error) {
        return error;
    }

    measurements[0].signalType = SignalType::CO2_PARTS_PER_MILLION;
    measurements[0].dataPoint.t_offset = timeStamp;
    measurements[0].dataPoint.value = co2;
    measurements[0].metaData = _metaData;

    measurements[1].signalType = SignalType::TEMPERATURE_DEGREES_CELSIUS;
    measurements[1].dataPoint.t_offset = timeStamp;
    measurements[1].dataPoint.value = temperatureValue;
    measurements[1].metaData = _metaData;

    measurements[2].signalType = SignalType::RELATIVE_HUMIDITY_PERCENTAGE;
    measurements[2].dataPoint.t_offset = timeStamp;
    measurements[2].dataPoint.value = relativeHumidityValue;
    measurements[2].metaData = _metaData;

    return HighLevelError::NoError;
}

uint16_t Stcc4::initializationStep() {
    // stop potentially previously started measurement
    uint16_t error = _driver.stopContinuousMeasurement();
    if (error) {
        return error;
    }

    uint32_t productId;
    uint64_t serialNumber;
    error = _driver.getProductId(productId, serialNumber);
    if (error) {
        return error;
    }

    _metaData.deviceID = serialNumber;

    error = _driver.startContinuousMeasurement();
    return error;
}

SensorType Stcc4::getSensorType() const {
    return _metaData.deviceType.sensorType;
}

MetaData Stcc4::getMetaData() const {
    return _metaData;
}

size_t Stcc4::getNumberOfDataPoints() const {
    return 3;
}

unsigned long Stcc4::getMinimumMeasurementIntervalMs() const {
    return 1000;
}

void* Stcc4::getDriver() {
    return reinterpret_cast<void*>(&_driver);
}
