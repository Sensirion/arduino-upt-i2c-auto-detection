/*
 * Copyright (c) 2021, Sensirion AG
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
#ifndef _I_SENSOR_H_
#define _I_SENSOR_H_

#include "Arduino.h"
#include "Data.h"
#include "Sensirion_UPT_Core.h"

enum class SensorState { UNDEFINED, INITIALIZING, RUNNING, LOST };

/* Class handling communication with a particular sensor over the I2C bus */
class ISensor {
  private:
    static const uint16_t _NUMBER_OF_ALLOWED_CONSECUTIVE_ERRORS = 3;
    SensorState _sensorState = SensorState::UNDEFINED;
    uint16_t _latestMeasurementError = 0;
    uint16_t _measurementErrorCounter = 0;
    uint16_t _initStepCounter = 0;
    uint32_t _latestMeasurementTimeStamp = 0;
    uint32_t _customMeasurementInterval = 0;

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
     * @brief Perform extended sensor initialization.
     * As most sensors don't require this, overriding this method is optional.
     */
    virtual uint16_t initializationStep() {
        return 0;
    };

    /**
     * @brief Get the specific SensorId of the ISensor realization
     *
     * @return SensorId
     */
    virtual SensorID getSensorId() const = 0;

    /**
     * @brief Get the number of DataPoints this sensor occupies in the Data
     * object.
     *
     * @return size_t
     */
    virtual size_t getNumberOfDataPoints() const = 0;

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
     * @brief Get a pointer to the sensordriver, reinterpret_cast-ed into a void
     * pointer
     *
     * @return void*
     */
    virtual void* getDriver() = 0;

    /**
     * @brief getter method for _NUMBER_OF_ALLOWED_CONSECUTIVE_ERRORS
     */
    uint16_t getNumberOfAllowedConsecutiveErrors() const;

    /**
     * @brief getter method for _sensorState
     */
    SensorState getSensorState() const;

    /**
     * @brief setter method for _sensorState
     */
    void setSensorState(SensorState);

    /**
     * @brief setter method for  _latestMeasurementError
     */
    void setLatestMeasurementError(uint16_t);

    /**
     * @brief getter method for _measurementErrorCounter
     */
    uint16_t getMeasurementErrorCounter() const;

    /**
     * @brief reset _measurementErrorCounter to 0
     */
    void resetMeasurementErrorCounter();

    /**
     * @brief increment _measurementErrorCounter
     */
    void incrementMeasurementErrorCounter();

    /**
     * @brief getter method for _initStepCounter
     */
    uint16_t getInitStepsCounter() const;

    /**
     * @brief increment _initStepCounter
     */
    void incrementInitStepsCounter();

    /**
     * @brief getter method for _latestMeasurementTimeStamp
     */
    uint32_t getLatestMeasurementTimeStamp() const;

    /**
     * @brief setter method for _latestMeasurementTimeStamp
     */
    void setLatestMeasurementTimeStamp(uint32_t);

    /**
     * @brief getter method for _customMeasurementInterval
     */
    uint32_t getMeasurementInterval() const;

    /**
     * @brief setter method for _customMeasurementInterval
     */
    void setMeasurementInterval(uint32_t);

    /**
     * @brief update Sensor reading in Data
     */
    void updateSensorSignals(Data&);
};

#endif /* _I_SENSOR_H_ */
