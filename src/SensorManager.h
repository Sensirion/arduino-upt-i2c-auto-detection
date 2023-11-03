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

#include "Data.h"
#include "IAutoDetector.h"
#include "SensirionCore.h"
#include "SensorList.h"

/*Class to manage the sensors connected to the board's I2C bus. Handles
 * detection and signal polling in accordance to the sensor's minimal polling
 * intervals */
class SensorManager {
  private:
    Data _data;
    SensorList _sensorList;
    IAutoDetector& _detector;

    /**
     * @brief Writes sensor signals into _data, modifies sensor status
     *
     * @param[in] Isensor* pointer to a ISensor instance to poll for signals
     *
     * @param[in] index of the ISensor index in _sensorList
     */
    void _updateSensor(ISensor* sensor, int index);

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
    explicit SensorManager(IAutoDetector& detector_) : _detector(detector_){};

    /**
     * @brief Must be called before any other method
     *
     * Searches I2C bus for available sensors, initializes _data
     */
    void begin();

    /**
     * @brief Calls measure methods on all available sensors, while respecting
     * state and interval conditions
     *
     * Iterates through list of sensors identified during the scan of the I2C
     * bus and polls them for signals
     *
     * @return AutoDetectorError, LOST_SENSOR_ERROR incase one or more sensors
     * were lost during polling
     */
    AutoDetectorError updateData();

    /**
     * @brief getter method for _data
     */
    const Data& getData() const;

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
            if (_sensorList.getSensor(i) == nullptr) {
                continue;
            }
            if (_sensorList.getSensor(i)->getSensorId() == id) {
                pDriver =
                    reinterpret_cast<T*>(_sensorList.getSensor(i)->getDriver());
                return NO_ERROR;
            }
        }
        return DRIVER_NOT_FOUND_ERROR;
    };
};

#endif /* _SENSOR_MANAGER_H_ */
