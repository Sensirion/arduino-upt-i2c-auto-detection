#include "SensorWrappers/Scd30.h"
#include "SensirionCore.h"
#include "Sensirion_UPT_Core.h"


Scd30::Scd30(TwoWire& wire, const uint16_t address) : 
    _wire(wire), _address{address}, _metaData{sensirion::upt::core::SensorType::SCD30()}{};

uint16_t Scd30::start() {
    _driver.begin(_wire, _address);
    return 0;
}

uint16_t Scd30::measureAndWrite(MeasurementList& measurements,
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

    measurements.emplace_back(_metaData, 
        sensirion::upt::core::SignalType::CO2_PARTS_PER_MILLION,
        sensirion::upt::core::DataPoint{timeStamp, co2Concentration});

    measurements.emplace_back(_metaData, 
        sensirion::upt::core::SignalType::TEMPERATURE_DEGREES_CELSIUS, 
        sensirion::upt::core::DataPoint{timeStamp, temperature});

    measurements.emplace_back(_metaData, 
        sensirion::upt::core::SignalType::RELATIVE_HUMIDITY_PERCENTAGE, 
        sensirion::upt::core::DataPoint{timeStamp, humidity});


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
    uint64_t sensorID = 0;
    for (size_t i = 0; i < 64; i++) {
        sensorID |= (random(2) << i);
    }
    _metaData.deviceID = sensorID;

    /* See explanatory comment for measureAndWrite() */
    uint16_t dataReadyFlag;
    error = _driver.getDataReady(dataReadyFlag);
    return error;
}

sensirion::upt::core::DeviceType Scd30::getSensorType() const {
    return _metaData.deviceType;
    ;
}

sensirion::upt::core::MetaData Scd30::getMetaData() const {
    return _metaData;
}

size_t Scd30::getNumberOfDataPoints() const {
    return 3;
}

unsigned long Scd30::getMinimumMeasurementIntervalMs() const {
    return 2000;
}

void* Scd30::getDriver() {
    return reinterpret_cast<void*>(&_driver);
}
