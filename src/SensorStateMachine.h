#ifndef _SENSOR_STATE_MACHINE_H_
#define _SENSOR_STATE_MACHINE_H_

#include "Data.h"
#include "ISensor.h"
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
    uint16_t _lastMeasurementError;
    uint16_t _measurementErrorCounter;
    uint32_t _lastMeasurementTimeStampMs;
    uint32_t _measurementIntervalMs;

    ISensor* _sensor;
    Data _sensorSignals;

    /**
     * @brief Update state machine for sensors whose state is UNINITIALIZED or
     * INITIALIZING
     *
     * @note Toggles _sensorStatus to RUNNING incase all initialisation Steps
     * are completed
     */
    void initializationRoutine();

    /**
     * @brief Update state machine for sensors whose state is RUNNING
     *
     * @note Doesn't throw an error in case the measurement fails, but records
     * the error and increments the error counter in the state machine. Does not
     * perform a measurement if the measurement interval is too short, or too
     * long.
     */
    void readSignalsRoutine();

    /**
     * @brief Query sensor for new signals
     */
    void readSignals();

  public:
    SensorStateMachine()
        : _sensorState(SensorStatus::UNDEFINED), _lastMeasurementError(0),
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
     * @brief setter method for _sensorState
     */
    void setSensorState(SensorStatus);

    /**
     * @brief setter method for _measurementIntervalMs.
     *
     * @note Function call has no effect if the requested measurement interval
     * is smaller than the sensor's minimum measurement interval.
     */
    void setMeasurementInterval(uint32_t);

    /**
     * @brief update state machine
     */
    void update();

    /**
     * @brief getter method for sensor handled by state machine
     *
     * @note: Needed to fetch number of signal DataPoints from SensorManager
     */
    ISensor* getSensor() const;

    /**
     * @brief getter method for address of sensor signals
     */
    const Data* getSignals() const;
};

#endif /* _SENSOR_STATE_MACHINE_H_ */
