#ifndef AUTO_DETECTOR_ERRORS_H
#define AUTO_DETECTOR_ERRORS_H

#include <cstdint>

enum AutoDetectorError : uint16_t {
    NO_ERROR = 0,
    DRIVER_NOT_FOUND_ERROR = 1,
    LOST_SENSOR_ERROR = 2,
    SENSOR_READY_STATE_DECAYED_ERROR = 3,
    I2C_ERROR = 4
};

#endif /* AUTO_DETECTOR_ERRORS_H */
