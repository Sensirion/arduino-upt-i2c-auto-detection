#include "SensorWrappers/Sen66.h"
#include "SensirionCore.h"

Sen66::Sen66(TwoWire& wire) : mWire(wire) {
    mMetaData.deviceType.sensorType = SensorType::SEN66;
    mMetaData.platform = DevicePlatform::WIRED;
};

uint16_t Sen66::start() {
    mDriver.begin(mWire, Sen66::I2C_ADDRESS);
    return 0;
}

uint16_t Sen66::measureAndWrite(Measurement measurements[],
                                const unsigned long timeStamp) {
    uint16_t error = 0;

    // Read Measurement
    float massConcentrationPm1p0 = 0.0;
    float massConcentrationPm2p5 = 0.0;
    float massConcentrationPm4p0 = 0.0;
    float massConcentrationPm10p0 = 0.0;
    float humidity = 0.0;
    float temperature = 0.0;
    float vocIndex = 0.0;
    float noxIndex = 0.0;
    uint16_t co2 = 0.0;

    error = mDriver.readMeasuredValues(
        massConcentrationPm1p0, massConcentrationPm2p5, massConcentrationPm4p0,
        massConcentrationPm10p0, humidity, temperature, vocIndex, noxIndex,
        co2);

    if (error) {
        return error;
    }

    measurements[0].signalType = SignalType::PM1P0_MICRO_GRAMM_PER_CUBIC_METER;
    measurements[0].dataPoint.t_offset = timeStamp;
    measurements[0].dataPoint.value = massConcentrationPm1p0;
    measurements[0].metaData = mMetaData;

    measurements[1].signalType = SignalType::PM2P5_MICRO_GRAMM_PER_CUBIC_METER;
    measurements[1].dataPoint.t_offset = timeStamp;
    measurements[1].dataPoint.value = massConcentrationPm2p5;
    measurements[1].metaData = mMetaData;

    measurements[2].signalType = SignalType::PM4P0_MICRO_GRAMM_PER_CUBIC_METER;
    measurements[2].dataPoint.t_offset = timeStamp;
    measurements[2].dataPoint.value = massConcentrationPm4p0;
    measurements[2].metaData = mMetaData;

    measurements[3].signalType = SignalType::PM10P0_MICRO_GRAMM_PER_CUBIC_METER;
    measurements[3].dataPoint.t_offset = timeStamp;
    measurements[3].dataPoint.value = massConcentrationPm10p0;
    measurements[3].metaData = mMetaData;

    measurements[4].signalType = SignalType::RELATIVE_HUMIDITY_PERCENTAGE;
    measurements[4].dataPoint.t_offset = timeStamp;
    measurements[4].dataPoint.value = humidity;
    measurements[4].metaData = mMetaData;

    measurements[5].signalType = SignalType::TEMPERATURE_DEGREES_CELSIUS;
    measurements[5].dataPoint.t_offset = timeStamp;
    measurements[5].dataPoint.value = temperature;
    measurements[5].metaData = mMetaData;

    measurements[6].signalType = SignalType::VOC_INDEX;
    measurements[6].dataPoint.t_offset = timeStamp;
    measurements[6].dataPoint.value = vocIndex;
    measurements[6].metaData = mMetaData;

    measurements[7].signalType = SignalType::NOX_INDEX;
    measurements[7].dataPoint.t_offset = timeStamp;
    measurements[7].dataPoint.value = noxIndex;
    measurements[7].metaData = mMetaData;

    // Filter out 0xFFFF CO2 value
    if (co2 == 65535) {
        co2 = 0;
    }

    measurements[8].signalType = SignalType::CO2_PARTS_PER_MILLION;
    measurements[8].dataPoint.t_offset = timeStamp;
    measurements[8].dataPoint.value = co2;
    measurements[8].metaData = mMetaData;

    return HighLevelError::NoError;
}

uint16_t Sen66::initializationStep() {
    // Reset the device to ensure a known state
    uint16_t error = mDriver.deviceReset();
    if (error) {
        return error;
    }

    delay(1200);

    // Get sensor unique ID (last 8 chars of serial no.)
    constexpr uint16_t serialNumberSize = 32;
    uint8_t serialNumber[serialNumberSize];
    error = mDriver.getSerialNumber(serialNumber, serialNumberSize);
    if (error) {
        return error;
    }
    const size_t actualLen =
        strlen(reinterpret_cast<const char*>(serialNumber));
    const size_t numBytesToCopy = min(8, static_cast<int>(actualLen));
    uint64_t sensorID = 0;
    for (int i = 0; i < numBytesToCopy - 1; i++) {
        sensorID |= (serialNumber[actualLen - numBytesToCopy - 1 + i]);
        sensorID = sensorID << 8;
    }
    sensorID |= serialNumber[actualLen - 1];

    mMetaData.deviceID = sensorID;
    mMetaData.deviceType.sensorType = SensorType::SEN66;

    // Start Measurement
    error = mDriver.startContinuousMeasurement();

    return error;
}

SensorType Sen66::getSensorType() const {
    return mMetaData.deviceType.sensorType;
}

MetaData Sen66::getMetaData() const {
    return mMetaData;
}

size_t Sen66::getNumberOfDataPoints() const {
    return 9;
}

unsigned long Sen66::getMinimumMeasurementIntervalMs() const {
    return 1000;
}

void* Sen66::getDriver() {
    return &mDriver;
}

unsigned long Sen66::getInitializationIntervalMs() const {
    return 1200;
}
