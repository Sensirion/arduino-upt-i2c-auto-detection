#include "ISensor.h"

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
