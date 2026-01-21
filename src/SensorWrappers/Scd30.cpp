#include "SensorWrappers/Scd30.h"
#include "SensirionCore.h"
#include "Sensirion_UPT_Core.h"

namespace sensirion::upt::i2c_autodetect {

Scd30::Scd30(TwoWire& wire, const uint16_t address)
    : mWire(wire), mAddress{address}, mMetadata{core::SCD30()} {};

uint16_t Scd30::start() {
    mDriver.begin(mWire, mAddress);
    return 0;
}

uint16_t Scd30::measureAndWrite(MeasurementList& measurements,
                                const unsigned long timeStamp) {
    // Check data ready
    uint16_t dataReadyFlag = 0;
    uint16_t error = mDriver.getDataReady(dataReadyFlag);
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
        mDriver.readMeasurementData(co2Concentration, temperature, humidity);
    if (error) {
        return error;
    }

    measurements.emplace_back(mMetadata,
                              core::SignalType::CO2_PARTS_PER_MILLION,
                              core::DataPoint{timeStamp, co2Concentration});

    measurements.emplace_back(mMetadata,
                              core::SignalType::TEMPERATURE_DEGREES_CELSIUS,
                              core::DataPoint{timeStamp, temperature});

    measurements.emplace_back(mMetadata,
                              core::SignalType::RELATIVE_HUMIDITY_PERCENTAGE,
                              core::DataPoint{timeStamp, humidity});

    /* Prepare the next reading by querying the dataReadyFlag. We don't need the
     * value of the flag, but the query seems to finalize setting off the
     * measurement process and enables a much faster signal readout at the next
     * call of this function as it then is not required to enter a wait loop
     * (see SensirionI2cScd30::blockingReadMeasurementData()). This procedure is
     * only required for SCD30. */
    error = mDriver.getDataReady(dataReadyFlag);
    if (error) {
        return error;
    }
    return NoError;
}

uint16_t Scd30::initializationStep() {
    // stop potentially previously started measurement
    mDriver.stopPeriodicMeasurement();
    // Start Measurement
    uint16_t error = mDriver.startPeriodicMeasurement(0);
    if (error) {
        return error;
    }

    // SCD30 does not support serial no. retrieval via driver
    uint64_t sensorID = 0;
    for (size_t i = 0; i < 64; i++) {
        sensorID |= (random(2) << i);
    }
    mMetadata.deviceID = sensorID;

    /* See explanatory comment for measureAndWrite() */
    uint16_t dataReadyFlag;
    error = mDriver.getDataReady(dataReadyFlag);
    return error;
}

core::DeviceType Scd30::getDeviceType() const {
    return mMetadata.deviceType;
    ;
}

core::MetaData Scd30::getMetaData() const {
    return mMetadata;
}

size_t Scd30::getNumberOfDataPoints() const {
    return 3;
}

uint8_t Scd30::getI2CAddress() const {
    return mAddress;
};

unsigned long Scd30::getMinimumMeasurementIntervalMs() const {
    return 2000;
}

bool Scd30::probe() {
    uint8_t major, minor;
    // stop potential running measurement
    mDriver.stopPeriodicMeasurement();
    const uint16_t error = mDriver.readFirmwareVersion(major, minor);
    return (error == NoError);
}

void* Scd30::getDriver() {
    return &mDriver;
}
}  // namespace sensirion::upt::i2c_autodetect