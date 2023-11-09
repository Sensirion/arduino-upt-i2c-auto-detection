#ifndef _SENSOR_STATE_MACHINE_H_
#define _SENSOR_STATE_MACHINE_H_

#include "Data.h"
#include "ICommonSensorOperations.h"
#include <stdint.h>

enum class SensorStatus { UNDEFINED, INITIALIZING, RUNNING, LOST };

/* Class handling the state machine for the sensors without including the state
 * information and operations common to all sensor types to the interface class.
 */
class SensorStateMachine : public ICommonSensorOperations {
  private:
    SensorStatus _sensorState;
    uint16_t _lastMeasurementError;
    uint16_t _measurementErrorCounter;
    uint16_t _initStepsCounter;
    uint32_t _lastMeasurementTimeStampMs;
    uint32_t _customMeasurementIntervalMs;

    /**
     * @brief Return empty DataPoints containing only the source device string,
     * and initialize the sensor.
     *
     * @param[in] Data& Data container which write empty DataPoints
     *
     * @note Toggles _sensorStatus to RUNNING incase all initialisation Steps
     * are completed
     */
    void initializationRoutine(Data&);

    /**
     * @brief Wite sensor signals to DataPoint buffer
     *
     * @param[in] Data& Data container which write measurement DataPoints
     *
     * @note Doesn't throw an error in case the measurement fails, but records
     * the error and increments the error counter in the state machine. Does not
     * perform a measurement if the measurement interval is too short.
     */
    void readSignalsRoutine(Data&);

  public:
    SensorStateMachine()
        : _sensorState(SensorStatus::UNDEFINED), _lastMeasurementError(0),
          _measurementErrorCounter(0), _initStepsCounter(0),
          _lastMeasurementTimeStampMs(0), _customMeasurementIntervalMs(0){};

    /**
     * @brief getter method for _sensorState
     */
    SensorStatus getSensorState() const;

    /**
     * @brief setter method for _sensorState
     */
    void setSensorState(SensorStatus);

    /**
     * @brief getter method for _customMeasurementIntervalMs
     */
    uint32_t getMeasurementInterval() const;

    /**
     * @brief setter method for _customMeasurementIntervalMs.
     *
     * @note Function call has no effect if the requested measurement interval
     * is smaller than the sensor's minimum measurement interval.
     */
    void setMeasurementInterval(uint32_t);

    /**
     * @brief update Sensor reading in Data
     */
    void updateSensorSignals(Data&);
};

#endif /* _SENSOR_STATE_MACHINE_H_ */
