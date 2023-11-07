#include "SensorStateMachine.h"
#include "utils.h"

SensorStatus SensorStateMachine::getSensorState() const {
    return _sensorState;
}

void SensorStateMachine::setSensorState(SensorStatus s) {
    _sensorState = s;
}

void SensorStateMachine::setLastMeasurementError(uint16_t err) {
    _lastMeasurementError = err;
}

uint16_t SensorStateMachine::getMeasurementErrorCounter() const {
    return _measurementErrorCounter;
}

void SensorStateMachine::resetMeasurementErrorCounter() {
    _measurementErrorCounter = 0;
}

void SensorStateMachine::incrementMeasurementErrorCounter() {
    _measurementErrorCounter++;
}

uint16_t SensorStateMachine::getInitStepsCounter() const {
    return _initStepsCounter;
}

void SensorStateMachine::incrementInitStepsCounter() {
    _initStepsCounter++;
}

uint32_t SensorStateMachine::getLastMeasurementTimeStamp() const {
    return _lastMeasurementTimeStampMs;
}

void SensorStateMachine::setLastMeasurementTimeStamp(uint32_t ts) {
    _lastMeasurementTimeStampMs = ts;
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

void SensorStateMachine::updateSensorSignals(Data& data) {
    // Collect variables for readability
    unsigned long currentTimeStampMs = millis();
    unsigned long initSteps = getInitializationSteps();
    unsigned long initIntervalMs = getInitializationIntervalMs();
    unsigned long measureIntervalMs = getMeasurementInterval();

    uint16_t measureAndWriteError = 0x1234;
    DataPoint sensorSignalsBuffer[getNumberOfDataPoints()];

    // State handling
    switch (_sensorState) {
        case SensorStatus::UNDEFINED:
            break;

        case SensorStatus::INITIALIZING:
            // Set Sensor name of empty Datapoints for initialization period
            if (getInitStepsCounter() == 0) {
                for (size_t i = 0; i < getNumberOfDataPoints(); ++i) {
                    data.addDataPoint(DataPoint(SignalType::UNDEFINED, 0.0, 0,
                                                sensorName(getSensorId())));
                }
            }

            // Only perform initialization every initialization interval
            if (!timeIntervalPassed(initIntervalMs, currentTimeStampMs,
                                    getLastMeasurementTimeStamp())) {
                break;
            }
            initializationStep();
            incrementInitStepsCounter();

            // Check if initialization is done
            if (getInitStepsCounter() >= initSteps) {
                setSensorState(SensorStatus::RUNNING);
            }
            break;

        case SensorStatus::RUNNING:
            // Only perform measurement every measurement interval
            if (!timeIntervalPassed(measureIntervalMs, currentTimeStampMs,
                                    getLastMeasurementTimeStamp())) {
                break;
            }

            // Perform measurement
            measureAndWriteError =
                measureAndWrite(sensorSignalsBuffer, currentTimeStampMs);
            setLastMeasurementError(measureAndWriteError);

            if (measureAndWriteError) {
                incrementMeasurementErrorCounter();
                if (getMeasurementErrorCounter() >=
                    getNumberOfAllowedConsecutiveErrors()) {
                    setSensorState(SensorStatus::LOST);
                }
                break;
            }

            setLastMeasurementTimeStamp(currentTimeStampMs);

            // Push DataPoints to Data container
            for (size_t i = 0; i < getNumberOfDataPoints(); ++i) {
                data.addDataPoint(sensorSignalsBuffer[i]);
            }
            resetMeasurementErrorCounter();
            setLastMeasurementTimeStamp(currentTimeStampMs);

            break;

        case SensorStatus::LOST:
            break;

        default:
            break;
    }

    return;
}
