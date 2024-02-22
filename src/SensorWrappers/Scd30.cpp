#include "SensorWrappers/Scd30.h"
#include "SensirionCore.h"
#include "Sensirion_UPT_Core.h"

uint16_t Scd30::start() {
    _driver.begin(_wire, I2C_ADDRESS);
    return 0;
}

uint16_t Scd30::measureAndWrite(Measurement measurements[],
                                const unsigned long timeStamp) {
    // Check data ready
    uint16_t dataReadyFlag = 0;
    uint16_t error = _driver.getDataReady(dataReadyFlag);
    if (error) {
        return error;
    }
    if (!dataReadyFlag) {
        return 1;
    }

    float co2Concentration;
    float temperature;
    float humidity;
    error =
        _driver.readMeasurementData(co2Concentration, temperature, humidity);
    if (error) {
        return error;
    }

    MetaData metaData;
    metaData.deviceID = _sensorID;
    metaData.deviceType.sensorType = _sensorType;
    metaData.platform = DevicePlatform::WIRED;

    measurements[0].signalType = SignalType::CO2_PARTS_PER_MILLION;
    measurements[0].dataPoint.t_offset = timeStamp;
    measurements[0].dataPoint.value = co2Concentration;
    measurements[0].metaData = metaData;

    measurements[1].signalType = SignalType::TEMPERATURE_DEGREES_CELSIUS;
    measurements[1].dataPoint.t_offset = timeStamp;
    measurements[1].dataPoint.value = temperature;
    measurements[1].metaData = metaData;

    measurements[2].signalType = SignalType::RELATIVE_HUMIDITY_PERCENTAGE;
    measurements[2].dataPoint.t_offset = timeStamp;
    measurements[2].dataPoint.value = humidity;
    measurements[2].metaData = metaData;

    /* Prepare next reading by querying the dataReadyFlag. We don't need the
     * value of the flag, but the query seems to finalize setting off the
     * measurement process, and enables much faster signal readout at the next
     * call of this function as it then is not required to enter a wait loop
     * (see SensirionI2cScd30::blockingReadMeasurementData()). This procedure is
     * only required for SCD30. */
    error = _driver.getDataReady(dataReadyFlag);
    if (error) {
        return error;
    }
    return HighLevelError::NoError;
}

uint16_t Scd30::initializationStep() {
    // stop potentially previously started measurement
    _driver.stopPeriodicMeasurement();
    // Start Measurement
    uint16_t error = _driver.startPeriodicMeasurement(0);
    if (error) {
        return error;
    }

    // SCD30 does not support serial no. retrieval via driver
    _sensorID = 0;
    for (size_t i = 0; i < 64; i++) {
        _sensorID |= (random(2) << i);
    }

    /* See explanatory comment for measureAndWrite() */
    uint16_t dataReadyFlag;
    error = _driver.getDataReady(dataReadyFlag);
    return error;
}

SensorType Scd30::getSensorType() const {
    return _sensorType;
}

size_t Scd30::getNumberOfDataPoints() const {
    return 3;
}

unsigned long Scd30::getMinimumMeasurementIntervalMs() const {
    return 2000;
}

bool Scd30::requiresInitializationStep() const {
    return true;
}

void* Scd30::getDriver() {
    return reinterpret_cast<void*>(&_driver);
}
