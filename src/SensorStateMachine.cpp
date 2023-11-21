#include "SensorStateMachine.h"
#include "utils.h"

SensorStateMachine::SensorStateMachine(ISensor* pSensor)
    : _lastMeasurementError(0), _measurementErrorCounter(0),
      _initStepsCounter(0), _lastMeasurementTimeStampMs(0),
      _measurementIntervalMs(0), _sensor(pSensor) {
    if (_sensor->requiresInitializationStep()) {
        _sensorState = SensorStatus::UNINITIALIZED;
    } else {
        _sensorState = SensorStatus::RUNNING;
    }
    _measurementIntervalMs = _sensor->getMinimumMeasurementIntervalMs();
    _sensorSignals.init(_sensor->getNumberOfDataPoints());
};

SensorStatus SensorStateMachine::getSensorState() const {
    return _sensorState;
}

void SensorStateMachine::setSensorState(SensorStatus s) {
    _sensorState = s;
}

void SensorStateMachine::setMeasurementInterval(uint32_t interval) {
    if (interval > _sensor->getMinimumMeasurementIntervalMs()) {
        _measurementIntervalMs = interval;
    }
}

void SensorStateMachine::initializationRoutine() {
    if (_sensorState == SensorStatus::UNINITIALIZED) {
        _sensor->initializationStep();
        _lastMeasurementTimeStampMs = millis();
        _sensorState = SensorStatus::INITIALIZING;

        for (size_t i = 0; i < _sensor->getNumberOfDataPoints(); ++i) {
            _sensorSignals.addDataPoint(
                DataPoint(SignalType::UNDEFINED, 0.0, 0,
                          sensorName(_sensor->getSensorId())));
        }
    }

    if (timeIntervalPassed(_sensor->getInitializationIntervalMs(), millis(),
                           _lastMeasurementTimeStampMs)) {
        _sensorState = SensorStatus::RUNNING;
        _lastMeasurementTimeStampMs = millis();
    }
}

void SensorStateMachine::readSignals() {
    DataPoint sensorSignalsBuffer[_sensor->getNumberOfDataPoints()];
    uint32_t currentTimeStampMs = millis();

    uint16_t measureAndWriteError =
        _sensor->measureAndWrite(sensorSignalsBuffer, currentTimeStampMs);
    _lastMeasurementError = measureAndWriteError;

    if (measureAndWriteError) {
        _measurementErrorCounter++;
        if (_measurementErrorCounter >=
            _sensor->getNumberOfAllowedConsecutiveErrors()) {
            _sensorState = SensorStatus::LOST;
        }
        return;
    }

    _lastMeasurementTimeStampMs = currentTimeStampMs;
    _measurementErrorCounter = 0;

    for (size_t i = 0; i < _sensor->getNumberOfDataPoints(); ++i) {
        _sensorSignals.addDataPoint(sensorSignalsBuffer[i]);
    }
    _sensorSignals.resetWriteHead();
}

void SensorStateMachine::readSignalsRoutine() {
    unsigned long timeSinceLastMeasurementMs =
        millis() - _lastMeasurementTimeStampMs;

    /* Determine timing relationship vs. last measurement */
    enum class timeLineRegion {
        INSIDE_MIN_INTERVAL,
        INSIDE_VALID_BAND,
        OUTSIDE_VALID_INITIALIZATION
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
    switch (tlr_position) {
        case timeLineRegion::INSIDE_MIN_INTERVAL:
            break;

        case timeLineRegion::INSIDE_VALID_BAND:
            readSignals();
            break;

        case timeLineRegion::OUTSIDE_VALID_INITIALIZATION:
            _sensorState = SensorStatus::UNINITIALIZED;
            initializationRoutine();
            // throw error
            break;

        default:
            break;
    }
}

void SensorStateMachine::update() {
    switch (_sensorState) {
        case SensorStatus::UNDEFINED:
            break;

        case SensorStatus::UNINITIALIZED:
            initializationRoutine();
            break;

        case SensorStatus::INITIALIZING:
            initializationRoutine();
            break;

        case SensorStatus::RUNNING:
            readSignalsRoutine();
            break;

        case SensorStatus::LOST:
            break;

        default:
            break;
    }
    return;
}

ISensor* SensorStateMachine::getSensor() const {
    return _sensor;
}

const Data* SensorStateMachine::getSignals() const {
    return &_sensorSignals;
}
