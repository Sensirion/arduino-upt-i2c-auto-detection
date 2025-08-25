#ifndef I_SENSOR_H
#define I_SENSOR_H

#include "Arduino.h"
#include "Sensirion_UPT_Core.h"
#include <vector>

/* Class handling communication with a particular sensor over a communication
 * bus */
class ISensor {
  private:
    static constexpr uint16_t NUMBER_OF_ALLOWED_CONSECUTIVE_ERRORS = 3;

  public:

    using DeviceType = sensirion::upt::core::DeviceType;
    using MeasurementList = std::vector<sensirion::upt::core::Measurement>;

    virtual ~ISensor() = default;

    /**
     * @brief Initialize sensor driver such that data may be read out
     *
     * @return A uint16_t error corresponding to SensirionErrors.h of
     * SensirionCore, where 0 value corresponds to no error.
     */
    virtual uint16_t start() = 0;

    /**
     * @brief Perform extended sensor initialization (if applicable) and
     * determines sensor metadata.
     */
    virtual uint16_t initializationStep() = 0;

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
    virtual uint16_t measureAndWrite(MeasurementList& measurements,
                                     unsigned long timeStamp) = 0;

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
     * conditioning procedure must be executed again. The time interval within
     * which the sensor must be called for a reading can be queried with this
     * method.
     *
     * @returns Time (in ms) within which the sensor must be called for
     * a measurement, else it decays to the UNINITIALIZED state. Returns -1 if
     * no such interval is defined for the sensor (ie. decay time is infinite).
     */
    virtual long readyStateDecayTimeMs() const {
        return -1;
    }

    /**
     * @brief Get the specific SensorType of the ISensor realization
     *
     * @return DeviceType
     */
    virtual DeviceType getDeviceType() const = 0;

    /**
     * @brief Get the MetaData of the ISensor realization
     *
     * @return MetaData
     */
    virtual sensirion::upt::core::MetaData getMetaData() const = 0;

    /**
     * @brief getter method for _NUMBER_OF_ALLOWED_CONSECUTIVE_ERRORS
     */
    static uint16_t getNumberOfAllowedConsecutiveErrors() {
        return NUMBER_OF_ALLOWED_CONSECUTIVE_ERRORS;
    }

    /**
     * @brief Get a pointer to the sensor driver, reinterpret_cast-ed into a
     * void pointer
     *
     * @return void*
     */
    virtual void* getDriver() = 0;
};

#endif /* I_SENSOR_H */
