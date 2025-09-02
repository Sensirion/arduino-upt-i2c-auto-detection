#include "SensorWrappers/Sen66.h"
#include "SensirionCore.h"

namespace sensirion::upt::i2c_autodetect{

Sen66::Sen66(TwoWire& wire, uint16_t address) : mWire(wire), 
    mAddress{address}, mMetaData{core::SEN66()} {
};

uint16_t Sen66::start() {
    mDriver.begin(mWire, mAddress);
    return 0;
}

uint16_t Sen66::measureAndWrite(MeasurementList& measurements,
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

    measurements.emplace_back(mMetaData,
        core::SignalType::PM1P0_MICRO_GRAMM_PER_CUBIC_METER,
        core::DataPoint{timeStamp, massConcentrationPm1p0}
    );

    measurements.emplace_back(mMetaData,
        core::SignalType::PM2P5_MICRO_GRAMM_PER_CUBIC_METER,
        core::DataPoint{timeStamp, massConcentrationPm2p5}
    );    

    measurements.emplace_back(mMetaData,
        core::SignalType::PM4P0_MICRO_GRAMM_PER_CUBIC_METER,
        core::DataPoint{timeStamp, massConcentrationPm4p0}
    );

    measurements.emplace_back(mMetaData,
        core::SignalType::PM10P0_MICRO_GRAMM_PER_CUBIC_METER,
        core::DataPoint{timeStamp, massConcentrationPm10p0}
    );  

    measurements.emplace_back(mMetaData,
        core::SignalType::RELATIVE_HUMIDITY_PERCENTAGE,
        core::DataPoint{timeStamp, humidity}
    );

    measurements.emplace_back(mMetaData,
        core::SignalType::TEMPERATURE_DEGREES_CELSIUS,
        core::DataPoint{timeStamp, temperature}
    );      

    measurements.emplace_back(mMetaData,
        core::SignalType::VOC_INDEX,
        core::DataPoint{timeStamp, vocIndex}
    );   

    measurements.emplace_back(mMetaData,
        core::SignalType::NOX_INDEX,
        core::DataPoint{timeStamp, noxIndex}
    );   


    // Filter out 0xFFFF CO2 value
    if (co2 == 65535) {
        co2 = 0;
    }

    measurements.emplace_back(mMetaData,
        core::SignalType::CO2_PARTS_PER_MILLION,
        core::DataPoint{timeStamp, static_cast<float>(co2)}
    );       

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
    int8_t serialNumber[serialNumberSize] = {0};
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

    // Start Measurement
    error = mDriver.startContinuousMeasurement();

    return error;
}

core::DeviceType Sen66::getDeviceType() const {
    return mMetaData.deviceType;
}

core::MetaData Sen66::getMetaData() const {
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
} // namespace sensirion::upt::i2c_autodetect 