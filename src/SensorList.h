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

#ifndef _SENSOR_LIST_H
#define _SENSOR_LIST_H

#include "AutoDetectorErrors.h"
#include "SensorStateMachine.h"

/**
 * @note We reduce the size of the sensor universe for the purposes of
 * autodetection, since we cannot communicate with two sensors sharing the same
 * I2C-address. This allows us to use the address as an effective hash (using
 * SensorType would leave "holes" in the hashmap).
 */
enum class SensorHash {
    _UNDEFINED,
    _SCD4X,  // 0x62
    _SFA3X,  // 0x5D
    _SVM4X,  // 0x6A
    _SHT4X,  // 0x44
    _SEN5X,  // 0x69
    _SCD30,  // 0x61
    _STC3X,  // 0x29
    _SGP4X,  // 0x59
};

/* Class to handle the list of sensors on the i2c bus */
class SensorList {
  private:
    const uint8_t _numSensors;
    SensorStateMachine** _sensors = nullptr;
    size_t _hashSensorType(SensorType sensorType) const;

  public:
    explicit SensorList(uint8_t numSensors);

    SensorList(const SensorList&) = delete;  // Illegal operation
    SensorList&
    operator=(const SensorList& other) = delete;  // Illegal operation

    ~SensorList();

    /**
     * @brief add a sensor to the list of tracked sensors. Ignores sensors that
     * are already in the list
     *
     * @param[in] ISensor* pointer to the sensor to be added to the list
     *
     * @param[out] AutoDetectorError
     *          SENSOR_LIST_ALREADY_CONTAINS_SENSOR_ERROR in case the list is
     *          already full
     *          NO_ERROR on success
     */
    AutoDetectorError addSensor(ISensor* pSensor);

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
    size_t getTotalNumberOfDataPoints() const;

    /**
     * @brief getter method for list size
     */
    int getLength() const;

    /**
     * @brief getter method for a stored state machine
     *
     * @note returns a nullptr if no sensor state machine is stored at the
     * requested index
     */
    SensorStateMachine* getSensorStateMachine(size_t);

    /**
     * @brief getter method for a stored sensor
     */
    ISensor* getSensor(SensorType sensorType) const;

    /**
     * @brief check if the given Sensor is contained in the list.
     *
     * @param[in] SensorType of the sensor to be checked for in the list
     *
     * @param[out] bool: True if the sensor is found, false otherwise.
     */
    bool containsSensor(SensorType) const;

    /**
     * @brief remove lost sensors from list
     */
    void removeLostSensors();
};

#endif /* _SENSOR_LIST_H */
