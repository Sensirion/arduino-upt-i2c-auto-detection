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
#include "utils.h"

void SensorManager::begin() {
    _detector.findSensors(_sensorList);
}

AutoDetectorError SensorManager::updateStateMachines() {
    for (int i = 0; i < _sensorList.getLength(); ++i) {
        SensorStateMachine* ssm = _sensorList.getSensorStateMachine(i);
        if (ssm) {
            ssm->update();
        }
    }
    return NO_ERROR;
}

void SensorManager::getData(const Data** dataPack) {
    for (size_t i = 0; i < _MAX_NUM_SENSORS; i++) {
        dataPack[i] = nullptr;
    }

    for (int i = 0; i < _MAX_NUM_SENSORS; ++i) {
        const SensorStateMachine* ssm = _sensorList.getSensorStateMachine(i);
        if (ssm) {
            size_t hashIdx =
                static_cast<size_t>(ssm->getSensor()->getSensorId());
            dataPack[hashIdx] = ssm->getSignals();
        }
    }
}

void SensorManager::setInterval(unsigned long interval, SensorID sensorId) {
    for (int i = 0; i < _sensorList.getLength(); ++i) {
        SensorStateMachine* ssm = _sensorList.getSensorStateMachine(i);
        if (ssm && ssm->getSensor()->getSensorId() == sensorId) {
            ssm->setMeasurementInterval(interval);
        }
    }
}

int SensorManager::getMaxNumberOfSensors() const {
    return _MAX_NUM_SENSORS;
}
