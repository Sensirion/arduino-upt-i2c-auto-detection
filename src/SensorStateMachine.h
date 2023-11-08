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
