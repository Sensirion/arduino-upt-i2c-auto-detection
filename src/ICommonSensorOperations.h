#ifndef _SENSOR_OPERATIONS_H_
#define _SENSOR_OPERATIONS_H_

#include "DataPoint.h"
#include "SensorID.h"
#include <cstddef>
#include <stdint.h>

/* Intermediate interface class declaring common sensor operations needed both
 * for the interface and the state machine */
class ICommonSensorOperations {
  public:
    /**
     * @brief Get the minimum measurement interval of the sensor. This must be
     * larger than the longest possible measurement duration.
     *
     * @return unsigned long
     */
    virtual unsigned long getMinimumMeasurementIntervalMs() const = 0;

    /**
     * @brief Get the number of times the initializationStep method should be
     * called. Note that most sensors don't require an extended initialization.
     *
     * @return const unsigned long
     */
    virtual unsigned long getInitializationSteps() const {
        return 0;
    };

    /**
     * @brief Get the interval at which the initializationStep method should be
     * called during initialization in milliseconds.
     *
     * @return const unsigned long
     */
    virtual unsigned long getInitializationIntervalMs() const {
        return 0;
    };

    /**
     * @brief Get the number of DataPoints this sensor occupies in the Data
     * object.
     *
     * @return size_t
     */
    virtual size_t getNumberOfDataPoints() const = 0;

    /**
     * @brief Perform extended sensor initialization.
     * As most sensors don't require this, overriding this method is optional.
     */
    virtual uint16_t initializationStep() {
        return 0;
    };

    /**
     * @brief Call driver methods to perform measurement and update DataPoints
     *
     * @param dataPoints argument must be at least getNumberOfDataPoints()
     * long
     *
     * @param timeStamp at time of function call, represents milliseconds
     * passed since program startup.
     *
     * @return A uint16_t error corresponding to SensirionErrors.h of
     * SensirionCore, where 0 value corresponds to no error.
     */
    virtual uint16_t measureAndWrite(DataPoint dataPoints[],
                                     const unsigned long timeStamp) = 0;

    /**
     * @brief Get the specific SensorId of the ISensor realization
     *
     * @return SensorId
     */
    virtual SensorID getSensorId() const = 0;

    /**
     * @brief getter method for _NUMBER_OF_ALLOWED_CONSECUTIVE_ERRORS
     */
    virtual uint16_t getNumberOfAllowedConsecutiveErrors() const = 0;
};

#endif /* _SENSOR_OPERATIONS_H_ */
