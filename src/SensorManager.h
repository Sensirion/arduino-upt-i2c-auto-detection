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

class SensorManager {
  public:
    static constexpr int NUMBER_OF_ALLOWED_CONSECUTIVE_ERRORS = 3;
    void init();
    AutoDetectorError updateData();
    const Data& getData() const;
    void setInterval(unsigned long interval, SensorId sensorId);
    explicit SensorManager(IAutoDetector& detector_) : _detector(detector_){};
    /*
     * Retrieve specific sensor driver instance T from
     * SensorManager::_sensorList
     *
     * @param pDriver nullptr initialized pointer to specific Sensirion sensor
     * driver class T.
     * @param id SensorId corresponding to ISensor implementation for sensor
     * driver class T.
     * @return AutodDetectorError::NO_ERROR = 0 on success. Only in this case
     * may the driver methods be called. e.g.: pDriver->driverMethod()
     */
    template <class T>
    AutoDetectorError getSensorDriver(T*& pDriver, SensorId id) {
        for (int i = 0; i < SensorList::LENGTH; ++i) {
            if (_sensorList.sensors[i] == nullptr) {
                continue;
            }
            if (_sensorList.sensors[i]->getSensorId() == id) {
                pDriver =
                    reinterpret_cast<T*>(_sensorList.sensors[i]->getDriver());
                return NO_ERROR;
            }
        }
        return DRIVER_NOT_FOUND_ERROR;
    };

  private:
    Data _data;
    SensorList _sensorList;
    IAutoDetector& _detector;
    void measure(const int index, const size_t position);
    bool timeIntervalPassed(const int index,
                            const unsigned long currentTimeStamp);

    void updateSensorStatus(const int index);
};

#endif /* _SENSOR_MANAGER_H_ */
