#include "SensorWrappers/Stc3x.h"
#include "SensirionCore.h"
#include "Sensirion_UPT_Core.h"

Stc3x::Stc3x(TwoWire& wire, uint16_t address) : _wire(wire), _address{address},
    mMetadata{sensirion::upt::core::SensorType::STC3X()}
 {// The device type is determined more precisely at initializationStep()
 };

uint16_t Stc3x::start() {
    _driver.begin(_wire, _address);
    uint16_t error = _driver.setBinaryGas(0x0001);
    return error;
}

uint16_t Stc3x::measureAndWrite(MeasurementList& measurements,
                                const unsigned long timeStamp) {
    float gasValue;
    float temperatureValue;

    uint16_t error =
        _driver.measureGasConcentration(gasValue, temperatureValue);
    if (error) {
        return error;
    }

    measurements.emplace_back(mMetadata, 
        sensirion::upt::core::SignalType::GAS_CONCENTRATION_VOLUME_PERCENTAGE,
        sensirion::upt::core::DataPoint{timeStamp, gasValue});

    measurements.emplace_back(mMetadata, 
        sensirion::upt::core::SignalType::TEMPERATURE_DEGREES_CELSIUS,
        sensirion::upt::core::DataPoint{timeStamp, temperatureValue});

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
        mMetadata.deviceType = sensirion::upt::core::SensorType::STC31();
    }  // else keep default STC3X

    // Sensor Serial No
    uint64_t sensorID = 0;
    sensorID |=
        (uint64_t)serialNumberRawHigh << 32 | (uint64_t)serialNumberRawLow;
    mMetadata.deviceID = sensorID;

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
     * Sensor would assume a relative humidity of 0% if we would not set this
     * explicitly, which could lead to negative gas concentration values.
     */
    error = _driver.setRelativeHumidity(50);
    if (error) {
        return error;
    }

    return HighLevelError::NoError;
}

sensirion::upt::core::DeviceType Stc3x::getSensorType() const {
    return mMetadata.deviceType;
}

sensirion::upt::core::MetaData Stc3x::getMetaData() const {
    return mMetadata;
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
