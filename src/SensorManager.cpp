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

void SensorManager::begin() {
    _sensorList.reset();
    _detector.findSensors(_sensorList);
    size_t length = _sensorList.getTotalNumberOfDataPoints();
    _data.init(length);
}

AutoDetectorError SensorManager::updateData() {
    size_t writeOffset = 0;
    uint16_t numberOfSensorsLostBeforeUpdate =
        _sensorList.getNumberOfSensorsLost();
    for (int i = 0; i < SensorList::LENGTH; ++i) {
        ISensor* sensor = _sensorList.sensors[i];
        if (sensor == nullptr) {
            continue;
        }
        _updateSensor(sensor, i, writeOffset);
    }
    uint16_t numberOfSensorsLostAfterUpdate =
        _sensorList.getNumberOfSensorsLost();
    if (numberOfSensorsLostAfterUpdate > numberOfSensorsLostBeforeUpdate) {
        return LOST_SENSOR_ERROR;
    } else {
        return NO_ERROR;
    }
}

const Data& SensorManager::getData() const {
    return _data;
}

void SensorManager::setInterval(unsigned long interval, SensorID sensorId) {
    for (int i = 0; i < SensorList::LENGTH; ++i) {
        ISensor* sensor = _sensorList.sensors[i];
        if (sensor == nullptr)
            continue;
        if (sensor->getSensorId() != sensorId &&
            interval < sensor->getMinimumMeasurementIntervalMs()) {
            continue;
        }
        _sensorList.measurementIntervals[i] = interval;
    }
}

bool SensorManager::_timeIntervalPassed(
    const unsigned long interval, const unsigned long currentTimeStamp,
    const unsigned long latestUpdateTimeStamp) {
    if (currentTimeStamp < interval) {
        return true;
    }
    unsigned long elapsedTime = currentTimeStamp - latestUpdateTimeStamp;
    return elapsedTime >= interval;
}

void SensorManager::_updateSensor(ISensor* sensor, int index,
                                  size_t& writeOffset) {
    // Collect variables for readability
    unsigned long currentTimeStamp = millis();
    unsigned long initSteps = sensor->getInitializationSteps();
    unsigned long initIntervalMs = sensor->getInitializationIntervalMs();
    unsigned long measureIntervalMs = _sensorList.measurementIntervals[index];

    const unsigned long& latestUpdateTimeStamp =
        _sensorList.latestUpdateTimeStamps[index];
    uint16_t& sensorError = _sensorList.latestMeasurementErrors[index];
    uint16_t& errorCounter = _sensorList.measurementErrorCounters[index];
    uint16_t& initStepsCounter = _sensorList.initStepCounters[index];
    SensorState& state = _sensorList.sensorStates[index];

    DataPoint* writePosition = _data.dataPoints + writeOffset;

    // State handling
    switch (state) {
        case SensorState::UNDEFINED:
            break;

        case SensorState::INITIALIZING:
            // Check if initialization is done
            if (initStepsCounter >= initSteps) {
                state = SensorState::RUNNING;
            }
            // Set Sensor name of empty Datapoints for initialization period
            if (initStepsCounter == 0) {
                for (size_t i = 0; i < sensor->getNumberOfDataPoints(); ++i) {
                    DataPoint* dataPoint = writePosition + i;
                    dataPoint->sourceDevice = sensorName(sensor->getSensorId());
                }
            }
            // Only perform initialization every initialization interval
            if (!_timeIntervalPassed(initIntervalMs, currentTimeStamp,
                                     latestUpdateTimeStamp)) {
                break;
            }
            sensor->initializationStep();
            initStepsCounter += 1;
            break;

        case SensorState::RUNNING:
            // Only perform measurement every measurement interval
            if (!_timeIntervalPassed(measureIntervalMs, currentTimeStamp,
                                     latestUpdateTimeStamp)) {
                break;
            }
            sensorError =
                sensor->measureAndWrite(writePosition, currentTimeStamp);
            // Update error counter
            if (!sensorError) {
                errorCounter = 0;
                break;
            } else {
                errorCounter += 1;
            }
            // Check for max number of allowed errors
            if (errorCounter >=
                sensor->getNumberOfAllowedConsecutiveErrors()) {
                state = SensorState::LOST;
            }
            break;

        case SensorState::LOST:
            break;

        default:
            break;
    }

    writeOffset +=
        sensor->getNumberOfDataPoints();  // only gets executed after "break"
                                          // but not after "continue"
    return;
}
