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
