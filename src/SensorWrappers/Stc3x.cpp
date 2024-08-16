#include "SensorWrappers/Stc3x.h"
#include "SensirionCore.h"
#include "Sensirion_UPT_Core.h"

Stc3x::Stc3x(TwoWire& wire) : _wire(wire) {
    _metaData.deviceType.sensorType =
        SensorType::STC3X;  // Determined more precisely at initializationStep()
    _metaData.platform = DevicePlatform::WIRED;
};

uint16_t Stc3x::start() {
    _driver.begin(_wire, I2C_ADDRESS);
    uint16_t error = _driver.setBinaryGas(0x0001);
    return error;
}

uint16_t Stc3x::measureAndWrite(Measurement measurements[],
                                const unsigned long timeStamp) {
    float gasValue;
    float temperatureValue;

    uint16_t error =
        _driver.measureGasConcentration(gasValue, temperatureValue);
    if (error) {
        return error;
    }

    measurements[0].signalType =
        SignalType::GAS_CONCENTRATION_VOLUME_PERCENTAGE;
    measurements[0].dataPoint.t_offset = timeStamp;
    measurements[0].dataPoint.value = gasValue;
    measurements[0].metaData = _metaData;

    measurements[1].signalType = SignalType::TEMPERATURE_DEGREES_CELSIUS;
    measurements[1].dataPoint.t_offset = timeStamp;
    measurements[1].dataPoint.value = temperatureValue;
    measurements[1].metaData = _metaData;

    return HighLevelError::NoError;
}

uint16_t Stc3x::initializationStep() {
    uint16_t error = _driver.prepareProductIdentifier();
    if (error) {
        return error;
    }

    uint32_t productNumber;
    uint32_t serialNumberRawLow;
    uint32_t serialNumberRawHigh;

    error = _driver.readProductIdentifier(productNumber, serialNumberRawHigh,
                                          serialNumberRawLow);

    if (error) {
        return error;
    }
    // Sensor Product version
    /**
     * See
     * https://sensirion.com/media/documents/7B1D0EA7/61652CD0/Sensirion_Thermal_Conductivity_Datasheet_STC31_D1_1.pdf
     * Section 3.3.13
     */
    const uint32_t stc31ProductNumber = 0x08010301;
    const uint32_t mask = 0xFFFFFF00;
    const uint32_t maskedProductNo = productNumber & mask;
    const uint32_t maskedSTC31ProductNo = stc31ProductNumber & mask;
    if (maskedSTC31ProductNo == maskedProductNo) {
        _metaData.deviceType.sensorType = SensorType::STC31;
    }  // else keep default STC3X

    // Sensor Serial No
    uint64_t sensorID = 0;
    sensorID |=
        (uint64_t)serialNumberRawHigh << 32 | (uint64_t)serialNumberRawLow;
    _metaData.deviceID = sensorID;

    // Select gas mode
    /**
     * See
     * https://sensirion.com/media/documents/7B1D0EA7/61652CD0/Sensirion_Thermal_Conductivity_Datasheet_STC31_D1_1.pdf
     * Section 3.3.2
     *
     * Available Gas modes:
     *  0x0000: CO2 in N2   (range 0-100% vol.)
     *  0x0001: CO2 in Air  (range 0-100% vol.)
     *  0x0002: CO2 in N2   (range 0-25%  vol.)
     *  0x0003: CO2 in Air  (range 0-25%  vol.)
     */
    error = _driver.setBinaryGas(0x0003);
    if (error) {
        return error;
    }

    // Set relative humidity
    /**
     * See
     * https://sensirion.com/media/documents/7B1D0EA7/61652CD0/Sensirion_Thermal_Conductivity_Datasheet_STC31_D1_1.pdf
     * Section 3.3.3
     *
     * Sensor would assume a relative humidity of 0% if we would not set this explicitly, which could
     * lead to negative gas concentration values.
     */
    error = _driver.setRelativeHumidity(50);
    if (error) {
        return error;
    }

    return HighLevelError::NoError;
}

SensorType Stc3x::getSensorType() const {
    return _metaData.deviceType.sensorType;
}

MetaData Stc3x::getMetaData() const {
    return _metaData;
}

size_t Stc3x::getNumberOfDataPoints() const {
    return 2;
}

unsigned long Stc3x::getMinimumMeasurementIntervalMs() const {
    return 1000;
}

void* Stc3x::getDriver() {
    return reinterpret_cast<void*>(&_driver);
}
