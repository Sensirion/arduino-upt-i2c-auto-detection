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

void ISensor::setLatestMeasurementError(uint16_t err) {
    _latestMeasurementError = err;
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
    return _initStepCounter;
}

void ISensor::incrementInitStepsCounter() {
    _initStepCounter++;
}

uint32_t ISensor::getLatestMeasurementTimeStamp() const {
    return _latestMeasurementTimeStamp;
}

void ISensor::setLatestMeasurementTimeStamp(uint32_t ts) {
    _latestMeasurementTimeStamp = ts;
}

uint32_t ISensor::getMeasurementInterval() const {
    if (_customMeasurementInterval > getMinimumMeasurementIntervalMs()) {
        return _customMeasurementInterval;
    } else {
        return getMinimumMeasurementIntervalMs();
    }
}

void ISensor::setMeasurementInterval(uint32_t interval) {
    if (interval > getMinimumMeasurementIntervalMs()) {
        _customMeasurementInterval = interval;
    }
}

void ISensor::updateSensorSignals(Data& data) {
    // Collect variables for readability
    unsigned long currentTimeStamp = millis();
    unsigned long initSteps = getInitializationSteps();
    unsigned long initIntervalMs = getInitializationIntervalMs();
    unsigned long measureIntervalMs = getMeasurementInterval();

    uint16_t measureAndWriteError = 0x1234;
    DataPoint sensorSignalsBuffer[getNumberOfDataPoints()];

    // State handling
    switch (getSensorState()) {
        case SensorState::UNDEFINED:
            break;

        case SensorState::INITIALIZING:
            // Check if initialization is done
            if (getInitStepsCounter() >= initSteps) {
                setSensorState(SensorState::RUNNING);
            }
            // Set Sensor name of empty Datapoints for initialization period
            if (getInitStepsCounter() == 0) {
                for (size_t i = 0; i < getNumberOfDataPoints(); ++i) {
                    data.addDataPoint(DataPoint(SignalType::UNDEFINED, 0.0, 0,
                                                sensorName(getSensorId())));
                }
            }
            // Only perform initialization every initialization interval
            if (!timeIntervalPassed(initIntervalMs, currentTimeStamp,
                                    getLatestMeasurementTimeStamp())) {
                break;
            }
            initializationStep();
            incrementInitStepsCounter();
            break;

        case SensorState::RUNNING:
            // Only perform measurement every measurement interval
            if (!timeIntervalPassed(measureIntervalMs, currentTimeStamp,
                                    getLatestMeasurementTimeStamp())) {
                break;
            }

            measureAndWriteError =
                measureAndWrite(sensorSignalsBuffer, currentTimeStamp);
            setLatestMeasurementError(measureAndWriteError);

            // Update error counter
            if (measureAndWriteError) {
                incrementMeasurementErrorCounter();
                if (getMeasurementErrorCounter() >=
                    getNumberOfAllowedConsecutiveErrors()) {
                    setSensorState(SensorState::LOST);
                }
                break;
            }

            for (size_t i = 0; i < getNumberOfDataPoints(); ++i) {
                data.addDataPoint(sensorSignalsBuffer[i]);
            }
            resetMeasurementErrorCounter();
            setLatestMeasurementTimeStamp(currentTimeStamp);

            break;

        case SensorState::LOST:
            break;

        default:
            break;
    }

    return;
}
