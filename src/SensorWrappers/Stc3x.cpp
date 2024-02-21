#include "SensorWrappers/Stc3x.h"
#include "SensirionCore.h"
#include "Sensirion_UPT_Core.h"

uint16_t Stc3x::start() {
    _driver.begin(_wire);
    uint16_t error = _driver.setBinaryGas(0x0001);
    return error;
}

uint16_t Stc3x::measureAndWrite(Measurement measurements[],
                                const unsigned long timeStamp) {
    uint16_t gasTicks;
    uint16_t temperatureTicks;

    uint16_t error =
        _driver.measureGasConcentration(gasTicks, temperatureTicks);
    if (error) {
        return error;
    }

    MetaData metaData;
    metaData.deviceID = _sensorID;
    metaData.deviceType.sensorType = _sensorType;
    metaData.platform = DevicePlatform::WIRED;

    measurements[0].signalType =
        SignalType::GAS_CONCENTRATION_VOLUME_PERCENTAGE;
    measurements[0].dataPoint.t_offset = timeStamp;
    measurements[0].dataPoint.value =
        100 * (static_cast<float>(gasTicks) - 16384.0) / 32768.0;
    measurements[0].metaData = metaData;

    measurements[1].signalType = SignalType::TEMPERATURE_DEGREES_CELSIUS;
    measurements[1].dataPoint.t_offset = timeStamp;
    measurements[1].dataPoint.value =
        static_cast<float>(temperatureTicks) / 200.0;
    measurements[1].metaData = metaData;

    return HighLevelError::NoError;
}

uint16_t Stc3x::initializationStep() {
    uint16_t error = _driver.prepareProductIdentifier();
    if (error) {
        return error;
    }

    uint32_t productNumber;
    uint8_t serialNumberRaw[32];
    uint8_t serialNumberSize = 32;

    error = _driver.readProductIdentifier(productNumber, serialNumberRaw,
                                          serialNumberSize);
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
        _sensorType = SensorType::STC31;
    }  // else keep default STC3X

    // Sensor Serial No
    _sensorID = 0;
    _sensorID |= (uint64_t)serialNumberRaw[0] << 56 |
                 (uint64_t)serialNumberRaw[1] << 48 |
                 (uint64_t)serialNumberRaw[2] << 40 |
                 (uint64_t)serialNumberRaw[3] << 32 |
                 (uint64_t)serialNumberRaw[4] << 24 |
                 (uint64_t)serialNumberRaw[5] << 16 |
                 (uint64_t)serialNumberRaw[6] << 8 |
                 (uint64_t)serialNumberRaw[7];

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

    return HighLevelError::NoError;
}

SensorType Stc3x::getSensorType() const {
    return _sensorType;
}

size_t Stc3x::getNumberOfDataPoints() const {
    return 2;
}

unsigned long Stc3x::getMinimumMeasurementIntervalMs() const {
    return 1000;
}

bool Stc3x::requiresInitializationStep() const {
    return 1;
}

void* Stc3x::getDriver() {
    return reinterpret_cast<void*>(&_driver);
}
