#include "ISensor.h"

uint16_t ISensor::getNumberOfAllowedConsecutiveErrors() const {
    return _NUMBER_OF_ALLOWED_CONSECUTIVE_ERRORS;
}
