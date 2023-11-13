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

void SensorStateMachine::initializationRoutine(Data& dataContainer) {

    if (_sensorState == SensorStatus::UNINITIALIZED) {
        _sensor->initializationStep();
        _lastMeasurementTimeStampMs = millis();
        _sensorState = SensorStatus::INITIALIZING;
    }

    for (size_t i = 0; i < _sensor->getNumberOfDataPoints(); ++i) {
            dataContainer.addDataPoint(
                DataPoint(SignalType::UNDEFINED, 0.0, 0,
                          sensorName(_sensor->getSensorId())));
    }

    if (timeIntervalPassed(_sensor->getInitializationIntervalMs(), millis(),
                           _lastMeasurementTimeStampMs)) {
        _sensorState = SensorStatus::RUNNING;
    }
}

void SensorStateMachine::readSignalsRoutine(Data& dataContainer) {
    if (timeIntervalPassed(_measurementIntervalMs, millis(),
                           _lastMeasurementTimeStampMs)) {
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
            dataContainer.addDataPoint(sensorSignalsBuffer[i]);
        }
    }
}

void SensorStateMachine::updateSensorSignals(Data& data) {
    // State handling
    switch (_sensorState) {
        case SensorStatus::UNDEFINED:
            break;

        case SensorStatus::UNINITIALIZED:
            initializationRoutine(data);
            break;

        case SensorStatus::INITIALIZING:
            initializationRoutine(data);
            break;

        case SensorStatus::RUNNING:
            readSignalsRoutine(data);
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
