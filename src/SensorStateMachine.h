#ifndef _SENSOR_STATE_MACHINE_H_
#define _SENSOR_STATE_MACHINE_H_

#include "AutoDetectorErrors.h"
#include "ISensor.h"
#include "MeasurementList.h"
#include "SensirionErrors.h"
#include <stdint.h>

enum class SensorStatus {
    UNDEFINED,
    UNINITIALIZED,  // Sensor is online on the bus but neither running a
                    // conditioning program nor ready to produce a measurement
    INITIALIZING,   // Sensor is running a conditioning program
    RUNNING,  // Sensor is ready to produce readings on command. This state can
              // decay to INITIALIZING.
    LOST  // Sensor has not replied to a certain number of consecutive commands
};

/*
 * Class handling the state machine for the sensors without including the state
 * information and operations common to all sensor types to the interface class.
 */
class SensorStateMachine {
  private:
    SensorStatus _sensorState;
    uint8_t _initErrorCounter;
    uint8_t _measurementErrorCounter;
    uint32_t _lastMeasurementTimeStampMs;
    uint32_t _measurementIntervalMs;

    ISensor* _sensor;
    MeasurementList _sensorSignals;

    /**
     * @brief initialize the state machine. Promotes sensor state to
     * INITIALIZING or RUNNING when appropriate.
     *
     * @note Needs to be outside of constructor because state machines may decay
     * to UNINITIALIZED
     *
     * @return  I2C_ERROR if ISensor::initializationStep() fails (in which case
     *            the driver error is decoded and printed to console)
     *          NO_ERROR on success
     */
    AutoDetectorError _initialize();

    /**
     * @brief Update state machine for sensors whose state is INITIALIZING
     *
     * @note Toggles _sensorStatus to RUNNING incase all initialisation Steps
     * are completed
     */
    void _initializationRoutine();

    /**
     * @brief Update state machine for sensors whose state is RUNNING
     *
     * @note Does not perform a measurement if the measurement interval is too
     * short, or too long.
     *
     * @return  I2C_ERROR if _readSignals() fails (in which case the driver
     *            error is decoded and printed to console)
     *          SENSOR_READY_STATE_DECAYED_ERROR if too much time has elapsed
     *            since last measurement was performed NO_ERROR on success
     */
    AutoDetectorError _readSignalsRoutine();

    /**
     * @brief Query sensor for new signals
     *
     * @return  I2C_ERROR if ISensor::measureAndWrite() fails (in which case the
     *            error is decoded and printed to console)
     *          NO_ERROR on success
     */
    AutoDetectorError _readSignals();

  public:
    SensorStateMachine()
        : _sensorState(SensorStatus::UNDEFINED), _initErrorCounter(0),
          _measurementErrorCounter(0), _lastMeasurementTimeStampMs(0),
          _measurementIntervalMs(0), _sensor(nullptr){};

    /**
     * @brief constructor with ISensor pointer, used by autodetector
     */
    explicit SensorStateMachine(ISensor*);

    /**
     * @brief getter method for _sensorState
     */
    SensorStatus getSensorState() const;

    /**
     * @brief setter method for _measurementIntervalMs.
     *
     * @note Function call has no effect if the requested measurement interval
     * is smaller than the sensor's minimum measurement interval.
     *
     * @return  1 if the specified interval is shorter than the minimum
     *            measurement interval allowed for the sensor
     *          NO_ERROR on success
     */
    uint16_t setMeasurementInterval(uint32_t);

    /**
     * @brief update state machine
     *
     * @note May print I2C Communication error messages to console via Arduino
     * Serial, but such errors may not be fatal.
     *
     * @return  I2C_ERROR if bus communication fails (in which case the
     *            driver error is decoded and printed to console)
     *          SENSOR_LOST_ERROR if allowable number of consecutive operation
     *            errors was exceeded during update
     *          SENSOR_READY_STATE_DECAYED_ERROR if too much time has elapsed
     *            since last measurement was performed
     *          NO_ERROR on success
     */
    AutoDetectorError update();

    /**
     * @brief getter method for sensor handled by state machine
     *
     * @note: Needed to fetch number of signal Measurements from SensorManager
     */
    ISensor* getSensor() const;

    /**
     * @brief getter method for address of sensor signals
     */
    const MeasurementList* getSignals() const;
};

#endif /* _SENSOR_STATE_MACHINE_H_ */
