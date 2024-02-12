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

#include "SensorManager.h"

void SensorManager::refreshConnectedSensors() {
    _sensorList.removeLostSensors();
    _detector.findSensors(_sensorList);
}

void SensorManager::executeSensorCommunication() {
    for (int i = 0; i < _sensorList.getLength(); ++i) {
        SensorStateMachine* ssm = _sensorList.getSensorStateMachine(i);
        if (ssm) {
            AutoDetectorError error = ssm->update();
            const char* sensorName =
                sensorLabel(ssm->getSensor()->getSensorType());
            switch (error) {
                case I2C_ERROR:
                    Serial.printf("An I2C error occurred while attempting to "
                                  "execute a command on sensor %s.\n",
                                  sensorName);
                    break;
                case LOST_SENSOR_ERROR:
                    Serial.printf(
                        "Sensor %s was removed from list of active sensors.\n",
                        sensorName);
                    break;
                case SENSOR_READY_STATE_DECAYED_ERROR:
                    Serial.printf(
                        "AutoDetect refresh rate too low: sensor %s "
                        "conditioning deprecated. Decrease update interval.\n",
                        sensorName);
                    break;
                case NO_ERROR:
                default:
                    break;
            }
        }
    }
    return;
}

void SensorManager::getSensorReadings(const MeasurementList** dataHashmap) {
    for (int i = 0; i < _MAX_NUM_SENSORS; ++i) {
        const SensorStateMachine* ssm = _sensorList.getSensorStateMachine(i);
        if (ssm) {
            dataHashmap[i] = ssm->getSignals();
        }
    }
}

void SensorManager::refreshAndGetSensorReadings(
    const MeasurementList** dataHashmap) {
    refreshConnectedSensors();
    executeSensorCommunication();
    getSensorReadings(dataHashmap);
}

void SensorManager::setInterval(unsigned long interval, SensorType sensorId) {
    for (int i = 0; i < _sensorList.getLength(); ++i) {
        SensorStateMachine* ssm = _sensorList.getSensorStateMachine(i);
        if (ssm && ssm->getSensor()->getSensorType() == sensorId) {
            ssm->setMeasurementInterval(interval);
        }
    }
}

int SensorManager::getMaxNumberOfSensors() const {
    return _MAX_NUM_SENSORS;
}
