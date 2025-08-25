#include "SensorStateMachine.h"

#include <SensirionErrors.h>

static const char* TAG = "SensorStateMachine";

bool timeIntervalPassed(const uint32_t interval,
                        const uint32_t currentTimeStamp,
                        const uint32_t latestUpdateTimeStamp) {
    if (currentTimeStamp < interval) {
        return true;
    }
    unsigned long elapsedTime = currentTimeStamp - latestUpdateTimeStamp;
    return elapsedTime >= interval;
}

SensorStateMachine::SensorStateMachine(ISensor* pSensor)
    : mSensorState(SensorStatus::UNINITIALIZED), mInitErrorCounter(0),
      mMeasurementErrorCounter(0), mLastMeasurementTimeStampMs(0),
      mMeasurementIntervalMs(0), mSensor(pSensor) {
    mSensor->start();
};

AutoDetectorError SensorStateMachine::_initialize() {
    uint16_t error = mSensor->initializationStep();
    if (error) {
        char errorMsg[256];
        errorToString(error, errorMsg, 256);
        ESP_LOGE(TAG, "Failed to perform initialization step of sensor %s: %s",
                 mSensor->getDeviceType().data(), errorMsg);
        return I2C_ERROR;
    }
    mMeasurementIntervalMs = mSensor->getMinimumMeasurementIntervalMs();
    mLastMeasurementTimeStampMs = millis();

    if (mSensor->getInitializationIntervalMs() > 0) {
        // SGP4X, SCD4X
        mSensorState = SensorStatus::INITIALIZING;
    } else {
        mSensorState = SensorStatus::RUNNING;
    }

    return NO_ERROR;
}

void SensorStateMachine::_initializationRoutine() {
    if (timeIntervalPassed(mSensor->getInitializationIntervalMs(), millis(),
                           mLastMeasurementTimeStampMs)) {
        mSensorState = SensorStatus::RUNNING;
        mLastMeasurementTimeStampMs = millis();
    }
}

AutoDetectorError SensorStateMachine::_readSignalsRoutine() {
    unsigned long timeSinceLastMeasurementMs =
        millis() - mLastMeasurementTimeStampMs;

    /* Determine timing relationship vs. last measurement */
    enum class timeLineRegion {
        INSIDE_MIN_INTERVAL,          // Measurement not yet due to be performed
        INSIDE_VALID_BAND,            // May request a reading
        OUTSIDE_VALID_INITIALIZATION  // Sensor running status has decayed,
                                      // conditioning must be performed
    };
    timeLineRegion tlr_position = timeLineRegion::INSIDE_MIN_INTERVAL;

    if (timeSinceLastMeasurementMs >= mMeasurementIntervalMs) {
        tlr_position = timeLineRegion::INSIDE_VALID_BAND;
    }

    if (mSensor->readyStateDecayTimeMs() > 0 &&
        timeSinceLastMeasurementMs > mSensor->readyStateDecayTimeMs()) {
        tlr_position = timeLineRegion::OUTSIDE_VALID_INITIALIZATION;
    }

    /* Perform appropriate action */
    AutoDetectorError error = NO_ERROR;
    switch (tlr_position) {
        case timeLineRegion::INSIDE_MIN_INTERVAL:
            break;

        case timeLineRegion::INSIDE_VALID_BAND:
            error = _readSignals();
            if (error) {
                return error;
            }
            break;

        case timeLineRegion::OUTSIDE_VALID_INITIALIZATION:
            mSensorState = SensorStatus::UNINITIALIZED;
            return SENSOR_READY_STATE_DECAYED_ERROR;

        default:
            break;
    }

    return NO_ERROR;
}

AutoDetectorError SensorStateMachine::_readSignals() {
    
    const uint32_t nowMS = millis();
    mSensorSignals.clear();
    const uint16_t error = mSensor->measureAndWrite(mSensorSignals, nowMS);

    if (error) {
        char errorMsg[256];
        errorToString(error, errorMsg, 256);
        ESP_LOGE(TAG, "Failed to read measurements for sensor %s: %s",
                 mSensor->getDeviceType().data(), errorMsg);
        return I2C_ERROR;
    }

    mLastMeasurementTimeStampMs = nowMS;

    return NO_ERROR;
}

SensorStatus SensorStateMachine::getSensorState() const {
    return mSensorState;
}

uint16_t SensorStateMachine::setMeasurementInterval(uint32_t interval) {
    if (interval > mSensor->getMinimumMeasurementIntervalMs()) {
        mMeasurementIntervalMs = interval;
        return NO_ERROR;
    }
    return 1;
}

AutoDetectorError SensorStateMachine::update() {
    AutoDetectorError error = NO_ERROR;
    switch (mSensorState) {
        case SensorStatus::UNDEFINED:
            break;

        case SensorStatus::UNINITIALIZED:
            error = _initialize();
            if (error) {
                mInitErrorCounter++;
            } else {
                mInitErrorCounter = 0;
            }
            break;

        case SensorStatus::INITIALIZING:
            _initializationRoutine();
            break;

        case SensorStatus::RUNNING:
            error = _readSignalsRoutine();
            if (error) {
                mMeasurementErrorCounter++;
            } else {
                mMeasurementErrorCounter = 0;
            }
            break;

        case SensorStatus::LOST:
        default:
            break;
    }

    const uint16_t nAllowed = ISensor::getNumberOfAllowedConsecutiveErrors();
    if (mInitErrorCounter > nAllowed || mMeasurementErrorCounter > nAllowed) {
        mSensorState = SensorStatus::LOST;
        return LOST_SENSOR_ERROR;
    }

    return error;
}

ISensor* SensorStateMachine::getSensor() const {
    return mSensor;
}

const ISensor::MeasurementList& SensorStateMachine::getSignals() const {
    return mSensorSignals;
}
