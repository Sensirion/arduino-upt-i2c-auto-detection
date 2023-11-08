#include "SensorStateMachine.h"
#include "utils.h"

SensorStatus SensorStateMachine::getSensorState() const {
    return _sensorState;
}

void SensorStateMachine::setSensorState(SensorStatus s) {
    _sensorState = s;
}

uint32_t SensorStateMachine::getMeasurementInterval() const {
    if (_customMeasurementIntervalMs > getMinimumMeasurementIntervalMs()) {
        return _customMeasurementIntervalMs;
    } else {
        return getMinimumMeasurementIntervalMs();
    }
}

void SensorStateMachine::setMeasurementInterval(uint32_t interval) {
    if (interval > getMinimumMeasurementIntervalMs()) {
        _customMeasurementIntervalMs = interval;
    }
}

void SensorStateMachine::initializeSensorRoutine(Data& dataContainer) {
    if (_initStepsCounter == 0) {
        for (size_t i = 0; i < getNumberOfDataPoints(); ++i) {
            dataContainer.addDataPoint(DataPoint(SignalType::UNDEFINED, 0.0, 0,
                                                 sensorName(getSensorId())));
        }
    }

    if (timeIntervalPassed(getInitializationIntervalMs(), millis(),
                           _lastMeasurementTimeStampMs)) {
        initializationStep();
        _initStepsCounter++;

        if (_initStepsCounter >= getInitializationSteps()) {
            _sensorState = SensorStatus::RUNNING;
        }
    }
}

void SensorStateMachine::readSignalsRoutine(Data& dataContainer) {
    if (timeIntervalPassed(getMeasurementInterval(), millis(),
                           _lastMeasurementTimeStampMs)) {
        DataPoint sensorSignalsBuffer[getNumberOfDataPoints()];
        uint32_t currentTimeStampMs = millis();

        uint16_t measureAndWriteError =
            measureAndWrite(sensorSignalsBuffer, currentTimeStampMs);
        _lastMeasurementError = measureAndWriteError;

        if (measureAndWriteError) {
            _measurementErrorCounter++;
            if (_measurementErrorCounter >=
                getNumberOfAllowedConsecutiveErrors()) {
                _sensorState = SensorStatus::LOST;
            }
            return;
        }

        _lastMeasurementTimeStampMs = currentTimeStampMs;
        _measurementErrorCounter = 0;

        for (size_t i = 0; i < getNumberOfDataPoints(); ++i) {
            dataContainer.addDataPoint(sensorSignalsBuffer[i]);
        }
    }
}

void SensorStateMachine::updateSensorSignals(Data& data) {
    // State handling
    switch (_sensorState) {
        case SensorStatus::UNDEFINED:
            break;

        case SensorStatus::INITIALIZING:
            initializeSensorRoutine(data);
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
