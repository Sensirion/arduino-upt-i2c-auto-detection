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
#ifndef _SENSOR_LIST_H
#define _SENSOR_LIST_H

#include "AutoDetectorErrors.h"
#include "ISensor.h"

enum class SensorState { UNDEFINED, INITIALIZING, RUNNING, LOST };

struct SensorList {
    static constexpr int LENGTH = 16;
    static constexpr int NUMBER_OF_ALLOWED_CONSECUTIVE_ERRORS = 3;
    ISensor* sensors[LENGTH] =
        {};  // = {} initializes all elements to 0, i.e. here to nullptr
    uint16_t latestMeasurementErrors[LENGTH] = {};
    uint16_t measurementErrorCounters[LENGTH] = {};
    uint16_t initStepCounters[LENGTH] = {};
    unsigned long latestUpdateTimeStamps[LENGTH] = {};
    unsigned long measurementIntervals[LENGTH] = {};
    SensorState sensorStates[LENGTH] = {};
    /**
     * @brief add a sensor to the list of tracked sensors
     *
     * @param[in] ISensor* pointer to the sensor to be added to teh list
     *
     * @param[out] AutoDetectorError::FULL_SENSOR_LIST_ERROR in case the list is
     * already full
     */
    AutoDetectorError addSensor(ISensor* pSensor);
    /// @brief Resets the list
    void reset();
    /**
     * @brief Counts sensors contained in the list
     *
     * @param[out] size_t number of sensors in the list
     */
    size_t count();
    /**
     * @brief count number of signals measured by all sensors contained in the
     * list
     *
     * @param[out] size_t number of DataPoints returned by the sensors in the
     * list
     */
    size_t getTotalNumberOfDataPoints();
    /**
     * @brief Counts number of sensors whose state is SensorState::LOST
     *
     * @param[out] uint16_t number of lost sensors
     */
    uint16_t getNumberOfSensorsLost();
};

#endif /* _SENSOR_LIST_H */
