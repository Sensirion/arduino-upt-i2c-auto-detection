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
#include "SensorManager.h"

void SensorManager::init() {
    _sensorList.reset();
    _detector.findSensors(_sensorList);
    size_t length = _sensorList.getTotalNumberOfDataPoints();
    _data.init(length);
}

bool SensorManager::timeIntervalPassed(const int index,
                                       const unsigned long currentTimeStamp) {
    unsigned long elapsedTime =
        currentTimeStamp - _sensorList.latestMeasurementTimeStamps[index];
    if (elapsedTime < _sensorList.intervals[index]) {
        return false;
    } else {
        return true;
    }
}

void SensorManager::updateSensorStatus(const int index) {
    if (_sensorList.latestMeasurementErrors[index]) {
        ++_sensorList.errorCounter[index];
    } else {
        _sensorList.errorCounter[index] = 0;
    }
    if (_sensorList.errorCounter[index] >
        NUMBER_OF_ALLOWED_CONSECUTIVE_ERRORS) {
        _sensorList.sensorsLost[index] = true;
    }
}

void SensorManager::measure(const int index, const size_t position) {
    if (_sensorList.sensorsLost[index]) {
        return;
    }
    unsigned long currentTimeStamp = millis();
    if (timeIntervalPassed(index, currentTimeStamp) == false) {
        return;
    }
    _sensorList.latestMeasurementTimeStamps[index] = currentTimeStamp;
    _sensorList.latestMeasurementErrors[index] =
        _sensorList.sensors[index]->measure(_data.dataPoints + position,
                                            currentTimeStamp);
    updateSensorStatus(index);
}

AutoDetectorError SensorManager::updateData() {
    size_t position = 0;
    uint16_t numberOfSensorsLostBefore = _sensorList.getNumberOfSensorsLost();
    for (int i = 0; i < SensorList::LENGTH; ++i) {
        if (_sensorList.sensors[i] == nullptr) {
            continue;
        }
        measure(i, position);
        position += _sensorList.sensors[i]->getNumberOfDataPoints();
    }
    uint16_t numberOfSensorsLostAfter = _sensorList.getNumberOfSensorsLost();
    if (numberOfSensorsLostAfter > numberOfSensorsLostBefore) {
        return LOST_SENSOR_ERROR;
    } else {
        return NO_ERROR;
    }
}

const Data& SensorManager::getData() const {
    return _data;
}

void SensorManager::setInterval(unsigned long interval, SensorId sensorId) {
    for (int i = 0; i < SensorList::LENGTH; ++i) {
        if (_sensorList.sensors[i] == nullptr)
            continue;
        if (_sensorList.sensors[i]->getSensorId() == sensorId) {
            if (interval <
                _sensorList.sensors[i]->getMinimumMeasurementInterval()) {
                continue;
            }
            _sensorList.intervals[i] = interval;
        }
    }
}
