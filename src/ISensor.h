#ifndef _I_SENSOR_H_
#define _I_SENSOR_H_

#include "Arduino.h"
#include "Sensirion_UPT_Core.h"

/* Class handling communication with a particular sensor over a communication
 * bus */
class ISensor {
  private:
    static const uint16_t _NUMBER_OF_ALLOWED_CONSECUTIVE_ERRORS = 3;

  public:
    virtual ~ISensor() = default;

    /**
     * @brief Initialize sensor driver such that data may be read out
     *
     * @return A uint16_t error corresponding to SensirionErrors.h of
     * SensirionCore, where 0 value corresponds to no error.
     */
    virtual uint16_t start() = 0;

    /**
     * @brief Query sensor whether an initialization step is required or not.
     *
     * @param[out] bool true if either a conditioning or initialization command
     * must be called before measurements are available
     */
    virtual bool requiresInitializationStep() const {
        return false;
    };

    /**
     * @brief Perform extended sensor initialization.
     * As most sensors don't require this, overriding this method is optional.
     */
    virtual uint16_t initializationStep() {
        return 0;
    };

    /**
     * @brief Get the duration of the conditioning period
     *
     * @return const unsigned long
     */
    virtual unsigned long getInitializationIntervalMs() const {
        return 0;
    };

    /**
     * @brief Get the number of signal DataPoints this sensor can deliver
     *
     * @return size_t
     */
    virtual size_t getNumberOfDataPoints() const = 0;

    /**
     * @brief Call driver methods to perform measurement and update DataPoints
     *
     * @param measurements argument must be at least getNumberOfDataPoints()
     * long
     *
     * @param timeStamp at time of function call, represents milliseconds
     * passed since program startup.
     *
     * @return A uint16_t error corresponding to SensirionErrors.h of
     * SensirionCore, where 0 value corresponds to no error.
     */
    virtual uint16_t measureAndWrite(Measurement measurements[],
                                     const unsigned long timeStamp) = 0;

    /**
     * @brief Get the minimum measurement interval of the sensor. This must be
     * larger than the longest possible measurement duration.
     *
     * @return unsigned long
     */
    virtual unsigned long getMinimumMeasurementIntervalMs() const = 0;

    /**
     * @brief Get ready condition decay time.
     *
     * Some sensors that require special hardware for the measurement (such as
     * heaters) need to regularly be called for measurements, or else the
     * conditioning procedure must be exectued again. The time interval within
     * which the sensor must be called for a reading can be queried with this
     * method.
     *
     * @param[out] long time (in ms) within which the sensor must be called for
     * a measurement, else it decays to the UNINITIALIZED state. Returns -1 if
     * no such interval is defined for the sensor (ie. decay time is infinite).
     */
    virtual long readyStateDecayTimeMs() const {
        return -1;
    }

    /**
     * @brief Get the specific SensorType of the ISensor realization
     *
     * @return SensorType
     */
    virtual SensorType getSensorType() const = 0;

    /**
     * @brief Get the MetaData of the ISensor realization
     * 
     * @return MetaData
    */
    virtual MetaData getMetaData() const = 0;

    /**
     * @brief getter method for _NUMBER_OF_ALLOWED_CONSECUTIVE_ERRORS
     */
    uint16_t getNumberOfAllowedConsecutiveErrors() const {
        return _NUMBER_OF_ALLOWED_CONSECUTIVE_ERRORS;
    }

    /**
     * @brief Get a pointer to the sensordriver, reinterpret_cast-ed into a void
     * pointer
     *
     * @return void*
     */
    virtual void* getDriver() = 0;
};

#endif /* _I_SENSOR_H_ */
