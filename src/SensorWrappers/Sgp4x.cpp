#include "SensorWrappers/Sgp4x.h"
#include "SensirionCore.h"
#include "Sensirion_UPT_Core.h"

Sgp41::Sgp41(TwoWire& wire, uint16_t address) : _wire(wire), _address{address} {
    _metaData.deviceType.sensorType = SensorType::SGP4X;
    _metaData.platform = DevicePlatform::WIRED;
};

uint16_t Sgp41::start() {
    _driver.begin(_wire);
    return 0;
}

uint16_t Sgp41::measureAndWrite(Measurement measurements[],
                                const unsigned long timeStamp) {
    uint16_t srawVoc = 0;
    uint16_t srawNox = 0;

    uint16_t error =
        _driver.measureRawSignals(_defaultRh, _defaultT, srawVoc, srawNox);
    if (error) {
        return error;
    }

    measurements[0].signalType = SignalType::RAW_VOC_INDEX;
    measurements[0].dataPoint.t_offset = timeStamp;
    measurements[0].dataPoint.value = static_cast<float>(srawVoc);
    measurements[0].metaData = _metaData;

    measurements[1].signalType = SignalType::RAW_NOX_INDEX;
    measurements[1].dataPoint.t_offset = timeStamp;
    measurements[1].dataPoint.value = static_cast<float>(srawNox);
    measurements[1].metaData = _metaData;

    return HighLevelError::NoError;
}

uint16_t Sgp41::initializationStep() {
    // Read serial No.
    uint16_t serialNo[3];
    uint16_t error = _driver.getSerialNumber(serialNo);
    if (error) {
        return error;
    }
    uint64_t sensorID = 0;
    sensorID |= (static_cast<uint64_t>(serialNo[0]) << 16 * 2) |
                (serialNo[1] << 16) | serialNo[2];
    _metaData.deviceID = sensorID;

    uint16_t srawVoc;  // discarded during initialization
    error = _driver.executeConditioning(_defaultRh, _defaultT, srawVoc);
    return error;
}

SensorType Sgp41::getSensorType() const {
    return _metaData.deviceType.sensorType;
}

MetaData Sgp41::getMetaData() const {
    return _metaData;
}

size_t Sgp41::getNumberOfDataPoints() const {
    return 2;
}

unsigned long Sgp41::getMinimumMeasurementIntervalMs() const {
    return 1000;
}

unsigned long Sgp41::getInitializationIntervalMs() const {
    return 8000;
}

void* Sgp41::getDriver() {
    return reinterpret_cast<void*>(&_driver);
}

long Sgp41::readyStateDecayTimeMs() const {
    return 2000;
}
