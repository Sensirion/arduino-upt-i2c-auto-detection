#include "SensorStateMachine.h"

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
    : _sensorState(SensorStatus::UNINITIALIZED), _initErrorCounter(0),
      _measurementErrorCounter(0), _lastMeasurementTimeStampMs(0),
      _measurementIntervalMs(0), _sensor(pSensor) {
    _sensor->start();
};

AutoDetectorError SensorStateMachine::_initialize() {
    uint16_t error = _sensor->initializationStep();
    if (error) {
        char errorMsg[256];
        errorToString(error, errorMsg, 256);
        ESP_LOGE(TAG, "Failed to perform initialization step of sensor %s: %s", sensorLabel(_sensor->getSensorType()), errorMsg);
        return I2C_ERROR;
    }

    _sensorSignals.init(_sensor->getNumberOfDataPoints());
    _measurementIntervalMs = _sensor->getMinimumMeasurementIntervalMs();
    _lastMeasurementTimeStampMs = millis();

    if (_sensor->getInitializationIntervalMs() > 0) {
        // SGP4X, SCD4X
        _sensorState = SensorStatus::INITIALIZING;
    } else {
        _sensorState = SensorStatus::RUNNING;
    }

    for (size_t i = 0; i < _sensor->getNumberOfDataPoints(); ++i) {
        Measurement measurement;
        measurement.metaData = _sensor->getMetaData();
        _sensorSignals.addMeasurement(measurement);
    }

    return NO_ERROR;
}

void SensorStateMachine::_initializationRoutine() {
    if (timeIntervalPassed(_sensor->getInitializationIntervalMs(), millis(),
                           _lastMeasurementTimeStampMs)) {
        _sensorState = SensorStatus::RUNNING;
        _lastMeasurementTimeStampMs = millis();
    }
}

AutoDetectorError SensorStateMachine::_readSignalsRoutine() {
    unsigned long timeSinceLastMeasurementMs =
        millis() - _lastMeasurementTimeStampMs;

    /* Determine timing relationship vs. last measurement */
    enum class timeLineRegion {
        INSIDE_MIN_INTERVAL,          // Measurement not yet due to be performed
        INSIDE_VALID_BAND,            // May request a reading
        OUTSIDE_VALID_INITIALIZATION  // Sensor running status has decayed,
                                      // conditionning must be performed
    };
    timeLineRegion tlr_position = timeLineRegion::INSIDE_MIN_INTERVAL;

    if (timeSinceLastMeasurementMs >= _measurementIntervalMs) {
        tlr_position = timeLineRegion::INSIDE_VALID_BAND;
    }

    if (_sensor->readyStateDecayTimeMs() > 0 &&
        timeSinceLastMeasurementMs > _sensor->readyStateDecayTimeMs()) {
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
            _sensorState = SensorStatus::UNINITIALIZED;
            return SENSOR_READY_STATE_DECAYED_ERROR;

        default:
            break;
    }

    return NO_ERROR;
}

AutoDetectorError SensorStateMachine::_readSignals() {
    Measurement signalsBuf[_sensor->getNumberOfDataPoints()];
    uint32_t nowMS = millis();

    uint16_t error = _sensor->measureAndWrite(signalsBuf, nowMS);

    if (error) {
        char errorMsg[256];
        errorToString(error, errorMsg, 256);
        ESP_LOGE(TAG, "Failed to read measurements for sensor %s: %s", sensorLabel(_sensor->getSensorType()), errorMsg);
        return I2C_ERROR;
    }

    _lastMeasurementTimeStampMs = nowMS;

    for (size_t i = 0; i < _sensor->getNumberOfDataPoints(); ++i) {
        _sensorSignals.addMeasurement(signalsBuf[i]);
    }
    _sensorSignals.resetWriteHead();

    return NO_ERROR;
}

SensorStatus SensorStateMachine::getSensorState() const {
    return _sensorState;
}

uint16_t SensorStateMachine::setMeasurementInterval(uint32_t interval) {
    if (interval > _sensor->getMinimumMeasurementIntervalMs()) {
        _measurementIntervalMs = interval;
        return NO_ERROR;
    }
    return 1;
}

AutoDetectorError SensorStateMachine::update() {
    AutoDetectorError error = NO_ERROR;
    switch (_sensorState) {
        case SensorStatus::UNDEFINED:
            break;

        case SensorStatus::UNINITIALIZED:
            error = _initialize();
            if (error) {
                _initErrorCounter++;
            } else {
                _initErrorCounter = 0;
            }
            break;

        case SensorStatus::INITIALIZING:
            _initializationRoutine();
            break;

        case SensorStatus::RUNNING:
            error = _readSignalsRoutine();
            if (error) {
                _measurementErrorCounter++;
            } else {
                _measurementErrorCounter = 0;
            }
            break;

        case SensorStatus::LOST:
            break;

        default:
            break;
    }

    uint16_t nAllowed = _sensor->getNumberOfAllowedConsecutiveErrors();
    if (_initErrorCounter > nAllowed || _measurementErrorCounter > nAllowed) {
        _sensorState = SensorStatus::LOST;
        return LOST_SENSOR_ERROR;
    }

    return error;
}

ISensor* SensorStateMachine::getSensor() const {
    return _sensor;
}

const MeasurementList* SensorStateMachine::getSignals() const {
    return &_sensorSignals;
}
