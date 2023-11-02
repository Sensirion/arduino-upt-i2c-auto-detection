#include "ISensor.h"

uint16_t ISensor::getNumberOfAllowedConsecutiveErrors() const {
    return _NUMBER_OF_ALLOWED_CONSECUTIVE_ERRORS;
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
