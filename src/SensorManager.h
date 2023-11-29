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

#ifndef _SENSOR_MANAGER_H_
#define _SENSOR_MANAGER_H_

#include "DataPointList.h"
#include "IAutoDetector.h"
#include "SensirionCore.h"
#include "SensorList.h"

/* Class to manage the sensors connected to the board's I2C bus. Handles
 * detection and signal polling in accordance to the sensor's minimal polling
 * intervals */
class SensorManager {
  private:
    static const uint8_t _MAX_NUM_SENSORS = 16;
    SensorList _sensorList;
    IAutoDetector& _detector;

  public:
    /**
     * @brief constructor
     *
     * @param[in] IAutoDetector& detector instance with which to seek for
     * connected sensors
     *
     * @note With this construction we by default limit ourselves to one I2C
     * bus, ie. it is not possible to seek for sensors on both the 3.3V and 5V
     * buses
     */
    explicit SensorManager(IAutoDetector& detector_)
        : _sensorList(_MAX_NUM_SENSORS), _detector(detector_){};

    /**
     * @brief Remove lost sensors and check bus for connected sensors
     */
    void refreshConnectedSensors();

    /**
     * @brief Updates all sensor state machines, which fetches signal updates
     * (whenever available)
     */
    void executeSensorCommunication();

    /**
     * @brief obtain a hashmap of read-only pointers to the sensor signal
     * readings.
     *
     * @param[in] DataPointList** location to which write the references to the
     * individual state machines data, hashed by their respective SensorIDs.
     * Size of the hashmap can be queried using
     * SensorManager::getMaxNumberOfSensors().
     */
    void getSensorReadings(const DataPointList**);

    /**
     * @brief Sets polling interval for the specified sensor after checking if
     * it is valid
     *
     * @param[in] interval desired measurement interval
     *
     * @param[in] SensorID target sensor
     *
     * @note Does not return an error in case the validity checks fail, in which
     * case the interval is not set for the sensor
     */
    void setInterval(unsigned long interval, SensorID sensorId);

    /**
     * @brief getter method for number of sensors
     */
    int getMaxNumberOfSensors() const;

    /**
     * Retrieve specific sensor driver instance T from
     * SensorManager::_sensorList
     *
     * @param[in] pDriver nullptr initialized pointer to specific Sensirion
     * sensor driver class T.
     *
     * @param[in] id SensorId corresponding to ISensor implementation for sensor
     * driver class T.
     *
     * @param[out] AutodDetectorError::DRIVER_NOT_FOUND_ERROR in case of failure
     * to retrieve the driver. Only in case of NO_ERROR may the driver methods
     * be called. e.g.: pDriver->driverMethod()
     */
    template <class T>
    AutoDetectorError getSensorDriver(T*& pDriver, SensorID id) {
        for (int i = 0; i < _sensorList.getLength(); ++i) {
            if (_sensorList.getSensor(i) &&
                _sensorList.getSensor(i)->getSensorId() == id) {
                pDriver =
                    reinterpret_cast<T*>(_sensorList.getSensor(i)->getDriver());
                return NO_ERROR;
            }
        }
        return DRIVER_NOT_FOUND_ERROR;
    };
};

#endif /* _SENSOR_MANAGER_H_ */
