#include "ISensor.h"
#include "utils.h"

uint16_t ISensor::getNumberOfAllowedConsecutiveErrors() const {
    return _NUMBER_OF_ALLOWED_CONSECUTIVE_ERRORS;
}

SensorState ISensor::getSensorState() const {
    return _sensorState;
}

void ISensor::setSensorState(SensorState s) {
    _sensorState = s;
}

void ISensor::setLastMeasurementError(uint16_t err) {
    _lastMeasurementError = err;
}

uint16_t ISensor::getMeasurementErrorCounter() const {
    return _measurementErrorCounter;
}

void ISensor::resetMeasurementErrorCounter() {
    _measurementErrorCounter = 0;
}

void ISensor::incrementMeasurementErrorCounter() {
    _measurementErrorCounter++;
}

uint16_t ISensor::getInitStepsCounter() const {
    return _initStepsCounter;
}

void ISensor::incrementInitStepsCounter() {
    _initStepsCounter++;
}

uint32_t ISensor::getLastMeasurementTimeStamp() const {
    return _lastMeasurementTimeStampMs;
}

void ISensor::setLastMeasurementTimeStamp(uint32_t ts) {
    _lastMeasurementTimeStampMs = ts;
}

uint32_t ISensor::getMeasurementInterval() const {
    if (_customMeasurementIntervalMs > getMinimumMeasurementIntervalMs()) {
        return _customMeasurementIntervalMs;
    } else {
        return getMinimumMeasurementIntervalMs();
    }
}

void ISensor::setMeasurementInterval(uint32_t interval) {
    if (interval > getMinimumMeasurementIntervalMs()) {
        _customMeasurementIntervalMs = interval;
    }
}

void ISensor::updateSensorSignals(Data& data) {
    // Collect variables for readability
    unsigned long currentTimeStampMs = millis();
    unsigned long initSteps = getInitializationSteps();
    unsigned long initIntervalMs = getInitializationIntervalMs();
    unsigned long measureIntervalMs = getMeasurementInterval();

    uint16_t measureAndWriteError = 0x1234;
    DataPoint sensorSignalsBuffer[getNumberOfDataPoints()];

    // State handling
    switch (_sensorState) {
        case SensorState::UNDEFINED:
            break;

        case SensorState::INITIALIZING:
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
                setSensorState(SensorState::RUNNING);
            }
            break;

        case SensorState::RUNNING:
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
                    setSensorState(SensorState::LOST);
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

        case SensorState::LOST:
            break;

        default:
            break;
    }

    return;
}
