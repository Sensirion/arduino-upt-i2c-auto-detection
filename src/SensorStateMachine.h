/*
 * Copyright (c) 2023, Sensirion AG
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * * Redistributions of source code must retain the above copyright notice, this
 *   list of conditions and the following disclaimer.
 *
 * * Redistributions in binary form must reproduce the above copyright notice,
 *   this list of conditions and the following disclaimer in the documentation
 *   and/or other materials provided with the distribution.
 *
 * * Neither the name of Sensirion AG nor the names of its
 *   contributors may be used to endorse or promote products derived from
 *   this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef _SENSOR_STATE_MACHINE_H_
#define _SENSOR_STATE_MACHINE_H_

#include "ISensor.h"
#include "MeasurementList.h"
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
    MeasurementList _sensorSignals;

    /**
     * @brief initialize the state machine
     *
     * @note Needs to be outside of constructor because state machines may decay
     * to UNINITIALIZED
     */
    void _initialize();

    /**
     * @brief Update state machine for sensors whose state is UNINITIALIZED or
     * INITIALIZING
     *
     * @note Toggles _sensorStatus to RUNNING incase all initialisation Steps
     * are completed
     */
    void _initializationRoutine();

    /**
     * @brief Update state machine for sensors whose state is RUNNING
     *
     * @note Doesn't throw an error in case the measurement fails, but records
     * the error and increments the error counter in the state machine. Does not
     * perform a measurement if the measurement interval is too short, or too
     * long.
     */
    void _readSignalsRoutine();

    /**
     * @brief Query sensor for new signals
     */
    void _readSignals();

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
    const MeasurementList* getSignals() const;
};

#endif /* _SENSOR_STATE_MACHINE_H_ */
