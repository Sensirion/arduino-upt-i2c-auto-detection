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
#include "SensorList.h"

AutoDetectorError SensorList::addSensor(ISensor* pSensor) {
    for (int i = 0; i < _MAX_NUM_SENSORS; ++i) {
        if (_sensors[i] == nullptr) {
            _sensors[i] = pSensor;
            if (_sensors[i]->getInitializationSteps() > 0) {
                _sensors[i]->setSensorState(SensorState::INITIALIZING);
            } else {
                _sensors[i]->setSensorState(SensorState::RUNNING);
            }
            return NO_ERROR;
        }
    }
    return FULL_SENSOR_LIST_ERROR;
}

size_t SensorList::count() {
    size_t numberOfSensors = 0;
    for (int i = 0; i < _MAX_NUM_SENSORS; ++i) {
        if (_sensors[i]) {
            ++numberOfSensors;
        }
    }
    return numberOfSensors;
}

size_t SensorList::getTotalNumberOfDataPoints() {
    size_t totalNumberOfDataPoints = 0;
    for (int i = 0; i < _MAX_NUM_SENSORS; ++i) {
        if (_sensors[i]) {
            totalNumberOfDataPoints += _sensors[i]->getNumberOfDataPoints();
        }
    }
    return totalNumberOfDataPoints;
}

void SensorList::reset() {
    for (int i = 0; i < _MAX_NUM_SENSORS; ++i) {
        delete _sensors[i];
        _sensors[i] = nullptr;
    }
}

uint16_t SensorList::getNumberOfSensorsLost() {
    uint16_t count = 0;
    for (int i = 0; i < _MAX_NUM_SENSORS; ++i) {
        if (_sensors[i] == NULL) {
            continue;
        }
        if (_sensors[i]->getSensorState() == SensorState::LOST) {
            ++count;
        }
    }
    return count;
}

int SensorList::getLength() const {
    return _MAX_NUM_SENSORS;
}

ISensor* SensorList::getSensor(size_t i) const {
    return _sensors[i];
}
